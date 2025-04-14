#include <matter_drivers.h>
#include <matter_interaction.h>
#include <bridge_driver.h>
#include <lwip/sockets.h>
#include <wifi_conf.h>

#include <app/CommandHandler.h>
#include <app/ConcreteAttributePath.h>
#include <app/reporting/reporting.h>
#include <app/util/attribute-storage.h>
#include <app/util/endpoint-config-api.h>
#include <app/util/endpoint-config-defines.h>
#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <lib/support/ZclString.h>
#include <protocols/interaction_model/StatusCode.h>

using namespace ::chip;
using namespace ::chip::DeviceLayer;
using namespace ::chip::Platform;
using namespace ::chip::app::Clusters;
using chip::Protocols::InteractionModel::Status;

#if defined(CONFIG_PLATFORM_8710C)
#define PWM_PIN         PA_23
#define GPIO_IRQ_PIN    PA_17
#elif defined(CONFIG_PLATFORM_8721D)
#define PWM_PIN         PB_5
#define GPIO_IRQ_PIN    PA_12
#elif defined (CONFIG_AMEBASMART)
#define PWM_PIN         PA_5
#define GPIO_IRQ_PIN    PA_10
#elif defined (CONFIG_AMEBALITE)
#define PWM_PIN         PA_31
#define GPIO_IRQ_PIN    PA_29
#elif defined (CONFIG_AMEBADPLUS)
#define PWM_PIN         PB_18
#define GPIO_IRQ_PIN    PA_12
#endif

MatterBridge ALight1;
MatterBridge ALight2;

static const int kNodeLabelSize = 32;
// Current ZCL implementation of Struct uses a max-size array of 254 bytes
static const int kDescriptorAttributeArraySize = 254;

static EndpointId gCurrentEndpointId;
static EndpointId gFirstDynamicEndpointId;
static MatterBridge *gDevices[CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT];

// (taken from chip-devices.xml)
#define DEVICE_TYPE_BRIDGED_NODE 0x0013
// (taken from lo-devices.xml)
#define DEVICE_TYPE_LO_ON_OFF_LIGHT 0x0100

// (taken from chip-devices.xml)
#define DEVICE_TYPE_ROOT_NODE 0x0016
// (taken from chip-devices.xml)
#define DEVICE_TYPE_BRIDGE 0x000e

// Device Version for dynamic endpoints:
#define DEVICE_VERSION_DEFAULT 1

/* BRIDGED DEVICE ENDPOINT: contains the following clusters:
   - On/Off
   - Descriptor
   - Bridged Device Basic Information
*/

// Declare On/Off cluster attributes
DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(onOffAttrs)
DECLARE_DYNAMIC_ATTRIBUTE(OnOff::Attributes::OnOff::Id, BOOLEAN, 1, 0), /* on/off */
                          DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

// Declare Descriptor cluster attributes
DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(descriptorAttrs)
DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::DeviceTypeList::Id, ARRAY, kDescriptorAttributeArraySize, 0), /* device list */
                          DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::ServerList::Id, ARRAY, kDescriptorAttributeArraySize, 0), /* server list */
                          DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::ClientList::Id, ARRAY, kDescriptorAttributeArraySize, 0), /* client list */
                          DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::PartsList::Id, ARRAY, kDescriptorAttributeArraySize, 0),  /* parts list */
                          DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

// Declare Bridged Device Basic Information cluster attributes
DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(bridgedDeviceBasicAttrs)
DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::NodeLabel::Id, CHAR_STRING, kNodeLabelSize, 0), /* NodeLabel */
                          DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::Reachable::Id, BOOLEAN, 1, 0),              /* Reachable */
                          DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

// Declare Cluster List for Bridged Light endpoint
// TODO: It's not clear whether it would be better to get the command lists from
// the ZAP config on our last fixed endpoint instead.
constexpr CommandId onOffIncomingCommands[] = {
    app::Clusters::OnOff::Commands::Off::Id,
    app::Clusters::OnOff::Commands::On::Id,
    app::Clusters::OnOff::Commands::Toggle::Id,
    app::Clusters::OnOff::Commands::OffWithEffect::Id,
    app::Clusters::OnOff::Commands::OnWithRecallGlobalScene::Id,
    app::Clusters::OnOff::Commands::OnWithTimedOff::Id,
    kInvalidCommandId,
};

DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(bridgedLightClusters)
DECLARE_DYNAMIC_CLUSTER(OnOff::Id, onOffAttrs, ZAP_CLUSTER_MASK(SERVER), onOffIncomingCommands, nullptr),
                        DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttrs, ZAP_CLUSTER_MASK(SERVER), nullptr, nullptr),
                        DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicAttrs, ZAP_CLUSTER_MASK(SERVER), nullptr,
                                nullptr) DECLARE_DYNAMIC_CLUSTER_LIST_END;

// Declare Bridged Light endpoint
DECLARE_DYNAMIC_ENDPOINT(bridgedLightEndpoint, bridgedLightClusters);

DataVersion gLight1DataVersions[ArraySize(bridgedLightClusters)];
DataVersion gLight2DataVersions[ArraySize(bridgedLightClusters)];
DataVersion gLight3DataVersions[ArraySize(bridgedLightClusters)];
DataVersion gLight4DataVersions[ArraySize(bridgedLightClusters)];

/* REVISION definitions:
 */

#define ZCL_DESCRIPTOR_CLUSTER_REVISION (1u)
#define ZCL_BRIDGED_DEVICE_BASIC_INFORMATION_CLUSTER_REVISION (1u)
#define ZCL_FIXED_LABEL_CLUSTER_REVISION (1u)
#define ZCL_ON_OFF_CLUSTER_REVISION (4u)

#define SERVER_PORT        5000
static struct bridge_table bridge_table[CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT];

const EmberAfDeviceType gBridgedOnOffDeviceTypes[] = { { DEVICE_TYPE_LO_ON_OFF_LIGHT, DEVICE_VERSION_DEFAULT },
    { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT }
};

int AddDeviceEndpoint(MatterBridge *dev, EmberAfEndpointType *ep, const Span<const EmberAfDeviceType> &deviceTypeList,
                      const Span<DataVersion> &dataVersionStorage, chip::EndpointId parentEndpointId)
{
    uint8_t index = 0;

    while (index < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT) {
        if (NULL == gDevices[index]) {
            gDevices[index] = dev;
            CHIP_ERROR ret;
            while (true) {
                dev->SetEndpointId(gCurrentEndpointId);
                ret = emberAfSetDynamicEndpoint(index, gCurrentEndpointId, ep, dataVersionStorage, deviceTypeList, parentEndpointId);
                if (ret == CHIP_NO_ERROR) {
                    ChipLogProgress(DeviceLayer, "Added device %s to dynamic endpoint %d (index=%d)", dev->GetName(),
                                    gCurrentEndpointId, index);
                    return index;
                } else if (ret != CHIP_ERROR_ENDPOINT_EXISTS) {
                    return -1;
                }

                // Handle wrap condition
                if (++gCurrentEndpointId < gFirstDynamicEndpointId) {
                    gCurrentEndpointId = gFirstDynamicEndpointId;
                }
            }
        }
        index++;
    }

    ChipLogProgress(DeviceLayer, "Failed to add dynamic endpoint: No endpoints available!");

    return -1;
}

CHIP_ERROR RemoveDeviceEndpoint(MatterBridge *dev)
{
    for (uint8_t index = 0; index < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT; index++) {
        if (gDevices[index] == dev) {
            // Silence complaints about unused ep when progress logging
            // disabled.
            [[maybe_unused]] EndpointId ep = emberAfClearDynamicEndpoint(index);
            gDevices[index]                = nullptr;
            ChipLogProgress(DeviceLayer, "Removed device %s from dynamic endpoint %d (index=%d)", dev->GetName(), ep, index);

            return CHIP_NO_ERROR;
        }
    }

    return CHIP_ERROR_INTERNAL;
}

Status HandleReadBridgedDeviceBasicAttribute(MatterBridge *dev, chip::AttributeId attributeId, uint8_t *buffer,
        uint16_t maxReadLength)
{
    using namespace BridgedDeviceBasicInformation::Attributes;
    ChipLogProgress(DeviceLayer, "HandleReadBridgedDeviceBasicAttribute: attrId=%" PRIu32 ", maxReadLength=%u", attributeId,
                    maxReadLength);

    if ((attributeId == Reachable::Id) && (maxReadLength == 1)) {
        *buffer = dev->IsReachable() ? 1 : 0;
    } else if ((attributeId == NodeLabel::Id) && (maxReadLength == 32)) {
        MutableByteSpan zclNameSpan(buffer, maxReadLength);
        MakeZclCharString(zclNameSpan, dev->GetName());
    } else if ((attributeId == ClusterRevision::Id) && (maxReadLength == 2)) {
        *buffer = (uint16_t) ZCL_BRIDGED_DEVICE_BASIC_INFORMATION_CLUSTER_REVISION;
    } else {
        return Status::Failure;
    }

    return Status::Success;
}

