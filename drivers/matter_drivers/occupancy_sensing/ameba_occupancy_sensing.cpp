/*
 *
 *    Copyright (c) 2024 Project CHIP Authors
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

#include <app/CommandHandler.h>
#include <app/clusters/occupancy-sensor-server/occupancy-hal.h>
#include <app/clusters/occupancy-sensor-server/occupancy-sensor-server.h>
#include <platform/CHIPDeviceLayer.h>

using namespace chip;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OccupancySensing;
using namespace chip::app::Clusters::OccupancySensing::Structs;
using namespace chip::DeviceLayer;

using chip::Protocols::InteractionModel::Status;

Instance * gOccupancySensingCluster = nullptr;

void emberAfOccupancySensingClusterInitCallback(EndpointId endpointId)
{
    OccupancySensing::Structs::HoldTimeLimitsStruct::Type holdTimeLimits = {
        .holdTimeMin     = 1,
        .holdTimeMax     = 300,
        .holdTimeDefault = 10,
    };

    uint16_t holdTime = 10;

    VerifyOrDie(endpointId == 1); // this cluster is only enabled for endpoint 1.
    VerifyOrDie(gOccupancySensingCluster == nullptr);
    chip::BitMask<Feature, uint32_t> occupancySensingFeatures(Feature::kUltrasonic);
    gOccupancySensingCluster = new Instance(occupancySensingFeatures);

    if (gOccupancySensingCluster) {
        gOccupancySensingCluster->Init();
        SetHoldTimeLimits(endpointId, holdTimeLimits);
        SetHoldTime(endpointId, holdTime);
    }
}

HalOccupancySensorType halOccupancyGetSensorType(EndpointId endpoint)
{
    return HAL_OCCUPANCY_SENSOR_TYPE_ULTRASONIC;
}
