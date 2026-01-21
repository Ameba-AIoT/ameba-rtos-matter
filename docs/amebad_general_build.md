# Matter (previously CHIP) on AmebaD

- [Get Ameba SDK & Matter SDK](#get-ameba-sdk--matter-sdk)
- [Set Matter Build Environment](#set-matter-build-environment)
- [Set Ameba Build Environment](#set-ameba-build-environment)
- [Build CHIP library by GN and Final Firmware](#build-chip-library-by-gn-and-final-firmware)
- [Flash Image](#flash-image)

## Get Ameba SDK & Matter SDK

    Tested on Ubuntu 22.04 or above

Create and enter new directory

    mkdir dev
    cd dev

To check out this repository:

    git clone https://github.com/ambiot/ambd_matter.git

To check out Matter repository:

    git clone https://github.com/project-chip/connectedhomeip.git
    
Make sure ambd_matter and connectedhomeip are on the same directory level

    dev/
    ├── ambd_matter
    └── connectedhomeip

## Set Matter Build Environment

    > Find more details to setup linux build environment
    > https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/BUILDING.md

    cd connectedhomeip

    git submodule sync

    git submodule update --init --recursive

    source scripts/bootstrap.sh

    source scripts/activate.sh

## Set Ameba Build Environment

Navigate to the `ambd_matter` directory:

    cd ambd_matter

    git submodule update --init --recursive

## Build CHIP library by GN and Final Firmware

In this context, we will demostrate building of all-clusters-app.

### Make project_lp

Navigate to the `project_lp` directory:

    cd ambd_matter/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp/

Enter menuconfig to enable Matter configurations. Select `Enable Matter` under `Matter Config` and save the configuration when done.

    make menuconfig

Build the `project_lp`:

    make all

### Make Matter Libraries

Navigate to the `project_hp` directory:

    cd ambd_matter/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/

Ensure `Enable Matter` is selected under `Matter Config`.
If you want to support Matter BLE, under `CONFIG BT`, select `BT_Matter_Adapter` and save the configuration.

    make menuconfig

Start building the Matter libraries with:

    make -C asdk all_clusters

### Make project_hp

Ensure the same menuconfig settings as described in `Make Matter Libraries` and continue building the `project_hp` images.

    cd ambd_matter/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/

Build `project_hp`:

    make all

### Clean Ameba Matter libraries and application

    make clean

## Flash Image

The generated image is found in the ambd_matter sdk `project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp`

Follow the Image Tool [README](../tools/Image_Tool_Linux/README.md)
