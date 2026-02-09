#include <matter_drivers.h>
#include <matter_interaction.h>
#include <doorlock_driver.h>
#include <door_lock/ameba_lock_endpoint.h>
#include <door_lock/ameba_lock_manager.h>

#include <gpio_api.h>
#include <gpio_irq_api.h>

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <clusters/door-lock-server/door-lock-server.h>
#include <protocols/interaction_model/StatusCode.h>

using namespace ::chip::app;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::DoorLock;
using chip::Protocols::InteractionModel::Status;

#define DOORLOCK_ENDPOINT 1
#if defined (CONFIG_AMEBASMART)
#define PWM_PIN           PA_5
#define GPIO_IRQ_EDGE_PIN PA_10
#elif defined (CONFIG_AMEBALITE)
#define PWM_PIN           PA_31
#define GPIO_IRQ_EDGE_PIN PA_29
#elif defined (CONFIG_AMEBADPLUS)
#define PWM_PIN           PB_18
#define GPIO_IRQ_EDGE_PIN PA_12
#endif

MatterDoorLock doorLock;
gpio_irq_t gpio_edge;
int current_edge = IRQ_FALL;

// Button to trigger unlock or lock downlink event
void matter_gpio_edge_irq_handler(uint32_t id, gpio_irq_event event)
{
    (void)event;
    gpio_irq_event *edge = (gpio_irq_event *) id;
    if(doorLock.GetLockStatus() == true)
    {
        doorLock.Unlock();
        matter_driver_doorlock_lock_state_callback(static_cast<uint32_t>(DlLockState::kUnlocked));
    }
    else
    {
        doorLock.Lock();
        matter_driver_doorlock_lock_state_callback(static_cast<uint32_t>(DlLockState::kLocked));
    }
}

CHIP_ERROR matter_driver_doorlock_init(void)
{
    doorLock.Init(PWM_PIN);

    gpio_irq_init(&gpio_edge, GPIO_IRQ_EDGE_PIN, matter_gpio_edge_irq_handler, (uint32_t)(&current_edge));
    gpio_irq_set(&gpio_edge, IRQ_FALL, 1);
    gpio_irq_enable(&gpio_edge);

    return CHIP_NO_ERROR;
}

CHIP_ERROR matter_driver_doorlock_set_startup_value(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    chip::DeviceLayer::PlatformMgr().LockChipStack();

    // Set Lock State based on physical doorlock status
    if(doorLock.GetLockStatus() == true)
    {
        DoorLockServer::Instance().SetLockState(DOORLOCK_ENDPOINT, DlLockState::kLocked);
    }
    else
    {
        DoorLockServer::Instance().SetLockState(DOORLOCK_ENDPOINT, DlLockState::kUnlocked);
    }

    // Set RequirePINforRemoteOperation to 1
    auto status = Attributes::RequirePINforRemoteOperation::Set(DOORLOCK_ENDPOINT, true);
    if(status == Status::Success)
    {
        ChipLogProgress(DeviceLayer, "RequirePINforRemoteOperation is set to true");
    }
    else
    {
        ChipLogError(DeviceLayer, "Failed to set RequirePINforRemoteOperation to true");
    }

    // Add admin during initialization
    if (AmebaLockManager::Instance().InitAdmin(DOORLOCK_ENDPOINT))
    {
        ChipLogProgress(DeviceLayer, "Admin initialized");
    }
    else
    {
        ChipLogProgress(DeviceLayer, "Failed to initialize Admin");
    }

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    return err;
}

void matter_driver_doorlock_lock_state_callback(uint32_t id)
{
    AppEvent downlink_event;
    downlink_event.Type = AppEvent::kEventType_Downlink_DoorLock_Lock_State;
    downlink_event.value._u8 = (uint8_t) id;
    downlink_event.path.mEndpointId = DOORLOCK_ENDPOINT;
    downlink_event.mHandler = matter_driver_downlink_update_handler;
    PostDownlinkEvent(&downlink_event);
}

void matter_driver_uplink_update_handler(AppEvent *aEvent)
{
    chip::app::ConcreteAttributePath path = aEvent->path;

    // this example only considers endpoint 1
    VerifyOrExit(aEvent->path.mEndpointId == DOORLOCK_ENDPOINT,
                 ChipLogError(DeviceLayer, "Unexpected EndPoint ID: `0x%02x'", path.mEndpointId));

    switch (path.mClusterId)
    {
    case Clusters::DoorLock::Id:
        {
            ChipLogProgress(DeviceLayer, "DoorLock(ClusterId=0x%x) at Endpoint 0x%x: change AttributeId=0x%x\n", path.mClusterId, path.mEndpointId, path.mAttributeId);

            switch (path.mAttributeId)
            {
            case Clusters::DoorLock::Attributes::LockState::Id:
                {
                    switch (aEvent->value._u8)
                    {
                    case static_cast<uint8_t>(DlLockState::kNotFullyLocked):
                        ChipLogProgress(DeviceLayer, "LockState changed to not fully locked");
                        // If door is not fully locked, do something here ...
                        break;
                    case static_cast<uint8_t>(DlLockState::kLocked):
                        ChipLogProgress(DeviceLayer, "LockState changed to locked");
                        doorLock.Lock();
                        break;
                    case static_cast<uint8_t>(DlLockState::kUnlocked):
                        ChipLogProgress(DeviceLayer, "LockState changed to unlocked");
                        doorLock.Unlock();
                        break;
                    case static_cast<uint8_t>(DlLockState::kUnlatched):
                        ChipLogProgress(DeviceLayer, "LockState changed to unlatched");
                        doorLock.Unlock(); // Unlatched = Unlock ?
                        break;
                    default:
                        break;
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

exit:
    return;
}

void matter_driver_downlink_update_handler(AppEvent *aEvent)
{
    // this example only considers endpoint 1
    VerifyOrExit(aEvent->path.mEndpointId == DOORLOCK_ENDPOINT,
                 ChipLogError(DeviceLayer, "Unexpected EndPoint ID: `0x%02x'", aEvent->path.mEndpointId));

    chip::DeviceLayer::PlatformMgr().LockChipStack();

    switch (aEvent->Type)
    {
    case AppEvent::kEventType_Downlink_DoorLock_Lock_State:
        {
            ChipLogProgress(DeviceLayer, "Set Door Lock State Value 0x%x\n", aEvent->value._u8);
            DoorLockServer::Instance().SetLockState(aEvent->path.mEndpointId, static_cast<DoorLock::DlLockState>(aEvent->value._u8));
        }
        break;
    default:
        break;
    }

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

exit:
    return;
}
