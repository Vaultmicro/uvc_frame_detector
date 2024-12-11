import usb.core
import usb.util
import usb.backend.libusb1
import subprocess
import sys

def find_uvc_video_stream_interfaces():
    # 모든 USB 디바이스 열거
    backend = usb.backend.libusb1.get_backend(find_library=lambda x: r"./libusb-1.0.dll")
    devices = usb.core.find(find_all=True, backend=backend)
    uvc_devices = []  # 디바이스 주소와 엔드포인트 정보를 저장할 리스트

    for device in devices:
        try:
            # Association Descriptor를 확인하려면 Configuration 및 Interface를 순회
            for cfg in device:
                for intf in cfg:
                    # UVC (Video) Association Descriptor 확인
                    if intf.bInterfaceClass == 0x0E:  # Video class
                        if intf.bInterfaceSubClass == 0x02:  # Video Streaming
                            bFoundEP = False
                            for ep in intf:
                                # 정보 출력 및 저장
                                print(f"\nDevice Address: {device.address}")
                                print(f"Vendor ID: {hex(device.idVendor)}, Product ID: {hex(device.idProduct)}")

                                print(f"  Video Streaming Interface Found (Interface: {intf.bInterfaceNumber}) alt: {intf.bAlternateSetting}")
                                print(f"    Endpoint Address: {hex(ep.bEndpointAddress)}")
                                print(f"    Attributes: {hex(ep.bmAttributes)}")
                                print(f"    Max Packet Size: {ep.wMaxPacketSize}")
                                print(f"    Interval: {ep.bInterval}")
                                bFoundEP = True

                                # 디바이스 주소와 엔드포인트 정보를 저장
                                uvc_devices.append((device.address, ep.bEndpointAddress))

                            if bFoundEP:
                                break

        except usb.core.USBError as e:
            print(f"Error: {e}")
            continue

    return uvc_devices


def create_and_run_tshark_command(uvc_devices):
    if not uvc_devices:
        print("No UVC devices found.")
        return

    # 저장된 디바이스 및 엔드포인트 정보를 출력
    print("\nAvailable UVC Devices:")
    for idx, (dev_addr, ep_addr) in enumerate(uvc_devices):
        print(f"{idx}: Device Address: {dev_addr}, Endpoint Address: {hex(ep_addr)}")

    # 사용자 선택
    dev_index = int(input("\nPlease select a device index: "))
    if dev_index < 0 or dev_index >= len(uvc_devices):
        print("Invalid index.")
        return

    dev_address, endpoint_address = uvc_devices[dev_index]
    print(f"Selected Device Address: {dev_address}, Endpoint Address: {hex(endpoint_address)}")

    # 기본 엔드포인트 주소가 0x80로 가정
    default_endpoint = 0x80

    # Tshark 명령 생성
    tshark_path = r'"C:\Program Files\Wireshark\tshark"'
    usb_interface = r"\\.\USBPcap1"
    filter_expression = f'(usb.device_address == {dev_address}) && (usb.endpoint_address == {hex(default_endpoint)} || usb.endpoint_address == {hex(endpoint_address)})'
    command = (
        f'{tshark_path} -i {usb_interface} -s 0 -T fields '
        f' -e usb.transfer_type -e frame.time_epoch -e frame.len -e usb.capdata '
        f' -e usb.iso.data -e usbvideo.format.index -e usbvideo.frame.index '
        f' -e usbvideo.frame.width -e usbvideo.frame.height '
        f' -e usbvideo.streaming.descriptorSubType -e usbvideo.frame.interval '
        f' -e usbvideo.probe.maxVideoFrameSize -e usbvideo.probe.maxPayloadTransferSize -e usbvideo.format.numFrameDescriptors -e usbvideo.probe.clockFrequency '
        f' -e usb.idVendor -e usb.idProduct '
        f' -E separator=; '
        f' -Y "{filter_expression}" -Q'
    )

    print("\nGenerated Command:")
    print(command)

    # 명령 실행 및 파이프를 통해 다른 subprocess에 전달
    return subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
    


if __name__ == "__main__":
    uvc_devices = find_uvc_video_stream_interfaces()
    tshark_process = create_and_run_tshark_command(uvc_devices)
    # uvcfd_command from args
    if len(sys.argv) > 1:
        uvcfd_command = sys.argv[1]
    else:
        # TODO: change default uvcfd_command
        uvcfd_command = r"build\source\Debug\uvcfd.exe"

    subprocess.run(uvcfd_command, shell=True, stdin=tshark_process.stdout)
    tshark_process.stdout.close()
    tshark_process.wait()
