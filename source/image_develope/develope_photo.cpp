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

#include "develope_photo.hpp"
#include "validuvc/uvcpheader_checker.hpp"
#include "validuvc/control_config.hpp"
#include "rgb_to_jpeg.hpp"
#include "yuyv_to_rgb.hpp"

void DevFImage::develope_mjpeg_to_jpg(std::vector<std::vector<u_char>>& binary_data, const std::string& output_jpg_path) {
    std::ofstream output_file(output_jpg_path, std::ios::binary);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }

    for (const auto& chunk : binary_data) {
        output_file.write(reinterpret_cast<const char*>(chunk.data()), chunk.size());
    }
}

void DevFImage::develope_rgb_to_jpg(const DevFImageFormat& frame_format, std::vector<std::vector<u_char>>& frame_data, const std::string& output_jpg_path) {
    std::vector<u_char> rgb_data;
    size_t required_size = frame_format.width * frame_format.height * 3;
    rgb_data.reserve(required_size);

    for (auto& row : frame_data) {
        rgb_data.insert(rgb_data.end(), std::make_move_iterator(row.begin()), std::make_move_iterator(row.end()));
    }

    if (rgb_data.size() < required_size) {
        rgb_data.resize(required_size, 0);
        std::cerr << "Warning: RGB data was smaller than expected. Filled with 0." << std::endl;
    } else if (rgb_data.size() > required_size) {
        rgb_data.resize(required_size);
        std::cerr << "Warning: RGB data was larger than expected. Excess data was truncated." << std::endl;
    }

    saveJPEG(rgb_data, frame_format.width, frame_format.height, output_jpg_path);
}

void DevFImage::develope_yuyv_to_jpg(const DevFImageFormat& frame_format, std::vector<std::vector<u_char>>& frame_data, const std::string& output_jpg_path) {
    std::vector<u_char> yuyv_data;
    size_t required_size = frame_format.width * frame_format.height * 2;
    yuyv_data.reserve(required_size);

    for (auto& row : frame_data) {
        yuyv_data.insert(yuyv_data.end(), std::make_move_iterator(row.begin()), std::make_move_iterator(row.end()));
    }

    if (yuyv_data.size() < required_size) {
        yuyv_data.resize(required_size, 0);
        std::cerr << "Warning: YUYV data was smaller than expected. Filled with 0." << std::endl;
    } else if (yuyv_data.size() > required_size) {
        yuyv_data.resize(required_size);
        std::cerr << "Warning: YUYV data was larger than expected. Excess data was truncated." << std::endl;
    }

    std::vector<u_char> rgb_data = convertYUYVtoRGB(yuyv_data, frame_format.width, frame_format.height);

    std::cerr << "RGB Convertion Success" << std::endl;
    
    saveJPEG(rgb_data, frame_format.width, frame_format.height, output_jpg_path);
}

void DevFImage::develope_photo(const DevFImageFormat& frame_format, std::vector<std::vector<u_char>>& frame_data){
//recieve frame number, frame format and the data by using queue
#ifdef _WIN32
        std::string output_jpg_path = "images\\frame_" + std::to_string(frame_format.frame_number) + ".jpg";
#elif __linux__
        std::string output_jpg_path = "./images/frame_" + std::to_string(frame_format.frame_number) + ".jpg";
#endif

    bool save_success = false;

    if (frame_format.format == "mjpeg"){
        develope_mjpeg_to_jpg(frame_data, output_jpg_path);
        save_success = true;
    } else if (frame_format.format == "yuyv"){
        develope_yuyv_to_jpg(frame_format, frame_data, output_jpg_path);
        save_success = true;
    } else if (frame_format.format == "h264"){
        std::cout << "No support for H264 format." << std::endl;
        return;
    } else if (frame_format.format == "rgb"){
        develope_rgb_to_jpg(frame_format, frame_data, output_jpg_path);
        save_success = true;
    } else {
        std::cout << "Unsupported frame format: " << frame_format.format << std::endl;
    }


    if (save_success) {
        std::cout << "Frame " << frame_format.frame_number << " saved as JPEG in " << output_jpg_path << std::endl;
    } else {
        std::cerr << "Failed to save frame " << frame_format.frame_number << " in " << output_jpg_path << std::endl;
    }
};
