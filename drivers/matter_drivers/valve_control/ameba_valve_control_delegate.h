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

#include <app/clusters/time-synchronization-server/DefaultTimeSyncDelegate.h>
#include <app/clusters/valve-configuration-and-control-server/valve-configuration-and-control-delegate.h>
#include <app/clusters/valve-configuration-and-control-server/valve-configuration-and-control-cluster.h>

namespace chip {
namespace app {
namespace Clusters {
namespace ValveConfigurationAndControl {

class AmebaValveControlDelegate : public Delegate
{
public:
    DataModel::Nullable<chip::Percent> HandleOpenValve(DataModel::Nullable<chip::Percent> level) override;
    CHIP_ERROR HandleCloseValve() override;
    void HandleRemainingDurationTick(uint32_t duration) override;
};

AmebaValveControlDelegate * GetAmebaValveControlDelegate(void);
CHIP_ERROR AmebaValveControlDelegateInit(EndpointId endpoint);
void AmebaValveControlDelegateShutdown(void);

} // namespace ValveConfigurationAndControl

namespace TimeSynchronization {

class AmebaExtendedTimeSyncDelegate : public DefaultTimeSyncDelegate
{
public:
    void UTCTimeAvailabilityChanged(uint64_t time) override;
};

} // namespace TimeSynchronization
} // namespace Clusters
} // namespace app
} // namespace chip
