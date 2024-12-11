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

#ifndef DEVICE_INFO_HPP
#define DEVICE_INFO_HPP

#include <string>
#include <vector>
#include "control_config.hpp"

class DeviceInfo {
public:
    DeviceInfo();

    void set_vendor_id(int vendor_id);
    void set_product_id(int product_id);
    void set_serial_number(int serial_number);
    void set_name(const std::string& name);
    void set_description(const std::string& description);

    int get_vendor_id() const;
    int get_product_id() const;
    int get_serial_number() const;
    std::string get_name() const;
    std::string get_description() const;

private:
    int vendor_id_;
    int product_id_;
    int serial_number_;
    std::string name_;
    std::string description_;
};

class DeviceInfoList {
public:
    static DeviceInfoList& get_instance();
    void call_log_history();
    void update(int vendor_id, int product_id);
    void save_log_history();
    
    DeviceInfo current_device;

private:
    DeviceInfoList();
    DeviceInfoList(const DeviceInfoList&) = delete;
    DeviceInfoList& operator=(const DeviceInfoList&) = delete;

    std::vector<DeviceInfo> devices_;
    
};

#endif // DEVICE_INFO_HPP
