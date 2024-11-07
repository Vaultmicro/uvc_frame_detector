// develope_photo.hpp
#ifndef DEVELOP_PHOTO_HPP
#define DEVELOP_PHOTO_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>

#ifdef _WIN32
  typedef unsigned char u_char;
#endif

struct DevFImageFormat{
    int frame_number;
    int width;
    int height;
    std::string format;
};

extern std::mutex dev_f_image_mutex;
extern std::condition_variable dev_f_image_cv;
extern std::queue<std::vector<std::vector<u_char>>> dev_f_image_queue;
extern std::queue<DevFImageFormat> dev_f_image_format_queue;


void u_char_to_jpg(const std::vector<std::vector<u_char>>& binary_data, const std::string& output_jpg_path);
void develope_photo(const DevFImageFormat& frame_format, std::vector<std::vector<u_char>>& frame_data);

#endif // DEVELOP_PHOTO_HPP
