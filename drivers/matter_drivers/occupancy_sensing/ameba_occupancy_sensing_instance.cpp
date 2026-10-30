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
#include <occupancy_sensing/ameba_occupancy_sensing_instance.h>
#include <app/util/generic-callbacks.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OccupancySensing;

namespace {
LazyRegisteredServerCluster<OccupancySensingCluster> gAmebaOccupancySensingInstance;
} // namespace

LazyRegisteredServerCluster<OccupancySensingCluster> *OccupancySensing::GetAmebaOccupancySensingInstance(void)
{
    return &gAmebaOccupancySensingInstance;
}

CHIP_ERROR OccupancySensing::AmebaOccupancySensingInstanceInit(EndpointId endpoint)
{
    // Configure the Occupancy Sensing cluster with the required features.
    // kOccupancyEvent is always added by WithFeatures().
    OccupancySensingCluster::Config config(endpoint);
    config.WithFeatures(BitFlags<OccupancySensing::Feature>(OccupancySensing::Feature::kPassiveInfrared));

    gAmebaOccupancySensingInstance.Create(config);

    CHIP_ERROR err = CodegenDataModelProvider::Instance().Registry().Register(gAmebaOccupancySensingInstance.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "OccupancySensing cluster error registration: %" CHIP_ERROR_FORMAT, err.Format());
        return err;
    }

    ChipLogProgress(AppServer, "OccupancySensing cluster registered with FeatureMap 0x%02X",
                    gAmebaOccupancySensingInstance.Cluster().GetFeatureMap().Raw());

    return CHIP_NO_ERROR;
}

void OccupancySensing::AmebaOccupancySensingShutdown(void)
{
    if (!gAmebaOccupancySensingInstance.IsConstructed()) {
        return;
    }

    CHIP_ERROR err = CodegenDataModelProvider::Instance().Registry().Unregister(&gAmebaOccupancySensingInstance.Cluster());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "OccupancySensing unregister error: %" CHIP_ERROR_FORMAT, err.Format());
    }

    gAmebaOccupancySensingInstance.Destroy();
}

void MatterOccupancySensingClusterInitCallback(EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = OccupancySensing::AmebaOccupancySensingInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR) {
        ChipLogProgress(Zcl, "AmebaOccupancySensingInstanceInit Failed");
        return;
    }
}

void MatterOccupancySensingClusterShutdownCallback(EndpointId endpoint, MatterClusterShutdownType)
{
    OccupancySensing::AmebaOccupancySensingShutdown();
}

// Legacy PluginServer callback stubs
void MatterOccupancySensingPluginServerInitCallback() {}
void MatterOccupancySensingPluginServerShutdownCallback() {}
