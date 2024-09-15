/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/

#pragma once

#include <platform/CHIPDeviceLayer.h>
#include <app/ConcreteAttributePath.h>

struct AppEvent;
typedef void (*EventHandler)(AppEvent *);

struct AppEvent
{
    enum AppEventTypes
    {
        kEventType_Uplink = 0,
        kEventType_Downlink_OnOff,
        kEventType_Downlink_Identify,
        kEventType_Downlink_TempControl_SetPoint,
        kEventType_Downlink_Opstate_State,
        kEventType_Downlink_Opstate_Error_State,
        kEventType_Downlink_LW_SpinSpeed,
        kEventType_Downlink_LW_NumberOfRinses,
        kEventType_Downlink_LW_Mode,
        kEventType_Downlink_DW_Mode,
        kEventType_Downlink_DW_Alarm_Set,
        kEventType_Downlink_DW_Alarm_Reset,
        kEventType_Downlink_Refrigerator_Mode,
        kEventType_Downlink_Refrigerator_Alarm_State,

        /*Relative Humidity Measurement Cluster*/
        kEventType_Downlink_RelativeHumidityMeasurement_SetValue,

        /*Temperature Measurement Cluster*/
        kEventType_Downlink_TemperatureMeasurement_SetValue,

        /*Switch Cluster Event*/
        kEventType_Downlink_SwitchLatched,
        kEventType_Downlink_SwitchInitialPress,
        kEventType_Downlink_SwitchLongPress,
        kEventType_Downlink_SwitchShortRelease,
        kEventType_Downlink_SwitchLongRelease,
        kEventType_Downlink_SwitchMultiPressOngoing,
        kEventType_Downlink_SwitchMultiPressComplete,
    };

    uint16_t Type;
    chip::app::ConcreteAttributePath path;
    union
    {
       uint8_t _u8;
       uint16_t _u16;
       uint32_t _u32;
       uint64_t _u64;
       int8_t _i8;
       int16_t _i16;
       int32_t _i32;
       int64_t _i64;
       char _str[256];
    } value;
    EventHandler mHandler;
};

namespace chip {
namespace DeviceLayer {
namespace DeviceEventType {

/**
 * Enumerates for Fabric Event
 */
enum
{
    /**
     * Indicating the Commissioning session has been established
     */
    kEvent_CommissioningSessionEstablishmentStarted = kRange_PublicPlatformSpecific + 0x1000,
    /**
     * Indicating that Commissioning session has started
     */
    kEvent_CommissioningSessionStarted,
    /**
     * Indicating the established Commissioning session encountered an error
     */
    kEvent_CommissioningSessionEstablishmentError,
    /**
     * Indicating that Commissioning session has stopped
     */
    kEvent_CommissioningSessionStopped,
    /**
     * Indicating that Commissioning window is now opened
     */
    kEvent_CommissioningWindowOpened,
    /**
     * Indicating that Commissioning window is now closed
     */
    kEvent_CommissioningWindowClosed,
    /**
     * Indicating that a fabric is about to be removed
     */
    kEvent_FabricWillBeRemoved,
    /**
     * Indicating that a fabric is removed
     */
    kEvent_FabricRemoved,
    /**
     * Indicating that a fabric in Fabric Table is persisted to storage, by CommitPendingFabricData
     */
    kEvent_FabricCommitted,
    /**
     * Indicating that operational credentials are changed, which may not be persistent
     * Can be used to affect what is needed for UpdateNOC prior to commit
     */
    kEvent_FabricUpdated,
};

} // DeviceEventType
} // DeviceLayer
} // chip
