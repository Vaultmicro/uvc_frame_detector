# Make sure to install wireshark in the default location
# And make sure to move USBPCapCMD.exe to /extcap/wireshark/USBPcapCMD.exe in wireshark program files folder

$filepath = Read-Host "Please enter the path to the pcapng file"

$find_dev_num = '"C:\Program Files\Wireshark\tshark.exe" -r "'+ $filepath +'" -s 0 -T fields ' +
'-e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata -e usb.iso.data -e usbvideo.format.index '+
'-e usbvideo.frame.index -e usbvideo.frame.width -e usbvideo.frame.height -e usbvideo.streaming.descriptorSubType ' +
'-e usbvideo.frame.interval -e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize ' +
'-e usb.device_address -e usbvideo.format.numFrameDescriptors -e usb.idVendor -e usb.idProduct -E separator=; -Y "usb.transfer_type == 2"'

Write-Output $find_dev_num
cmd /c $find_dev_num

$devadd = Read-Host "Please select device address"
Write-Host "(0x80 is selected by default e.g. 1 for 0x81)"
$endpointadd = Read-Host "Please select endpoint address"

$command = '"C:\Program Files\Wireshark\tshark.exe" -r "'+ $filepath +'" -s 0 -T fields ' +
    '-e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata ' +
    '-e usb.iso.data -e usbvideo.format.index -e usbvideo.frame.index ' +
    '-e usbvideo.frame.width -e usbvideo.frame.height ' +
    '-e usbvideo.streaming.descriptorSubType -e usbvideo.frame.interval ' +
    '-e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize -e usbvideo.format.numFrameDescriptors ' +
    '-E separator=; -Y "usb.device_address == ' + $devadd + '" -Q | .\build\release\oldmanandsea_g.exe'


$command = '"C:\Program Files\Wireshark\tshark.exe" -r "'+ $filepath +'" -s 0 -T fields ' +
    '-e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata ' +
    '-e usb.iso.data -e usbvideo.format.index -e usbvideo.frame.index ' +
    '-e usbvideo.frame.width -e usbvideo.frame.height ' +
    '-e usbvideo.streaming.descriptorSubType -e usbvideo.frame.interval ' +
    '-e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize -e usbvideo.format.numFrameDescriptors ' +
    '-E separator=; -Y "usb.device_address == ' + $devadd + ' && ' +
    'usb.endpoint_address == 0x80 || usb.endpoint_address == 0x8' + $endpointadd + '" -Q | .\build\release\oldmanandsea_g.exe"'

Write-Output $commands
cmd /c $command


