# Ameba Image Flash Tool

A script to flash firmware images to AmebaZ2 and AmebaD boards using Ameba_ImageTool.

## Usage

    ./flash.sh <Chipset> <Device Port> <Path> [address]

### Arguments

| Argument     | Description                                                                 |
|-------------|-----------------------------------------------------------------------------|
| Chipset      | Target chipset. Valid options: AmebaZ2 or AmebaD                            |
| Device Port  | Serial device port, e.g., /dev/ttyUSB0                                      |
| Path         | Path to the image file (AmebaZ2) or GCC-RELEASE directory (AmebaD)         |
| address      | (Optional) Flash start address in hex. Default: 0x08000000                  |

**Examples**
```
# Flash a single image to AmebaZ2 at default address
./flash_ameba.sh AmebaZ2 /dev/ttyUSB0 path/to/image.bin

# Flash a single image to AmebaZ2 at a custom address
./flash_ameba.sh AmebaZ2 /dev/ttyUSB0 path/to/factorydata.bin 0x083FF000

# Flash an AmebaD board using GCC-RELEASE directory
./flash_ameba.sh AmebaD /dev/ttyUSB0 path/to/GCC-RELEASE

# Flash an AmebaD board with a specific image and custom address
./flash_ameba.sh AmebaD /dev/ttyUSB0 path/to/factorydata.bin 0x083FF000
```

**Notes**

- For AmebaD without specifying an address, the Path must point to a GCC-RELEASE directory. The script will automatically combine required images.
- The script automatically sets the baudrate to 1500000.
- Logs and temporary combined images are cleaned up after flashing.

### Flashing Instructions

1. Run the script as shown above.
2. The script will display the settings. Verify the chipset, device port, and image paths.
3. Enter UART DOWNLOAD mode on your board:
    - Push the UART DOWNLOAD button and hold it.
    - Re-power the board or press the Reset button.
    - Release the UART DOWNLOAD button.
4. Press Enter in the script to continue flashing.
5. Once complete, logs and temporary files are cleaned automatically.