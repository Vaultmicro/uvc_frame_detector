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
2. sudo ./moncapler -in usbmon1 -bs 41536 -bn 1 -dn 4
3. run any camera appliation, cheese, vlc, opencv ... e.g.) cheese

### See Usage
0. sudo ./moncapler

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

### Test Codes
Build with cmake above<br/>


### TODO
make the setup data save variables to find setcur and getcur<br/>
if scr available, make that with graph together with chorono list in moncapler.cpp <br/>
believe the process, there is no error till now<br/>