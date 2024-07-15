# Microwave Oven Example
This example is an implementation of the *Microwave Oven* device type. You can initialize any GPIO if necessary.
Note that these driver codes are meant to be just the skeleton, you should replace them and implement your own.

## ZAP
Since there is no example ZAP file for the Microwave Oven device type, we will use `microwaveoven-app.zap`.

## How it works
The Microwave Oven can be controlled in two ways, by the Matter controller, or by external means. 
In this example, we demonstrate both methods via Matter controller and external means.
If you wish to control by external means, you will need to use the `downlink` task shown in `matter_drivers.cpp`. Please feel free to add more based on your implementations. Meanwhile, controlling with Matter controller will trigger the `uplink` handler.

### Peripheral Initialization
The initializations are handled in `matter_drivers.cpp`.

### Matter Attribute Change Callback
Whenever the Matter controller changes the attribute of the LMicrowave Oven cluster, 2 types of callbacks will be invoked:
  1. MatterPreAttributeChangeCallback - Change the status/value before updating the attribute (TBD)
  2. MatterPostAttributeChangeCallback - Change the status/value after updating the attribute

These callbacks are defined in `core/matter_interaction.cpp`.
These callbacks will post an event to the uplink queue, which will be handled by `matter_driver_uplink_update_handler` in `matter_drivers.cpp`.
The driver codes will be called to carry out your actions depending on the Cluster and Attribute ID received.
You may add clusters and attributes handling in `matter_driver_uplink_update_handler` if they are not present. 

## How to build

### Configurations
Enable `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN` in `platform_opts.h`.
Ensure that `CONFIG_EXAMPLE_MATTER_CHIPTEST` is disabled.

### Setup the Build Environment
  
    cd connectedhomeip
    source scripts/activate.sh

<details>
  <summary>Building with AmebaZ2</summary>

### AmebaZ2 (RTL8710C)

#### Build Matter Libraries

    cd ambz2_matter/component/common/application/matter/example/microwaveoven
    make microwaveoven_port
    
#### Build the Final Firmware

    cd ambz2_matter/project/realtek_amebaz2_v0_example/GCC-RELEASE/
    make is_matter
    
#### Flash the Image
Refer to this [guide](https://github.com/ambiot/ambz2_matter/blob/main/tools/AmebaZ2/Image_Tool_Linux/README.md) to flash the image with the Linux Image Tool

#### Clean Matter Libraries

    cd ambz2_matter/project/realtek_amebaz2_v0_example/GCC-RELEASE/
    make clean_matter_libs

#### Clean Ameba Matter application

    cd ambz2_matter/project/realtek_amebaz2_v0_example/GCC-RELEASE/
    make clean_matter

</details>

<details>
  <summary>Building with AmebaD</summary>

### AmebaD (RTL8721D)

#### Build Matter Libraries

    cd ambd_matter/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make -C asdk microwaveoven_port
    
#### Build the Final Firmware

    cd ambd_matter/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp
    make all
    cd ambd_matter/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make all
    
#### Flash the Image
Refer to this [guide](https://github.com/ambiot/ambd_matter/blob/main/tools/AmebaD/Image_Tool_Linux/README.txt) to flash the image with the Linux Image Tool

#### Clean Matter Libraries and Firmware

    cd ambd_matter/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make clean

</details>