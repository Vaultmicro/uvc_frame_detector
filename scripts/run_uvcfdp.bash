#!/bin/bash
read -p "Please enter the path to the pcapng file: " filepath

find_dev_num="tshark -r \"$filepath\" -s 0 -T fields \
-e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata -e usb.iso.data -e usbvideo.format.index \
-e usbvideo.frame.index -e usbvideo.frame.width -e usbvideo.frame.height -e usbvideo.streaming.descriptorSubType \
-e usbvideo.frame.interval -e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize \
-e usb.device_address -e usbvideo.format.numFrameDescriptors -e usb.idVendor -e usb.idProduct -E separator=\; -Y \"usb.transfer_type == 2\""

echo "Executing command to find device numbers:"
echo $find_dev_num
eval $find_dev_num

read -p "Please select device address: " devadd
echo "(0x80 is selected by default e.g. 1 for 0x81)"
read -p "Please select endpoint address: " endpointadd

command="tshark -r \"$filepath\" -s 0 -T fields \
-e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata \
-e usb.iso.data -e usbvideo.format.index -e usbvideo.frame.index \
-e usbvideo.frame.width -e usbvideo.frame.height \
-e usbvideo.streaming.descriptorSubType -e usbvideo.frame.interval \
-e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize -e usbvideo.format.numFrameDescriptors \
-e usbvideo.probe.clockFrequency -e usb.idVendor -e usb.idProduct \
-E separator=\; -Y \"(usb.device_address == $devadd) && (usb.endpoint_address == 0x80 || usb.endpoint_address == 0x8$endpointadd)\" -Q | ../build/source/uvcfd"

echo "Executing final command:"
echo $command
eval $command
