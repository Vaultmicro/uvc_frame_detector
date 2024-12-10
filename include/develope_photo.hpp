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
#ifndef DEVELOP_PHOTO_HPP
#define DEVELOP_PHOTO_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#ifdef __linux__
#include <condition_variable>
#endif

#ifdef _WIN32
  typedef unsigned char u_char;
#endif

class DevFImage{
public:
  static DevFImage& instance(){
    static DevFImage devfimage;
    return devfimage;
  }

  struct DevFImageFormat{
    int frame_number;
    int width;
    int height;
    std::string format;
  };

  std::mutex dev_f_image_mutex;
  std::condition_variable dev_f_image_cv;
  std::queue<std::vector<std::vector<u_char>>> dev_f_image_queue;
  std::queue<DevFImageFormat> dev_f_image_format_queue;

  void u_char_to_jpg(const std::vector<std::vector<u_char>>& binary_data, const std::string& output_jpg_path);
  void develope_photo(const DevFImageFormat& frame_format, std::vector<std::vector<u_char>>& frame_data);

  void develope_mjpeg_to_jpg(std::vector<std::vector<u_char>>& binary_data, const std::string& output_jpg_path);
  void develope_rgb_to_jpg(const DevFImageFormat& frame_format, std::vector<std::vector<u_char>>& frame_data, const std::string& output_jpg_path);
  void develope_yuyv_to_jpg(const DevFImageFormat& frame_format, std::vector<std::vector<u_char>>& frame_data, const std::string& output_jpg_path);

private:
  DevFImage() = default;
  DevFImage(const DevFImage&) = delete;
  DevFImage& operator=(const DevFImage&) = delete;
};


// struct DevFImageFormat{
//     int frame_number;
//     int width;
//     int height;
//     std::string format;
// };


// extern std::mutex dev_f_image_mutex;
// extern std::condition_variable dev_f_image_cv;
// extern std::queue<std::vector<std::vector<u_char>>> dev_f_image_queue;
// extern std::queue<DevFImageFormat> dev_f_image_format_queue;


// void u_char_to_jpg(const std::vector<std::vector<u_char>>& binary_data, const std::string& output_jpg_path);
// void develope_photo(const DevFImageFormat& frame_format, std::vector<std::vector<u_char>>& frame_data);

#endif // DEVELOP_PHOTO_HPP
