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

#ifndef CONTROL_CONFIG_HPP
#define CONTROL_CONFIG_HPP

#include <string>
#include <cstdint>

class ControlConfig {
public:
    static int width;
    static int height;
    static int fps;
    static std::string frame_format;
    static uint32_t dwMaxVideoFrameSize;
    static uint32_t dwMaxPayloadTransferSize;
    static uint32_t dwTimeFrequency;

    static void set_width(int w);
    static void set_height(int h);
    static void set_fps(int f);
    static void set_frame_format(const std::string& format);
    static uint32_t set_dwMaxVideoFrameSize(uint32_t max_video_frame_size);
    static uint32_t set_dwMaxPayloadTransferSize(uint32_t max_payload_transfer_size);
    static uint32_t set_dwTimeFrequency(uint32_t time_frequency);

    static int get_width();
    static int get_height();
    static int get_fps();
    static std::string get_frame_format();
    static uint32_t get_dwMaxVideoFrameSize();
    static uint32_t get_dwMaxPayloadTransferSize();
    static uint32_t get_dwTimeFrequency();
};

#endif // CONTROL_CONFIG_HPP
