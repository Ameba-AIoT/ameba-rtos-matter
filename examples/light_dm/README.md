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
  <summary>Building with AmebaZ2/AmebaZ2Plus</summary>

### AmebaZ2/AmebaZ2Plus (RTL8710C)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| LED        | PA_23 |
| Button     | PA_17 |

#### Build Matter Libraries

**Navigate to `realtek_amebaz2_v0_example` for AmebaZ2 or `realtek_amebaz2plus_v0_example` for AmebaZ2Plus**

    cd ameba-rtos-z2/project/realtek_amebaX_v0_example/GCC-RELEASE/
    make light_dm
    
#### Build the Final Firmware

    cd ameba-rtos-z2/project/realtek_amebaX_v0_example/GCC-RELEASE/
    make is_matter

#### Clean Matter Libraries

    cd ameba-rtos-z2/project/realtek_amebaX_v0_example/GCC-RELEASE/
    make clean_matter_libs

#### Clean Ameba Matter application

    cd ameba-rtos-z2/project/realtek_amebaX_v0_example/GCC-RELEASE/
    make clean_matter

</details>

<details>
  <summary>Building with AmebaD</summary>

### AmebaD (RTL8721D)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| LED        | PB_5  |
| Button     | PA_12 |

### PSRAM usage
Due to insufficient memory in SRAM, we will use the PSRAM for dynamic allocation.
To run this example without error, you need to enable PSRAM.
In `rtl8721dhp_intfcfg.c`, set the below configurations

    PSRAMCFG_TypeDef psram_dev_config = {
      .psram_dev_enable = TRUE,         //enable psram
      .psram_dev_cal_enable = TRUE,     //enable psram calibration function
      .psram_dev_retention = TRUE,
    }

#### Build Matter Libraries

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make -C asdk light_dm
    
#### Build the Final Firmware

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp
    make all
    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make all

#### Clean Matter Libraries and Firmware

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make clean

</details>
