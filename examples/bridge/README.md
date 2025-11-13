# Matter Bridge Example

This example demonstrates an implementation of the **Matter Bridge** device type.
The Bridge will be communicating with the other non-Matter peripherals via TCP sockets. You will need 2 non-Matter peripherals running TCP client socket.
Furthermore, it uses **Dynamic Endpoint** to allocate/remove endpoint to each bridged node.

## Dynamic Endpoints

In this example it is limited for dynamic endpoints with endpoint device type statically defined in the zap file, which then generates code and static structures. 
To support endpoints that are not statically defined, please check **bridge_dm** example.

## Macros Definition

To facilitate the creation of these endpoint structures, several macros are defined:

`DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(attrListName)`
`DECLARE_DYNAMIC_ATTRIBUTE(attId, attType, attSizeBytes, attrMask)`
`DECLARE_DYNAMIC_ATTRIBUTE_LIST_END(clusterRevision)`

-   These three macros are used to declare a list of attributes for use within a cluster.
    The declaration must begin with the `DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN` macro which will define the name of the allocated attribute structure.
    Each attribute is then added by the `DECLARE_DYNAMIC_ATTRIBUTE` macro. Finally, `DECLARE_DYNAMIC_ATTRIBUTE_LIST_END` macro should be used to close the definition.

-   All attributes defined with these macros will be configured as `ATTRIBUTE_MASK_EXTERNAL_STORAGE` in the ZCL database and therefore will rely on the application to maintain storage for the attribute. 
    Consequently, reads or writes to these attributes must be handled within the application by the `emberAfExternalAttributeWriteCallback` and `emberAfExternalAttributeReadCallback` functions.
    See the bridge application's `main.cpp` for an example of this implementation.

`DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(clusterListName)`
`DECLARE_DYNAMIC_CLUSTER(clusterId, clusterAttrs, incomingCommands, outgoingCommands)`
`DECLARE_DYNAMIC_CLUSTER_LIST_END`

