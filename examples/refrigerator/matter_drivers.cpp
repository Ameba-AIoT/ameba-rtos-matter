#include <matter_drivers.h>
#include <matter_interaction.h>
#include <refrigerator_driver.h>
#include <refrigerator_mode/ameba_tcc_mode.h>

#include <gpio_api.h>
#include <gpio_irq_api.h>
#include <gpio_irq_ex_api.h>

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <clusters/refrigerator-alarm-server/refrigerator-alarm-server.h>
#include <protocols/interaction_model/StatusCode.h>

using namespace ::chip::app;
using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::RefrigeratorAlarm;
using namespace chip::app::Clusters::RefrigeratorAndTemperatureControlledCabinetMode;
using chip::Protocols::InteractionModel::Status;

#if defined(CONFIG_PLATFORM_8710C)
#define GPIO_IRQ_LEVEL_PIN    PA_17
#define GPIO_LED_PIN          PA_19
#elif defined(CONFIG_PLATFORM_8721D)
#define GPIO_IRQ_LEVEL_PIN    PA_28
#define GPIO_LED_PIN          PB_4
#endif

MatterRefrigerator refrigerator;
gpio_irq_t gpio_level;
uint32_t current_level = IRQ_LOW;

// Set identify cluster and its callback on ep1
static Identify gIdentify1 = {
    chip::EndpointId{ 1 }, matter_driver_on_identify_start, matter_driver_on_identify_stop, Clusters::Identify::IdentifyTypeEnum::kVisibleIndicator, matter_driver_on_trigger_effect,
};

void matter_driver_gpio_level_irq_handler(uint32_t id, gpio_irq_event event)
{
    uint32_t *level = (uint32_t *) id;

    if (*level == IRQ_LOW) // Door closed
    {
        refrigerator.SetDoorStatus((uint8_t) 0);
        matter_driver_set_door_callback((uint32_t) 0);

        // Change to listen to high level event
        *level = IRQ_HIGH;
        gpio_irq_set(&gpio_level, (gpio_irq_event)IRQ_HIGH, 1);
    }
    else if (*level == IRQ_HIGH)  // Door opened
    {
        refrigerator.SetDoorStatus((uint8_t) 1);
        matter_driver_set_door_callback((uint32_t) 1);

        // Change to listen to low level event
        *level = IRQ_LOW;
        gpio_irq_set(&gpio_level, (gpio_irq_event)IRQ_LOW, 1);
    }
}

CHIP_ERROR matter_driver_refrigerator_init(void)
{
    refrigerator.Init(GPIO_LED_PIN);

    gpio_irq_init(&gpio_level, GPIO_IRQ_LEVEL_PIN, matter_driver_gpio_level_irq_handler, (uint32_t)(&current_level));
    gpio_irq_set(&gpio_level, (gpio_irq_event)IRQ_LOW, 1);
    gpio_irq_enable(&gpio_level);

    return CHIP_NO_ERROR;
}

CHIP_ERROR matter_driver_refrigerator_set_startup_value(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    Status status;
    Clusters::ModeBase::Commands::ChangeToModeResponse::Type modeChangedResponse;
    Clusters::ModeBase::Instance & refrigeratorObject = Clusters::RefrigeratorAndTemperatureControlledCabinetMode::Instance();
    RefrigeratorAlarmServer & refrigeratorAlarmObject = RefrigeratorAlarmServer::Instance();

    chip::DeviceLayer::PlatformMgr().LockChipStack();

    modeChangedResponse.status = to_underlying(refrigeratorObject.UpdateCurrentMode(to_underlying(ModeTag::kRapidCool))); // Set refrigerator mode
    if (modeChangedResponse.status != to_underlying(Clusters::ModeBase::StatusCode::kSuccess))
    {
        ChipLogProgress(DeviceLayer, "Failed to set Refrigerator Mode!\n");
        err = CHIP_ERROR_INTERNAL;
    }

    BitMask<AlarmMap> supported; // Set refrigerator alarm supported value
    supported.SetField(AlarmMap::kDoorOpen, 1);
    refrigeratorAlarmObject.SetSupportedValue(1, supported);
    if (status != Status::Success)
    {
        ChipLogProgress(DeviceLayer, "Failed to set Refrigerator Alarm Supported Value!\n");
        err = CHIP_ERROR_INTERNAL;
    }

    BitMask<AlarmMap> mask; // Set refrigerator alarm mask value
    mask.SetField(AlarmMap::kDoorOpen, 1);
    refrigeratorAlarmObject.SetMaskValue(1, mask);
    if (status != Status::Success)
    {
        ChipLogProgress(DeviceLayer, "Failed to set Refrigerator Alarm Mask Value!\n");
        err = CHIP_ERROR_INTERNAL;
    }

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    return err;
}

