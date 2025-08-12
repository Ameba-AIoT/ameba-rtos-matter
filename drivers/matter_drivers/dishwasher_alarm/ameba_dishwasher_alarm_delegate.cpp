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

#include <dishwasher_alarm/ameba_dishwasher_alarm_delegate.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::DishwasherAlarm;

static AmebaDishwasherAlarmDelegate * gAmebaDishwasherAlarmDelegate = nullptr;

bool AmebaDishwasherAlarmDelegate::ModifyEnabledAlarmsCallback(const BitMask<AlarmMap> mask)
{
    return true;
}

bool AmebaDishwasherAlarmDelegate::ResetAlarmsCallback(const BitMask<AlarmMap> alarms)
{
    return true;
}

AmebaDishwasherAlarmDelegate * DishwasherAlarm::GetAmebaDishwasherAlarmDelegate(void)
{
    return gAmebaDishwasherAlarmDelegate;
}

CHIP_ERROR DishwasherAlarm::AmebaDishWasherAlarmDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaDishwasherAlarmDelegate == nullptr, CHIP_ERROR_INTERNAL);

    gAmebaDishwasherAlarmDelegate = new DishwasherAlarm::AmebaDishwasherAlarmDelegate;

    VerifyOrReturnError(gAmebaDishwasherAlarmDelegate != nullptr, CHIP_ERROR_INTERNAL);

    DishwasherAlarm::SetDefaultDelegate(endpoint, gAmebaDishwasherAlarmDelegate);

    return CHIP_NO_ERROR;
}

void DishwasherAlarm::AmebaDishWasherAlarmDelegateShutdown(void)
{
    if (gAmebaDishwasherAlarmDelegate != nullptr)
    {
        delete gAmebaDishwasherAlarmDelegate;
        gAmebaDishwasherAlarmDelegate = nullptr;
    }
}
