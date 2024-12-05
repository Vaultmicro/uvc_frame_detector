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

#include "utils/verbose.hpp"
#include <functional>
#ifdef GUI_SET
#include "gui/gui_win.hpp"
#endif

// Initialize verbose level
int VerboseStream::verbose_level = 2;

// VerboseStream definitions
namespace CtrlPrint {
  VerboseStream v_cout_1(1, "", std::cout);
  VerboseStream v_cerr_1(1, "", std::cerr);
  VerboseStream v_cout_2(2, "", std::cout);
  VerboseStream v_cerr_2(2, "", std::cerr);
  VerboseStream v_cout_3(3, "", std::cout);
  VerboseStream v_cerr_3(3, "", std::cerr);
  VerboseStream v_cout_4(4, "", std::cout);
  VerboseStream v_cerr_4(4, "", std::cerr);
  VerboseStream v_cout_5(5, "", std::cout);
  VerboseStream v_cerr_5(5, "", std::cerr);
}

// VerboseStream constructor implementation
VerboseStream::VerboseStream(int level, const std::string& prefix,
                             std::ostream& output_stream)
    : level_(level), prefix_(prefix), output_stream_(output_stream) {}

// VerboseStream implementation
VerboseStream& VerboseStream::operator<<(std::ostream& (*manip)(std::ostream&) ) {
  if (VerboseStream::verbose_level >= level_) {
    buffer_ << manip;
    flush();
  }
  return *this;
}



void VerboseStream::flush() {
  if (VerboseStream::verbose_level >= level_) {
#ifdef GUI_SET
    WindowManager& uvcfd_win = WindowManager::getInstance();
    // WindowData& data = manager.getWindowData(gui_window_number);

    WindowData* data = nullptr;
    
    switch (gui_window_number) {
    case 0: data = &uvcfd_win.getWin_ErrorFrame(); break;
    case 1: data = &uvcfd_win.getWin_FrameTime(); break;
    case 2: data = &uvcfd_win.getWin_Summary(); break;
    case 3: data = &uvcfd_win.getWin_ControlConfig(); break;
    case 4: data = &uvcfd_win.getWin_Statistics(); break;
    case 5: data = &uvcfd_win.getWin_Debug(); break;
    case 6: data = &uvcfd_win.getWin_PreviousValid(); break;
    case 7: data = &uvcfd_win.getWin_LostInbetweenError(); break;
    case 8: data = &uvcfd_win.getWin_CurrentError(); break;
    case 11: data = &uvcfd_win.getWin_LogButtons(); break;
    case 13: data = &uvcfd_win.getWin_ValidFrame(); break;
    default: break;
    }

    if (data != nullptr) {
      if (frame_error_flag){
        data->pushback_errorlog(buffer_.str());
        // manager.pushbackErrorLogText(gui_window_number,buffer_.str());
      }
      if (frame_suspicious_flag){
        data->pushback_suspiciouslog(buffer_.str());
        // manager.pushbackSuspiciousLogText(gui_window_number,buffer_.str());
      }
      if (print_whole_flag){
        data->set_move_customtext(std::move(buffer_.str()));
        // manager.setmoveCustomText(gui_window_number, buffer_.str());
      } else {
        data->add_move_customtext(buffer_.str());
        // manager.addmoveCustomText(gui_window_number, buffer_.str()); 
      }
    }
#else
    output_stream_ << buffer_.str();
#endif
    buffer_.str("");  // Clear the buffer after flushing
  }
}
