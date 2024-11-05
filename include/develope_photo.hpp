// develope_photo.hpp
#ifndef DEVELOP_PHOTO_HPP
#define DEVELOP_PHOTO_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#ifdef _WIN32
  typedef unsigned char u_char;
#endif

void u_char_to_jpg(const std::vector<u_char>& binary_data, const std::string& output_jpg_path);
void develope_photo(const std::vector<u_char>& frame_data);

#endif // DEVELOP_PHOTO_HPP
