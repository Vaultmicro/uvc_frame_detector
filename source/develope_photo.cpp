
#include "develope_photo.hpp"
#include "validuvc/uvcpheader_checker.hpp"
#include "validuvc/control_config.hpp"

std::mutex dev_f_image_mutex;
std::condition_variable dev_f_image_cv;
std::queue<std::vector<std::vector<u_char>>> dev_f_image_queue;
std::queue<DevFImageFormat> dev_f_image_format_queue;

void u_char_to_jpg(const std::vector<std::vector<u_char>>& binary_data, const std::string& output_jpg_path) {
    std::ofstream output_file(output_jpg_path, std::ios::binary);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }

    for (const auto& chunk : binary_data) {
        output_file.write(reinterpret_cast<const char*>(chunk.data()), chunk.size());
    }
}

void develope_photo(const DevFImageFormat& frame_format, const std::vector<std::vector<u_char>>& frame_data){
// #ifdef GUI_SET
// gui_window_number = 12;

//recieve frame number, frame format and the data by using queue

    if (frame_format.format == "mjpeg"){
#ifdef _WIN32
        std::string output_jpg_path = "images\\frame_" + std::to_string(frame_format.frame_number) + ".jpg";
#elif __linux__
        std::string output_jpg_path = "./images/frame_" + std::to_string(frame_format.frame_number) + ".jpg";
#endif
        u_char_to_jpg(frame_data, output_jpg_path);
        std::cout << "Frame " << frame_format.frame_number << " saved as " << output_jpg_path << std::endl;

    } else if (frame_format.format == "yuyv"){

    } else if (frame_format.format == "h264"){

    } else if (frame_format.format == "rgb"){

    } else {
        std::cout << "Unsupported frame format: " << frame_format.format << std::endl;
    }


// #endif
};
