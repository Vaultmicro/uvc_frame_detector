/*********************************************************************
 * Copyright (c) 2024 Vaultmicro, Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*********************************************************************/


#include "libuvc/libuvc.h"
#include <stdio.h>
#include <unistd.h>

#include <pthread.h>
#include <stdint.h>
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "validuvc/uvc_stream.hpp"
#include "validuvc/uvcpheader_checker.hpp"
#include "validuvc/control_config.hpp"
#include "utils/verbose.hpp"
#include "utils/logger.hpp"

#include <csignal>


#define MAX_QUEUE_SIZE 100
uint8_t* packet_queue[MAX_QUEUE_SIZE];
size_t packet_sizes[MAX_QUEUE_SIZE];
int queue_front = 0;
int queue_rear = 0;  
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cv = PTHREAD_COND_INITIALIZER;
int stop_processing = 0;


void clean_exit(int signum) {
    stop_processing = 1;  // Set the stop flag to true
    pthread_cond_signal(&queue_cv);  // Wake up any waiting threads
}

void process_packets() {
    UVCPHeaderChecker header_checker;

    while (!stop_processing) {
      // std::cout << "CORAL" << std::endl;
      pthread_mutex_lock(&queue_mutex);

      // Wait until the queue is not empty or stop_processing is set
      while (queue_front == queue_rear && !stop_processing) {
          pthread_cond_wait(&queue_cv, &queue_mutex);
      }

      if (stop_processing && queue_front == queue_rear) {
          pthread_mutex_unlock(&queue_mutex);
          break;
      }

      // Get the packet from the queue
      uint8_t* packet = packet_queue[queue_front];
      size_t packet_size = packet_sizes[queue_front];
      queue_front = (queue_front + 1) % MAX_QUEUE_SIZE;

      pthread_mutex_unlock(&queue_mutex);


      // Convert the packet to std::vector for C++ processing
      std::vector<uint8_t> packet_vector(packet, packet + packet_size);

      auto received_time = std::chrono::steady_clock::now();

      // Validate the packet
      uint8_t valid_err = header_checker.payload_valid_ctrl(packet_vector, received_time);

      if (valid_err) {
          fprintf(stderr, "Invalid packet detected\n");
          free(packet);  // Free the original C-style allocated memory
          continue;
      }

      free(packet);  // Free the original C-style allocated memory
  }

  header_checker.print_stats();
}

// Callback function to handle incoming frames
void cb(uvc_frame_t *frame, void *ptr) {
    if (stop_processing) {
        return;
    }
  enum uvc_frame_format *frame_format = (enum uvc_frame_format *)ptr;
  //printf("callback! frame_format = %d, width = %d, height = %d, length = %lu, ptr = %p\n",
  //  frame->frame_format, frame->width, frame->height, frame->data_bytes, ptr);

  // Check if frame data is received
  if (frame->sequence % 30 == 0) {
    // printf(" * got image %u\n",  frame->sequence);
  }
}

