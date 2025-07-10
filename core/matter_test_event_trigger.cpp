/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#include <test_event_trigger/AmebaTestEventTriggerDelegate.h>
#include <device_energy_management/ameba_energy_management_common_main.h>
#include <matter_test_event_trigger.h>
#include <app/util/af-types.h>
#include <lib/core/CHIPCore.h>
#include <lib/core/CHIPError.h>
#include <lib/support/CodeUtils.h>
#include <energy_evse/ameba_energy_evse_delegate_impl.h>
#include <energy_evse/ameba_energy_evse_manufacturer_impl.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::DeviceEnergyManagement;
using namespace chip::app::Clusters::EnergyEvse;

struct EVSETestEventSaveData
{
    int64_t mOldMaxHardwareCurrentLimit;
    int64_t mOldCircuitCapacity;
    int64_t mOldUserMaximumChargeCurrent;
    int64_t mOldCableAssemblyLimit;
    StateEnum mOldHwStateBasic;           /* For storing hwState before Basic Func event */
    StateEnum mOldHwStatePluggedIn;       /* For storing hwState before PluggedIn event */
    StateEnum mOldHwStatePluggedInDemand; /* For storing hwState before PluggedInDemand event */
};

static EVSETestEventSaveData sEVSETestEventSaveData;

extern bool AmebaHandleGlobalTestEventTrigger(uint64_t eventTrigger);

namespace {

constexpr uint16_t MAX_SLOTS             = 10;
constexpr uint16_t MAX_POWER_ADJUSTMENTS = 5;

chip::app::Clusters::DeviceEnergyManagement::Structs::SlotStruct::Type sSlots[MAX_SLOTS];
chip::app::Clusters::DeviceEnergyManagement::Structs::ForecastStruct::Type sForecastStruct;

chip::app::Clusters::DeviceEnergyManagement::Structs::PowerAdjustStruct::Type sPowerAdjustments[MAX_POWER_ADJUSTMENTS];
chip::app::Clusters::DeviceEnergyManagement::Structs::PowerAdjustCapabilityStruct::Type sPowerAdjustCapabilityStruct;
chip::app::DataModel::Nullable<chip::app::Clusters::DeviceEnergyManagement::Structs::PowerAdjustCapabilityStruct::Type>
    sPowerAdjustmentCapability;

} // namespace

CHIP_ERROR ConfigureForecast(uint16_t numSlots)
{
    uint32_t matterEpoch = 0;

    CHIP_ERROR err = System::Clock::GetClock_MatterEpochS(matterEpoch);
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(Support, "ConfigureForecast could not get time");
        return err;
    }

    // planned start time, in UTC, for the entire Forecast. Allow to be a little
    // time in the future as forecastStruct.startTime is used in some tests.
    sForecastStruct.startTime = matterEpoch + 60;

    // earliest start time, in UTC, that the entire Forecast can be shifted to. null value indicates that it can be started
    // immediately.
    sForecastStruct.earliestStartTime = MakeOptional(DataModel::MakeNullable(matterEpoch));

    // planned end time, in UTC, for the entire Forecast.
    sForecastStruct.endTime = matterEpoch * 3;

    // latest end time, in UTC, for the entire Forecast
    sForecastStruct.latestEndTime = MakeOptional(matterEpoch * 3);

    sForecastStruct.isPausable = true;

    sForecastStruct.activeSlotNumber.SetNonNull(0);

    sSlots[0].minDuration       = 10;
    sSlots[0].maxDuration       = 20;
    sSlots[0].defaultDuration   = 15;
    sSlots[0].elapsedSlotTime   = 0;
    sSlots[0].remainingSlotTime = 0;

    sSlots[0].slotIsPausable.SetValue(true);
    sSlots[0].minPauseDuration.SetValue(10);
    sSlots[0].maxPauseDuration.SetValue(60);

    if (GetDEMDelegate()->HasFeature(DeviceEnergyManagement::Feature::kPowerForecastReporting))
    {
        sSlots[0].nominalPower.SetValue(2500000);
        sSlots[0].minPower.SetValue(1200000);
        sSlots[0].maxPower.SetValue(7600000);
    }

    sSlots[0].nominalEnergy.SetValue(2000);

    if (GetDEMDelegate()->HasFeature(DeviceEnergyManagement::Feature::kStateForecastReporting))
    {
        sSlots[0].manufacturerESAState.SetValue(23);
    }

    for (uint16_t slotNo = 1; slotNo < numSlots; slotNo++)
    {
        sSlots[slotNo].minDuration       = 2 * sSlots[slotNo - 1].minDuration;
        sSlots[slotNo].maxDuration       = 2 * sSlots[slotNo - 1].maxDuration;
        sSlots[slotNo].defaultDuration   = 2 * sSlots[slotNo - 1].defaultDuration;
        sSlots[slotNo].elapsedSlotTime   = 2 * sSlots[slotNo - 1].elapsedSlotTime;
        sSlots[slotNo].remainingSlotTime = 2 * sSlots[slotNo - 1].remainingSlotTime;

        // Need slotNo == 1 not to be pausible for test DEM 2.4 step 3b
        sSlots[slotNo].slotIsPausable.SetValue((slotNo & 1) == 0 ? true : false);
        sSlots[slotNo].minPauseDuration.SetValue(2 * sSlots[slotNo - 1].slotIsPausable.Value());
        sSlots[slotNo].maxPauseDuration.SetValue(2 * sSlots[slotNo - 1].maxPauseDuration.Value());

        if (GetDEMDelegate()->HasFeature(DeviceEnergyManagement::Feature::kPowerForecastReporting))
        {
            sSlots[slotNo].nominalPower.SetValue(sSlots[slotNo - 1].nominalPower.Value());
            sSlots[slotNo].minPower.SetValue(sSlots[slotNo - 1].minPower.Value());
            sSlots[slotNo].maxPower.SetValue(sSlots[slotNo - 1].maxPower.Value());

            sSlots[slotNo].nominalEnergy.SetValue(2 * sSlots[slotNo - 1].nominalEnergy.Value());
        }

        if (GetDEMDelegate()->HasFeature(DeviceEnergyManagement::Feature::kStateForecastReporting))
        {
            sSlots[slotNo].manufacturerESAState.SetValue(sSlots[slotNo - 1].manufacturerESAState.Value() + 1);
        }
    }

    sForecastStruct.slots = DataModel::List<const DeviceEnergyManagement::Structs::SlotStruct::Type>(sSlots, numSlots);

    GetDEMDelegate()->SetForecast(DataModel::MakeNullable(sForecastStruct));

    return CHIP_NO_ERROR;
}


