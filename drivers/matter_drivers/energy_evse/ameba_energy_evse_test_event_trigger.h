/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
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

bool AmebaHandleEnergyEvseTestEventTrigger(uint64_t eventTrigger);

namespace chip {

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