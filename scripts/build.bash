#!/bin/bash

sudo modprobe usbmon


cd ..
mkdir build
cd build
cmake ..
make
cd ../scripts