# Matter (previously CHIP) on Ameba RTOS

- [Ameba RTOS v1.2 SDK](#ameba-rtos-v12-sdk)
    - [Get Ameba SDK & Matter SDK](#get-ameba-sdk--matter-sdk)
    - [Set Matter Build Environment](#set-matter-build-environment)
    - [Set Ameba Build Environment](#set-ameba-build-environment)
    - [Build CHIP library by GN and Final Firmware](#build-chip-library-by-gn-and-final-firmware)
    - [Matter Build Commands](#matter-build-commands)
    - [Flash Image](#flash-image-using-amebapy-command)
    - [Monitor Ameba](#monitor-ameba-log-using-amebapy-command)
- [Ameba RTOS v1.1 SDK](#ameba-rtos-v11-sdk)
    - [Get Ameba SDK & Matter SDK](#get-ameba-sdk--matter-sdk-1)
    - [Set Matter Build Environment](#set-matter-build-environment-1)
    - [Set Ameba Build Environment](#set-ameba-build-environment-1)
    - [Build CHIP library by GN and Final Firmware](#build-chip-library-by-gn-and-final-firmware-1)
    - [Flash Image](#flash-image-using-python-script)
    - [Monitor Ameba](#Monitor-Ameba-Log-using-Python-script)
- [Ameba RTOS v1.0 SDK](#ameba-rtos-v10-sdk)
    - [Get Ameba SDK & Matter SDK](#get-ameba-sdk--matter-sdk-2)
    - [Set Matter Build Environment](#set-matter-build-environment-2)
    - [Set Ameba Build Environment](#set-ameba-build-environment-2)
    - [Build CHIP library by GN and Final Firmware](#build-chip-library-by-gn-and-final-firmware-2)
    - [Flash Image](#flash-image-using-windows-image-tool)

## Ameba RTOS v1.2 SDK

### Get Ameba SDK & Matter SDK

    Tested on Ubuntu 24.04 or above

Create and enter new directory

    mkdir dev
    cd dev

To check out ameba-rtos repository:

    git clone https://github.com/Ameba-AIoT/ameba-rtos.git -b release/v1.2

To check out ameba-rtos-matter repository:

    git clone https://github.com/Ameba-AIoT/ameba-rtos-matter.git -b ameba-rtos/release/v1.5

To check out Matter repository:

    git clone https://github.com/project-chip/connectedhomeip.git -b v1.5-branch

Make sure ameba-rtos, ameba-rtos-matter, and connectedhomeip are on the same directory level

    dev/
    ├── ameba-rtos
    ├── ameba-rtos-matter
    └── connectedhomeip

### Set Matter Build Environment

    > Find more details to setup linux build environment
    > https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/BUILDING.md

    cd ameba-rtos-matter

    chmod u+x matter_setup.sh

    ./matter_setup.sh ../ameba-rtos

    source matter_env.sh

### Set Ameba Build Environment

Navigate to the `ameba-rtos-matter` directory:

    cd ameba-rtos-matter

    ameba.py soc < RTL8721Dx / RTL8726E / RTL8720E / RTL8713E / RTL8710E / RTL8730E >

### Build CHIP library by GN and Final Firmware

In this context, we will demostrate building of all-clusters-app.

#### Matter Helper Commands

In this version, there are 3 matter helper commands that can be used after Matter environment is activated:
- `matter_apply_conf`: Apply default matter configuration to the project.
- `matter_build_proj`: Build Matter app. Matter device type example needs to be specified.
- `matter_clean_proj`: Clean the whole project.

When starting a new terminal to build Matter App, do not forget to activate the Matter environment

    source matter_env.sh

#### Enable Matter Settings

Navigate to the `ameba-rtos-matter` directory:

    cd ameba-rtos-matter

Menuconfig for matter:
- To apply default Matter config, run `matter_apply_conf`

```bash
matter_apply_conf
```

- If other menuconfig needs to be adjusted, run `ameba.py menuconfig`
- To enable Matter, under `CONFIG APPLICATION`, select `Matter Config`, and enable `Enable Matter`.
- If you want to support Matter BLE, under `CONFIG BT`, select `Enable BT`. Go back to the main menuconfig, select `CONFIG APPLICATION`, under `Matter Config`, enable `BLE Matter Adapter`.
- If you want to support [Matter ESF](matter_commissioning_and_control_guide.md#enable-matter-esf), select `CONFIG APPLICATION`, under `Matter Config`, enable `Enable Matter Terms and Condition`

```bash
ameba.py menuconfig
```

#### Build Matter Libraries and Final Firmware

Within `ameba-rtos-matter` folder, start building the Matter libraries and the final firmware with the following command:

<details>
  <summary>With Matter helper command (Recommended!)</summary>

    matter_build_proj all_clusters

</details>

<details>
  <summary>Without Matter helper command</summary>

    ameba.py build -D EXAMPLE="$PWD" MATTER_EXAMPLE=all_clusters

</details>

#### Clean Ameba Matter libraries and application

Within `ameba-rtos-matter` folder, clean the whole project with the following command:

<details>
  <summary>With Matter helper command (Recommended!)</summary>

    matter_clean_proj

</details>

<details>
  <summary>Without Matter helper command</summary>

    cd build_RTL87XXX/build && ninja clean_matter_libs && cd ../../ && ameba.py clean

</details>

### Flash Image using ameba.py command

The generated images are found in the build folder `build_RTL87XXX` of `ameba-rtos-matter` sdk:
- bootloader image: `km4_boot_all.bin`
- application image:
    - RTL8721Dx: `km0_km4_app.bin`
    - RTL8726E / RTL8720E / RTL8713E / RTL8710E: `kr4_km4_app.bin`
    - RTL8730E: `km0_km4_ca32_app.bin`

Both bootloader and appication images will be flashed. Within `ameba-rtos-matter` folder, flash the image to the Ameba port (e.g. `/dev/ttyUSB0`).

    ameba.py flash -p /dev/ttyUSB0

If the app image is too large, please add the --image/-i option:

<details>
  <summary>Complete flash command to RTL8721Dx</summary>

    ameba.py flash -p /dev/ttyUSB0 -i km4_boot_all.bin 0x08000000 0x08014000 -i km0_km4_app.bin 0x08014000 0x08300000

</details>

<details>
  <summary>Complete flash command to RTL8726E / RTL8720E / RTL8713E / RTL8710E</summary>

    ameba.py flash -p /dev/ttyUSB0 -i km4_boot_all.bin 0x08000000 0x08014000 -i kr4_km4_app.bin 0x08014000 0x08300000

</details>

<details>
  <summary>Complete flash command to RTL8730E</summary>

    ameba.py flash -p /dev/ttyUSB0 -i km4_boot_all.bin 0x08000000 0x08040000 -i km0_km4_ca32_app.bin 0x08040000 0x08340000

</details>

### Monitor Ameba Log using ameba.py command

Within `ameba-rtos-matter` folder, monitor Ameba log through the Ameba port (e.g. `/dev/ttyUSB0`).

    ameba.py monitor -p /dev/ttyUSB0 -b 1500000

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

> [!NOTE]
> Project directory for each IC is defined as the following:
> - RTL8721Dx: `ameba-rtos/amebadplus_gcc_project/`
> - RTL8726E / RTL8720E / RTL8713E / RTL8710E: `ameba-rtos/amebalite_gcc_project/`
> - RTL8730E: `ameba-rtos/amebasmart_gcc_project/`

In this context, we will demostrate building of all-clusters-app.

#### Enable Matter Settings

Navigate to the corresponding project directory based on the IC used:

    cd ameba-rtos/amebaxxx_gcc_project/

Menuconfig for matter:
- To enable Matter, under `CONFIG APPLICATION`, select `Matter Config`, and enable `Enable Matter`.
- If you want to support Matter BLE, under `CONFIG BT`, select `Enable BT`. Go back to the main menuconfig, select `CONFIG APPLICATION`, under `Matter Config`, enable `BLE Matter Adapter`.
- If you want to support [Matter ESF](matter_commissioning_and_control_guide.md#enable-matter-esf), select `CONFIG APPLICATION`, under `Matter Config`, enable `Enable Matter Terms and Condition`

```bash
python menuconfig.py
```

#### Build Matter Libraries and Final Firmware

Within the corresponding project directory, start building the Matter libraries and the final firmware with the following command:

    python build.py -D MATTER_EXAMPLE=all_clusters

#### Clean Ameba Matter libraries and application

Within the corresponding project directory, clean the whole project with the following command:

    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

### Flash Image using Python script

The generated images are found in the project directory of `ameba-rtos` sdk:
- bootloader image: `km4_boot_all.bin`
- application image:
    - RTL8721Dx: `km0_km4_app.bin`
    - RTL8726E / RTL8720E / RTL8713E / RTL8710E: `kr4_km4_app.bin`
    - RTL8730E: `km0_km4_ca32_app.bin`

Both bootloader and appication images will be flashed. Within the corresponding project directory, flash the image to the Ameba port (e.g. `/dev/ttyUSB0`).

    python flash.py -p /dev/ttyUSB0

If the app image is too large, please add the --image/-i option:

<details>
  <summary>Complete flash command to RTL8721Dx</summary>

    python flash.py -p /dev/ttyUSB0 -i km4_boot_all.bin 0x08000000 0x08014000 -i km0_km4_app.bin 0x08014000 0x08300000

</details>

<details>
  <summary>Complete flash command to RTL8726E / RTL8720E / RTL8713E / RTL8710E</summary>

    python flash.py -p /dev/ttyUSB0 -i km4_boot_all.bin 0x08000000 0x08014000 -i kr4_km4_app.bin 0x08014000 0x08300000

</details>

<details>
  <summary>Complete flash command to RTL8730E</summary>

    python flash.py -p /dev/ttyUSB0 -i km4_boot_all.bin 0x08000000 0x08020000 -i km0_km4_ca32_app.bin 0x08020000 0x08400000

</details>

### Monitor Ameba Log using Python script

Within the corresponding project directory, monitor Ameba log through the Ameba port (e.g. `/dev/ttyUSB0`).

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

> [!NOTE]
> Project directory for each IC is defined as the following:
> - RTL8721Dx: `ameba-rtos/amebadplus_gcc_project/`
> - RTL8726E / RTL8720E / RTL8713E / RTL8710E: `ameba-rtos/amebalite_gcc_project/`
> - RTL8730E: `ameba-rtos/amebasmart_gcc_project/`

In this context, we will demostrate building of all-clusters-app.

#### Enable Matter Settings

> [!NOTE]
> Location of `Mattter Config` and `SSL Config` in `menuconfig`:
> - RTL8721Dx / RTL8726E / RTL8720E / RTL8713E / RTL8710E: located under `MENUCONFIG FOR KM4 CONFIG`
> - RTL8730E: located under `MENUCONFIG FOR CA32 CONFIG`

Navigate to the corresponding project directory based on the IC used:

    cd ameba-rtos/amebaxxx_gcc_project/

Menuconfig for matter:

- To enable Matter, navigate to `Matter Config`, and enable `Enable Matter`.
    - Under the same config, set the `Matter Version` to `MATTER_V_1_5`
- Change mbedtls version to matter. Navigate to `SSL Config`, then enable `Matter MBEDTLS Enable`
- If you want to support Matter BLE, under `CONFIG BT`, enable `BT Example Demo`, then select `BLE_Matter_Adapter`, and save the configuration.
- If you want to support [Matter ESF](matter_commissioning_and_control_guide.md#enable-matter-esf), under `Matter Config`, enable `Enable Matter Terms and Condition`

```
make menuconfig
```

#### Make Matter Libraries

Within the corresponding project directory, start building the Matter libraries with:

<details>
  <summary>Matter library build command for RTL8721Dx / RTL8726E / RTL8720E / RTL8713E / RTL8710E</summary>

    make -C project_km4/asdk all_clusters

</details>

<details>
  <summary>Matter library build command for RTL8730E</summary>

    make -C project_ap/asdk all_clusters

</details>

#### Make Final Firmware

Within the corresponding project directory, build the final firmware:

    make all MATTER_EXAMPLE=chiptest

#### Clean Ameba Matter libraries and application

Within the corresponding project directory, clean the whole project with the following command:

    make clean

### Flash Image using Windows Image Tool

The generated images are found in the project directory of `ameba-rtos` sdk:
- bootloader image: `km4_boot_all.bin`
- application image:
    - RTL8721Dx: `km0_km4_app.bin`
    - RTL8726E / RTL8720E / RTL8713E / RTL8710E: `kr4_km4_app.bin`
    - RTL8730E: `km0_km4_ca32_app.bin`

Both bootloader and appication images will be flashed. Find more detail in [ameba-rtos/README.md](https://github.com/Ameba-AIoT/ameba-rtos/blob/release/v1.0/README.md#flashing)
