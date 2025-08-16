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

#include <air_quality/ameba_air_quality_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::AirQuality;

namespace {
static Instance * gAmebaAirQualityInstance = nullptr;
} // namespace

void AirQuality::SetAirQuality(AirQualityEnum aNewAirQuality)
{
    if (gAmebaAirQualityInstance != nullptr) {
        ChipLogProgress(DeviceLayer, "Update AirQuality to 0x%x", aNewAirQuality);
        gAmebaAirQualityInstance->UpdateAirQuality(aNewAirQuality);
    }
}

AirQualityEnum AirQuality::CurrentAirQuality(void)
{
    if (gAmebaAirQualityInstance != nullptr) {
        return gAmebaAirQualityInstance->GetAirQuality();
    }
    return AirQualityEnum::kUnknown;
}

Instance * AirQuality::GetAmebaAirQualityInstance(void)
{
    return gAmebaAirQualityInstance;
}

CHIP_ERROR AirQuality::AmebaAirQualityInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaAirQualityInstance == nullptr, CHIP_ERROR_INTERNAL);

    chip::BitMask<AirQuality::Feature, uint32_t> Features(
        AirQuality::Feature::kModerate,
        AirQuality::Feature::kFair,
        AirQuality::Feature::kVeryPoor,
        AirQuality::Feature::kExtremelyPoor);

    gAmebaAirQualityInstance = new AirQuality::Instance(endpoint, Features);
    VerifyOrReturnError(gAmebaAirQualityInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaAirQualityInstance->Init();

    return CHIP_NO_ERROR;
}

void AirQuality::AmebaAirQualityInstanceShutdown(void)
{
    if (gAmebaAirQualityInstance != nullptr) {
        delete gAmebaAirQualityInstance;
        gAmebaAirQualityInstance = nullptr;
    }
}

void emberAfAirQualityClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaAirQualityInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaAirQualityInstanceInit Failed");
        return;
    }
}

void emberAfAirQualityClusterShutdownCallback(chip::EndpointId endpoint)
{
    AmebaAirQualityInstanceShutdown();
}