void* uvc_streaming_thread(void* arg) {
    uvc_device_handle_t* devh = (uvc_device_handle_t*)arg;
    uvc_stream_ctrl_t ctrl;
    uvc_error_t res;

    /* Initialize stream control settings here (width, height, frame format, etc.) */
    // (Example: width = 640, height = 480, frame_format = UVC_COLOR_FORMAT_MJPEG)
    const uvc_format_desc_t *format_desc = uvc_get_format_descs(devh);
    const uvc_frame_desc_t *frame_desc = format_desc->frame_descs;
    enum uvc_frame_format frame_format;
    int width = 1280;
    int height = 720;
    int fps = 30;

    // Determine the format based on descriptor subtype
    switch (format_desc->bDescriptorSubtype) {
    case UVC_VS_FORMAT_MJPEG:
        frame_format = UVC_COLOR_FORMAT_MJPEG;
        break;
    case UVC_VS_FORMAT_FRAME_BASED:
        frame_format = UVC_FRAME_FORMAT_H264;
        break;
    default:
        frame_format = UVC_FRAME_FORMAT_YUYV;
        break;
    }

    // If frame descriptor is present, use its settings
    while (frame_desc) {
        width = frame_desc->wWidth;
        height = frame_desc->wHeight;
        fps = 10000000 / frame_desc->dwDefaultFrameInterval;
        
        printf("\nTrying format: (%4s) %dx%d %dfps\n", format_desc->fourccFormat, width, height, fps);

        res = uvc_get_stream_ctrl_format_size(devh, &ctrl, frame_format, width, height, fps);
        if (res >= 0) {
            printf("Found valid mode: %dx%d %dfps\n", width, height, fps);
            break;  // Exit loop if valid mode is found
        } else {
            uvc_perror(res, "get_mode");  // Print the error message for the invalid mode
        }

        frame_desc = frame_desc->next;  // Go to next frame descriptor
    }

    printf("\nFirst format: (%4s) %dx%d %dfps\n", format_desc->fourccFormat, width, height, fps);

    // Assume `devh` is already initialized and device is opened.
    res = uvc_get_stream_ctrl_format_size(devh, &ctrl, frame_format, width, height, fps);
    if (res < 0) {
        uvc_perror(res, "get_mode");
        return NULL;
    }

    uvc_print_stream_ctrl(&ctrl, stderr);

    res = uvc_start_streaming(devh, &ctrl, cb, (void*)12345, 0);
    if (res < 0) {
        uvc_perror(res, "start_streaming");
        return NULL;
    }

    puts("Streaming started in a separate thread...");
    // sleep(30); // Stream for seconds
    while (!stop_processing) {
        usleep(1000);  // Allow thread to keep running
    }

    // // Enable auto exposure mode
    // puts("Enabling auto exposure ...");
    // const uint8_t UVC_AUTO_EXPOSURE_MODE_AUTO = 2;
    // res = uvc_set_ae_mode(devh, UVC_AUTO_EXPOSURE_MODE_AUTO);
    // if (res == UVC_SUCCESS) {
    //     puts(" ... enabled auto exposure");
    // } else if (res == UVC_ERROR_PIPE) {
    //     // Try aperture priority mode if full AE mode is not supported
    //     puts(" ... full AE not supported, trying aperture priority mode");
    //     const uint8_t UVC_AUTO_EXPOSURE_MODE_APERTURE_PRIORITY = 8;
    //     res = uvc_set_ae_mode(devh, UVC_AUTO_EXPOSURE_MODE_APERTURE_PRIORITY);
    //     if (res < 0) {
    //         uvc_perror(res, " ... uvc_set_ae_mode failed to enable aperture priority mode");
    //     } else {
    //         puts(" ... enabled aperture priority auto exposure mode");
    //     }
    // } else {
    //     uvc_perror(res, " ... uvc_set_ae_mode failed to enable auto exposure mode");
    // }

    uvc_stop_streaming(devh);
    puts("Streaming stopped.");

    return NULL;
}

void* process_packets_thread(void* arg) {
  process_packets();
  return NULL;
}


int main(int argc, char** argv) {
    uvc_context_t* ctx;
    uvc_device_t* dev;
    uvc_device_handle_t* devh;
    uvc_error_t res;

    res = uvc_init(&ctx, NULL);
    if (res < 0) {
        uvc_perror(res, "uvc_init");
        return res;
    }
    puts("UVC initialized");

    res = uvc_find_device(ctx, &dev, 0, 0, NULL);
    if (res < 0) {
        uvc_perror(res, "uvc_find_device");
        uvc_exit(ctx);
        return res;
    }
    puts("Device found");

    res = uvc_open(dev, &devh);
    if (res < 0) {
        uvc_perror(res, "uvc_open");
        uvc_unref_device(dev);
        uvc_exit(ctx);
        return res;
    }
    puts("Device opened");

    std::signal(SIGINT, clean_exit);



    // uvc_print_diag(devh, stderr);

    // Create threads
    pthread_t streaming_thread, packet_thread;

    // Start the threads
    pthread_create(&streaming_thread, NULL, uvc_streaming_thread, (void*)devh);
    pthread_create(&packet_thread, NULL, process_packets_thread, NULL);

    // Wait for both threads to complete
    pthread_join(streaming_thread, NULL);
    pthread_join(packet_thread, NULL);

    // Clean up
    uvc_close(devh);
    uvc_unref_device(dev);
    uvc_exit(ctx);
    puts("UVC exited");

    return 0;
}
