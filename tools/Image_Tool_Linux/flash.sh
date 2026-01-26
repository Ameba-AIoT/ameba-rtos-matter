#!/bin/bash

usage() {
    cat << EOF
Usage: $0 <Chipset> <Device Port> <Path> [address]

Arguments:
  Chipset      Target chipset. Valid options: AmebaZ2 | AmebaD
  Device Port  Serial device port, e.g., /dev/ttyUSB0
  Path         Path to the image file (AmebaZ2) or GCC-RELEASE directory (AmebaD)
  address      (Optional) Flash start address in hex. Default: 0x08000000

Examples:
  $0 AmebaZ2 /dev/ttyUSB0 path/to/image.bin
  $0 AmebaZ2 /dev/ttyUSB0 path/to/factorydata.bin 0x083FF000
  $0 AmebaD /dev/ttyUSB0 path/to/GCC-RELEASE
  $0 AmebaD /dev/ttyUSB0 path/to/factorydata.bin 0x083FF000

Notes:
  - For AmebaD without specifying an address, 'Path' must point to a 'GCC-RELEASE' directory.
  - Ensure the UART DOWNLOAD mode is activated on the board before flashing.
EOF
    exit 1
}

# Check for help flag
if [[ "$1" == "-h" || "$1" == "--help" ]]; then
    usage
fi

# Check arguments
if [[ $# -lt 3 || $# -gt 4 ]]; then
    echo "Error: Invalid number of arguments."
    usage
fi

if [[ -e "./Ameba_ImageTool" ]]; then
    chmod 777 ./Ameba_ImageTool
else
    echo "Error: Ameba_ImageTool not found in current directory."
    exit 1
fi

CHIPSET=$1
DEVICE_PORT=$2
IMAGE_PATH=$3
ADDRESS=${4:-0x08000000}  # default address is 0x0 if not provided

# Check if the path exists
if [[ ! -e "$IMAGE_PATH" ]]; then
    echo "Error: $IMAGE_PATH doesn't exist."
    exit 1
fi

chmod 777 ./Ameba_ImageTool
./Ameba_ImageTool -set chip $CHIPSET
./Ameba_ImageTool -add device $DEVICE_PORT
./Ameba_ImageTool -set baudrate 1500000
./Ameba_ImageTool -set address $ADDRESS

case "$CHIPSET" in
    AmebaZ2)
        echo "Chipset is AmebaZ2"
        ./Ameba_ImageTool -set image $IMAGE_PATH
        ;;
    AmebaD)
        echo "Chipset is AmebaD"
        echo "Image path : $IMAGE_PATH"
        if [[ -n "$4" ]]; then
            ./Ameba_ImageTool -set image $IMAGE_PATH
        else
            IMAGE_PATH="${IMAGE_PATH%/}"
            if [[ "${IMAGE_PATH##*/}" != "GCC-RELEASE" ]]; then
                echo "Error: IMAGE_PATH must be a 'GCC-RELEASE' directory"
                exit 1
            fi
            KM0_BOOT_ALL_IMG=$IMAGE_PATH/project_lp/asdk/image/km0_boot_all.bin
            KM4_BOOT_ALL_IMG=$IMAGE_PATH/project_hp/asdk/image/km4_boot_all.bin
            KM0_KM4_IMG=$IMAGE_PATH/project_hp/asdk/image/km0_km4_image2.bin

            for file in "$KM0_BOOT_ALL_IMG" "$KM4_BOOT_ALL_IMG" "$KM0_KM4_IMG"; do
                if [[ ! -f "$file" ]]; then
                    echo "Error: Required file '$file' does not exist."
                    exit 1
                fi
            done
        ./Ameba_ImageTool -combine $KM0_BOOT_ALL_IMG 0x0000 $KM4_BOOT_ALL_IMG 0x4000 $KM0_KM4_IMG 0x6000
        ./Ameba_ImageTool -set image $PWD/Image_All.bin
        fi
        ;;
    *)
        echo "Error: Invalid chipset '$CHIPSET'"
        echo "Valid options: AmebaZ2 | AmebaD"
        exit 1
        ;;
esac

./Ameba_ImageTool -show
read -p "Check if the settings are correct, then enter UART DOWNLOAD mode
1) Push the UART DOWNLOAD button and keep it pressed.
2) Re-power on the board or press the Reset button.
3) Release the UART DOWNLOAD button.
Once entered UART Download mode, press Enter to continue"
./Ameba_ImageTool -download
rm -rf log* Image_All.bin*