# Matter Lighting Application Example using Ameba Data Model

This example demonstrates an implementation of the **Matter Bridge** with **Ameba Data Model**

## Ameba Data Model

The **Ameba Data Model** provides a flexible framework for dynamically managing **Matter endpoints** at runtime.
It allows developers to **add, remove, and configure device endpoints** during runtime.

In this example, the Ameba Data Model demonstrates **dynamic endpoint management** for lighting devices:

- A **Root Node** endpoint is initialized on **Endpoint 0** to serve as the primary node descriptor.
- A **Aggregator** endpoint is initialized on **Endpoint 1** to serve as the primary node descriptor.
- A **Dimmable Light** endpoint is created on **Endpoint 2** during startup.
- After a short delay (20 seconds), the **Endpoint 2** light is dynamically removed from the Matter data model.

This showcases how the Ameba Data Model can manage device lifecycles in real time—supporting use cases such as **modular lighting systems**, **expandable appliances**, or **multi-instance devices** within a single Matter node.

### 🧠 Implemented APIs

The following APIs are provided to manage dynamic endpoints within the **Ameba Data Model**.
They handle initialization, addition, activation, and removal of Matter endpoints during runtime.

| **API / Task** | **Purpose** | **Function / Description** |
|----------------|-------------|-----------------------------|
| `matter_root_node_preset()` | Root node initialization | Initializes **Endpoint 0**, which serves as the Matter **Root Node** for the device. |
| `matter_aggregator_preset()` | Aggregator initialization | Initializes **Endpoint 1**, which serves as the **Bridge (Aggregator) Node**, managing and linking bridged devices within the Matter network. |
| `matter_dimmable_light_preset()` | Dimmable light setup | Creates and initializes a **Dimmable Light** endpoint at the next available endpoint ID. |
| `enableAllEndpoints()` | Endpoint activation | Enables all dynamically added endpoints and makes them active within the Matter network. |
| `addEndpoint()` | Add endpoint | Dynamically adds a new endpoint to the device at runtime. |
| `getEndpoint()` | Retrieve endpoint | Returns a reference to an existing endpoint for inspection or modification. |
| `removeEndpoint()` | Remove endpoint | Removes a specific endpoint from the Matter data model at runtime. |

## 📘 ZAP Configuration

- **ZAP File:** `bridge-app.zap`

> **Note:**
> Before implementation, review the Matter Specification to ensure compliance with required device types and cluster configurations.

## 🧩 Endpoint Configuration

This example defines multiple endpoints to represent both the **Bridge (Aggregator)** and its **Bridged Devices**.

| **Endpoint ID** | **Device Name** | **Description** |
|-----------------|-----------------|-----------------|
| **1** | Aggregator | The main **Bridge Node** endpoint responsible for managing and exposing bridged devices to the Matter network. |
| **N** | Bridged Device | Represents an individual bridged device (e.g., Light, Sensor, or Switch) connected and managed through the bridge. |

> **Note:**
> The number of bridged device endpoints (**N**) depends on how many devices are dynamically added and managed by the bridge application.
> You can modify the configuration according to your device requirements.

## 🔧 Supported Clusters

The following clusters are supported by the **Aggregator (Bridge Node)** device type:

| **Cluster Name** | **Function** | **Role** |
|------------------|--------------|-----------|
| **Identify** | Allows the device to be visually or audibly identified during commissioning or testing. | Server |
| **Actions** | Enables the bridge to manage and trigger predefined actions or behaviors across bridged devices. | Server |

> **Note:**
> Depending on the types of bridged devices connected, additional clusters may be supported dynamically.
> The clusters listed above are specific to the **Aggregator (Bridge Node)**.
> You can modify or extend this configuration according to your device requirements.

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
They handle **attribute reporting**, **device state synchronization**, and **hardware event management** between the Matter stack and the bridge node (aggregator).

| **API / Task** | **Purpose** | **Function / Description** |
|----------------|-------------|----------------------------|
| `CallReportingCallback()` | Attribute reporting callback | Invoked by the platform to report attribute changes to the Matter data model. Cleans up allocated reporting paths after completion. |
| `ScheduleReportingCallback()` | Schedule attribute reporting | Creates and schedules a reporting task for a specific cluster and attribute when a bridged device state changes. |
| `HandleDeviceStatusChanged()` | Device status synchronization | Monitors changes in bridged device properties (e.g., *Reachable*, *Name*) and triggers attribute reports accordingly. |
| `HandleDeviceOnOffStatusChanged()` | On/Off status update handler | Detects changes in On/Off state or general device attributes and schedules Matter attribute reporting. |
| `matter_driver_bridge_light_init()` | Button initialization | Configures the physical button input for local light control on the bridged device. |
| `matter_customer_bridge_code()` | LED initialization | Initializes the LED hardware for status indication and sets the default operational state. |
| `matter_driver_uplink_update_handler()` | Matter → Driver event handler | Processes cluster or attribute updates received from the Matter stack (uplink) and applies them to hardware. |
| `matter_driver_downlink_update_handler()` | Driver → Matter event handler | Handles events originating from hardware or external inputs (downlink) and updates the Matter attribute states accordingly. |

---

### 🧩 Summary

| **Component** | **Description** |
|----------------|-----------------|
| **ZAP File** | `bridge-app.zap` |
| **Main Example File** | `example_matter_bridge.cpp` |
| **Main Matter Driver File** | `matter_drivers.cpp` |
| **Main Device Driver File** | `bridge_dm_driver.cpp` |

---

### 💡 Notes

- Adjust the configuration parameters to match your hardware design and application requirements.
- Always keep the **Matter attribute values** synchronized with **hardware state** to ensure accurate reporting to controllers.
- Extend functionality by adding support for additional clusters, sensors, or custom application logic as needed.

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
  <summary><b>Building with AmebaZ2/AmebaZ2plus</b></summary>

### Build Matter Libraries

    cd amebaz2_sdk/project/realtek_amebaXX_v0_example/GCC-RELEASE/
    make bridge_dm

### Build the Final Firmware

    cd amebaz2_sdk/project/realtek_amebaXX_v0_example/GCC-RELEASE/
    make is_matter

### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos-matter/tree/release/main/tools/Image_Tool_Linux/AmebaZ2/README.md) to flash the image with the Linux Image Tool

### Clean Matter Libraries

    cd amebaz2_sdk/project/realtek_amebaXX_v0_example/GCC-RELEASE/
    make clean_matter_libs

### Clean Ameba Matter application

    cd amebaz2_sdk/project/realtek_amebaXX_v0_example/GCC-RELEASE/
    make clean_matter

  </div>
</details>

<details>
  <summary><b>Building with AmebaD</b></summary>

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
    make -C asdk bridge_dm

### Build the Final Firmware

    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp
    make all
    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make all

### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos-matter/tree/release/main/tools/Image_Tool_Linux/AmebaD/README.md) to flash the image with the Linux Image Tool

### Clean Matter Libraries and Firmware

    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make clean

</details>
