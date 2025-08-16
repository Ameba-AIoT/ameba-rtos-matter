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
#include <app/clusters/resource-monitoring-server/resource-monitoring-cluster-objects.h>
#include <app/clusters/resource-monitoring-server/resource-monitoring-server.h>

namespace chip {
namespace app {
namespace Clusters {
namespace ActivatedCarbonFilterMonitoring {

class AmebaActivatedCarbonFilterMonitoringDelegate : public ResourceMonitoring::Delegate
{
public:
    ~AmebaActivatedCarbonFilterMonitoringDelegate() override = default;

private:
    CHIP_ERROR Init() override;
    chip::Protocols::InteractionModel::Status PreResetCondition() override;
    chip::Protocols::InteractionModel::Status PostResetCondition() override;
};

AmebaActivatedCarbonFilterMonitoringDelegate * GetAmebaActivatedCarbonFilterDelegate(void);
CHIP_ERROR AmebaActivatedCarbonFilterDelegateInit(EndpointId endpoint);
void AmebaActivatedCarbonFilterDelegateShutdown(void);

} // namespace ActivatedCarbonFilterMonitoring

namespace HepaFilterMonitoring {

class AmebaHepaFilterMonitoringDelegate : public ResourceMonitoring::Delegate
{
public:
    ~AmebaHepaFilterMonitoringDelegate() override = default;

private:
    CHIP_ERROR Init() override;
    chip::Protocols::InteractionModel::Status PreResetCondition() override;
    chip::Protocols::InteractionModel::Status PostResetCondition() override;
};

AmebaHepaFilterMonitoringDelegate * GetAmebaHepaFilterDelegate(void);
CHIP_ERROR AmebaHepaFilterDelegateInit(EndpointId endpoint);
void AmebaHepaFilterDelegateInit(void);

class AmebaImmutableReplacementProductListManager : public ResourceMonitoring::ReplacementProductListManager
{
public:
    CHIP_ERROR Next(ResourceMonitoring::ReplacementProductStruct & item) override;
};

} // namespace HepaFilterMonitoring
} // namespace Clusters
} // namespace app
} // namespace chip
