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
#include <turbojpeg.h>

#include "rgb_to_jpeg.hpp"

std::vector<u_char> readRGBFile(const std::string& filename, int width, int height) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return {};
    }

    std::vector<u_char> rgbData(width * height * 3);
    file.read(reinterpret_cast<char*>(rgbData.data()), rgbData.size());

    if (!file) {
        std::cerr << "Error: Could not read RGB data from file " << filename << std::endl;
        return {};
    }

    return rgbData;
}

bool saveJPEG(const std::vector<u_char>& rgbData, int width, int height, const std::string& filename) {
    tjhandle compressor = tjInitCompress();
    if (!compressor) {
        std::cerr << "Error: Could not initialize TurboJPEG compressor." << std::endl;
        return false;
    }

    unsigned char* jpegBuffer = nullptr;
    unsigned long jpegSize = 0;
    int quality = 85;

    if (tjCompress2(compressor, rgbData.data(), width, 0, height, TJPF_RGB, &jpegBuffer, &jpegSize, TJSAMP_444, quality, TJFLAG_FASTDCT) < 0) {
        std::cerr << "Error: Failed to compress JPEG image: " << tjGetErrorStr() << std::endl;
        tjDestroy(compressor);
        return false;
    }

    FILE* outfile = nullptr;
#ifdef _WIN32
    if (fopen_s(&outfile, filename.c_str(), "wb") != 0 || !outfile) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        tjFree(jpegBuffer);
        tjDestroy(compressor);
        return false;
    }
#else
    outfile = fopen(filename.c_str(), "wb");
    if (!outfile) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        tjFree(jpegBuffer);
        tjDestroy(compressor);
        return false;
    }

#endif

    fwrite(jpegBuffer, jpegSize, 1, outfile);
    fclose(outfile);

    tjFree(jpegBuffer);
    tjDestroy(compressor);

    // std::cout << "JPEG file saved as " << filename << std::endl;
    return true;
}


// int main() {
//     std::string rgbFilename = "..\\src\\smpte.rgb";
//     int width = 1280; 
//     int height = 720; 

//     std::vector<u_char> rgbData = readRGBFile(rgbFilename, width, height);
//     if (rgbData.empty()) {
//         std::cerr << "Failed to read RGB file." << std::endl;
//         return 1;
//     }

//     std::string jpegFilename = "..\\images\\smpte.jpg";
//     if (!saveJPEG(rgbData, width, height, jpegFilename)) {
//         std::cerr << "Failed to save JPEG file." << std::endl;
//         return 1;
//     }

//     return 0;
// }
