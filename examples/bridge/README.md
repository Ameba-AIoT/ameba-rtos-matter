# Bridge-app Example
This example is an implementation of the *Bridge* device type. The Bridge will be communicating with the other non-Matter peripherals via TCP sockets.
You will need 2 non-Matter peripherals running TCP client socket.

### Dynamic Endpoints
The Bridge Example makes use of Dynamic Endpoints. Current SDK support is limited for dynamic endpoints, since endpoints are typically defined (along with the clusters and attributes they contain) in a .zap file which then generates code and static structures to define the endpoints. 

To support endpoints that are not statically defined, the ZCL attribute storage mechanisms will hold additional endpoint information for `NUM_DYNAMIC_ENDPOINTS` additional endpoints. These additional endpoint structures must be defined by the application and can change at runtime. 

To facilitate the creation of these endpoint structures, several macros are
defined:

`DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(attrListName)`
`DECLARE_DYNAMIC_ATTRIBUTE(attId, attType, attSizeBytes, attrMask)`
`DECLARE_DYNAMIC_ATTRIBUTE_LIST_END(clusterRevision)`

-   These three macros are used to declare a list of attributes for use within a
    cluster. The declaration must begin with the
    `DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN` macro which will define the name of
    the allocated attribute structure. Each attribute is then added by the
    `DECLARE_DYNAMIC_ATTRIBUTE` macro. Finally,
    `DECLARE_DYNAMIC_ATTRIBUTE_LIST_END` macro should be used to close the
    definition.

-   All attributes defined with these macros will be configured as
    `ATTRIBUTE_MASK_EXTERNAL_STORAGE` in the ZCL database and therefore will
    rely on the application to maintain storage for the attribute. Consequently,
    reads or writes to these attributes must be handled within the application
    by the `emberAfExternalAttributeWriteCallback` and
    `emberAfExternalAttributeReadCallback` functions. See the bridge
    application's `main.cpp` for an example of this implementation.

`DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(clusterListName)`
`DECLARE_DYNAMIC_CLUSTER(clusterId, clusterAttrs, incomingCommands, outgoingCommands)`
`DECLARE_DYNAMIC_CLUSTER_LIST_END`

-   These three macros are used to declare a list of clusters for use within a
    endpoint. The declaration must begin with the
    `DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN` macro which will define the name of the
    allocated cluster structure. Each cluster is then added by the
    `DECLARE_DYNAMIC_CLUSTER` macro referencing attribute list previously
    defined by the `DECLARE_DYNAMIC_ATTRIBUTE...` macros and the lists of
    incoming/outgoing commands terminated by kInvalidCommandId (or nullptr if
    there aren't any commands in the list). Finally,
    `DECLARE_DYNAMIC_CLUSTER_LIST_END` macro should be used to close the
    definition.

`DECLARE_DYNAMIC_ENDPOINT(endpointName, clusterList)`

-   This macro is used to declare an endpoint and its associated cluster list,
    which must be previously defined by the `DECLARE_DYNAMIC_CLUSTER...` macros.

### Limitations

Because code generation is dependent upon the clusters and attributes defined in
the .zap file (for static endpoint generation), it is necessary to include a
defined endpoint within the .zap that contains _all_ the clusters that may be
used on dynamic endpoints. On the bridge example, this is done on endpoint 2,
which is used as a 'dummy' endpoint that will be disabled at runtime. Endpoint 0
is also defined in the .zap and contains the root descriptor cluster. Endpoint 1
contains the bridge basic and configuration cluster.

### How it Works

The example demonstrates the use of dynamic endpoints and the concept of adding
and removing endpoints at runtime. First, the example declares a
`bridgedLightEndpoint` data structure for a Light endpoint with `OnOff`,
`Descriptor`, `BridgedDeviceBasicInformation`, and `FixedLabel` clusters.

Using this declared endpoint structure, two endpoints for two bridged lights
are dynamically added at endpoint ID's `3`, and `4`, representing
`Light 1` and `Light 2` respectively.

All endpoints populate the `Bridged Device Basic Information` and `Fixed Label`
clusters. In the `Bridged Device Basic Information` cluster, the `reachable`
attribute is simulated. In the `Fixed Label` cluster, the `LabelList` attribute
is simulated with the value/label pair `"room"`/`[light name]`.

### Server Initialization
The TCP server is initialized using `matter_driver_bridge_setup_server()`.
This will create a thread that will initialize the bridge table that will store the connections with the respective endpoints.
After that, it will wait (blocking) for client devices to connect and update the bridge table accordingly.

### Adding New Devices
To connect a new device to the bridge, initialize the bridge object (see `matter_driver_bridge_light_init()` for reference) and setup the endpoint with the macros described above.
Then invoke `AddDeviceEndpoint()`, passing in the necessary object, endpoint, device type and other arguments.

### Removing Devices
To remove a device from the bridge, simply invoke `RemoveDeviceEndpoint()`, passing in the bridge device object of the device you want to remove.

## Interaction with Bridged Devices
The Bridge will handle and store the attributes of the Bridged Devices. This is implemented in `emberAfExternalAttributeReadCallback()` and `emberAfExternalAttributeWriteCallback()`.

### Bridge to Bridged Devices
When the Bridge receives a **Read** request for an attribute of one of the Bridged Devices, it will return the attribute value stored in its own database. 

When the Bridge receives a **Write** request for an attribute of one of the Bridged Devices, the handler will update the attribute on its database and will send a command via TCP socket to the actual Bridged Device, which will then carry out necessary actions like turning on an LED.

### Bridged Devices to Bridge
Whenever there is a change of state in the actual Bridged Devices, the Bridged should be informed of this change. This is still work in progress.

## How to build

### Configurations
Enable `CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_BRIDGE` in `platform_opts_matter.h`.
Ensure that `CONFIG_EXAMPLE_MATTER_CHIPTEST` is disabled.

### Setup the Build Environment

    cd connectedhomeip
    source scripts/activate.sh

<details>
  <summary>Building with AmebaD</summary>

### AmebaD (RTL8721D)

#### Build Matter Libraries

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make -C asdk bridge_port

#### Build the Final Firmware

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp
    make all
    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make all

#### Clean Matter Libraries and Firmware

    cd ameba-rtos-d/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp
    make clean

</details>
