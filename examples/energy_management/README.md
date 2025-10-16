# Energy-management-app Example
This example is an implementation of the *Energy Management App*.
This example is demonstrated alongside with *Electric Vehicle Supply Equipment* or *Water Heater* device example.
Note that these driver codes are meant to be just the skeleton, you should replace them and implement your own.

## How it works
The Energy-management-app can be controlled in two ways, by the Matter controller, or by external means. 
In this example, we demonstrate both methods via Matter controller and external means.
If you wish to control by external means, you will need to use the `downlink` task shown in `matter_drivers.cpp`. Please feel free to add more based on your implementations. Meanwhile, controlling with Matter controller will trigger the `uplink` handler.

### Peripheral Initialization
The initializations are handled in `matter_drivers.cpp`.

### Matter Attribute Change Callback
Whenever the Matter controller changes the attribute of the Energy Management cluster, 2 types of callbacks will be invoked:
  1. MatterPreAttributeChangeCallback - Change the status/value before updating the attribute (TBD)
  2. MatterPostAttributeChangeCallback - Change the status/value after updating the attribute

These callbacks are defined in `core/matter_interaction.cpp`.
These callbacks will post an event to the uplink queue, which will be handled by `matter_driver_uplink_update_handler` in `matter_drivers.cpp`.
The driver codes will be called to carry out your actions depending on the Cluster and Attribute ID received.
You may add clusters and attributes handling in `matter_driver_uplink_update_handler` if they are not present. 

## How to build

### Configurations
`CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_ENERGY_MANAGEMENT` are automatically enabled in the Makefiles.

#### Configure Matter Main Feature
Navigate to `project/amebaxxx/Makefile.include.matter`, and enable the two configurations:

```Makefile
# matter timer
GLOBAL_CFLAGS += -DCONFIG_ENABLE_AMEBA_SNTP=1

# ameba TestEvent Trigger EnableKey
GLOBAL_CFLAGS += -DCONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER=1
```

By default, both configurations are disabled.
Energy Management App needs both of the features to be enabled.
Please enable both of them manually in the `Makefile.include.matter`.

#### Configure Device Example
Navigate to `examples/energy_management/matter_drivers.h`, and find the following configurations:

```C
/**
 * @brief  Choose which device should be compiled along the Energy Management example.
 */
#define CONFIG_EXAMPLE_MATTER_EVSE_DEVICE 1
#define CONFIG_EXAMPLE_MATTER_WHM_DEVICE  0
```

By default it will compile the Electric Vehicle Supply Equipment device example.
If Water Heater device wants to be compiled, disable `CONFIG_EXAMPLE_MATTER_EVSE_DEVICE`, then enable `CONFIG_EXAMPLE_MATTER_WHM_DEVICE`.
Only one device example can be compiled at a time, so make sure only one of them is enabled.

#### Configure Device Energy Management Feature support
Navigate to `examples/energy_management/matter_drivers.h`, and find the following configurations:

```C
/**
 * @brief  Choose which DEM feature support should be enabled, or both can be disabled.
 */
#define CONFIG_MATTER_DEM_SUPPORT_POWER_FORECAST_REPORTING 0
#define CONFIG_MATTER_DEM_SUPPORT_STATE_FORECAST_REPORTING 0
```

By default, both are disabled.
Only one DEM feature support can be enabled at a time, so make sure only one of them is enabled.

### Setup the Build Environment
  
    cd connectedhomeip
    source scripts/activate.sh

<details>
  <summary>Building with AmebaDplus</summary>

### AmebaDplus (RTL8721Dx)

#### Build Matter Libraries

    cd ameba-rtos/amebadplus_gcc_project
    make -C project_km4/asdk energy_management_port

#### Build the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project
    make all MATTER_EXAMPLE=energy_management

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

    cd ameba-rtos/amebalite_gcc_project
    make -C project_km4/asdk energy_management_port

#### Build the Final Firmware

    cd ameba-rtos/amebalite_gcc_project
    make all MATTER_EXAMPLE=energy_management

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

    cd ameba-rtos/amebasmart_gcc_project
    make -C project_ap/asdk energy_management_port

#### Build the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project
    make all MATTER_EXAMPLE=energy_management

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/master/README.md#flashing) to flash the image with Windows Image Tool

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebasmart_gcc_project/project_ap
    make clean
</details>