Status HandleReadOnOffAttribute(MatterBridge *dev, chip::AttributeId attributeId, uint8_t *buffer, uint16_t maxReadLength)
{
    ChipLogProgress(DeviceLayer, "HandleReadOnOffAttribute: attrId=%" PRIu32 ", maxReadLength=%u", attributeId, maxReadLength);

    if ((attributeId == OnOff::Attributes::OnOff::Id) && (maxReadLength == 1)) {
        *buffer = dev->IsTurnedOn() ? 1 : 0;
    } else if ((attributeId == OnOff::Attributes::ClusterRevision::Id) && (maxReadLength == 2)) {
        *buffer = (uint16_t) ZCL_ON_OFF_CLUSTER_REVISION;
    } else {
        return Status::Failure;
    }

    return Status::Success;
}

Status HandleWriteOnOffAttribute(MatterBridge *dev, chip::AttributeId attributeId, uint8_t  *buffer)
{
    ChipLogProgress(DeviceLayer, "HandleWriteOnOffAttribute: attrId=%d", attributeId);

    if ((attributeId == OnOff::Attributes::OnOff::Id) && (dev->IsReachable())) {
        if (*buffer) {
            dev->Set(true, true);
        } else {
            dev->Set(false, true);
        }
        // TODO: We need to store the attribute values in a database
    } else {
        return Status::Failure;
    }

    return Status::Success;
}


Status emberAfExternalAttributeReadCallback(EndpointId endpoint, ClusterId clusterId,
        const EmberAfAttributeMetadata *attributeMetadata, uint8_t *buffer,
        uint16_t maxReadLength)
{
    uint16_t endpointIndex = emberAfGetDynamicIndexFromEndpoint(endpoint);

    Status ret = Status::Failure;

    if ((endpointIndex < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT) && (gDevices[endpointIndex] != nullptr)) {
        MatterBridge *dev = gDevices[endpointIndex];

        if (clusterId == BridgedDeviceBasicInformation::Id) {
            ret = HandleReadBridgedDeviceBasicAttribute(dev, attributeMetadata->attributeId, buffer, maxReadLength);
        } else if (clusterId == OnOff::Id) {
            return HandleReadOnOffAttribute(dev, attributeMetadata->attributeId, buffer, maxReadLength);
        }
    }

    return ret;
}

Status emberAfExternalAttributeWriteCallback(EndpointId endpoint, ClusterId clusterId,
        const EmberAfAttributeMetadata *attributeMetadata, uint8_t *buffer)
{
    uint16_t endpointIndex = emberAfGetDynamicIndexFromEndpoint(endpoint);

    Status ret = Status::Failure;

    // ChipLogProgress(DeviceLayer, "emberAfExternalAttributeWriteCallback: ep=%d", endpointIndex);

    if (endpointIndex < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT) {
        MatterBridge *dev = gDevices[endpointIndex];

        if ((dev->IsReachable()) && (clusterId == OnOff::Id)) {
            return HandleWriteOnOffAttribute(dev, attributeMetadata->attributeId, buffer);
        }
    }

    return ret;
}

void matter_driver_bridge_send_callback(MatterBridge *dev)
{
    int send_size, i = 0;
    char sendmsg[2][15] = {"Device Turn 0", "Device Turn 1"};

    for (i = 0; i < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT; i++) {
        if (dev->GetEndpointId() == bridge_table[i].bridge_endpoint) {
            // Send data
            ChipLogProgress(DeviceLayer, "Sending to table[%d]: sock_conn(%d) endpoint(%d) ==> %s\n", i,
                            bridge_table[i].sock_conn,
                            bridge_table[i].bridge_endpoint,
                            inet_ntoa(bridge_table[i].bridged_device_addr));
            send_size = write(bridge_table[i].sock_conn, sendmsg[dev->IsTurnedOn()], strlen(sendmsg[dev->IsTurnedOn()]));
            if (send_size > 0) {
                ChipLogProgress(DeviceLayer, "Send data < %d bytes>: %s\n", send_size, sendmsg[dev->IsTurnedOn()]);
            } else {
                ChipLogProgress(DeviceLayer, "Error: write\n");
            }
        }
    }
}

