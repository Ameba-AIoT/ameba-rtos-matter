# Thermostat-app Example
This example is an implementation of the *Thermostat* device type. Peripherals consists of a thermostat and the thermostat user interface itself. Note that these driver codes are meant to be just the skeleton, you should replace them and implement your own.

## How it works
We support bidirectional exchanges of attrubute updates (see `light` example) but this example will only make use of uplink updates.
Feel free to create downlink updates by posting events to the downlink queue (again, see `light` example).

### Peripheral Initialization
Both the initializations of the thermostat and the thermostat UI are handled in `matter_drivers.cpp`.

### Matter Attribute Change Callback
Whenever the Matter controller changes the attribute of the Thermostat cluster, 2 types of callbacks will be invoked:
  1. MatterPreAttributeChangeCallback - Do action before updating the On/Off attribute (TBD)
  2. MatterPostAttributeChangeCallback - Do action after updating the On/Off attribute

These callbacks are defined in `core/matter_interaction.cpp`.
These callbacks will post an event to the uplink queue, which will be handled by `matter_driver_uplink_update_handler` in `matter_drivers.cpp`.
The driver codes will be called to carry out your actions depending on the Cluster and Attribute ID received.
You may add clusters and attributes handling in `matter_driver_uplink_update_handler` if they are not present. 

## How to build

### Configurations
`CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_THERMOSTAT` are automatically enabled in the Makefiles / CMake.

### Setup the Build Environment
  
    cd connectedhomeip
    source scripts/activate.sh

---

<details>
  <summary>Building with AmebaDplus</summary>

### AmebaDplus (RTL8721Dx)

<details>
  <summary>Building with ameba-rtos_v1.2 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos-matter/
    ameba.py soc RTL8721Dx
    matter_build_proj thermostat_port

#### Flash the Image
Refer to this [guide](../../docs/ameba-rtos_general_build.md#flash-image-using-amebapy-command) to flash the image with ameba.py command.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos-matter/
    matter_clean_proj

</details>

<details>
  <summary>Building with ameba-rtos_v1.1 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project
    python build.py -D MATTER_EXAMPLE=thermostat_port

#### Flash the Image
Refer to this [guide](../../docs/ameba-rtos_general_build.md#flash-image-using-python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebadplus_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>

<details>
  <summary>Building with ameba-rtos_v1.0 SDK</summary>

#### Build Matter Libraries

    cd ameba-rtos/amebadplus_gcc_project
    make -C project_km4/asdk thermostat_port

#### Build the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project
    make all MATTER_EXAMPLE=thermostat

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

### AmebaLite (RTL8726E / RTL8720E / RTL8713E / RTL8710E)

<details>
  <summary>Building with ameba-rtos_v1.2 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos-matter/
    ameba.py soc < RTL8726E / RTL8720E / RTL8713E / RTL8710E >
    matter_build_proj thermostat_port

#### Flash the Image
Refer to this [guide](../../docs/ameba-rtos_general_build.md#flash-image-using-amebapy-command) to flash the image with ameba.py command.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos-matter/
    matter_clean_proj

</details>

<details>
  <summary>Building with ameba-rtos_v1.1 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebalite_gcc_project
    python build.py -D MATTER_EXAMPLE=thermostat_port

#### Flash the Image
Refer to this [guide](../../docs/ameba-rtos_general_build.md#flash-image-using-python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebalite_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>

<details>
  <summary>Building with ameba-rtos_v1.0 SDK</summary>

#### Build Matter Libraries

    cd ameba-rtos/amebalite_gcc_project
    make -C project_km4/asdk thermostat_port

#### Build the Final Firmware

    cd ameba-rtos/amebalite_gcc_project
    make all MATTER_EXAMPLE=thermostat

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

<details>
  <summary>Building with ameba-rtos_v1.2 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos-matter/
    ameba.py soc RTL8730E
    matter_build_proj thermostat_port

#### Flash the Image
Refer to this [guide](../../docs/ameba-rtos_general_build.md#flash-image-using-amebapy-command) to flash the image with ameba.py command.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos-matter/
    matter_clean_proj

</details>

<details>
  <summary>Building with ameba-rtos_v1.1 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project
    python build.py -D MATTER_EXAMPLE=thermostat_port

#### Flash the Image
Refer to this [guide](../../docs/ameba-rtos_general_build.md#flash-image-using-python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebasmart_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>

<details>
  <summary>Building with ameba-rtos_v1.0 SDK</summary>

#### Build Matter Libraries

    cd ameba-rtos/amebasmart_gcc_project
    make -C project_ap/asdk thermostat_port

#### Build the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project
    make all MATTER_EXAMPLE=thermostat

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/release/v1.0/README.md#flashing) to flash the image with Windows Image Tool.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebasmart_gcc_project
    make clean

</details>

</details>

---

<details>
  <summary>Building with AmebaGreen2</summary>

### AmebaGreen2 (RTL8721F)

<details>
  <summary>Building with ameba-rtos_v1.2 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos-matter/
    ameba.py soc RTL8721F
    matter_build_proj thermostat_port

#### Flash the Image
Refer to this [guide](../../docs/ameba-rtos_general_build.md#flash-image-using-amebapy-command) to flash the image with ameba.py command.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos-matter/
    matter_clean_proj

</details>

</details>

---
