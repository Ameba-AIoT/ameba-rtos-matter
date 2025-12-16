# Laundry Washer Example
This example is an implementation of the *Laundry Washer* device type.
You can initialize any GPIO if necessary. Please check the different GPIO pin used for respective ICs below.
Note that these driver codes are meant to be just the skeleton, you should replace them and implement your own.

## ZAP
Since there is no example ZAP file for the laundry washer device type, we will use `laundrywasher-app.zap`.

## How it works
The laundry washer can be controlled in two ways, by the Matter controller, or by external means. 
In this example, we demonstrate both methods via Matter controller and external means.
If you wish to control by external means, you will need to use the `downlink` task shown in `matter_drivers.cpp`. Please feel free to add more based on your implementations. Meanwhile, controlling with Matter controller will trigger the `uplink` handler.

### Peripheral Initialization
The initializations are handled in `matter_drivers.cpp`.

### Matter Attribute Change Callback
Whenever the Matter controller changes the attribute of the Laundry Washer cluster, 2 types of callbacks will be invoked:
  1. MatterPreAttributeChangeCallback - Change the status/value before updating the attribute (TBD)
  2. MatterPostAttributeChangeCallback - Change the status/value after updating the attribute

These callbacks are defined in `core/matter_interaction.cpp`.
These callbacks will post an event to the uplink queue, which will be handled by `matter_driver_uplink_update_handler` in `matter_drivers.cpp`.
The driver codes will be called to carry out your actions depending on the Cluster and Attribute ID received.
You may add clusters and attributes handling in `matter_driver_uplink_update_handler` if they are not present. 

## How to build

### Configurations
`CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER` are automatically enabled in the Makefiles.

### Setup the Build Environment
  
    cd connectedhomeip
    source scripts/activate.sh

<details>
  <summary>Building with AmebaDplus</summary>

### AmebaDplus (RTL8721Dx)

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project
    python build.py -D MATTER_EXAMPLE=laundrywasher_port

#### Flash the Image
Refer to this [guide](../../docs/amebadplus_general_build.md#Flash-Image-using-Python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebadplus_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>

<details>
  <summary>Building with AmebaLite</summary>

### AmebaLite (RTL8720EA / RTL8726EA)

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebalite_gcc_project
    python build.py -D MATTER_EXAMPLE=laundrywasher_port

#### Flash the Image
Refer to this [guide](../../docs/amebalite_general_build.md#Flash-Image-using-Python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebalite_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>

<details>
  <summary>Building with AmebaSmart</summary>

### AmebaSmart (RTL8730E)

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project
    python build.py -D MATTER_EXAMPLE=laundrywasher_port

#### Flash the Image
Refer to this [guide](../../docs/amebasmart_general_build.md#Flash-Image-using-Python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebasmart_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>
