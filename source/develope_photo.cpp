

#include <iostream>
#include "validuvc/uvcpheader_checker.hpp"
#include "validuvc/control_config.hpp"

void u_char_to_jpg(const std::vector<u_char>& binary_data, const std::string& output_jpg_path) {
    std::ofstream output_file(output_jpg_path, std::ios::binary);
    if (!output_file.is_open()) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }
    output_file.write(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
}


void develope_photo(){
// #ifdef GUI_SET
// gui_window_number = 12;

//recieve frame number, frame format and the data by using queue

    std::string develope_frame_format;

    if (develope_frame_format == "mjpeg"){

    } else if (develope_frame_format == "yuyv"){

    } else if (develope_frame_format == "h264"){

    } else if (develope_frame_format == "rgb"){

    } else {
        v_cerr_2 << "Unsupported frame format: " << develope_frame_format << std::endl;
    }



// #endif
};
