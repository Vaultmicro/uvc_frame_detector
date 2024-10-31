# Make sure to install wireshark in the default location
# And make sure to move USBPCapCMD.exe to /extcap/wireshark/USBPcapCMD.exe in wireshark program files folder

$filepath = Read-Host "Please enter the path to the pcapng file"

$find_dev_num = '"C:\Program Files\Wireshark\tshark.exe" -r "'+ $filepath +'" -T fields ' +
'-e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata -e usb.iso.data -e usbvideo.format.index '+
'-e usbvideo.frame.index -e usbvideo.frame.width -e usbvideo.frame.height -e usbvideo.streaming.descriptorSubType ' +
'-e usbvideo.frame.interval -e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize ' +
'-e usb.device_address -e usb.idVendor -e usb.idProduct -E separator=; -Y "usb.transfer_type == 2"'

Write-Output $find_dev_num
cmd /c $find_dev_num

$devadd = Read-Host "Please select device address"

$command = '"C:\Program Files\Wireshark\tshark.exe" -r "'+ $filepath +'" -T fields ' +
    '-e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata ' +
    '-e usb.iso.data -e usbvideo.format.index -e usbvideo.frame.index ' +
    '-e usbvideo.frame.width -e usbvideo.frame.height ' +
    '-e usbvideo.streaming.descriptorSubType -e usbvideo.frame.interval ' +
    '-e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize ' +
    '-E separator=; -Y "usb.device_address == ' + $devadd + '" -Q | .\build\debug\oldmanandsea_g.exe'

Write-Output $commands
cmd /c $command
