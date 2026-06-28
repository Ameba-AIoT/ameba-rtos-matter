# Matter Lighting Application Example using Ameba Data Model

This example demonstrates an implementation of the **Matter Dynamic Endpoint** with **Ameba Data Model**

The Matter Dynamic Endpoint allows user to set the device type during runtime.

Each device type has its own cluster requirements.
Please refer to the **Matter Specification** for detailed information on supported attributes and features.

## Ameba Data Model

The **Ameba Data Model** provides a flexible framework for dynamically managing **Matter endpoints** at runtime.
It allows developers to **add, remove, and configure device endpoints** during runtime.

In this example, the Ameba Data Model demonstrates **dynamic endpoint management** for lighting devices:

- A **Root Node** endpoint is initialized on **Endpoint 0** to serve as the primary node descriptor.
- A **Placeholder** endpoint is created on **Endpoint 1** which is for enabling the supported clusters that will be used for the dynamic endpoint creation.

This showcases how the Ameba Data Model can manage device lifecycles in real time—supporting use cases such as **modular lighting systems**, **expandable appliances**, or **multi-instance devices** within a single Matter node.

### Fixed and Dynamic Endpoint

|-------------|-------------------------|------------------|
|  Allocation |         Fixed           |     Dynamic      |
|-------------|-------------------------|------------------|
|  EndpointID |     0     |      1      |      2,3,4,..    |
| Device Type | Root Node | Placeholder |      Device      |
|    Status   |  Enabled  |   Disabled  | Enabled/Disabled |
|-------------|-------------------------|------------------|

- The ZAP-CLI will generate 2 Fixed Endpoints based on the ZAP file.
- Since there are 2 endpoints, FIXED_ENDPOINT_COUNT will be set to 2.
- However, the last fixed endpoint is Placeholder, so LAST_FIXED_ENDPOINT_ID is 1.
  - Placeholder for all of the supported clusters so that ZAP will generate the requisite code.
- First dynamic endpoint will be allocated right after the last fixed endpoint, so FIRST_DYNAMIC_ENDPOINT_ID is 2.
- Because the placeholder endpoint is going to be disabled, so ENABLED_FIXED_ENDPOINT_COUNT is 1.
- The last enabled fixed endpoint becomes the parent of the first dynamic endpoint, which is the Root Node.
- So, DYNAMIC_ENDPOINTS_FIRST_PARENT_ENDPOINT_ID is 0.

### 🧠 Implemented APIs

The following APIs are provided to manage dynamic endpoints within the **Ameba Data Model**.  
They handle initialization, addition, activation, and removal of Matter endpoints during runtime.

| **API / Task** | **Purpose** | **Function / Description** |
|----------------|-------------|-----------------------------|
| `enableAllEndpoints()` | Endpoint activation | Enables all dynamically added endpoints and makes them active within the Matter network. |
| `addEndpoint()` | Add endpoint | Dynamically adds a new endpoint to the device at runtime. |
| `getEndpoint()` | Retrieve endpoint | Returns a reference to an existing endpoint for inspection or modification. |
| `removeEndpoint()` | Remove endpoint | Removes a specific endpoint from the Matter data model at runtime. |

## 📘 ZAP Configuration

- **ZAP File:** `dynamic-endpoint-app.zap`

> **Note:**
> Before implementation, review the Matter Specification to ensure compliance with required device types and cluster configurations.

## 🧩 Endpoint Configuration

The example defines **one endpoints**:

| **Endpoint ID** | **Device Name**        | **Description** |
|-----------------|------------------------|-----------------|
| **1** | Temporary Device Type acting as the placeholder | Includes all the supported clusters that will be used for adding dynamic endpoint |

> **Note:**
> You can modify the configuration according to your device requirements.

## 🔧 Supported Clusters

The supported clusters SHALL depends on the device type that is being created during runtime.

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
| `matter_driver_on_identify_start()` | Identify start | Notifies that the identify operation has started |
| `matter_driver_on_identify_stop()` | Identify stop | Notifies that the identify operation has stopped |
| `matter_driver_on_trigger_effect()` | Identify device | Triggers a visual or functional effect to identify the device |
| `matter_driver_uplink_update_handler()` | Matter → Driver event handler | Processes cluster/attribute changes from the Matter stack (uplink) and updates hardware peripherals |
| `matter_driver_downlink_update_handler()` | Driver → Matter event handler | Processes hardware or external input events (downlink) and updates Matter attributes |

---

### 🧩 Summary

| **Component** | **Description** |
|----------------|-----------------|
| **ZAP File** | `dynamic-endpoint-app.zap` |
| **Main Example File** | `example_matter_dynamic_endpoint_dm.cpp` |
| **Main Matter Driver File** | `matter_drivers.cpp` |

---

### 💡 Notes

- Adjust the configuration parameters to match your hardware design and application requirements.
- Always keep the **Matter attribute values** synchronized with **hardware state** to ensure accurate reporting to controllers.
- Extend functionality by adding support for additional clusters, sensors, or custom application logic as needed.

## How to build

### Configurations
Enable `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_DYNAMIC_ENDPOINT` in `platform_opts_matter.h`.
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
    make dynamic_endpoint_dm

### Build the Final Firmware

    cd amebaz2_sdk/project/realtek_amebaXX_v0_example/GCC-RELEASE/
    make is_matter

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
    make -C asdk dynamic_endpoint_dm
    
### Build the Final Firmware

    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp
    make all
    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make all

### Clean Matter Libraries and Firmware

    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make clean

</details>

### Flash the Image
Refer to this [guide](../../tools/Image_Tool_Linux/README.md) to flash the image with the Linux Image Tool