void matter_driver_set_mode_callback(uint32_t id)
{
    AppEvent downlink_event;
    downlink_event.Type = AppEvent::kEventType_Downlink_Refrigerator_Mode;
    downlink_event.value._u16 = (uint16_t) id;
    downlink_event.mHandler = matter_driver_downlink_update_handler;
    PostDownlinkEvent(&downlink_event);
}

void matter_driver_set_door_callback(uint32_t id)
{
    AppEvent downlink_event;
    downlink_event.Type = AppEvent::kEventType_Downlink_Refrigerator_Alarm_State;
    downlink_event.value._u8 = (uint8_t) id;
    downlink_event.mHandler = matter_driver_downlink_update_handler;
    PostDownlinkEvent(&downlink_event);
}

void matter_driver_on_identify_start(Identify *identify)
{
    ChipLogProgress(Zcl, "OnIdentifyStart");
}

void matter_driver_on_identify_stop(Identify *identify)
{
    ChipLogProgress(Zcl, "OnIdentifyStop");
}

void matter_driver_on_trigger_effect(Identify *identify)
{
    switch (identify->mCurrentEffectIdentifier)
    {
    case Clusters::Identify::EffectIdentifierEnum::kBlink:
        ChipLogProgress(Zcl, "Clusters::Identify::EffectIdentifierEnum::kBlink");
        break;
    case Clusters::Identify::EffectIdentifierEnum::kBreathe:
        ChipLogProgress(Zcl, "Clusters::Identify::EffectIdentifierEnum::kBreathe");
        break;
    case Clusters::Identify::EffectIdentifierEnum::kOkay:
        ChipLogProgress(Zcl, "Clusters::Identify::EffectIdentifierEnum::kOkay");
        break;
    case Clusters::Identify::EffectIdentifierEnum::kChannelChange:
        ChipLogProgress(Zcl, "Clusters::Identify::EffectIdentifierEnum::kChannelChange");
        break;
    default:
        ChipLogProgress(Zcl, "No identifier effect");
        return;
    }
}

void matter_driver_uplink_update_handler(AppEvent *aEvent)
{
    chip::app::ConcreteAttributePath path = aEvent->path;

    // this example only considers endpoint 1
    VerifyOrExit(aEvent->path.mEndpointId == 1,
                 ChipLogError(DeviceLayer, "Unexpected EndPoint ID: `0x%02x'", path.mEndpointId));

    switch (path.mClusterId)
    {
    case Clusters::RefrigeratorAndTemperatureControlledCabinetMode::Id:
        {
            ChipLogProgress(DeviceLayer, "RefrigeratorAndTemperatureControlledCabinetMode(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
            if (path.mAttributeId == Clusters::RefrigeratorAndTemperatureControlledCabinetMode::Attributes::CurrentMode::Id)
            {
                refrigerator.SetMode(aEvent->value._u16);
            }
        }
        break;
    case Clusters::RefrigeratorAlarm::Id:
        {
            ChipLogProgress(DeviceLayer, "RefrigeratorAlarm(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
            if (path.mAttributeId == Clusters::RefrigeratorAlarm::Attributes::State::Id)
            {
                refrigerator.SetAlarm();
            }
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
    Clusters::ModeBase::Commands::ChangeToModeResponse::Type modeChangedResponse;
    Status alarmChangedStatus;
    Clusters::ModeBase::Instance & refrigeratorObject = Clusters::RefrigeratorAndTemperatureControlledCabinetMode::Instance();
    RefrigeratorAlarmServer & refrigeratorAlarmObject = RefrigeratorAlarmServer::Instance();

    chip::DeviceLayer::PlatformMgr().LockChipStack();

    switch (event->Type)
    {
    case AppEvent::kEventType_Downlink_Refrigerator_Mode:
        {
            modeChangedResponse.status = to_underlying(refrigeratorObject.UpdateCurrentMode(event->value._u16));
            if (modeChangedResponse.status != to_underlying(Clusters::ModeBase::StatusCode::kSuccess))
            {
                ChipLogProgress(DeviceLayer, "Failed to set refrigerator mode!\n");
            }
        }
        break;
    case AppEvent::kEventType_Downlink_Refrigerator_Alarm_State:
        {
            BitMask<AlarmMap> value;
            value.SetField(AlarmMap::kDoorOpen, event->value._u8);
            ChipLogProgress(DeviceLayer, "Set Refrigerator Alarm State Value 0x%x\n", event->value._u8);
            alarmChangedStatus = refrigeratorAlarmObject.SetStateValue(1, value);
            if (alarmChangedStatus != Status::Success)
            {
                ChipLogProgress(DeviceLayer, "Failed to set door status!\n");
            }
        }
        break;
    default:
        break;
    }

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}