namespace
{
void CallReportingCallback(intptr_t closure)
{
    auto path = reinterpret_cast<app::ConcreteAttributePath *>(closure);
    MatterReportingAttributeChangeCallback(*path);
    Platform::Delete(path);
}

void ScheduleReportingCallback(MatterBridge *dev, ClusterId cluster, AttributeId attribute)
{
    auto *path = Platform::New<app::ConcreteAttributePath>(dev->GetEndpointId(), cluster, attribute);
    PlatformMgr().ScheduleWork(CallReportingCallback, reinterpret_cast<intptr_t>(path));
    matter_driver_bridge_send_callback(dev);
}
} // anonymous namespace

void HandleDeviceStatusChanged(MatterBridge *dev, MatterBridge::Changed_t itemChangedMask)
{
    if (itemChangedMask & MatterBridge::kChanged_Reachable) {
        ScheduleReportingCallback(dev, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::Reachable::Id);
    }

    if (itemChangedMask & MatterBridge::kChanged_State) {
        ScheduleReportingCallback(dev, OnOff::Id, OnOff::Attributes::OnOff::Id);
    }

    if (itemChangedMask & MatterBridge::kChanged_Name) {
        ScheduleReportingCallback(dev, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::NodeLabel::Id);
    }
}

bool emberAfActionsClusterInstantActionCallback(app::CommandHandler *commandObj, const app::ConcreteCommandPath &commandPath,
        const Actions::Commands::InstantAction::DecodableType &commandData)
{
    // No actions are implemented, just return status NotFound.
    commandObj->AddStatus(commandPath, Protocols::InteractionModel::Status::NotFound);
    return true;
}

CHIP_ERROR matter_driver_bridge_light_init(void)
{
    // bridge will have own database named gDevices.
    // Clear database
    memset(gDevices, 0, sizeof(gDevices));

    ALight1.Init("Light 1", "Office");
    ALight2.Init("Light 2", "Office");

    ALight1.SetReachable(true);
    ALight2.SetReachable(true);

    // Whenever bridged device changes its state
    ALight1.SetChangeCallback(&HandleDeviceStatusChanged);
    ALight2.SetChangeCallback(&HandleDeviceStatusChanged);

    return CHIP_NO_ERROR;
}

void matter_driver_bridge_endpoint_assign(void)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    // Set starting endpoint id where dynamic endpoints will be assigned, which
    // will be the next consecutive endpoint id after the last fixed endpoint.
    gFirstDynamicEndpointId = static_cast<chip::EndpointId>(
                                  static_cast<int>(emberAfEndpointFromIndex(static_cast<uint16_t>(emberAfFixedEndpointCount() - 1))) + 1);
    gCurrentEndpointId = gFirstDynamicEndpointId;

    // Disable last fixed endpoint, which is used as a placeholder for all of the
    // supported clusters so that ZAP will generated the requisite code.
    emberAfEndpointEnableDisable(emberAfEndpointFromIndex(static_cast<uint16_t>(emberAfFixedEndpointCount() - 1)), false);

    AddDeviceEndpoint(&ALight1, &bridgedLightEndpoint, Span<const EmberAfDeviceType>(gBridgedOnOffDeviceTypes),
                      Span<DataVersion>(gLight1DataVersions), 1);

    AddDeviceEndpoint(&ALight2, &bridgedLightEndpoint, Span<const EmberAfDeviceType>(gBridgedOnOffDeviceTypes),
                      Span<DataVersion>(gLight2DataVersions), 1);

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}

void matter_driver_bridge_recv_thread(void *param)
{
    int recv_size, i = 0;
    int client_fd = -1;
    char sendmsg[2][15] = {"On", "Off"};
    char buf[100];
    ChipLogProgress(DeviceLayer, "RX thread starts\n");

    // Receive data
    while (1) {
        for (i = 0; i < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT; i++) {
            recv_size = read(bridge_table[i].sock_conn, buf, sizeof(buf));
            if (recv_size > 0) {
                buf[recv_size] = 0;
                ChipLogProgress(DeviceLayer, "Recv data < %d bytes>: %s from Endpoint(%d)\n", recv_size, buf, bridge_table[i].bridge_endpoint);

                MatterBridge *dev = gDevices[bridge_table[i].bridge_endpoint - gFirstDynamicEndpointId];
                if (memcmp(buf, sendmsg[0], recv_size) == 0) {
                    dev->Set(true, false);
                } else {
                    dev->Set(false, false);
                }
            } else if (recv_size == 0) {
                ChipLogProgress(DeviceLayer, "Socket disconnected\n");
            }
            vTaskDelay(1000);
        }
    }

}

