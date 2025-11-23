# Matter Lighting Application Example using Ameba Data Model

This example demonstrates an implementation of the **Matter Lighting Application** with **Ameba Data Model**

The Matter Lighting Application can represent several lighting device types:

1. **On/Off Light**
2. **Dimmable Light**
3. **Color Temperature Light**
4. **Extended Color Light**

Each device type has its own cluster requirements.
Please refer to the **Matter Specification** for detailed information on supported attributes and features.

## Ameba Data Model

The **Ameba Data Model** provides a flexible framework for dynamically managing **Matter endpoints** at runtime.
It allows developers to **add, remove, and configure device endpoints** during runtime.

In this example, the Ameba Data Model demonstrates **dynamic endpoint management** for lighting devices:

- A **Root Node** endpoint is initialized on **Endpoint 0** to serve as the primary node descriptor.
- A **Dimmable Light** endpoint is created on **Endpoint 1** during startup.
- After a short delay (20 seconds), a second **Dimmable Light** endpoint is dynamically added on **Endpoint 2**.
- After another delay (20 seconds), the **Endpoint 2** light is dynamically removed from the Matter data model.

This showcases how the Ameba Data Model can manage device lifecycles in real time—supporting use cases such as **modular lighting systems**, **expandable appliances**, or **multi-instance devices** within a single Matter node.

### 🧠 Implemented APIs

The following APIs are provided to manage dynamic endpoints within the **Ameba Data Model**.  
They handle initialization, addition, activation, and removal of Matter endpoints during runtime.

| **API / Task** | **Purpose** | **Function / Description** |
|----------------|-------------|-----------------------------|
| `matter_root_node_preset()` | Root node initialization | Initializes **Endpoint 0**, which serves as the Matter **Root Node** for the device. |
| `matter_dimmable_light_preset()` | Dimmable light setup | Creates and initializes a **Dimmable Light** endpoint at the next available endpoint ID. |
| `enableAllEndpoints()` | Endpoint activation | Enables all dynamically added endpoints and makes them active within the Matter network. |
| `addEndpoint()` | Add endpoint | Dynamically adds a new endpoint to the device at runtime. |
| `getEndpoint()` | Retrieve endpoint | Returns a reference to an existing endpoint for inspection or modification. |
| `removeEndpoint()` | Remove endpoint | Removes a specific endpoint from the Matter data model at runtime. |

## 📘 ZAP Configuration

- **ZAP File:** `lighting-app.zap`
  Located under `connectedhomeip/examples/lighting-app/lighting-common/lighting-app.zap`
- **Device Type IDs:**
  - On/Off Light – `0x0100`
  - Dimmable Light – `0x0101`
  - Color Temperature Light – `0x010C`
  - Extended Color Light – `0x010D`

> **Note:**
> Before implementation, review the Matter Specification to ensure compliance with required device types and cluster configurations.

## 🧩 Endpoint Configuration

The example defines **one endpoints**:

| **Endpoint ID** | **Device Name**        | **Description** |
|-----------------|------------------------|-----------------|
| **1** | Lighting App | Main functional endpoint for the lighting device |

> **Note:**
> You can modify the configuration according to your device requirements.

## 🔧 Supported Clusters

The following clusters can be supported by this device type:

| **Cluster Name** | **Function** | **Role**  |
|------------------|--------------|-----------|
| **Identify** | Allows the device to be visually or audibly identified during commissioning | Server |
| **On/Off** | Powers the device on or off | Server |
| **Groups** | Enables the device to be controlled as part of a group | Server |
| **Level Control** | Controls the brightness level (dimming) | Server |
| **Scenes** | Enables storing, recalling, and managing predefined device states or settings to create custom scenes | Server |
| **Color Control** | Controls light color or color temperature | Server |

> **Note:**
> You can modify the configuration according to your device requirements.

## Example Implementation

### 🔁 Attribute Change Handling

The **Attribute Change Handling** layer manages synchronization between the Matter data model and the hardware drivers.
It ensures that updates from the Matter controller or from external inputs remain consistent across the entire system.

There are two modes of control:

1. **Matter Controller** – Controlled through a connected Matter controller (e.g., mobile app, hub, or test tool).
2. **External Control** – Managed through a physical input, such as a button press, with real-time state synchronization to Matter.

#### 🧭 Matter Controller–Driven Updates

When the Matter controller updates an attribute, two callbacks are invoked:

1. **`MatterPreAttributeChangeCallback`** – Triggered *before* the attribute is updated.
   Use this for validation, filtering, or pre-update logic.

2. **`MatterPostAttributeChangeCallback`** – Triggered *after* the attribute has been written to the Matter data model.

`MatterPostAttributeChangeCallback` is defined in **`core/matter_interaction.cpp`**.
They post events to the **uplink queue**, which are then handled by **`matter_driver_uplink_update_handler`** in **`matter_drivers.cpp`**.

The handler interprets the **Cluster ID** and **Attribute ID**, then performs the corresponding driver action.

