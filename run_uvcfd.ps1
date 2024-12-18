# Make sure to install wireshark in the default location
# And make sure to move USBPCapCMD.exe to /extcap/wireshark/USBPcapCMD.exe in wireshark program files folder

cmd /c '"C:\Program Files\Wireshark\extcap\wireshark\USBPcapCMD.exe"  --extcap-interface \\.\USBPcap1 --extcap-config'

$devadd = Read-Host "Please select device address"
Write-Host "(0x80 is selected by default e.g. 1 for 0x81)"
$endpointadd = Read-Host "Please select endpoint address"


$command = '"C:\Program Files\Wireshark\tshark" -i \\.\USBPcap1 -s 0 -T fields ' +
    '-e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata ' +
    '-e usb.iso.data -e usbvideo.format.index -e usbvideo.frame.index ' +
    '-e usbvideo.frame.width -e usbvideo.frame.height ' +
    '-e usbvideo.streaming.descriptorSubType -e usbvideo.frame.interval ' +
    '-e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize -e usbvideo.format.numFrameDescriptors -e usbvideo.probe.clockFrequency ' +
    '-e usb.idVendor -e usb.idProduct ' +
    '-E separator=; -Y "(usb.device_address == ' + $devadd + ') && (' +
    'usb.endpoint_address == 0x80 || usb.endpoint_address == 0x8' + $endpointadd + ')" -Q | .\build\source\release\uvcfd.exe"'

Write-Output $command

cmd /c $command
