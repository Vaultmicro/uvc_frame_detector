
#include <turbojpeg.h>
#include <algorithm>

#include "yuyv_to_rgb.hpp"

#ifdef _WIN32
typedef unsigned char u_char;
#endif

std::vector<u_char> convertYUYVtoRGB(const std::vector<u_char>& yuyvData, int width, int height) {
    std::vector<u_char> rgbData(width * height * 3);
    int pixelPairs = (width * height) / 2;
    
    for (int i = 0; i < pixelPairs; ++i) {
        int y0 = yuyvData[i * 4 + 0] - 16;
        int u  = yuyvData[i * 4 + 1] - 128;
        int y1 = yuyvData[i * 4 + 2] - 16;
        int v  = yuyvData[i * 4 + 3] - 128;

        int c0 = 298 * y0;
        int c1 = 298 * y1;
        int d  = 409 * v + 128;
        int e  = -100 * u + 128;
        int f  = -208 * v + 128;
        int g  = 516 * u + 128;

        int rgbIndex = i * 6;
        rgbData[rgbIndex + 0] = std::clamp((c0 + d) >> 8, 0, 255);
        rgbData[rgbIndex + 1] = std::clamp((c0 + e + f) >> 8, 0, 255);
        rgbData[rgbIndex + 2] = std::clamp((c0 + g) >> 8, 0, 255);

        rgbData[rgbIndex + 3] = std::clamp((c1 + d) >> 8, 0, 255);
        rgbData[rgbIndex + 4] = std::clamp((c1 + e + f) >> 8, 0, 255);
        rgbData[rgbIndex + 5] = std::clamp((c1 + g) >> 8, 0, 255);
    }
    return rgbData;
}

std::vector<u_char> combineYUYVtoRGB(std::vector<std::vector<u_char>>& yuyvDataList, int width, int height) {
    std::vector<u_char> combinedRGB;
    combinedRGB.reserve(yuyvDataList.size() * width * height * 3);

    for (auto& yuyvData : yuyvDataList) {
        std::vector<u_char> rgbData = convertYUYVtoRGB(yuyvData, width, height);
        combinedRGB.insert(combinedRGB.end(), std::make_move_iterator(rgbData.begin()), std::make_move_iterator(rgbData.end()));
    }
    
    return combinedRGB;
}
