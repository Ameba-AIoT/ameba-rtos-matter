# Matter (previously CHIP) on AmebaDplus

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

    git clone https://github.com/mikaelajiwidodo/ameba-rtos.git -b ameba-rtos-v1.0/matter/release/v1.4

To check out Matter repository:

    git clone https://github.com/project-chip/connectedhomeip.git
    
Make sure ameba-rtos and connectedhomeip are on the same directory level

    dev/
    ├── ameba-rtos
    └── connectedhomeip

## Set Matter Build Environment

    > Find more details to setup linux build environment
    > https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/BUILDING.md

    cd connectedhomeip

	git switch v1.4-branch

    git submodule sync

    git submodule update --init --recursive

    source scripts/bootstrap.sh

    source scripts/activate.sh

## Set Ameba Build Environment

Navigate to the `ameba-rtos` directory:

    cd ameba-rtos

    git submodule update --init --recursive

## Build CHIP library by GN and Final Firmware

In this context, we will demostrate building of all-clusters-app.

### Make Matter Libraries

Navigate to the `amebadplus_gcc_project` directory:

    cd ameba-rtos/amebadplus_gcc_project/

Menuconfig for matter:
- To enable Matter, select `MENUCONFIG FOR KM4 CONFIG`, then select `Matter Config`, and enable `Enable Matter`.
- Change mbedtls version to matter. Under `MENUCONFIG FOR KM4 CONFIG`, select `SSL Config`, then enable `Matter MBEDTLS Enable`
- If you want to support Matter BLE, under `CONFIG BT`, select `BLE_Matter_Adapter` and save the configuration.

```
make menuconfig
```

Navigate to the `project_km4` directory:

    cd ameba-rtos/amebadplus_gcc_project/project_km4/

Start building the Matter libraries with:

    make -C asdk all_clusters

### Make project_km4

Ensure the same menuconfig settings as described in `Make Matter Libraries` and continue building the `project_km4` images.

    cd ameba-rtos/amebadplus_gcc_project/project_km4/

Build `project_km4`:

    make EXAMPLE=chiptest

### Make project_km0

Navigate to the `project_km0` directory:

    cd ameba-rtos/amebadplus_gcc_project/project_km0/

Build the `project_km0`:

    make all

### Clean Ameba Matter libraries and application

    make clean

## Flash Image

The generated image is found in the ameba-rtos sdk `ameba-rtos/amebadplus_gcc_project/project_km4/asdk/image`, both `km4_boot_all.bin` and `km0_km4_app.bin` will be flashed.

- Find more detail in [ameba-rtos/README.md](https://github.com/Ameba-AIoT/ameba-rtos/blob/master/README.md#flashing)

