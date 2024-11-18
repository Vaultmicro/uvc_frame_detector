# UVC Frame Detector (uvc_frame_detector)

version 0.3.4
New method for Graphs and Buttons.  
New feature included, capturing and summarize suspicious frames.

---

## Overview
The UVC Frame Detector (UVCFD) analyzes USB video frames by capturing data, validating headers, and detecting errors in payloads and frames. It supports live camera streams and pre-recorded `pcapng` files.

### Key Features:
- Real-time analysis of USB video data.
- Validation and error detection for payloads and frames.
- Comprehensive error statistics.
---

## Executable Files

### Main Project :  
### UVCFD (UVC Frame Detector)  
This programme uses pipeline with tshark gets parsed data, recombine urb into payloads and frames.  
Detects invalid datas and payload headers, frames.  
Shows error data, stats. _t stands for tui, _g stands for gui.  
Can use live stream using camera application
Or can use pcapng file 

in window using camera application  
  ```
.\run_uvcfd.ps1  
  ```

in window using pcapng  
  ```
.\run_uvcfdp.ps1  
  ```

in linux  
  ```
./run_uvcfd.bash
  ```
  
</br>
</br>

### Side Projects:
### Moncapler
This programme uses usbmon* in linux to get raw data, recombine urb into payloads and frames.  
Uses same validation with oldmanandsea, however controlconfig data is not yet programmed.  

### UVCPerf
This programme uses usbmon* in linux to get raw data live, recombine urb into payloads and frames.  
Uses same validation with oldmanandsea & Moncapler, however controlconfig data is not yet programmed.  

### Valid_test, Frame_test_iso, Frame_test_bulk, Example  
Uses captured raw log data to test whether validation for payload and frame is well made.  
Google test is used to test each unit, and linux urb is used as an example (but can run in window).  

### Log_test, Log_test_t, Log_test_g
Gets txt file from wireshark, File>Export Packet Dissections>As Plain Text>Details: All expanded  
Use tests/change_shark/shkwhl_b for bulk shkwhl_i for iso  
And modify the log_test code to find designate txt file, run. _t stands for tui, _g stands for gui  
Auto controlconfig is not programmed.  

### Test_packet_handler
Can build and run in linux only, tests moncapler whether it has combined urb blocks into valid frames.  



## Usage

### In Window  
(only cmd works, not ps1, open in dev mode if possible)  
in ps1, put "cmd /c" at the start of command  
To run this code in window, must install wireshark in computer with USBPcap  
If USBPcap is not installed with Wireshark, reinstall wireshark and check USBPcap install  
After install, reboot  
C:\Program Files\Wireshark  
Find Wireshark.exe > go to Help(H) > Wireshark info(A) > Folder  
Check if Extcap Path is set extcap/wireshark  
If so go to C:\Program Files\Wireshark\extcap and move USBPcapCMD.exe to wireshark directory   


### Build
0. mkdir log
1. mkdir build
2. cd build
3. Configure the build system using CMake:
- For Debug build:
  ```
  cmake -DCMAKE_BUILD_TYPE=Debug ..
  ```
- For Release build:
  ```
  cmake -DCMAKE_BUILD_TYPE=Release ..
  ```
4. Configure the build system using CMake:
- For Debug build:
  ```
  cmake --build . --config Debug
  ```
- For Release build:
  ```
  cmake --build . --config Release
  ```
5. cd Debug/Release  

### Compiler info  
```
-- Building for: Visual Studio 17 2022  
-- Selecting Windows SDK version 10.0.26100.0 to target Windows 10.0.19045.  
-- The CXX compiler identification is MSVC 19.41.34120.0  
-- The C compiler identification is MSVC 19.41.34120.0  
```


### Run  
  
Use the shellscript given.
0. .\run_uvcfd
  
If want to force the format then type down below...  
Can find maximum frame size and maximum payload size in  
for Window > download usb device tree viewer and check video streaming format type descriptor: dwMaxVideoFrameBufferSize
for linux type lsusb -v and find dwMaxVideoFrameBufferSize  
if leave blank for -fw -fh -fps -ff -mf -mp, everything will be set automatically  
each indicate frame_width frame_height frame_per_sec frame_format max_frame_size max_payload_size  

-e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.iso.data // Must be in correct order  
if you are in build directory, can change C:\\-----PROJECT_DIRECTORY_PATH-----\build into .\Debug\oldmanandsea.exe  

### In LINUX

### Build
0. sudo modprobe usbmon
1. mkdir log
2. mkdir build
3. cd build
4. cmake ..
5. make

### Run 
0. cd build
1. lsusb <br/>
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub <br/>
Bus 001 Device 002: ID 80ee:0021 VirtualBox USB Tablet <br/>
Bus 001 Device 004: ID 2e1a:4c01 Insta360 Insta360 Link <br/>
Bus 001 Device 005: ID 046d:085e Logitech, Inc. BRIO Ultra HD Webcam <br/>
Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub <br/>
2. sudo ./uvc_frame_detector -in usbmon1 -bs 41536 -bn 1 -dn 4 -fw 1280 -fh 720 -fps 30 -ff mjpeg -mf 16777216 -v 2 -lv 1 <br/>

-interface <br/>
usbmon0 for all usb transfers, usbmon1 only for usb bus 1, usbmon2 only for usb bus 2 ...<br/>
-bus number, device number <br/>
can find by lsusb <br/>
-frame width, frame height, frame per second, frame format <br/>
needs to be designated by user <br/>
some of the tests will not be played <br/>
currently auto set to 1280x720 <br/>
-verbose, verbose log<br/>
setting up levels of printings in screen and log 

3. run any camera appliation, guvcview, cheese, vlc, opencv ... e.g.) guvcview

### See Usage
0. sudo ./moncapler

### Example without camera
0. ./example
To test validation algorithm, can test with colour bar screen of jpeg<br/>
Build with cmake above<br/>

### Test Codes
0. ./valid_test
Build with cmake above<br/>


## How It Works

It brings the data of the usbmon (usbmonitor)<br/>
So you will have to choose which usbmon to use<br/>
And then it filters the data by looking at urb header and find specific device's urb<br/>
When it is found, devide them into IN OUT, CONTROL BULK ISO<br/>
And recombine urb block with each algorithms to have complete transfer data,<br/>
starting with payload header<br/>
Then another thread validate the transferred data by looking at the headers<br/>
When validation is finished, transfers are combined into a frame<br/>
When it is done, fps are calculated and shows whether frame has errors<br/>
Error statistics will be given<br/>
Below is one of them <br/>

# uvcperf

Modified Libuvc stream file to get the live stream from the camera and validate the data at the final stage  

## Usage
Same building method written above  
Changed libuvc streaming part and made valiate statistics for streaming  
ONLY ON LINUX environment  

## RUN
0. cd build
1. sudo ./uvcperf  
ctrl + c to end  

# UML Diagram
![uml_img](./documents/dark_uvc_frame_detector.drawio.png)

# Example
![example_0](./documents/034_0.jpg)
![example_1](./documents/034_1.jpg)

![usage](./documents/usage.gif)

### TODO
Test Linux version <br/>