> **Note:**
> Modify the action to be taken in `matter_driver_uplink_update_handler` for application-specific logic.

#### 🔘 External or Physical Control

External hardware inputs (e.g., buttons or sensors) can also modify attributes within the Matter stack.

When a button is pressed, the **GPIO interrupt handler** posts an event to the **downlink queue**, which is processed by **`matter_driver_downlink_update_handler`**.

**Implementation Steps:**

1. In **`matter_drivers.cpp`**, configure the GPIO pin.
2. In the GPIO interrupt callback , create and post an event to the downlink queue.
3. In **`matter_driver_downlink_update_handler`**, define how the event modifies Matter attributes.

> **Example Use Case:**
> A button press toggles the **On/Off** cluster state locally, and the change is propagated to all connected Matter controllers.

This ensures full synchronization between **physical inputs** and **digital Matter control**, maintaining consistent device state across all interfaces.

---

### 🧠 Implemented Driver APIs

The following APIs are implemented in **`matter_drivers.cpp`**.
They provide the main interfaces for peripheral initialization, event handling, and attribute synchronization between the Matter stack and device hardware.

| **API / Task** | **Purpose** | **Function / Description** |
|----------------|-------------|----------------------------|
| `matter_driver_button_callback()` | Button event callback | Registers a callback function to handle physical button presses to control lighting features |
| `matter_driver_button_init()` | Button initialization | Configures the physical button input used for local light control |
| `matter_driver_led_init()` | LED initialization | Initializes the LED hardware and sets its default state |
| `matter_driver_led_set_startup_value()` | Startup configuration | Sets initial values for the lighting clusters and synchronizes Matter attributes with the device’s hardware state |
| `matter_driver_on_identify_start()` | Identify start | Notifies that the identify operation has started |
| `matter_driver_on_identify_stop()` | Identify stop | Notifies that the identify operation has stopped |
| `matter_driver_on_trigger_effect()` | Identify device | Triggers a visual or functional effect to identify the device |
| `matter_driver_uplink_update_handler()` | Matter → Driver event handler | Processes cluster/attribute changes from the Matter stack (uplink) and updates hardware peripherals |
| `matter_driver_downlink_update_handler()` | Driver → Matter event handler | Processes hardware or external input events (downlink) and updates Matter attributes |

---

### 🧩 Summary

| **Component** | **Description** |
|----------------|-----------------|
| **ZAP File** | `lighting-app.zap` |
| **Main Example File** | `example_matter_light.cpp` |
| **Main Matter Driver File** | `matter_drivers.cpp` |
| **Main Device Driver File** | `led_driver.cpp` |

---

### 💡 Notes

- Adjust the configuration parameters to match your hardware design and application requirements.
- Always keep the **Matter attribute values** synchronized with **hardware state** to ensure accurate reporting to controllers.
- Extend functionality by adding support for additional clusters, sensors, or custom application logic as needed.

## How to build

### Configurations
Enable `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_LIGHT` in `platform_opts_matter.h`.
Ensure that `CONFIG_EXAMPLE_MATTER_CHIPTEST` is disabled.

### Setup the Build Environment

    cd connectedhomeip
    source scripts/activate.sh

<details>
  <summary><b>Building with AmebaZ2/AmebaZ2plus</b></summary>

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| LED        | PA_23 |
| Button     | PA_17 |

### Build Matter Libraries

    cd amebaz2_sdk/project/realtek_amebaXX_v0_example/GCC-RELEASE/
    make light_port

### Build the Final Firmware

    cd amebaz2_sdk/project/realtek_amebaXX_v0_example/GCC-RELEASE/
    make is_matter

### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos-matter/tree/release/v1.4/tools/Image_Tool_Linux/AmebaZ2/README.md) to flash the image with the Linux Image Tool

### Clean Matter Libraries

    cd amebaz2_sdk/project/realtek_amebaXX_v0_example/GCC-RELEASE/
    make clean_matter_libs

### Clean Ameba Matter application

    cd amebaz2_sdk/project/realtek_amebaXX_v0_example/GCC-RELEASE/
    make clean_matter

</details>

<details>
  <summary><b>Building with AmebaD</b></summary>

### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| LED        | PB_5  |
| Button     | PA_12 |

### PSRAM usage
Due to insufficient memory in SRAM, we will use the PSRAM for dynamic allocation.
To run this example without error, you need to enable PSRAM.
In `rtl8721dhp_intfcfg.c`, set the below configurations

    PSRAMCFG_TypeDef psram_dev_config = {
      .psram_dev_enable = TRUE,          //enable psram
      .psram_dev_cal_enable = TRUE,      //enable psram calibration function
      .psram_dev_retention = TRUE,
    }

### Build Matter Libraries

    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make -C asdk light_port
    
### Build the Final Firmware

    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp
    make all
    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make all

### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos-matter/tree/release/v1.4/tools/Image_Tool_Linux/AmebaD/README.md) to flash the image with the Linux Image Tool

### Clean Matter Libraries and Firmware

    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make clean

</details>