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

#include "validuvc/control_config.hpp"

#include <algorithm>
#include <iostream>
#include <cstdint>

// int ControlConfig::width = 1280;
// int ControlConfig::height = 720;
// int ControlConfig::fps = 30;
// std::string ControlConfig::frame_format = "mjpeg";
// uint64_t ControlConfig::dwMaxVideoFrameSize = 16777216;
// uint64_t ControlConfig::dwMaxPayloadTransferSize = 1310720;

int ControlConfig::width = 1;
int ControlConfig::height = 1;
int ControlConfig::fps = 1;
std::string ControlConfig::frame_format = "mjpeg";
uint32_t ControlConfig::dwMaxVideoFrameSize = 1;
uint32_t ControlConfig::dwMaxPayloadTransferSize = 1;
uint32_t ControlConfig::dwTimeFrequency = 1;


void ControlConfig::set_width(int w) { width = w; }

void ControlConfig::set_height(int h) { height = h; }

void ControlConfig::set_fps(int f) { fps = f; }

void ControlConfig::set_frame_format(const std::string& format) {
  std::string format_lower = format;
  std::transform(format_lower.begin(), format_lower.end(), format_lower.begin(),
                 ::tolower);

  if (format_lower == "mjpeg" || format_lower == "h264" ||
      format_lower == "yuyv" || format_lower == "rgb") {
    frame_format = format_lower;
  } else {
    std::cerr << "Unsupported frame format: " << format
              << ". Using default format 'mjpeg'." << std::endl;
    frame_format = "mjpeg";
  }
}

void ControlConfig::set_dwMaxVideoFrameSize(uint32_t max_video_frame_size) {
  dwMaxVideoFrameSize = max_video_frame_size;
}

void ControlConfig::set_dwMaxPayloadTransferSize(uint32_t max_payload_transfer_size) {
  dwMaxPayloadTransferSize = max_payload_transfer_size;
}

void ControlConfig::set_dwTimeFrequency(uint32_t time_frequency) {
  dwTimeFrequency = time_frequency;
}

int ControlConfig::get_width() { return width; }

int ControlConfig::get_height() { return height; }

int ControlConfig::get_fps() { return fps; }

std::string ControlConfig::get_frame_format() { return frame_format; }

uint32_t ControlConfig::get_dwMaxVideoFrameSize() { return dwMaxVideoFrameSize; }

uint32_t ControlConfig::get_dwMaxPayloadTransferSize() { return dwMaxPayloadTransferSize; }

uint32_t ControlConfig::get_dwTimeFrequency() { return dwTimeFrequency; }


