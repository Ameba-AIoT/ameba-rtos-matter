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

using namespace chip;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OccupancySensing;
using namespace chip::app::Clusters::OccupancySensing::Structs;

static OccupancySensing::Instance * gAmebaOccupancySensingInstances = nullptr;

Instance * OccupancySensing::GetOccupancySensingInstance(void)
{
    return gAmebaOccupancySensingInstances;
}

CHIP_ERROR OccupancySensing::AmebaOccupancySensingInstanceInit(EndpointId endpointId)
{
    VerifyOrReturnError(gAmebaOccupancySensingInstances == nullptr, CHIP_ERROR_INTERNAL);

    BitMask<OccupancySensing::Feature, uint32_t> Features(
        OccupancySensing::Feature::kPassiveInfrared);

    gAmebaOccupancySensingInstances = new OccupancySensing::Instance(Features);

    VerifyOrReturnError(gAmebaOccupancySensingInstances != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaOccupancySensingInstances->Init();

    OccupancySensing::Structs::HoldTimeLimitsStruct::Type holdTimeLimits = {
        .holdTimeMin     = 1,
        .holdTimeMax     = 300,
        .holdTimeDefault = 10,
    };

    uint16_t holdTime = 10;
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = SetHoldTimeLimits(endpointId, holdTimeLimits);
    VerifyOrReturnError(ret != CHIP_NO_ERROR, ret);
    ret = SetHoldTime(endpointId, holdTime);
    VerifyOrReturnError(ret != CHIP_NO_ERROR, ret);

    return CHIP_NO_ERROR;
}

void OccupancySensing::AmebaOccupancySensingInstanceShutdown(void)
{
    if (gAmebaOccupancySensingInstances != nullptr) {
        delete gAmebaOccupancySensingInstances;
        gAmebaOccupancySensingInstances = nullptr;
    }
}

void emberAfOccupancySensingClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaOccupancySensingInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOccupancySensingInstanceInit Failed");
        return;
    }
}

void emberAfOccupancySensingClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaOccupancySensingInstanceShutdown();
}

