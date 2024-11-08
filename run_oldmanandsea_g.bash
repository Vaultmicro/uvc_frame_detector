#!/bin/bash

echo "USB Device Lists:"
lsusb

read -p "Please select device address: " devadd

command="sudo tshark -i usbmon1 -s 0 -T fields \
    -e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata \
    -e usb.iso.data -e usbvideo.format.index -e usbvideo.frame.index \
    -e usbvideo.frame.width -e usbvideo.frame.height \
    -e usbvideo.streaming.descriptorSubType -e usbvideo.frame.interval \
    -e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize -e usbvideo.format.numFrameDescriptors \
    -E separator=\; -Y \"usb.device_address == $devadd\" -Q | ./build/oldmanandsea_g"

echo "$command"

eval "$command"