void matter_driver_bridge_server_thread(void *param)
{
    int server_fd = -1, conn_fd = -1, i;
    int check_onoff = 0, current_endpoint = gFirstDynamicEndpointId;
    struct sockaddr_in server_addr, client_addr, local_name, remote_name;
    socklen_t addrlen = sizeof(struct sockaddr_in);

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    while (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS)
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    while (matter_wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS)
#endif
    {
        vTaskDelay(2000);
    }

    // Set server address
    memset(&bridge_table,  0, sizeof(bridge_table));
    memset(&server_addr, 0, addrlen);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    do {
        // Create a TCP socket
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            ChipLogProgress(DeviceLayer, "Error: socket\n");
            break;
        }

        // Bind the socket to a local address
        if (bind(server_fd, (struct sockaddr *) &server_addr, addrlen) != 0) {
            ChipLogProgress(DeviceLayer, "Error: bind\n");
            break;
        }

        // Listen for socket connection
        if (listen(server_fd, 3) != 0) {
            ChipLogProgress(DeviceLayer, "Error: listen\n");
            break;
        }
        ChipLogProgress(DeviceLayer, "Socket is listening on port: %d\n", SERVER_PORT);

        while (1) {
            // Accept socket connection
            if ((conn_fd = accept(server_fd, (struct sockaddr *) &client_addr, &addrlen)) >= 0) {
                // Get address of locally-bound socket and connected peer socket
                getsockname(conn_fd, (struct sockaddr *)&local_name, &addrlen);
                getpeername(conn_fd, (struct sockaddr *)&remote_name, &addrlen);

                for (i = 0; i < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT; i ++) {
                    if (bridge_table[i].bridged_device_addr == NULL) {
                        bridge_table[i].bridge_endpoint = current_endpoint;
                        current_endpoint++;
                        bridge_table[i].bridged_device_addr = remote_name.sin_addr.s_addr;
                        bridge_table[i].sock_conn = conn_fd;
                        ChipLogProgress(DeviceLayer, "Add into table[%d]: sock_conn(%d) endpoint(%d) ==> %s\n", i,
                                        bridge_table[i].sock_conn,
                                        bridge_table[i].bridge_endpoint,
                                        inet_ntoa(bridge_table[i].bridged_device_addr));

                        int send_size;
                        char sendmsg[15] = "Read status";

                        // Read from bridged device current status
                        ChipLogProgress(DeviceLayer, "Read status from table[%d]: sock_conn(%d) endpoint(%d) ==> %s\n", i,
                                        bridge_table[i].sock_conn,
                                        bridge_table[i].bridge_endpoint,
                                        inet_ntoa(bridge_table[i].bridged_device_addr));
                        send_size = write(bridge_table[i].sock_conn, sendmsg, sizeof(sendmsg));
                        if (send_size > 0) {
                            ChipLogProgress(DeviceLayer, "Send data < %d bytes>: %s\n", send_size, sendmsg);
                        } else {
                            ChipLogProgress(DeviceLayer, "Error: write\n");
                        }
                        break;
                    }
                }
            } else {
                ChipLogProgress(DeviceLayer, "Error: accept\n");
            }
        }
    } while (0);
}

void matter_driver_bridge_setup_server(void)
{
    if (xTaskCreate(matter_driver_bridge_server_thread, ((const char *)"matter_driver_bridge_server_thread"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        printf("\n\r%s xTaskCreate(matter_driver_bridge_server_thread) failed\n", __FUNCTION__);
    }
}

void matter_driver_bridge_recv_server(void)
{
    if (xTaskCreate(matter_driver_bridge_recv_thread, ((const char *)"matter_driver_bridge_recv_thread"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        printf("\n\r%s xTaskCreate(matter_driver_bridge_recv_thread) failed\n", __FUNCTION__);
    }
}

void matter_driver_uplink_update_handler(AppEvent *aEvent)
{

exit:
    return;
}

void matter_driver_downlink_update_handler(AppEvent *event)
{

exit:
    return;
}
