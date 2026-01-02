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
`CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_REFRIGERATOR` are automatically enabled in the Makefiles.

### Setup the Build Environment
  
    cd connectedhomeip
    source scripts/activate.sh

---

<details>
  <summary>Building with AmebaDplus</summary>

### AmebaDplus (RTL8721Dx)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| LED        | PB_18 |
| Button     | PA_12 |

<details>
  <summary>Building with ameba-rtos_v1.1 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project
    python build.py -D MATTER_EXAMPLE=refrigerator_port

#### Flash the Image
Refer to this [guide](../../docs/amebadplus_general_build.md#Flash-Image-using-Python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebadplus_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>

<details>
  <summary>Building with ameba-rtos_v1.0 SDK</summary>

#### Build Matter Libraries

    cd ameba-rtos/amebadplus_gcc_project
    make -C project_km4/asdk refrigerator_port

#### Build the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project
    make all MATTER_EXAMPLE=refrigerator

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/release/v1.0/README.md#flashing) to flash the image with Windows Image Tool.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebadplus_gcc_project
    make clean

</details>

</details>

---

<details>
  <summary>Building with AmebaLite</summary>

### AmebaLite (RTL8720EA / RTL8726EA)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| LED        | PA_31 |
| Button     | PA_29 |

<details>
  <summary>Building with ameba-rtos_v1.1 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebalite_gcc_project
    python build.py -D MATTER_EXAMPLE=refrigerator_port

#### Flash the Image
Refer to this [guide](../../docs/amebalite_general_build.md#Flash-Image-using-Python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebalite_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>

<details>
  <summary>Building with ameba-rtos_v1.0 SDK</summary>

#### Build Matter Libraries

    cd ameba-rtos/amebalite_gcc_project
    make -C project_km4/asdk refrigerator_port

#### Build the Final Firmware

    cd ameba-rtos/amebalite_gcc_project
    make all MATTER_EXAMPLE=refrigerator

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/release/v1.0/README.md#flashing) to flash the image with Windows Image Tool.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebalite_gcc_project
    make clean

</details>

</details>

---

<details>
  <summary>Building with AmebaSmart</summary>

### AmebaSmart (RTL8730E)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| LED        | PA_5  |
| Button     | PA_10 |

<details>
  <summary>Building with ameba-rtos_v1.1 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project
    python build.py -D MATTER_EXAMPLE=refrigerator_port

#### Flash the Image
Refer to this [guide](../../docs/amebasmart_general_build.md#Flash-Image-using-Python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebasmart_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>

<details>
  <summary>Building with ameba-rtos_v1.0 SDK</summary>

#### Build Matter Libraries

    cd ameba-rtos/amebasmart_gcc_project
    make -C project_ap/asdk refrigerator_port

#### Build the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project
    make all MATTER_EXAMPLE=refrigerator

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/release/v1.0/README.md#flashing) to flash the image with Windows Image Tool.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebasmart_gcc_project
    make clean

</details>

</details>

---
