# Refrigerator Example
This example is an implementation of the *Refrigerator* device type. Peripherals consists of a LED, and button.
You can initialize any GPIO if necessary. Please check the different GPIO pin used for respective ICs below.

Note that these driver codes are meant to be just the skeleton, you should replace them and implement your own.

## ZAP
We will use `refrigerator-app.zap` instead of the zap file within the connectedhomeip example.

## How it works
The refrigerator can be controlled in two ways, by the Matter controller, or by external means. 
In this example, we demonstrate both methods via Matter controller and external means.
If you wish to control by external means, you will need to use the `downlink` task shown in `matter_drivers.cpp`. Please feel free to add more based on your implementations. Meanwhile, controlling with Matter controller will trigger the `uplink` handler.

### Peripheral Initialization
The initializations are handled in `matter_drivers.cpp`.

### Matter Attribute Change Callback
Whenever the Matter controller changes the attribute of the Refrigerator cluster, 2 types of callbacks will be invoked:
  1. MatterPreAttributeChangeCallback - Change the status/value before updating the attribute (TBD)
  2. MatterPostAttributeChangeCallback - Change the status/value after updating the attribute

These callbacks are defined in `core/matter_interaction.cpp`.
These callbacks will post an event to the uplink queue, which will be handled by `matter_driver_uplink_update_handler` in `matter_drivers.cpp`.
The driver codes will be called to carry out your actions depending on the Cluster and Attribute ID received.
You may add clusters and attributes handling in `matter_driver_uplink_update_handler` if they are not present. 

## How to build

### Configurations
Enable `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_REFRIGERATOR` in `platform_opts_matter.h`.
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
| LED        | PA_19 |
| Button     | PA_17 |

#### Build Matter Libraries

**Navigate to `realtek_amebaz2_v0_example` for AmebaZ2 or `realtek_amebaz2plus_v0_example` for AmebaZ2Plus**

    cd ameba-rtos-z2/project/realtek_amebaX_v0_example/GCC-RELEASE/
    make refrigerator_port
    
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
| LED        | PB_4  |
| Button     | PA_28 |

#### Build Matter Libraries

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make -C asdk refrigerator_port
    
#### Build the Final Firmware

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp
    make all
    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make all

#### Clean Matter Libraries

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
| LED        | PE_0  |
| Button     | PA_3  |
  
#### Build Matter Libraries

    cd sdk/project/realtek_amebapro2_v0_example/GCC-RELEASE
    make refrigerator_port

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
