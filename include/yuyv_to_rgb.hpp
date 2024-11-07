#ifndef YTR_HPP
#define YTR_HPP

#include <iostream>
#include <vector>
#include <turbojpeg.h>

#ifdef _WIN32
typedef unsigned char u_char;
#endif

std::vector<u_char> convertYUYVtoRGB(const std::vector<u_char>& yuyvData, int width, int height);

#endif // YTR_HPP