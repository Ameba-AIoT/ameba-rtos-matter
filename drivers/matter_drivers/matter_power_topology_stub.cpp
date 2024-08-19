/*
 *
 *    Copyright (c) 2024 Project CHIP Authors
 *    All rights reserved.
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
#include <matter_power_topology_delegate.h>
#include <app/clusters/power-topology-server/power-topology-server.h>

using namespace chip;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::PowerTopology;

static std::unique_ptr<PowerTopology::Delegate> gDelegate;
static std::unique_ptr<PowerTopology::Instance> gInstance;

void emberAfPowerTopologyClusterInitCallback(chip::EndpointId endpointId)
{
    VerifyOrDie(endpointId == 1); // this cluster is only enabled for endpoint 1.
    VerifyOrDie(!gInstance);

    gDelegate = std::make_unique<PowerTopologyDelegate>();
    if (gDelegate)
    {
        gInstance = std::make_unique<Instance>(
            endpointId, *gDelegate, BitMask<Feature, uint32_t>(Feature::kNodeTopology),
            BitMask<OptionalAttributes, uint32_t>(0));

        gInstance->Init();
    }
}
