# Usage of 1-10 image tool for Ameba

1. Unzip Ameba_ImageTool_Linux.zip and make it executable
    - unzip Ameba_ImageTool_Linux.zip
    - chmod +x Ameba_ImageTool_Linux


2. Check tool usage
    - sudo ./Ameba_ImageTool_Linux -help


3. Set AmebaZ2 chip
	- sudo ./Ameba_ImageTool_Linux -set chip AmebaZ2

4. Scan device port

Before using this command, please set Ameba platform to download mode.

Enter into UART_DOWNLOAD mode.

	a) Push the UART DOWNLOAD button and keep it pressed.
	b) Re-power on the board or press the Reset button.
	c) Release the UART DOWNLOAD button.
	Now, Ameba board gets into UART_DOWNLOAD mode and is ready to receive data.

	- sudo ./Ameba_ImageTool_Linux -scan device
(Automatically add the uart device to config, so you can skip step #5)

5. Add Uart Device
	- sudo ./Ameba_ImageTool_Linux -add device /dev/ttyXXX

6. Set Baudrate
	- sudo ./Ameba_ImageTool_Linux -set baudrate 1500000

7.[No need for AmebaZ2] Combine Ameba three images
	- sudo ./Ameba_ImageTool_Linux -combine <file1> <offset1> <file2> <offset2> ...

	- sudo ./Ameba_ImageTool_Linux -combine km0_boot_all.bin 0x0000 km4_boot_all.bin 0x4000 km0_km4_image2.bin 0x6000
       (Tool will generate the Image_All.bin after combine command.)


8. Set image file path
    - sudo ./Ameba_ImageTool_Linux -set image /<Current_PATH>/flash_is.bin


9. Set image address
    - sudo ./Ameba_ImageTool_Linux -set address 0x08000000


10. Final check
	- sudo ./Ameba_ImageTool_Linux -show

Current Settings

	CPU      : AmebaZ2
	UART     : 1500000 bps parity:N flow control:False
	FW NAME  : /<Current_PATH>/flash_is.bin
	FW ADDR  : 0x8000000
	FW LEN   : 0
	OPTS     : keep data    :	True
		   keep wifi cal:	True
		   verification :	False
		   reset device :	False
		   factory mode :	False
		   save log     :	True
Port Settings

	PORT 0: active:True device:/dev/tty.XXX
	PORT 1: active:False device:NONE
	PORT 2: active:False device:NONE
	PORT 3: active:False device:NONE
	PORT 4: active:False device:NONE
	PORT 5: active:False device:NONE
	PORT 6: active:False device:NONE
	PORT 7: active:False device:NONE
	PORT 8: active:False device:NONE
	PORT 9: active:False device:NONE


11. Download image to AmebaZ2
(Notice : please set AmebaZ2 board to download mode before download command)
	- sudo ./Ameba_ImageTool_Linux -download

log file = log_2021_03_12_13_58_46.txt
[100, 0, 0, 0, 0, 0, 0, 0, 0, 0]command finish

status: ['OK 58.4s', 'Off', 'Off', 'Off', 'Off', 'Off', 'Off', 'Off', 'Off', 'Off']
close log file

12. Check finish log

# Using flashing script for easy flashing

1. Identify your device USB port

2. Run the flashing script
 
        ./flash.sh /dev/ttyUSB0 /home/dev/ameba-rtos-z2/project/realtek_amebaz2_v0_example/GCC-RELEASE/application_is/Debug/bin/flash_is.bin
        ./flash.sh deviceUSBPort outputFileDirectory
	
3. Alternatively, you may use the flash script to flash to a specific address

        ./flash.sh /dev/ttyUSB0 image.bin 0x081B7000
        address field is optional
 
4. Put the board to download mode when prompted, then press enter

