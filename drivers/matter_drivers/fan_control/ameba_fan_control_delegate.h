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
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/clusters/fan-control-server/fan-control-server.h>

namespace chip {
namespace app {
namespace Clusters {

namespace FanControl {

class AmebaFanControlDelegate : public AttributeAccessInterface, public Delegate
{
public:
    // Register for the FanControl cluster on all endpoints.
    AmebaFanControlDelegate(EndpointId aEndpointId) :
        AttributeAccessInterface(Optional<EndpointId>(aEndpointId), FanControl::Id), Delegate(aEndpointId)
    {}

    CHIP_ERROR Read(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder) override;
    Protocols::InteractionModel::Status HandleStep(StepDirectionEnum aDirection, bool aWrap, bool aLowestOff) override;

private:
    CHIP_ERROR ReadPercentCurrent(AttributeValueEncoder & aEncoder);
    CHIP_ERROR ReadSpeedCurrent(AttributeValueEncoder & aEncoder);
    CHIP_ERROR ReadPercentSetting(AttributeValueEncoder & aEncoder);
    CHIP_ERROR ReadSpeedSetting(AttributeValueEncoder & aEncoder);
};

AmebaFanControlDelegate * GetAmebaFanControlDelegate(void);
CHIP_ERROR AmebaFanControlDelegateInit(EndpointId endpoint);
void AmebaFanControlDelegateShutdown(void);

} // namespace FanControl
} // namespace Clusters
} // namespace app
} // namespace chip
