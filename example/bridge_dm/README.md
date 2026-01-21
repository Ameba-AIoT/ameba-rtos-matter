# Bridge-app using Ameba Data Model Example
This example is an implementation of the *Bridge* device type. The Bridge will be communicating with the other non-Matter peripherals via TCP sockets.
You will need 2 non-Matter peripherals running TCP client socket.

## ZAP
We will use `bridge-app.zap` instead of the zap file within the connectedhomeip example.

## Ameba Data Model
This example demonstrates adding and removing endpoints dynamically using the *Ameba Data Model*.
A `Root Node` device type will be created on Endpoint0, a `Aggregator` device type on Endpoint1 and a `Dimmable Light` device type on Endpoint2.
After 20 seconds delay, the `Dimmable Light` endpoint on Endpoint2 will be removed.

Additionally a new thread will be created for user to input their code to communicate with non-matter device based on the protocol (e.g., IP-based (TCP,UDP), BLE, zigbee and etc) they wish to use.

## How to build

### Configurations
Enable `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_BRIDGE` in `platform_opts_matter.h`.
Ensure that `CONFIG_EXAMPLE_MATTER_CHIPTEST` is disabled.

### Setup the Build Environment
  
    cd connectedhomeip
    source scripts/activate.sh

<details>
  <summary>Building with AmebaZ2</summary>

### AmebaZ2 (RTL8710C)

#### Build Matter Libraries

    cd sdk/project/realtek_amebaz2_v0_example/GCC-RELEASE/
    make bridge_dm

#### Build the Final Firmware

    cd sdk/project/realtek_amebaz2_v0_example/GCC-RELEASE/
    make is_matter

#### Clean Matter Libraries

    cd sdk/project/realtek_amebaz2_v0_example/GCC-RELEASE/
    make clean_matter_libs

#### Clean Ameba Matter application

    cd sdk/project/realtek_amebaz2_v0_example/GCC-RELEASE/
    make clean_matter

  </div>
</details>

<details>
  <summary>Building with AmebaD</summary>

### AmebaD (RTL8721D)

#### PSRAM usage
Due to insufficient memory in SRAM, we will use the PSRAM for dynamic allocation.
To run this example without error, you need to enable PSRAM.
In `rtl8721dhp_intfcfg.c`, set the below configurations

    PSRAMCFG_TypeDef psram_dev_config = {
      .psram_dev_enable = TRUE,         //enable psram
      .psram_dev_cal_enable = TRUE,         //enable psram calibration function
      .psram_dev_retention = TRUE,
    }

#### Build Matter Libraries

    cd sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make -C asdk bridge_dm
    
#### Build the Final Firmware

    cd sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp
    make all
    cd sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make all

#### Clean Matter Libraries and Firmware

    cd sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make clean

</details>

## Flash the Image
Refer to this [guide](../../tools/Image_Tool_Linux/README.md) to flash the image with the Linux Image Tool
