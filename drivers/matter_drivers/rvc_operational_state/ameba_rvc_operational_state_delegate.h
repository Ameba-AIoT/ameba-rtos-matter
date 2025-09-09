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
#include <app/clusters/operational-state-server/operational-state-server.h>

namespace chip {
namespace app {
namespace Clusters {
namespace RvcOperationalState {

// This is an application level delegate to handle operational state commands according to the specific business logic.
class AmebaRvcOperationalStateDelegate : public Delegate
{
private:
    const Clusters::OperationalState::GenericOperationalState rvcOpStateList[11] = {
        OperationalState::GenericOperationalState(to_underlying(OperationalState::OperationalStateEnum::kStopped)), // 0
        OperationalState::GenericOperationalState(to_underlying(OperationalState::OperationalStateEnum::kRunning)), // 1
        OperationalState::GenericOperationalState(to_underlying(OperationalState::OperationalStateEnum::kPaused)), // 2
        OperationalState::GenericOperationalState(to_underlying(OperationalState::OperationalStateEnum::kError)), // 3
        OperationalState::GenericOperationalState(to_underlying(Clusters::RvcOperationalState::OperationalStateEnum::kSeekingCharger)), // 64
        OperationalState::GenericOperationalState(to_underlying(Clusters::RvcOperationalState::OperationalStateEnum::kCharging)), // 65
        OperationalState::GenericOperationalState(to_underlying(Clusters::RvcOperationalState::OperationalStateEnum::kDocked)), // 66
        OperationalState::GenericOperationalState(to_underlying(Clusters::RvcOperationalState::OperationalStateEnum::kEmptyingDustBin)), //67
        OperationalState::GenericOperationalState(to_underlying(Clusters::RvcOperationalState::OperationalStateEnum::kCleaningMop)), // 68
        OperationalState::GenericOperationalState(to_underlying(Clusters::RvcOperationalState::OperationalStateEnum::kFillingWaterTank)), //69
        OperationalState::GenericOperationalState(to_underlying(Clusters::RvcOperationalState::OperationalStateEnum::kUpdatingMaps)), //70
    };

    Span<const OperationalState::GenericOperationalState> mOperationalStateList =
        Span<const OperationalState::GenericOperationalState>(rvcOpStateList);
    Span<const CharSpan> mOperationalPhaseList;

public:
    AmebaRvcOperationalStateDelegate() {}
    app::DataModel::Nullable<uint32_t> GetCountdownTime() override { return {}; };

    CHIP_ERROR GetOperationalStateAtIndex(size_t index, OperationalState::GenericOperationalState & operationalState) override;
    CHIP_ERROR GetOperationalPhaseAtIndex(size_t index, MutableCharSpan & operationalPhase) override;

    void HandlePauseStateCallback(OperationalState::GenericOperationalError & err) override;
    void HandleResumeStateCallback(OperationalState::GenericOperationalError & err) override;
    void HandleGoHomeCommandCallback(OperationalState::GenericOperationalError & err) override;
};

AmebaRvcOperationalStateDelegate * GetAmebaRvcOperationalStateDelegate(void);
CHIP_ERROR AmebaRvcOperationalStateDelegateInit(EndpointId endpoint);
void AmebaRvcOperationalStateDelegateShutdown(void);

} // namespace RvcOperationalState
} // namespace Clusters
} // namespace app
} // namespace chip
