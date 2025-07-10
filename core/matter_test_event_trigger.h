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
#pragma once

#include <stdint.h>

#include <app/TestEventTriggerDelegate.h>
#include <lib/core/CHIPError.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/Span.h>

/**
 * @brief User handler for handling the test event trigger
 *
 * @note If TestEventTrigger is enabled, it needs to be implemented in the app
 *
 * @param eventTrigger Event trigger to handle
 *
 * @warning *** DO NOT USE FOR STANDARD CLUSTER EVENT TRIGGERS ***
 *
 * @retval true on success
 * @retval false if error happened
 */
bool HandleDeviceEnergyManagementTestEventTrigger(uint64_t eventTrigger);

namespace chip {

enum class DeviceEnergyManagementTrigger : uint64_t
{
    // Simulate a fixed forecast power usage including one or more PowerAdjustmentStructs
    kPowerAdjustment = 0x0098'0000'0000'0000,
    // Clear the PowerAdjustment structs
    kPowerAdjustmentClear = 0x0098'0000'0000'0001,
    // Simulate user opt-out of Local Optimization
    kUserOptOutLocalOptimization = 0x0098'0000'0000'0002,
    // Simulate user opt-out of Grid Optimization
    kUserOptOutGridOptimization = 0x0098'0000'0000'0003,
    // Remove all user opt-out opting out
    kUserOptOutClearAll = 0x0098'0000'0000'0004,
    // Simulate a fixed forecast with EarliestStartTime earlier than startTime, and LatestEndTime greater than EndTime
    kStartTimeAdjustment = 0x0098'0000'0000'0005,
    // Clear the StartTimeAdjustment simulated forecast
    kStartTimeAdjustmentClear = 0x0098'0000'0000'0006,
    // Simulate a fixed forecast with one pausable slo with MinPauseDuration >1, MaxPauseDuration>1 and one non pausable slot
    kPausable = 0x0098'0000'0000'0007,
    // Simulate a moving time to the next forecast slot
    kPausableNextSlot = 0x0098'0000'0000'0008,
    // Clear the Pausable simulated forecast
    kPausableClear = 0x0098'0000'0000'0009,
    // Simulate a forecast power usage with at least 2 and at most 4 slots
    kForecastAdjustment = 0x0098'0000'0000'000A,
    // Simulate moving time to the next forecast slot
    kForecastAdjustmentNextSlot = 0x0098'0000'0000'000B,
    // Clear the forecast adjustment
    kForecastAdjustmentClear = 0x0098'0000'0000'000C,
    // Simulate a forecast power usage with at least 2 and at most 4 slots
    kConstraintBasedAdjustment = 0x0098'0000'0000'000D,
    //  Clear the constraint based adjustment
    kConstraintBasedAdjustmentClear = 0x0098'0000'0000'000E,
    // Simulate a forecast with at least 1 slot
    kForecast = 0x0098'0000'0000'000F,
    // Clear the forecast
    kForecastClear = 0x0098'0000'0000'0010,
};

enum class EnergyEvseTrigger : uint64_t
{
    // Scenarios
    // Basic Functionality Test Event       | Simulate installation with _{A_CIRCUIT_CAPACITY}_=32A and
    // _{A_USER_MAXIMUM_CHARGE_CURRENT}_=32A
    kBasicFunctionality = 0x0099000000000000,
    // Basic Functionality Test Event Clear | End simulation of installation
    kBasicFunctionalityClear = 0x0099000000000001,
    // EV Plugged-in Test Event | Simulate plugging the EV into the EVSE using a cable of 63A capacity
    kEVPluggedIn = 0x0099000000000002,
    // EV Plugged-in Test Event Clear | Simulate unplugging the EV
    kEVPluggedInClear = 0x0099000000000003,
    // EV Charge Demand Test Event | Simulate the EV presenting charge demand to the EVSE
    kEVChargeDemand = 0x0099000000000004,
    // EV Charge Demand Test Event Clear | Simulate the EV becoming fully charged
    kEVChargeDemandClear = 0x0099000000000005,
    // EV Charge TimeOfUse Mode | Simulate putting the EVSE into a Mode with the TimeOfUse tag included
    kEVTimeOfUseMode = 0x0099000000000006,
    // EVSE has a GroundFault fault
    kEVSEGroundFault = 0x0099000000000010,
    // EVSE has a OverTemperature fault
    kEVSEOverTemperatureFault = 0x0099000000000011,
    // EVSE faults have cleared
    kEVSEFaultClear = 0x0099000000000012,
    // EVSE Diagnostics Complete | Simulate diagnostics have been completed and return to normal
    kEVSEDiagnosticsComplete = 0x0099000000000020,
    // EV Charge TimeOfUse Mode clear | Simulate clearing the EVSE Mode TimeOfUse tag
    kEVTimeOfUseModeClear = 0x0099000000000021,
    // EVSE Set SoC Low | Simulate a vehicle reporting 20% State of Charge, 70kWh battery capacity
    kEVSESetSoCLow = 0x0099000000000030,
    // EVSE Set SoC High | Simulate a vehicle reporting 95% State of Charge, 70kWh battery capacity
    kEVSESetSoCHigh = 0x0099000000000031,
    // EVSE Set SoC Clear | Simulate no vehicle State of Charge information
    kEVSESetSoCClear = 0x0099000000000032,
    // EVSE Set VehicleID | Simulate a vehicle ID being sent
    kEVSESetVehicleID = 0x0099000000000040,
    // EVSE Trigger RFID | Simulate an RFID tag being activated
    kEVSETriggerRFID = 0x0099000000000050,
};
} // namespace chip
