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

ControlConfig& ControlConfig::instance() {
    static ControlConfig instance; // Guaranteed to be destroyed, instantiated on first use.
    return instance;
}

ControlConfig::ControlConfig() 
    : width(1), height(1), fps(1), frame_format("mjpeg"),
      dwMaxVideoFrameSize(1), dwMaxPayloadTransferSize(1), dwTimeFrequency(1) {}


void ControlConfig::set_width(int w) {
    width = w;
}

void ControlConfig::set_height(int h) {
    height = h;
}

void ControlConfig::set_fps(int f) {
    fps = f;
}

void ControlConfig::set_frame_format(const std::string& format) {
    std::string format_lower = format;
    std::transform(format_lower.begin(), format_lower.end(), format_lower.begin(), ::tolower);

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

int ControlConfig::get_width() const {
    return width;
}

int ControlConfig::get_height() const {
    return height;
}

int ControlConfig::get_fps() const {
    return fps;
}

std::string ControlConfig::get_frame_format() const {
    return frame_format;
}

uint32_t ControlConfig::get_dwMaxVideoFrameSize() const {
    return dwMaxVideoFrameSize;
}

uint32_t ControlConfig::get_dwMaxPayloadTransferSize() const {
    return dwMaxPayloadTransferSize;
}

uint32_t ControlConfig::get_dwTimeFrequency() const {
    return dwTimeFrequency;
}


