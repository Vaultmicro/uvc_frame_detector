#!/bin/bash

sudo modprobe usbmon
cd ../build
cmake ..
make
cd ../scripts