void SetTestEventTrigger_Forecast()
{
    ConfigureForecast(2);
}

void SetTestEventTrigger_ForecastClear()
{
    sForecastStruct.startTime = 0;
    sForecastStruct.endTime   = 0;
    sForecastStruct.earliestStartTime.ClearValue();
    sForecastStruct.latestEndTime.ClearValue();
    sForecastStruct.isPausable = false;
    sForecastStruct.activeSlotNumber.SetNull();
    sForecastStruct.slots = DataModel::List<const DeviceEnergyManagement::Structs::SlotStruct::Type>();

    GetDEMDelegate()->SetForecast(DataModel::MakeNullable(sForecastStruct));
}

EnergyEvseDelegate * GetEvseDelegate()
{
    printf("EnergyEvseDelegate * GetEvseDelegate() to call GetEvseManufacturer\n");
    EVSEManufacturer * mn = GetEvseManufacturer();
    VerifyOrDieWithMsg(mn != nullptr, AppServer, "EVSEManufacturer is null");
    EnergyEvseDelegate * dg = mn->GetEvseDelegate();
    VerifyOrDieWithMsg(dg != nullptr, AppServer, "EVSE Delegate is null");

    return dg;
}

void SetTestEventTrigger_BasicFunctionality()
{
    EnergyEvseDelegate * dg = GetEvseDelegate();

    sEVSETestEventSaveData.mOldMaxHardwareCurrentLimit  = dg->HwGetMaxHardwareCurrentLimit();
    sEVSETestEventSaveData.mOldCircuitCapacity          = dg->GetCircuitCapacity();
    sEVSETestEventSaveData.mOldUserMaximumChargeCurrent = dg->GetUserMaximumChargeCurrent();
    sEVSETestEventSaveData.mOldHwStateBasic             = dg->HwGetState();

    dg->HwSetMaxHardwareCurrentLimit(32000);
    dg->HwSetCircuitCapacity(32000);
    dg->SetUserMaximumChargeCurrent(32000);
    dg->HwSetState(StateEnum::kNotPluggedIn);
}

void SetTestEventTrigger_BasicFunctionalityClear()
{
    EnergyEvseDelegate * dg = GetEvseDelegate();

    dg->HwSetMaxHardwareCurrentLimit(sEVSETestEventSaveData.mOldMaxHardwareCurrentLimit);
    dg->HwSetCircuitCapacity(sEVSETestEventSaveData.mOldCircuitCapacity);
    dg->SetUserMaximumChargeCurrent(sEVSETestEventSaveData.mOldUserMaximumChargeCurrent);
    dg->HwSetState(sEVSETestEventSaveData.mOldHwStateBasic);
}

void SetTestEventTrigger_EVTimeOfUseMode()
{
    // TODO - See #34249
}

void SetTestEventTrigger_EVTimeOfUseModeClear()
{
    // TODO - See #34249
}

