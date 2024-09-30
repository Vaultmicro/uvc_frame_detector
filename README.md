# uvc_frame_detector


## Usage

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
2. sudo ./moncapler -in usbmon1 -bs 41536 -bn 1 -dn 4 -fw 1280 -fh 720 -fps 30 -ff mjpeg -v 2 -lv 1 <br/>
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

### Example Result
Data Length: 13424<br/>
Finish the transfer<br/>
Processing packet of size: 177264<br/>
UVC payload header is valid.<br/>
Payload is valid.<br/>
Data Length: 14169<br/>
Finish the transfer<br/>
Data Length: 13795<br/>
Finish the transfer<br/>
Processing packet of size: 2602841<br/>
UVC payload header is valid.<br/>
Payload is valid.<br/>
Processing packet of size: 79331<br/>
Invalid UVC payload header: Error bit is set.<br/>
Invalid packet detected<br/>

...<br/>
Capture Statistics:<br/>


Total Packets received in usbmon: 6982<br/>
Total Packets dropped by system buffer: 10122<br/>
Total Packets dropped by interface: 0<br/>


Total Packets counted in usbmon by application: 6952<br/>
Total Packet Length bytes: 40249996<br/>
Total Captured Packet Length bytes: 40249996<br/>


Filtered Packets (busnum=1, devnum=4): 6940<br/>
Filtered Packet Length bytes (busnum=1, devnum=4): 40249115<br/>
Filtered Captured Packet Length bytes (busnum=1, devnum=4): 40249115<br/>


If Filtered Packet Length bytes and Filtered Captured Packet Length bytes are different, then test enviroment is maybe unstable <br/>
Starting camera after running code may cause difference in this value, ignore it if think unnecessary <br/>

### Saved log file
To compare raw data with result, go to log dir<br/>

## Project Tree

.
├── build  
│   ├── bin  
│   ├── CMakeCache.txt  
│   ├── CMakeFiles  
│   ├── cmake_install.cmake  
│   ├── _deps  
│   ├── example  
│   ├── frames_log.txt  
│   ├── frame_test_bulk  
│   ├── frame_test_iso  
│   ├── lib  
│   ├── Makefile  
│   ├── moncapler  
│   └── valid_test  
├── CMakeLists.txt  
├── examples  
│   ├── smpte.cpp  
│   └── smpte.txt  
├── include  
│   ├── pcap  
│   ├── pcap-bpf.h  
│   ├── pcap.h  
│   ├── pcap-namedb.h  
│   ├── utils  
│   └── validuvc  
├── library  
│   ├── libpcap.a  
│   ├── libpcap.so -> libpcap.so.0.8  
│   ├── libpcap.so.0.8 -> libpcap.so.1.10.4  
│   └── libpcap.so.1.10.4  
├── log  
│   ├── frames_log.txt  
│   └── payload_headers_log.txt  
├── README.md  
├── scripts  
│   ├── build.bash  
│   ├── clang-foramt.bash  
│   └── run.bash  
├── source  
│   ├── appraiser.cpp  
│   ├── control_config.cpp  
│   ├── logger.cpp  
│   ├── moncapler.cpp  
│   ├── uvcpheader_checker.cpp  
│   └── verbose.cpp  
└── tests  
    ├── frame_test_bulk.cpp  
    ├── frame_test_iso.cpp  
    └── valid_test.cpp  



### TODO
if scr available, make that with graph together with chorono list in moncapler.cpp <br/>
believe the process, there is no error till now<br/>