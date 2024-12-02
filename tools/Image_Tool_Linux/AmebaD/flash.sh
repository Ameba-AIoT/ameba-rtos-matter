#!/usr/bin/env bash

# Flash firmware usage
# ./flash.sh <Device Port> <Path to km0_boot_all.bin> <Path to km4_boot_all.bin> <Path to km0_km4_image2.bin> <address[optional]>
# Flash facotrydata binary usage
# ./flash.sh <Device Port> <Path to ameba_factory.bin> <address>

if [ -z "$1" ]
  then
    echo "No device port provided"
    exit
fi

if [ -z "$2" ]
  then
    echo "No output directory provided"
    exit
fi

if [ "$#" -eq 3 ]; then
    FACTORY_DATA_IMG=$2

    sudo ./AmebaD_ImageTool -add device $1
    sudo ./AmebaD_ImageTool -set baudrate 1500000
    sudo ./AmebaD_ImageTool -set image $FACTORY_DATA_IMG
    if [ -n "$3" ]
    then
        echo "Flashing to address $3"
        sudo ./AmebaD_ImageTool -set address $3
    else
        # if no address is given, use default address 0x08000000
        echo "Flashing to address 0x08000000"
        sudo ./AmebaD_ImageTool -set address 0x08000000
    fi
    sudo ./AmebaD_ImageTool -show
    read -p "Check if the settings are correct, then enter UART DOWNLOAD mode
    1) Push the UART DOWNLOAD button and keep it pressed.
    2) Re-power on the board or press the Reset button.
    3) Release the UART DOWNLOAD button.
    Once AmebaD entered UART Download mode, press Enter to continue"
    sudo ./AmebaD_ImageTool -download
else
    SDK_ROOT_DIR=$PWD/../../..
    KM0_BOOT_ALL_IMG=$2
    KM4_BOOT_ALL_IMG=$3
    KM0_KM4_IMG=$4
    # sudo ./AmebaD_ImageTool -set chip AmebaD
    rm -rf log* Image_All.bin*
    sudo ./AmebaD_ImageTool -add device $1
    sudo ./AmebaD_ImageTool -set baudrate 1500000
    sudo ./AmebaD_ImageTool -combine $KM0_BOOT_ALL_IMG 0x0000 $KM4_BOOT_ALL_IMG 0x4000 $KM0_KM4_IMG 0x6000
    sudo ./AmebaD_ImageTool -set image $PWD/Image_All.bin
    if [ -n "$5" ]
    then
        echo "Flashing to address $5"
        sudo ./AmebaD_ImageTool -set address $5
    else
        # if no address is given, use default address 0x08000000
        echo "Flashing to address 0x08000000"
        sudo ./AmebaD_ImageTool -set address 0x08000000
    fi
    sudo ./AmebaD_ImageTool -show
    read -p "Check if the settings are correct, then enter UART DOWNLOAD mode
    1) Push the UART DOWNLOAD button and keep it pressed.
    2) Re-power on the board or press the Reset button.
    3) Release the UART DOWNLOAD button.
    Once AmebaD entered UART Download mode, press Enter to continue"
    sudo ./AmebaD_ImageTool -download
fi

