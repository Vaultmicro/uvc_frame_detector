#!/bin/bash

echo "USB Device Lists:"
lsusb

read -p "Please select device address: " devadd
echo "(0x80 is selected by default e.g. 1 for 0x81)"
read -p "Please select endpoint address: " endpointadd

command="tshark -i usbmon1 -s 0 -T fields \
-e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata \
-e usb.iso.data -e usbvideo.format.index -e usbvideo.frame.index \
-e usbvideo.frame.width -e usbvideo.frame.height \
-e usbvideo.streaming.descriptorSubType -e usbvideo.frame.interval \
-e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize -e usbvideo.format.numFrameDescriptors \
-e usbvideo.probe.clockFrequency -e usb.idVendor -e usb.idProduct \
-E separator=\; -Y \"(usb.device_address == $devadd) && (usb.endpoint_address == 0x80 || usb.endpoint_address == 0x8$endpointadd)\" -Q | ./build/source/uvcfd"

echo "Executing final command:"
echo $command
eval $command
