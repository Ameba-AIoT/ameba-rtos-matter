#include <matter_drivers.h>
#include <matter_interaction.h>
#include <microwaveoven_driver.h>
#include <microwave_oven/ameba_microwave_oven_device.h>

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <protocols/interaction_model/StatusCode.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OperationalState;
using namespace chip::app::Clusters::MicrowaveOvenMode;
using namespace chip::app::Clusters::MicrowaveOvenControl;
using chip::Protocols::InteractionModel::Status;

template <typename T>
using List              = chip::app::DataModel::List<T>;
using ModeTagStructType = chip::app::Clusters::detail::Structs::ModeTagStruct::Type;
using Status            = Protocols::InteractionModel::Status;

#if defined(CONFIG_PLATFORM_8710C)
#define PWM_PIN         PA_23
#elif defined(CONFIG_PLATFORM_8721D)
#define PWM_PIN         PB_5
#endif

MatterMicrowaveOven MicrowaveOven;

CHIP_ERROR matter_driver_microwave_oven_init(void)
{
    MicrowaveOven.Init(PWM_PIN);
    return CHIP_NO_ERROR;
}

CHIP_ERROR matter_driver_microwave_oven_set_startup_value(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    Status status;

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    MatterMicrowaveOvenServerInit();
    MicrowaveOven.setOpState((uint8_t) Clusters::OperationalState::OperationalStateEnum::kStopped);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

exit:
    if (err == CHIP_ERROR_INTERNAL)
    {
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    }

    return err;
}

void matter_driver_set_opstate_callback(uint32_t id)
{
    AppEvent downlink_event;
    downlink_event.Type = AppEvent::kEventType_Downlink_Opstate_State;
    downlink_event.value._u8 = (uint8_t) id; // 0: Stop; 1:Running ,2:Paused; 3: Error
    downlink_event.mHandler = matter_driver_downlink_update_handler;
    PostDownlinkEvent(&downlink_event);
}

void matter_driver_uplink_update_handler(AppEvent *aEvent)
{
    chip::app::ConcreteAttributePath path = aEvent->path;

    // this example only considers endpoint 1
    VerifyOrExit(aEvent->path.mEndpointId == 1,
                 ChipLogError(DeviceLayer, "Unexpected EndPoint ID: `0x%02x'", path.mEndpointId));

    switch (path.mClusterId)
    {
    case Clusters::OperationalState::Id:
        {
            ChipLogProgress(DeviceLayer, "OvenOperationalState(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
            MicrowaveOven.setOpState(aEvent->value._u8);
        }
        break;
    case Clusters::MicrowaveOvenMode::Id:
        {
            ChipLogProgress(DeviceLayer, "MicrowaveOvenMode(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
        }
        break;
    case Clusters::MicrowaveOvenControl::Id:
        {
            ChipLogProgress(DeviceLayer, "MicrowaveOvenControl(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
        }
        break;
    default:
        break;
    }

exit:
    return;
}

void matter_driver_downlink_update_handler(AppEvent *event)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();

    switch (event->Type)
    {
    case AppEvent::kEventType_Downlink_Opstate_State:
        {
            ChipLogProgress(DeviceLayer, "Set Oven Operational State 0x%x", event->value._u8);
            CHIP_ERROR err;
            err = GetMatterMicrowaveOvenServer()->GetOperationalStateInstance()->SetOperationalState(event->value._u8);
            if (err != CHIP_NO_ERROR)
            {
                ChipLogError(DeviceLayer, "ManualMicrowaveOvenOperationalStateSetStateCommandHandler Failed!\r\n");
            }
        }
        break;
    default:
        break;
    }

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}
