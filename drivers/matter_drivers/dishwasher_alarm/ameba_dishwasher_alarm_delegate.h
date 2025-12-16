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

#include <app-common/zap-generated/cluster-objects.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/clusters/dishwasher-alarm-server/dishwasher-alarm-server.h>

namespace chip {
namespace app {
namespace Clusters {
namespace DishwasherAlarm {

class AmebaDishwasherAlarmDelegate : public Delegate
{
public:
    /**
     *   @brief
     *   A notification that the Mask attribute will be changed.  When this happens, some previously suppressed
     *   alarms may need to be enabled, and previously enabled alarms may need to be suppressed.
     *   @param[in] mask The new value of the Mask attribute.
     *   @return The cluster will do this update if ModifyEnabledAlarmsCallback() returns true.
     *   The cluster will not do this update if ModifyEnabledAlarmsCallback() returns false.
     */
    bool ModifyEnabledAlarmsCallback(const BitMask<AlarmMap> mask) override;

    /**
     *   @brief
     *   A notification that resets active alarms (if possible)
     *   @param[in] alarms The value of reset alarms
     *   @return The cluster will reset active alarms if ResetAlarmsCallback() returns true.
     *   The cluster will not reset active alarms if ResetAlarmsCallback() returns false.
     */
    bool ResetAlarmsCallback(const BitMask<AlarmMap> alarms) override;

    ~AmebaDishwasherAlarmDelegate() = default;
};

AmebaDishwasherAlarmDelegate * GetAmebaDishwasherAlarmDelegate(void);
CHIP_ERROR AmebaDishWasherAlarmDelegateInit(EndpointId endpoint);
void AmebaDishWasherAlarmDelegateShutdown(void);

} // namespace DishwasherAlarm
} // namespace Clusters
} // namespace app
} // namespace chip
