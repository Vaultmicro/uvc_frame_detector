#ifndef RTJ_HPP
#define RTJ_HPP

#include <vector>
#include <string>

#ifdef _WIN32
typedef unsigned char u_char;
#endif

std::vector<u_char> readRGBFile(const std::string& filename, int width, int height);

bool saveJPEG(const std::vector<u_char>& rgbData, int width, int height, const std::string& filename);

#endif // RTJ_HPP