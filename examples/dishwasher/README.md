# Matter Dishwasher Example

This example demonstrates an implementation of the **Matter Dishwasher** device type.

## 📘 ZAP Configuration

- **ZAP File:** `dishwasher.zap`
- **Device Type ID:** `0x0075` (Matter Dishwasher)

> **Note:**
> Before implementation, review the **Matter Specification** to ensure compliance with required device types and cluster configurations.

## 🧩 Endpoint Configuration

This example defines **one endpoints**:

| **Endpoint ID** | **Device Name** | **Description** |
|-----------------|-----------------|-----------------|
| **1** | Dishwasher | Main functional endpoint for the dishwasher device |

> **Note:**
> You can modify the configuration according to your device requirements.

## 🔧 Supported Clusters

The following clusters can be supported by this device type:

| **Cluster Name** | **Function** | **Role** |
|------------------|--------------|-----------|
| **Identify** | Allows the device to be visually or audibly identified during commissioning | Server |
| **On/Off** | Powers the device on or off | Server |
| **Temperature Control** | Controls and reports temperature settings | Server |
| **Dishwasher Mode** | Defines available dishwasher modes | Server |
| **Dishwasher Alarm** | Handles fault and warning notifications | Server |
| **Operational State** | Reports current operational state | Server |

> **Note:**
> The configuration can be customized based on your device's features.

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
| `matter_driver_dishwasher_init()` | Device initialization | Initializes the device hardware for **Dishwasher** device |
| `matter_driver_dishwasher_set_startup_value()` | Startup configuration | Sets initial values for the dishwasher clusters and synchronizes Matter attributes with the device’s hardware state |
| `matter_driver_set_onoff_callback()` | Power control callback | Registers a callback function to handle **On/Off** cluster |
| `matter_driver_set_opstate_state_callback()` | Operational state handler | Registers a callback function to handle **Operational State** cluster **OperationalState** attribute |
| `matter_driver_set_opstate_error_callback()` | Error state handler | Registers a callback to handle **Operational State** cluster **OperationalError** attribute |
| `matter_driver_set_dishwasher_mode_callback()` | Mode control callback | Registers a callback to handle **Dishwasher Mode** cluster |
| `matter_driver_set_dishwasher_alarm_callback()` | Alarm event handler | Registers a callback to handle **Dishwasher Alarm** cluster |
| `matter_driver_set_temperature_callback()` | Temperature update handler | Registers a callback to handle **Temperature Control** cluster |
| `matter_driver_on_identify_start()` | Identify start | Notifies that the identify operation has started |
| `matter_driver_on_identify_stop()` | Identify stop | Notifies that the identify operation has stopped |
| `matter_driver_on_trigger_effect()` | Identify device | Triggers a visual or functional effect to identify the device |
| `matter_driver_uplink_update_handler()` | Matter → Driver event handler | Processes cluster/attribute changes from the Matter stack (uplink) and updates hardware peripherals |
| `matter_driver_downlink_update_handler()` | Driver → Matter event handler | Processes hardware or external input events (downlink) and updates Matter attributes |

---

### 🧩 Summary

| **Component** | **Description** |
|----------------|-----------------|
| **ZAP File** | `dishwasher.zap` |
| **Main Example File** | `example_matter_dishwasher.cpp` |
| **Main Matter Driver File** | `matter_drivers.cpp` |
| **Main Device Driver File** | `dishwasher_driver.cpp` |
| **Cluster Implementation File** | `ameba_dishwasher_mode.cpp` `ameba_operational_state_delegate_impl` |

---

### 💡 Notes

- Adjust the configuration parameters to match your hardware design and application requirements.
- Always keep the **Matter attribute values** synchronized with **hardware state** to ensure accurate reporting to controllers.
- Extend functionality by adding support for additional clusters, sensors, or custom application logic as needed.

## How to build

### Configurations
Enable `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_DISHWASHER` in `platform_opts_matter.h`.
Ensure that `CONFIG_EXAMPLE_MATTER_CHIPTEST` is disabled.

### Setup the Build Environment

    cd connectedhomeip
    source scripts/activate.sh

<details>
  <summary><b>Building with AmebaZ2/AmebaZ2plus</b></summary>

### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| Dishwasher | PA_23 |

### Build Matter Libraries

    cd amebaz2_sdk/project/realtek_amebaXX_v0_example/GCC-RELEASE/
    make dishwasher_port

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
| Dishwasher | PB_5 |

### Build Matter Libraries

    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make -C asdk dishwasher_port

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