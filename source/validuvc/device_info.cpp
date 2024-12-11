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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "validuvc/device_info.hpp"
// #include "nlohmann/json.hpp"


//Device Info
DeviceInfo::DeviceInfo() :
    vendor_id_(0), product_id_(0), serial_number_(0), name_("-"), description_("-") {}

void DeviceInfo::set_vendor_id(int vendor_id) { vendor_id_ = vendor_id; }
void DeviceInfo::set_product_id(int product_id) { product_id_ = product_id; }
void DeviceInfo::set_serial_number(int serial_number) { serial_number_ = serial_number; }
void DeviceInfo::set_name(const std::string& name) { name_ = name; }
void DeviceInfo::set_description(const std::string& description) { description_ = description; }

int DeviceInfo::get_vendor_id() const { return vendor_id_; }
int DeviceInfo::get_product_id() const { return product_id_; }
int DeviceInfo::get_serial_number() const { return serial_number_; }
std::string DeviceInfo::get_name() const { return name_; }
std::string DeviceInfo::get_description() const { return description_; }


//Device Info List
DeviceInfoList& DeviceInfoList::get_instance() {
    static DeviceInfoList instance;
    return instance;
}

DeviceInfoList::DeviceInfoList(){}

void DeviceInfoList::call_log_history(){}
// when constructor is called read once

void DeviceInfoList::update(int vendor_id, int product_id) {
    for (auto& device : devices_) {
        if (device.get_vendor_id() == vendor_id && 
            device.get_product_id() == product_id ) {
            
            current_device.set_vendor_id(device.get_vendor_id());
            current_device.set_product_id(device.get_product_id());
            current_device.set_serial_number(device.get_serial_number());
            current_device.set_name(device.get_name());
            current_device.set_description(device.get_description());
            
            // ControlConfig& c_device_desc = ControlConfig::instance();
            // c_device_desc.set_vendor_id(device.get_vendor_id());
            // c_device_desc.set_product_id(device.get_product_id());
            // c_device_desc.set_device_name(device.get_name());

            return;
        }
    }
    

    current_device.set_vendor_id(vendor_id);
    current_device.set_product_id(product_id);
    current_device.set_serial_number(0);
    current_device.set_name("Unknown");
    current_device.set_description("Unknown");

    devices_.push_back(current_device);

    // ControlConfig& c_device_desc = ControlConfig::instance();
    // c_device_desc.set_vendor_id(vendor_id);
    // c_device_desc.set_product_id(product_id);
    // c_device_desc.set_device_name("Unknown");
    
    // //saves to json file here
    // save_log_history();
    return;
}

void DeviceInfoList::save_log_history(){
    // nlohmann::json j;
    // for (const auto& device : devices_) {
    //     j.push_back({
    //         {"vendor_id", device.get_vendor_id()},
    //         {"product_id", device.get_product_id()},
    //         {"serial_number", device.get_serial_number()},
    //         {"name", device.get_name()},
    //         {"description", device.get_description()}
    //     });
    // }
    // std::ofstream o("device_info.json");
    // o << j.dump(4) << std::endl;
}