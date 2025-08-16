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

void SetCondition(uint8_t value);
void SetChangeIndication(ResourceMonitoring::ChangeIndicationEnum aNewChangeIndication);

ResourceMonitoring::Instance * GetAmebaActivatedCarbonFilterInstance(void);
CHIP_ERROR AmebaActivatedCarbonFilterInstanceInit(EndpointId endpoint);
void AmebaActivatedCarbonFilterInstanceShutdown(void);

} // namespace ActivatedCarbonFilterMonitoring

namespace HepaFilterMonitoring {

void SetCondition(uint8_t value);
void SetChangeIndication(ResourceMonitoring::ChangeIndicationEnum aNewChangeIndication);

ResourceMonitoring::Instance * GetAmebaHepaFilterInstance(void);
CHIP_ERROR AmebaHepaFilterInstanceInit(EndpointId endpoint);
void AmebaHepaFilterInstanceShutdown(void);

} // namespace HepaFilterMonitoring
} // namespace Clusters
} // namespace app
} // namespace chip