-   These three macros are used to declare a list of clusters for use within a endpoint.
    The declaration must begin with the `DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN` macro which will define the name of the allocated cluster structure.
    Each cluster is then added by the `DECLARE_DYNAMIC_CLUSTER` macro referencing attribute list previously defined by the `DECLARE_DYNAMIC_ATTRIBUTE...` macros and the lists of incoming/outgoing commands terminated by kInvalidCommandId (or nullptr if there aren't any commands in the list).
    Finally, `DECLARE_DYNAMIC_CLUSTER_LIST_END` macro should be used to close the definition.

`DECLARE_DYNAMIC_ENDPOINT(endpointName, clusterList)`

-   This macro is used to declare an endpoint and its associated cluster list, which must be previously defined by the `DECLARE_DYNAMIC_CLUSTER...` macros.

### Limitations

Because code generation is dependent upon the clusters and attributes defined in the .zap file (for static endpoint generation), it is necessary to include a defined endpoint within the .zap that contains _all_ the clusters that may be used on dynamic endpoints.
On the bridge example, this is done on endpoint 2, which is used as a 'dummy' endpoint that will be disabled at runtime.
- Endpoint 0 is also defined in the .zap and contains the root descriptor cluster.
- Endpoint 1 contains the bridge basic and configuration cluster.

## 📘 ZAP Configuration

- **ZAP File:** `thermostat-app.zap`

> **Note:**
> Before implementation, review the Matter Specification to ensure compliance with required device types and cluster configurations.

## 🧩 Endpoint Configuration

The example defines **one endpoints**:

| **Endpoint ID** | **Device Name**        | **Description** |
|-----------------|------------------------|-----------------|
| **1** | Aggregator | The main **Bridge Node** endpoint responsible for managing and exposing bridged devices to the Matter network. |
| **N** | Bridged Device | Represents an individual bridged device (e.g., Light, Sensor, or Switch) connected and managed through the bridge. |

> **Note:**
> You can modify the configuration according to your device requirements.

## 🔧 Supported Clusters

The following clusters are supported by the **Aggregator (Bridge Node)** device type:

| **Cluster Name** | **Function** | **Role** |
|------------------|--------------|-----------|
| **Identify** | Allows the device to be visually or audibly identified during commissioning or testing. | Server |
| **Actions** | Enables the bridge to manage and trigger predefined actions or behaviors across bridged devices. | Server |

> **Note:**
> Depending on the types of bridged devices connected.
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
| `AddDeviceEndpoint()` | Add bridged device | Adds a new bridged device endpoint to the bridge table and initializes clusters. |
| `RemoveDeviceEndpoint()` | Remove bridged device | Removes an existing bridged device endpoint and frees associated resources. |
| `HandleReadBridgedDeviceBasicAttribute()` | Read basic attributes | Reads standard attributes (e.g., `NodeLabel`, `Reachable`) from the bridged device. |
| `HandleReadOnOffAttribute()` | Read On/Off state | Reads the current On/Off state from the bridged device. |
| `HandleWriteOnOffAttribute()` | Write On/Off state | Updates the On/Off state on the bridged device and triggers hardware action. |
| `emberAfExternalAttributeReadCallback()` | External attribute read | Handles read requests for attributes stored externally by the bridge application. |
| `emberAfExternalAttributeWriteCallback()` | External attribute write | Handles write requests for attributes stored externally by the bridge application. |
| `matter_driver_bridge_send_callback()` | Command forwarding | If the Matter controller sends any command, this API forwards it to the bridge device. |
| `CallReportingCallback()` | Attribute reporting callback | Invoked by the platform to report attribute changes to the Matter data model. Cleans up allocated reporting paths after completion. |
| `ScheduleReportingCallback()` | Schedule attribute reporting | Creates and schedules a reporting task for a specific cluster and attribute when a bridged device state changes. |
| `HandleDeviceStatusChanged()` | Device status synchronization | Monitors changes in bridged device properties (e.g., *Reachable*, *Name*) and triggers attribute reports accordingly. |
| `matter_driver_bridge_light_init()` | Button initialization | Configures the physical button input for local light control on the bridged device. |
| `matter_driver_bridge_endpoint_assign()` | Endpoint assignment | Assigns a Matter endpoint to a bridged device. |
| `matter_driver_bridge_recv_thread()` | Receive thread | Continuously checks for attribute updates from bridged devices via TCP sockets. |
| `matter_driver_bridge_server_thread()` | Server thread | Continuously monitors incoming connections and accepts socket requests from bridged devices. |
| `matter_driver_bridge_setup_server()` | Server setup | Initializes the TCP server and starts the server thread. |
| `matter_driver_bridge_recv_server()` | Receive server | Starts the receive thread to process updates from bridged devices. |
| `matter_driver_uplink_update_handler()` | Matter → Driver event handler | Processes cluster or attribute updates received from the Matter stack (uplink) and applies them to hardware. |
| `matter_driver_downlink_update_handler()` | Driver → Matter event handler | Handles events originating from hardware or external inputs (downlink) and updates the Matter attribute states accordingly. |

---

### 🧩 Summary

| **Component** | **Description** |
|----------------|-----------------|
| **ZAP File** | `bridge-app.zap` |
| **Main Example File** | `example_matter_bridge.cpp` |
| **Main Matter Driver File** | `matter_drivers.cpp` |
| **Main Device Driver File** | `bridge_driver.cpp` |
| **Cluster Implementation File** | `ameba_bridged_actions_stubs.cpp` |

---

### 💡 Notes

- Adjust the configuration parameters to match your hardware design and application requirements.
- Always keep the **Matter attribute values** synchronized with **hardware state** to ensure accurate reporting to controllers.
- Extend functionality by adding support for additional clusters, sensors, or custom application logic as needed.

## How to build

### Configurations
Enable `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_BRIDGE` in `platform_opts_matter.h`.
Ensure that `CONFIG_EXAMPLE_MATTER_CHIPTEST` is disabled.

### Setup the Build Environment

    cd connectedhomeip
    source scripts/activate.sh

<details>
  <summary><b>Building with AmebaD</b></summary>

### Build Matter Libraries

    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make -C asdk bridge_port
    
### Build the Final Firmware

    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp
    make all
    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make all

### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos-matter/tree/release/v1.4.2/tools/Image_Tool_Linux/AmebaD/README.md) to flash the image with the Linux Image Tool

### Clean Matter Libraries and Firmware

    cd amebad_sdk/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make clean

</details>