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
  <summary>Building with AmebaDplus</summary>

### AmebaDplus (RTL8721Dx)

#### Build Matter Libraries

    cd ameba-rtos/amebadplus_gcc_project/project_km4
    make -C asdk bridge_dm

#### Build the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project/project_km4
    make EXAMPLE=bridge
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

#### Build Matter Libraries

    cd ameba-rtos/amebalite_gcc_project/project_km4
    make -C asdk bridge_dm

#### Build the Final Firmware

    cd ameba-rtos/amebalite_gcc_project/project_km4
    make EXAMPLE=bridge
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

#### Build Matter Libraries

    cd ameba-rtos/amebasmart_gcc_project/project_ap
    make -C asdk bridge_dm

#### Build the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project/project_ap
    make EXAMPLE=bridge
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
