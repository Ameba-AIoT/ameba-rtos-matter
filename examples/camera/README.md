# Camera-app Example
This example is an implementation of the *Camera* device type. 
You will need a USB Camera. Please use the camera that supports H.264 codec. Ameba actually supports three video formats, MJPEG, YUV, and H.264 format. Among the three supported formats, only H.264 format is suported in the Matter layer.

## Hardware Configuration
Some Ameba boards require hardware rework to support USB Host mode.
Populate the designated resistor footprint with a **0Ohm resistor** (or apply a solder bridge)
to enable VBUS power supply to the USB device.
Refer to the [EVB User Guide](https://aiot.realmcu.com/filelist?document_type=9) for details.

### Board Rework Reference
#### RTL8730E

| Board Variant      | Resistor | Rework Action                                    |
|--------------------|----------|--------------------------------------------------|
| RTL8730EA / QFN100 | R20      | Populate with 0Ohm resistor or solder bridge     |
| RTL8730EL / QFN144 | R166     | Populate with 0Ohm resistor, or use an OTG cable |

#### RTL8721F

| Board Variant    | Resistor | Rework Action                                           |
|------------------|----------|---------------------------------------------------------|
| RTL8721FC0-R00   | R52      | Populate with 0Ohm resistor                             |
| RTL8721FA0-3V0   | R52      | Populate with 0Ohm resistor                             |
| RTL8721FL0-2V0   | R2       | Populate with 0Ohm resistor *(pre-reworked by default)* |

## ZAP
We will use `camera-app.zap` to support minimum cluster requirements for Camera App.

## How it works
The Camera can be controlled in two ways, by the Matter controller, or by external means. 
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
1. `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_CAMERA` are automatically enabled in the Makefiles / CMake.

2. In menuconfig, navigate to `Matter Config` enable Matter TCP Endpoint:

        [*]     Enable Matter TCP Endpoint

    This is to support the WebRTC clusters.

3. In menuconfig, navigate to `CONFIG USB` and set the USB configuration as the following:

        [*] Enable USB
            USB Mode (Host)
        [*] UVC

### Setup the Build Environment
  
    cd connectedhomeip
    source scripts/activate.sh

---

<details>
  <summary>Building with AmebaSmart</summary>

### AmebaSmart (RTL8730E)

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos-matter/
    ameba.py soc RTL8730E
    matter_build_proj camera_port

#### Flash the Image
Refer to this [guide](../../docs/ameba-rtos_general_build.md#flash-image-using-amebapy-command) to flash the image with ameba.py command.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos-matter/
    matter_clean_proj

</details>

<details>
  <summary>Building with ameba-rtos_v1.2 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos-matter/
    ameba.py soc RTL8730E
    matter_build_proj camera_port

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
    python build.py -D MATTER_EXAMPLE=camera_port

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
    make -C project_ap/asdk camera_port

#### Build the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project
    make all MATTER_EXAMPLE=camera

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
    matter_build_proj camera_port

#### Flash the Image
Refer to this [guide](../../docs/ameba-rtos_general_build.md#flash-image-using-amebapy-command) to flash the image with ameba.py command.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos-matter/
    matter_clean_proj

</details>

</details>

---
