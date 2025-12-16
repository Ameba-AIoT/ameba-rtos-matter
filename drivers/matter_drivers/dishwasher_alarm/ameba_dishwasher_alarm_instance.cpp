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
#include <dishwasher_alarm/ameba_dishwasher_alarm_instance.h>
#include <protocols/interaction_model/StatusCode.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::DishwasherAlarm;
using Protocols::InteractionModel::Status;

CHIP_ERROR DishwasherAlarm::AmebaDishWasherAlarmInstanceInit(chip::EndpointId endpoint)
{
    Status status = Status::Success;

    BitMask<DishwasherAlarm::AlarmMap> supported;
    supported.SetField(DishwasherAlarm::AlarmMap::kDoorError, 1);
    status = DishwasherAlarm::DishwasherAlarmServer::Instance().SetSupportedValue(endpoint, supported);
    VerifyOrReturnError(status == Status::Success, CHIP_ERROR_INTERNAL);

    BitMask<DishwasherAlarm::AlarmMap> mask;
    mask.SetField(DishwasherAlarm::AlarmMap::kDoorError, 1);
    status = DishwasherAlarm::DishwasherAlarmServer::Instance().SetMaskValue(endpoint, mask);
    VerifyOrReturnError(status == Status::Success, CHIP_ERROR_INTERNAL);

    BitMask<DishwasherAlarm::AlarmMap> latch;
    latch.SetField(DishwasherAlarm::AlarmMap::kDoorError, 1);
    status = DishwasherAlarm::DishwasherAlarmServer::Instance().SetLatchValue(endpoint, latch);
    VerifyOrReturnError(status == Status::Success, CHIP_ERROR_INTERNAL);

    BitMask<DishwasherAlarm::AlarmMap> state;
    state.SetField(DishwasherAlarm::AlarmMap::kDoorError, 1);
    status = DishwasherAlarm::DishwasherAlarmServer::Instance().SetStateValue(endpoint, state);
    VerifyOrReturnError(status == Status::Success, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

void emberAfDishwasherAlarmClusterInitCallback(chip::EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaDishWasherAlarmDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaDishWasherAlarmDelegateInit Failed");
        return;
    }

    ret = DishwasherAlarm::AmebaDishWasherAlarmInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaDishWasherAlarmInstanceInit Failed");
        return;
    }
}

void emberAfDishwasherAlarmClusterShutdownCallback(chip::EndpointId endpoint)
{
    AmebaDishWasherAlarmDelegateShutdown();
}
