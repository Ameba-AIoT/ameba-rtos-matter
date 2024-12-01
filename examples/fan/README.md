# FAN Example
This example is an implementation of the *FAN* device type.
You will need a PWM fan. Please check the different GPIO pin used for respective ICs below.

## ZAP
Since there is no example ZAP file for the fan device type, we will use `fan-app.zap`.

## How it works
The fan can be controlled in two ways, by the Matter controller, or by external means. In this example, we only demonstrate control via Matter controller. If you wish to add more methods to control (eg. a push button), you will need to implement the `downlink` task and handler. See `lighting-app` for button example.
Thus, we only use 1 Uplink queue to for the fan to be controlled by the Matter controller. 

### Peripheral Initialization
The initializations of the fan are handled in `matter_drivers.cpp`.

### Fan Attribute Change
Whenever the Matter controller changes the Fanmode/Fanspeed attribute of the fan, 2 types of callbacks will be invoked:
  1. MatterPreAttributeChangeCallback - Change the Fanmode/Fanspeed before updating the Fanmode/Fanspeed attribute (TBD)
  2. MatterPostAttributeChangeCallback - Change the Fanmode/Fanspeed after updating the Fanmode/Fanspeed attribute

These callbacks are defined in `core/matter_interaction.cpp`.
These callbacks will post an event to the uplink queue, which will be handled by `matter_driver_uplink_update_handler` in `matter_drivers.cpp`.
The driver codes will be called to carry out your actions depending on the Cluster and Attribute ID received.
You may add clusters and attributes handling in `matter_driver_uplink_update_handler` if they are not present. 

## How to build

### Configurations
Enable `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_FAN` in `platform_opts_matter.h`.
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
| Fan        | PA_23 |

#### Build Matter Libraries

**Navigate to `realtek_amebaz2_v0_example` for AmebaZ2 or `realtek_amebaz2plus_v0_example` for AmebaZ2Plus**

    cd ameba-rtos-z2/project/realtek_amebaX_v0_example/GCC-RELEASE/
    make fan_port

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
| Fan        | PB_5  |

#### Build Matter Libraries

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make -C asdk fan_port
    
#### Build the Final Firmware

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp
    make all
    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make all

#### Clean Matter Libraries and Firmware

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make clean

</details>

<details>
  <summary>Building with AmebaPro2</summary>

### AmebaPro2 (RTL8732B)

Please contact Realtek to obtain the sdk.

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| Fan        | PE_0  |
  
#### Build Matter Libraries

    cd sdk/project/realtek_amebapro2_v0_example/GCC-RELEASE
    make fan_port

#### Build the Final Firmware

Please open a **NEW** terminal

    cd sdk/project/realtek_amebapro2_v0_example/GCC-RELEASE
    mkdir build_matter ; cd build_matter
    cmake .. -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake -DEXAMPLE=matter
    cmake --build . --target flash -j

#### Clean Matter Libraries

    cd sdk/project/realtek_amebapro2_v0_example/GCC-RELEASE
    make clean_matter_libs

</details>
