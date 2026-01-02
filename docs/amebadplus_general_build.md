# Matter (previously CHIP) on AmebaDplus

- [Ameba RTOS v1.1 SDK](#ameba-rtos-v11-sdk)
    - [Get Ameba SDK & Matter SDK](#get-ameba-sdk--matter-sdk)
    - [Set Matter Build Environment](#set-matter-build-environment)
    - [Set Ameba Build Environment](#set-ameba-build-environment)
    - [Build CHIP library by GN and Final Firmware](#build-chip-library-by-gn-and-final-firmware)
    - [Flash Image](#flash-image-using-python-script)
    - [Monitor Ameba](#Monitor-Ameba-Log-using-Python-script)
- [Ameba RTOS v1.0 SDK](#ameba-rtos-v10-sdk)
    - [Get Ameba SDK & Matter SDK](#get-ameba-sdk--matter-sdk-1)
    - [Set Matter Build Environment](#set-matter-build-environment-1)
    - [Set Ameba Build Environment](#set-ameba-build-environment-1)
    - [Build CHIP library by GN and Final Firmware](#build-chip-library-by-gn-and-final-firmware-1)
    - [Flash Image](#flash-image-using-windows-image-tool)

## Ameba RTOS v1.1 SDK

### Get Ameba SDK & Matter SDK

    Tested on Ubuntu 24.04 or above

Create and enter new directory

    mkdir dev
    cd dev

Get ameba-rtos v1.1 SDK + integrated patch + matter v1.5 patch, please contact our FAE to get the SDKs

To check out Matter repository:

    git clone https://github.com/project-chip/connectedhomeip.git

Make sure ameba-rtos and connectedhomeip are on the same directory level

    dev/
    ├── ameba-rtos
    └── connectedhomeip

### Install Required Python Modules

Navigate to the `ameba-rtos` directory:

    cd ameba-rtos

    chmod u+x ameba.sh

    ./ameba.sh

### Set Matter Build Environment

    > Find more details to setup linux build environment
    > https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/BUILDING.md

    cd connectedhomeip

	git switch v1.5-branch

    git submodule sync

    git submodule update --init --recursive

    source scripts/bootstrap.sh

    source scripts/activate.sh

    cd ../ameba-rtos

    pip install -r tools/requirements.txt

Note that the last two steps are required is to install additional python modules to the connectedhomeip environment

### Set Ameba Build Environment

Navigate to the `ameba-rtos` directory:

    cd ameba-rtos

    chmod u+x matter_setup.sh

    ./matter_setup.sh ameba-rtos v1.5

### Build CHIP library by GN and Final Firmware

In this context, we will demostrate building of all-clusters-app.

#### Enable Matter Settings

Navigate to the `amebadplus_gcc_project` directory:

    cd ameba-rtos/amebadplus_gcc_project/

Menuconfig for matter:
- To enable Matter, under `CONFIG APPLICATION`, select `Matter Config`, and enable `Enable Matter`.
- If you want to support Matter BLE, under `CONFIG BT`, select `Enable BT`. Go back to the main menuconfig, select `CONFIG APPLICATION`, under `Matter Config`, enable `BLE Matter Adapter`.
- If you want to support [Matter ESF](matter_commissioning_and_control_guide.md#enable-matter-esf), select `CONFIG APPLICATION`, under `Matter Config`, enable `Enable Matter Terms and Condition`

```bash
python menuconfig.py
```

#### Build Matter Libraries and Final Firmware

Within `amebadplus_gcc_project` folder, start building the Matter libraries and the final firmware with the following command:

    python build.py -D MATTER_EXAMPLE=all_clusters

#### Clean Ameba Matter libraries and application

Within `amebadplus_gcc_project` folder, clean the whole project with the following command:

    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

### Flash Image using Python script

The generated image is found in the ameba-rtos sdk `ameba-rtos/amebadplus_gcc_project`, both `km4_boot_all.bin` and `km0_km4_app.bin` will be flashed.

Within `amebadplus_gcc_project` folder, flash the image to the Ameba port (e.g. `/dev/ttyUSB0`).

    python flash.py -p /dev/ttyUSB0

If the app image is too large, please add the --image/-i option

    python flash.py -p /dev/ttyUSB0 -i km4_boot_all.bin 0x08000000 0x08014000 -i km0_km4_app.bin 0x08014000 0x08300000

### Monitor Ameba Log using Python script

Within `amebadplus_gcc_project` folder, monitor Ameba log through the Ameba port (e.g. `/dev/ttyUSB0`).

    python monitor.py -p /dev/ttyUSB0 -b 1500000

## Ameba RTOS v1.0 SDK

### Get Ameba SDK & Matter SDK

    Tested on Ubuntu 24.04 or above

Create and enter new directory

    mkdir dev
    cd dev

Get ameba-rtos v1.0 SDK + integrated patch + matter v1.5 patch, please contact our FAE to get the SDKs

To check out Matter repository:

    git clone https://github.com/project-chip/connectedhomeip.git

Make sure ameba-rtos and connectedhomeip are on the same directory level

    dev/
    ├── ameba-rtos
    └── connectedhomeip

### Set Matter Build Environment

    > Find more details to setup linux build environment
    > https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/BUILDING.md

    cd connectedhomeip

	git switch v1.5-branch

    git submodule sync

    git submodule update --init --recursive

    source scripts/bootstrap.sh

    source scripts/activate.sh

### Set Ameba Build Environment

Navigate to the `ameba-rtos` directory:

    cd ameba-rtos

    chmod u+x matter_setup.sh

    ./matter_setup.sh ameba-rtos v1.5

### Build CHIP library by GN and Final Firmware

In this context, we will demostrate building of all-clusters-app.

#### Make Matter Libraries

Navigate to the `amebadplus_gcc_project` directory:

    cd ameba-rtos/amebadplus_gcc_project/

Menuconfig for matter:
- To enable Matter, select `MENUCONFIG FOR KM4 CONFIG`, then select `Matter Config`, and enable `Enable Matter`.
    - Under the same config, set the `Matter Version` to `MATTER_V_1_5`
- Change mbedtls version to matter. Under `MENUCONFIG FOR KM4 CONFIG`, select `SSL Config`, then enable `Matter MBEDTLS Enable`
- If you want to support Matter BLE, under `CONFIG BT`, enable `BT Example Demo`, then select `BLE_Matter_Adapter`, and save the configuration.
- If you want to support [Matter ESF](matter_commissioning_and_control_guide.md#enable-matter-esf), under `Matter Config`, enable `Enable Matter Terms and Condition`

```
make menuconfig
```

Navigate to the `amebadplus_gcc_project` directory:

    cd ameba-rtos/amebadplus_gcc_project

Start building the Matter libraries with:

    make -C project_km4/asdk all_clusters

#### Make Final Firmware

Ensure the same menuconfig settings as described in `Make Matter Libraries`and continue building the final firmware.

    cd ameba-rtos/amebadplus_gcc_project/

Build the final firmware:

    make all MATTER_EXAMPLE=chiptest

#### Clean Ameba Matter libraries and application

    cd ameba-rtos/amebadplus_gcc_project/

    make clean

### Flash Image using Windows Image Tool

The generated image is found in the ameba-rtos sdk `ameba-rtos/amebadplus_gcc_project`, both `km4_boot_all.bin` and `km0_km4_app.bin` will be flashed.

- Find more detail in [ameba-rtos/README.md](https://github.com/Ameba-AIoT/ameba-rtos/blob/release/v1.0/README.md#flashing)
