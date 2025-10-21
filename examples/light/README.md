# Lighting-app Example
This example is an implementation of the *Dimmable Light* device type. 
You will need an LED and a button. Please check the different GPIO pin used for respective ICs below.

## How it works
The LED can be controlled in two ways, by the Matter controller, or by a button.
Thus, 2 streams of communication (FreeRTOS Queues) are needed to control and update the status of the LED effectively:
  1. Uplink - Matter controller changes the On/Off attribute of the LED -> callback triggers and invokes the LED driver to toggle the LED
  2. Downlink - Button press toggles the LED -> update the new Matter On/Off attribute of the LED

### Peripheral Initialization
Both the initializations of the LED and the button are handled in `matter_drivers.cpp`.
The initialization of the button sets up an IRQ that is triggered whenever the button is pressed.

### Button Press
Whenever the button is pressed, the interrupt handler will be invoked.
The interrupt handler will post an event to the downlink queue, which will be handled by `matter_driver_downlink_update_handler`.

To implement this, under `matter_drivers.cpp`, setup your GPIO interrupt callback to create and post events to the downlink queue. See `matter_driver_button_callback` for reference.
When creating the event to post to downlink queue, create a handler function for the event that will update the attributes on the Matter stack. See `matter_driver_downlink_update_handler` for reference.

### Matter Attribute Change Callback
Whenever the Matter controller changes the On/Off attribute of the LED, 2 types of callbacks will be invoked:
  1. MatterPreAttributeChangeCallback - Toggle the LED before updating the On/Off attribute (TBD)
  2. MatterPostAttributeChangeCallback - Toggle the LED after updating the On/Off attribute

These callbacks are defined in `core/matter_interaction.cpp`.
These callbacks will post an event to the uplink queue, which will be handled by `matter_driver_uplink_update_handler` in `matter_drivers.cpp`.
The driver codes will be called to carry out your actions (On/Off LED in this case) depending on the Cluster and Attribute ID received.
You may add clusters and attributes handling in `matter_driver_uplink_update_handler` if they are not present. 

## How to build

### Configurations
`CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_LIGHT` are automatically enabled in the Makefiles.

### Setup the Build Environment
  
    cd connectedhomeip
    source scripts/activate.sh

<details>
  <summary>Building with AmebaDplus</summary>

### AmebaDplus (RTL8721Dx)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| LED        | PB_18 |
| Button     | PA_12 |

#### Build Matter Libraries

    cd ameba-rtos/amebadplus_gcc_project
    make -C project_km4/asdk light_port

#### Build the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project
    make all MATTER_EXAMPLE=light

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
| LED        | PA_31 |
| Button     | PA_29 |

#### Build Matter Libraries

    cd ameba-rtos/amebalite_gcc_project
    make -C project_km4/asdk light_port

#### Build the Final Firmware

    cd ameba-rtos/amebalite_gcc_project
    make all MATTER_EXAMPLE=light

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
| LED        | PA_5  |
| Button     | PA_10 |

#### Build Matter Libraries

    cd ameba-rtos/amebasmart_gcc_project
    make -C project_ap/asdk light_port

#### Build the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project
    make all MATTER_EXAMPLE=light

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/master/README.md#flashing) to flash the image with Windows Image Tool

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebasmart_gcc_project/project_ap
    make clean
</details>
