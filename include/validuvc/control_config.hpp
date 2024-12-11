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
    // Getter for singleton instance
    static ControlConfig& instance();

    // Delete copy constructor and copy assignment operator
    ControlConfig(const ControlConfig&) = delete;
    ControlConfig& operator=(const ControlConfig&) = delete;

    // Public setters
    void set_vendor_id(int v);
    void set_product_id(int p);
    void set_device_name(const std::string& name);

    void set_width(int w);
    void set_height(int h);
    void set_fps(int f);
    void set_frame_format(const std::string& format);
    void set_dwMaxVideoFrameSize(uint32_t max_video_frame_size);
    void set_dwMaxPayloadTransferSize(uint32_t max_payload_transfer_size);
    void set_dwTimeFrequency(uint32_t time_frequency);

    // Public getters
    int get_vendor_id() const;
    int get_product_id() const;
    std::string get_device_name() const;

    int get_width() const;
    int get_height() const;
    int get_fps() const;
    std::string get_frame_format() const;
    uint32_t get_dwMaxVideoFrameSize() const;
    uint32_t get_dwMaxPayloadTransferSize() const;
    uint32_t get_dwTimeFrequency() const;

private:
    // Private constructor to prevent instantiation
    ControlConfig();

    // Device Info
    int vendor_id;
    int product_id;
    std::string device_name;

    // Member variables
    int width;
    int height;
    int fps;
    std::string frame_format;
    uint32_t dwMaxVideoFrameSize;
    uint32_t dwMaxPayloadTransferSize;
    uint32_t dwTimeFrequency;
};


#endif // CONTROL_CONFIG_HPP
