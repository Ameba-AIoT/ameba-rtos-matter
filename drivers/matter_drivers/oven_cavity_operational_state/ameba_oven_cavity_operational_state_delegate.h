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
namespace OvenCavityOperationalState {

class AmebaOvenCavityOperationalStateDelegate : public OperationalState::Delegate
{
private:
    inline static const Clusters::OperationalState::GenericOperationalState opStateList[] = {
        OperationalState::GenericOperationalState(to_underlying(OperationalState::OperationalStateEnum::kStopped)),
        OperationalState::GenericOperationalState(to_underlying(OperationalState::OperationalStateEnum::kRunning)),
        OperationalState::GenericOperationalState(to_underlying(OperationalState::OperationalStateEnum::kPaused)),
        OperationalState::GenericOperationalState(to_underlying(OperationalState::OperationalStateEnum::kError))
    };

    Span<const OperationalState::GenericOperationalState> mOperationalStateList =
        Span<const OperationalState::GenericOperationalState>(opStateList);
    Span<const CharSpan> mOperationalPhaseList;

public:
    DataModel::Nullable<uint32_t> GetCountdownTime() override { return DataModel::NullNullable; };

    CHIP_ERROR GetOperationalStateAtIndex(size_t index,
                                          Clusters::OperationalState::GenericOperationalState & operationalState) override;
    CHIP_ERROR GetOperationalPhaseAtIndex(size_t index, MutableCharSpan & operationalPhase) override;

    void HandlePauseStateCallback(Clusters::OperationalState::GenericOperationalError & err) override;
    void HandleResumeStateCallback(Clusters::OperationalState::GenericOperationalError & err) override;
    void HandleStartStateCallback(Clusters::OperationalState::GenericOperationalError & err) override;
    void HandleStopStateCallback(Clusters::OperationalState::GenericOperationalError & err) override;
};

AmebaOvenCavityOperationalStateDelegate * GetAmebaOvenCavityOperationalStateDelegate(void);
CHIP_ERROR AmebaOvenCavityOperationalStateDelegateInit(EndpointId endpoint);
void AmebaOvenCavityOperationalStateDelegateShutdown(void);

} // namespace OvenCavityOperationalState
} // namespace Clusters
} // namespace app
} // namespace chip
