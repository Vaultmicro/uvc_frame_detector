#include "validuvc/control_config.hpp"

#include <algorithm>
#include <iostream>

int ControlConfig::width = 1280;
int ControlConfig::height = 720;
int ControlConfig::fps = 30;
std::string ControlConfig::frame_format = "mjpeg";

void ControlConfig::set_width(int w) { width = w; }

void ControlConfig::set_height(int h) { height = h; }

void ControlConfig::set_fps(int f) { fps = f; }

void ControlConfig::set_frame_format(const std::string& format) {
  std::string format_lower = format;
  std::transform(format_lower.begin(), format_lower.end(), format_lower.begin(),
                 ::tolower);

  if (format_lower == "mjpeg" || format_lower == "h264" ||
      format_lower == "yuyv" || format_lower == "rgb") {
    frame_format = format_lower;
  } else {
    std::cerr << "Unsupported frame format: " << format
              << ". Using default format 'mjpeg'." << std::endl;
    frame_format = "mjpeg";
  }
}

int ControlConfig::get_width() { return width; }

int ControlConfig::get_height() { return height; }

int ControlConfig::get_fps() { return fps; }

std::string ControlConfig::get_frame_format() { return frame_format; }
