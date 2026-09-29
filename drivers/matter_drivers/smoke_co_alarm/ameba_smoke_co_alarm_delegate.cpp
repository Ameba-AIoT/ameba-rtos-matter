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

    SmokeCoAlarmCluster::Config config;
    config.featureMap.Set(Feature::kSmokeAlarm).Set(Feature::kCoAlarm);
    config.optionalAttribs = SmokeCoAlarmCluster::OptionalAttributeSet(SmokeCoAlarmCluster::OptionalAttributeSet::All());
    LogErrorOnFailure(SmokeCoAlarmServer::Instance().Init(endpoint, config, &sSmokeCODelegate));

    // utc_time_in_matter_epoch(datetime(2126, 1, 1, tzinfo=timezone.utc)) / 1_000_000
    SmokeCoAlarmServer::Instance().SetExpiryDate(endpoint, 3976214400);

    return CHIP_NO_ERROR;
}
