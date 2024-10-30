# Make sure to install wireshark in the default location
# And make sure to move USBPCapCMD.exe to /extcap/wireshark/USBPcapCMD.exe in wireshark program files folder

param(
    [int]$devadd
)

$command = '"C:\Program Files\Wireshark\tshark" -i \\.\USBPcap1 -T fields ' +
    '-e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata ' +
    '-e usb.iso.data -e usbvideo.format.index -e usbvideo.frame.index ' +
    '-e usbvideo.frame.width -e usbvideo.frame.height ' +
    '-e usbvideo.streaming.descriptorSubType -e usbvideo.frame.interval ' +
    '-e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize ' +
    '-E separator=; -Y "usb.device_address == ' + $devadd + '" -Q | .\oldmanandsea_g.exe'

cmd /c $command
