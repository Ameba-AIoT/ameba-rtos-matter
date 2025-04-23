# Lighting-app using Ameba Data Model Example
This example is an implementation of the *Dimmable Light* device type using the *Ameba Data Model*.
You will need an LED and a button. Please check the different GPIO pin used for respective ICs below.

## Ameba Data Model
This example demonstrates adding and removing endpoints dynamically using the *Ameba Data Model*.
A `Root Node` endpoint will be created on Endpoint0, and a `Dimmable Light` endpoint on Endpoint1.
After a 20 second delay, another `Dimmable Light` enpdoint will be added on Endpoint2.
After another 20 second delay, the `Dimmable Light` endpoint on Endpoint2 will be removed.

## How to build

### Configurations
Enable `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_LIGHT` in `platform_opts_matter.h`.
Ensure that `CONFIG_EXAMPLE_MATTER_CHIPTEST` is disabled.

### Setup the Build Environment
  
    cd connectedhomeip
    source scripts/activate.sh

<details>
  <summary>Building with AmebaDplus</summary>

### AmebaDplus (RTL8721Dx)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| LED        | PB_18 |
| Button     | PA_12 |

#### Build Matter Libraries

    cd ameba-rtos/amebadplus_gcc_project/project_km4
    make -C asdk light_dm

#### Build the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project/project_km4
    make EXAMPLE=light
    cd ameba-rtos/amebadplus_gcc_project/project_km0
    make all

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/master/README.md#flashing) to flash the image with Windows Image Tool

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebadplus_gcc_project/project_km4
    make clean
</details>

<details>
  <summary>Building with AmebaLite</summary>

### AmebaLite (RTL8720EA / RTL8726EA)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| LED        | PA_31 |
| Button     | PA_29 |

#### Build Matter Libraries

    cd ameba-rtos/amebalite_gcc_project/project_km4
    make -C asdk light_dm

#### Build the Final Firmware

    cd ameba-rtos/amebalite_gcc_project/project_km4
    make EXAMPLE=light
    cd ameba-rtos/amebalite_gcc_project/project_kr4
    make all

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/master/README.md#flashing) to flash the image with Windows Image Tool

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebalite_gcc_project/project_km4
    make clean
</details>

<details>
  <summary>Building with AmebaSmart</summary>

### AmebaSmart (RTL8730E)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| LED        | PA_5  |
| Button     | PA_10 |

#### Build Matter Libraries

    cd ameba-rtos/amebasmart_gcc_project/project_ap
    make -C asdk light_dm

#### Build the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project/project_ap
    make EXAMPLE=light
    cd ameba-rtos/amebasmart_gcc_project/project_hp
    make all
    cd ameba-rtos/amebasmart_gcc_project/project_lp
    make all

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/master/README.md#flashing) to flash the image with Windows Image Tool

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebasmart_gcc_project/project_ap
    make clean
</details>
