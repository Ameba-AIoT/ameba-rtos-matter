# Matter (previously CHIP) on AmebaLite

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

    chmod u+x matter_setup.sh

    ./matter_setup.sh ameba-rtos

## Build CHIP library by GN and Final Firmware

In this context, we will demostrate building of all-clusters-app.

### Make Matter Libraries

Navigate to the `amebalite_gcc_project` directory:

    cd ameba-rtos/amebalite_gcc_project/

Menuconfig for matter:
- To enable Matter, select `MENUCONFIG FOR KM4 CONFIG`, then select `Matter Config`, and enable `Enable Matter`.
- Change mbedtls version to matter. Under `MENUCONFIG FOR KM4 CONFIG`, select `SSL Config`, then enable `Matter MBEDTLS Enable`
- If you want to support Matter BLE, under `CONFIG BT`, enable `BT Example Demo`, then select `BLE_Matter_Adapter`, and save the configuration.
- If you want to support [Matter ESF](matter_commissioning_and_control_guide.md#enable-matter-esf), under `Matter Config`, enable `Enable Matter Terms and Condition`

```
make menuconfig
```

Navigate to the `project_km4` directory:

    cd ameba-rtos/amebalite_gcc_project/project_km4/

Start building the Matter libraries with:

    make -C asdk all_clusters

### Make Final Firmware

#### Make all projects in one go

Ensure the same menuconfig settings as described in `Make Matter Libraries`and continue building the final firmware.

    cd ameba-rtos/amebalite_gcc_project/

Build the final firmware:

    make all MATTER_EXAMPLE=chiptest

#### Make each project seperately

##### Make project_km4

Ensure the same menuconfig settings as described in `Make Matter Libraries` and continue building the `project_km4` images.

    cd ameba-rtos/amebalite_gcc_project/project_km4/

Build `project_km4`:

    make all MATTER_EXAMPLE=chiptest

##### Make project_kr4

Navigate to the `project_kr4` directory:

    cd ameba-rtos/amebalite_gcc_project/project_kr4/

Build the `project_kr4`:

    make all

### Clean Ameba Matter libraries and application

    make clean

## Flash Image

The generated image is found in the ameba-rtos sdk `ameba-rtos/amebalite_gcc_project/project_km4/asdk/image`, both `km4_boot_all.bin` and `kr4_km4_app.bin` will be flashed.

- Find more detail in [ameba-rtos/README.md](https://github.com/Ameba-AIoT/ameba-rtos/blob/master/README.md#flashing)

