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

    git clone https://github.com/Ameba-AIoT/ameba-rtos-d.git

To check out Matter repository:

    git clone https://github.com/project-chip/connectedhomeip.git
    
Make sure ameba-rtos-d and connectedhomeip are on the same directory level

    dev/
    ├── ameba-rtos-d
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

Navigate to the `ameba-rtos-d` directory:

    cd ameba-rtos-d

    chmod u+x matter_setup.sh ; ./matter_setup.sh amebad

    cd component/common/application/matter

    git checkout release/v1.4

## Build CHIP library by GN and Final Firmware

In this context, we will demostrate building of all-clusters-app.

### Make project_lp

Navigate to the `project_lp` directory:

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp/

Build the `project_lp`:

    make all

### Make Matter Libraries

Navigate to the `project_hp` directory:

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/

Enable Matter configurations using `menuconfig`
- Navigate to `CONFIG BT`, select `Enable BT` and select `BT_Matter_Adapter`.
- In the `SSL Config` section under the `MBEDTLS version`, select `MBEDTLS_MATTER_DEFINED`.
- In the `Matter Config` section, select `Enable Matter`.

    make menuconfig

Start building the Matter libraries with:

    make -C asdk all_clusters

### Make project_hp

Ensure the same menuconfig settings as described in `Make Matter Libraries` and continue building the `project_hp` images.

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/

Build `project_hp`:

    make all

### Clean Ameba Matter libraries and application

    make clean

## Flash Image

Method 1:

- Find more detail in AN0400 Chapter 8.

Method 2 (using image tool and flashing script):

- Follow the Image Tool README in ameba-rtos-matter/tools/Image_Tool/AmebaX
