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
