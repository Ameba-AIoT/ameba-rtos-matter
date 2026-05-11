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
`CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_BRIDGE` are automatically enabled in the Makefiles / CMake.

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
    matter_build_proj bridge_dm

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
    python build.py -D MATTER_EXAMPLE=bridge_dm

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
    make -C project_km4/asdk bridge_dm

#### Build the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project
    make all MATTER_EXAMPLE=bridge

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
    matter_build_proj bridge_dm

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
    python build.py -D MATTER_EXAMPLE=bridge_dm

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
    make -C project_km4/asdk bridge_dm

#### Build the Final Firmware

    cd ameba-rtos/amebalite_gcc_project
    make all MATTER_EXAMPLE=bridge

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
    matter_build_proj bridge_dm

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
    python build.py -D MATTER_EXAMPLE=bridge_dm

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
    make -C project_ap/asdk bridge_dm

#### Build the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project
    make all MATTER_EXAMPLE=bridge

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
    matter_build_proj bridge_dm

#### Flash the Image
Refer to this [guide](../../docs/ameba-rtos_general_build.md#flash-image-using-amebapy-command) to flash the image with ameba.py command.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos-matter/
    matter_clean_proj

</details>

</details>

---
