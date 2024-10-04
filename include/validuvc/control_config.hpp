#ifndef CONTROL_CONFIG_HPP
#define CONTROL_CONFIG_HPP

#include <string>
#include <cstdint>

class ControlConfig {
public:
    static int width;
    static int height;
    static int fps;
    static std::string frame_format;
    static uint64_t dwMaxVideoFrameSize;
    static uint64_t dwMaxPayloadTransferSize;

    // Setter 함수들
    static void set_width(int w);
    static void set_height(int h);
    static void set_fps(int f);
    static void set_frame_format(const std::string& format);

    // Getter 함수들
    static int get_width();
    static int get_height();
    static int get_fps();
    static std::string get_frame_format();

    static uint64_t set_dwMaxVideoFrameSize(uint64_t max_video_frame_size);
    static uint64_t set_dwMaxPayloadTransferSize(uint64_t max_payload_transfer_size);
};

#endif // CONTROL_CONFIG_HPP
