# Aircon Example
This example is an implementation of the *Room Air Conditioner* device type. 
You will need a PWM fan and a temperature/humidity sensor. Please check the different GPIO pin used for respective ICs below.

## ZAP
Since there is no example ZAP file for the aircon device type, we will use `aircon-app.zap`.

## How it works
The fan can be controlled in two ways, by the Matter controller, or by external means. In this example, we only demonstrate control via Matter controller. If you wish to add more methods to control (eg. a push button), you will need to implement the `downlink` task and handler. See `lighting-app` for button example.
Thus, we only use 1 Uplink queue to for the fan to be controlled by the Matter controller.

### Peripheral Initialization
Both the initializations of the fan and the temperature/humidity sensor are handled in `matter_drivers.cpp`.

### Fan Attribute Change
Whenever the Matter controller changes the Fanmode/Fanspeed attribute of the fan, 2 types of callbacks will be invoked:
  1. MatterPreAttributeChangeCallback - Change the Fanmode/Fanspeed before updating the Fanmode/Fanspeed attribute (TBD)
  2. MatterPostAttributeChangeCallback - Change the Fanmode/Fanspeed after updating the Fanmode/Fanspeed attribute

These callbacks are defined in `core/matter_interaction.cpp`.
These callbacks will post an event to the uplink queue, which will be handled by `matter_driver_uplink_update_handler` in `matter_drivers.cpp`.
The driver codes will be called to carry out your actions depending on the Cluster and Attribute ID received.
You may add clusters and attributes handling in `matter_driver_uplink_update_handler` if they are not present. 

### Temperature/Humidity Sensor Attribute Change
By calling `matter_driver_temphumsensor_start`, a task will be created to poll the temperature and humidity periodically.
After obtaining the temperature and humidity measurements, the task will update the respective attributes on the Matter data model by invoking the `Set()` function. 

## How to build

### Configurations
Enable `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_AIRCON` in `platform_opts_matter.h`.
Ensure that `CONFIG_EXAMPLE_MATTER_CHIPTEST` is disabled.

### Setup the Build Environment
  
    cd connectedhomeip
    source scripts/activate.sh

<details>
  <summary>Building with AmebaDplus</summary>

### AmebaDplus (RTL8721Dx)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| Fan        | PB_18  |
| Sensor     | Depends on type of sensor |

#### Build Matter Libraries

    cd ameba-rtos/amebadplus_gcc_project/project_km4
    make -C asdk aircon_port

#### Build the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project/project_km4
    make EXAMPLE=aircon
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

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| Fan        | PA_31  |
| Sensor     | Depends on type of sensor |

#### Build Matter Libraries

    cd ameba-rtos/amebalite_gcc_project/project_km4
    make -C asdk aircon_port

#### Build the Final Firmware

    cd ameba-rtos/amebalite_gcc_project/project_km4
    make EXAMPLE=aircon
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

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| Fan        | PA_5  |
| Sensor     | Depends on type of sensor |

#### Build Matter Libraries

    cd ameba-rtos/amebasmart_gcc_project/project_ap
    make -C asdk aircon_port

#### Build the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project/project_ap
    make EXAMPLE=aircon
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