void SetTestEventTrigger_EVPluggedIn()
{
    EnergyEvseDelegate * dg = GetEvseDelegate();

    sEVSETestEventSaveData.mOldCableAssemblyLimit = dg->HwGetCableAssemblyLimit();
    sEVSETestEventSaveData.mOldHwStatePluggedIn   = dg->HwGetState();

    dg->HwSetCableAssemblyLimit(63000);
    dg->HwSetState(StateEnum::kPluggedInNoDemand);
}

void SetTestEventTrigger_EVPluggedInClear()
{
    EnergyEvseDelegate * dg = GetEvseDelegate();
    dg->HwSetCableAssemblyLimit(sEVSETestEventSaveData.mOldCableAssemblyLimit);
    dg->HwSetState(sEVSETestEventSaveData.mOldHwStatePluggedIn);
}

bool HandleDeviceEnergyManagementTestEventTrigger(uint64_t eventTrigger)
{
    DeviceEnergyManagementTrigger trigger = static_cast<DeviceEnergyManagementTrigger>(eventTrigger);

    switch (trigger)
    {
    case DeviceEnergyManagementTrigger::kForecast:
        ChipLogProgress(Support, "[Forecast-Test-Event] => Create a forecast with at least 1 slot");
        SetTestEventTrigger_Forecast();
        break;
    case DeviceEnergyManagementTrigger::kForecastClear:
        ChipLogProgress(Support, "[ForecastClear-Test-Event] => Clear the forecast");
        SetTestEventTrigger_ForecastClear();
        break;

    default:
        return false;
    }

    return true;
}

bool HandleEnergyEvseTestEventTrigger(uint64_t eventTrigger)
{
    EnergyEvseTrigger trigger = static_cast<EnergyEvseTrigger>(eventTrigger);

    switch (trigger)
    {
    case EnergyEvseTrigger::kBasicFunctionality:
        ChipLogProgress(Support, "[EnergyEVSE-Test-Event] => Basic Functionality install");
        SetTestEventTrigger_BasicFunctionality();
        break;
    case EnergyEvseTrigger::kBasicFunctionalityClear:
        ChipLogProgress(Support, "[EnergyEVSE-Test-Event] => Basic Functionality clear");
        SetTestEventTrigger_BasicFunctionalityClear();
        break;
    case EnergyEvseTrigger::kEVTimeOfUseMode:
        ChipLogProgress(Support, "[EnergyEVSE-Test-Event] => EV TimeOfUse Mode");
        SetTestEventTrigger_EVTimeOfUseMode();
        break;
    case EnergyEvseTrigger::kEVTimeOfUseModeClear:
        ChipLogProgress(Support, "[EnergyEVSE-Test-Event] => EV TimeOfUse Mode clear");
        SetTestEventTrigger_EVTimeOfUseModeClear();
        break;
    case EnergyEvseTrigger::kEVPluggedIn:
        ChipLogProgress(Support, "[EnergyEVSE-Test-Event] => EV plugged in");
        SetTestEventTrigger_EVPluggedIn();
        break;
    case EnergyEvseTrigger::kEVPluggedInClear:
        ChipLogProgress(Support, "[EnergyEVSE-Test-Event] => EV unplugged");
        SetTestEventTrigger_EVPluggedInClear();
        break;
        
    default:
        return false;
    }

    return true;
}

bool AmebaHandleGlobalTestEventTrigger(uint64_t eventTrigger)
{
    // Customer can trigger action on the Ameba layer based on the eventTrigger
    ChipLogProgress(Support, "[AmebaHandleGlobalTestEventTrigger] Received Event Trigger: 0x%016llx", eventTrigger);
    return true;
}

namespace chip {

bool AmebaTestEventTriggerDelegate::DoesEnableKeyMatch(const ByteSpan & enableKey) const
{
    return !mEnableKey.empty() && mEnableKey.data_equal(enableKey);
}

CHIP_ERROR AmebaTestEventTriggerDelegate::HandleEventTrigger(uint64_t eventTrigger)
{
    // Custom implementation
    printf("*****%s %d  0x%x, Forecast=0x%x, BasicFunc=0x%x\n",
       __FUNCTION__, __LINE__, eventTrigger,DeviceEnergyManagementTrigger::kForecast, EnergyEvseTrigger::kBasicFunctionality);

    eventTrigger = clearEndpointInEventTrigger(eventTrigger);
    if ((HandleDeviceEnergyManagementTestEventTrigger(eventTrigger)) || (HandleEnergyEvseTestEventTrigger(eventTrigger))) 
    {
        return CHIP_NO_ERROR;
    }
    return CHIP_ERROR_INVALID_ARGUMENT;
}

} // namespace chip
