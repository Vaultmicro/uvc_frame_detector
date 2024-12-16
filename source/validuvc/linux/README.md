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


# uvcperf

Modified Libuvc stream file to get the live stream from the camera and validate the data at the final stage  

Same building method written above  
Changed libuvc streaming part and made valiate statistics for streaming  
ONLY ON LINUX environment  

0. cd build
1. sudo ./uvcperf  
ctrl + c to end  


## How It Works

It brings the data of the usbmon (usbmonitor) <br/>
So you will have to choose which usbmon to use<br/>
And then it filters the data by looking at urb header and find specific device's urb<br/>
When it is found, devide them into in out, control bulk iso<br/>
And recombine urb block with each algorithms to have complete transfer data,<br/>
starting with payload header<br/>
Then another thread validate the transferred data by looking at the headers<br/>
When validation is finished, transfers are combined into a frame<br/>
When it is done, fps are calculated and shows whether frame has errors<br/>
Error statistics will be given<br/>
