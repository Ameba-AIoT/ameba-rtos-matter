/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
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
#include <smoke_co_alarm/ameba_smoke_co_alarm_delegate.h>
#include <lib/support/CodeUtils.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::SmokeCoAlarm;

CHIP_ERROR SmokeCoAlarm::AmebaSmokeCoAlarmDelegateInit(EndpointId endpoint)
{
    static AmebaSmokeCoAlarmDelegate sSmokeCODelegate;
    constexpr uint32_t kExpiryDate = 3976214400;
    SmokeCoAlarmCluster::Config config;
    config
    .WithSmokeAlarm({
        .withContaminationState = true,
        .sensitivityLevel       = SensitivityEnum::kStandard,
    })
    .WithCOAlarm()
    .WithDeviceMuted()
    .WithInterconnectSmokeAlarm()
    .WithInterconnectCOAlarm()
    .WithExpiryDate(kExpiryDate)
    .WithUnmounted();

    LogErrorOnFailure(SmokeCoAlarmServer::Instance().Init(endpoint, config, &sSmokeCODelegate));

    return CHIP_NO_ERROR;
}
