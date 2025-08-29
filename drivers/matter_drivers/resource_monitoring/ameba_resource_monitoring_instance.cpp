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

#include <resource_monitoring/ameba_resource_monitoring_delegate.h>
#include <resource_monitoring/ameba_resource_monitoring_instance.h>
#include <protocols/interaction_model/StatusCode.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ResourceMonitoring;
using namespace chip::app::Clusters::ActivatedCarbonFilterMonitoring;
using namespace chip::app::Clusters::HepaFilterMonitoring;
using chip::Protocols::InteractionModel::Status;

ResourceMonitoring::Instance * gAmebaActivatedCarbonFilterInstance = nullptr;
ResourceMonitoring::Instance * gAmebaHepaFilterInstance = nullptr;

//-- Activated Carbon Filter Monitoring delegate methods
void ActivatedCarbonFilterMonitoring::SetCondition(uint8_t value)
{
    Status status;
    if (GetAmebaActivatedCarbonFilterInstance() != nullptr)
    {
        status = GetAmebaActivatedCarbonFilterInstance()->UpdateCondition(value);
        if (status != Status::Success)
        {
            ChipLogProgress(DeviceLayer, "Update Condition Failed");
        }
        else
        {
            ChipLogProgress(DeviceLayer, "Update Condition to %d", value);
        }
    }
}

void ActivatedCarbonFilterMonitoring::SetChangeIndication(ResourceMonitoring::ChangeIndicationEnum aNewChangeIndication)
{
    Status status;
    if (GetAmebaActivatedCarbonFilterInstance() != nullptr)
    {
        status = GetAmebaActivatedCarbonFilterInstance()->UpdateChangeIndication(aNewChangeIndication);
        if (status != Status::Success)
        {
            ChipLogProgress(DeviceLayer, "Change Indication Failed");
        }
        else
        {
            ChipLogProgress(DeviceLayer, "Change Indication to %d", aNewChangeIndication);
        }
    }
}

ResourceMonitoring::Instance *
ActivatedCarbonFilterMonitoring::GetAmebaActivatedCarbonFilterInstance(void)
{
    return gAmebaActivatedCarbonFilterInstance;
}

CHIP_ERROR ActivatedCarbonFilterMonitoring::AmebaActivatedCarbonFilterInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaActivatedCarbonFilterInstance == nullptr, CHIP_ERROR_INTERNAL);

    constexpr std::bitset<4> Features{
        static_cast<uint32_t>(ResourceMonitoring::Feature::kCondition)
    };

    auto * delegate = GetAmebaActivatedCarbonFilterDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaActivatedCarbonFilterInstance= new ResourceMonitoring::Instance(
        delegate,
        endpoint,
        ActivatedCarbonFilterMonitoring::Id,
        static_cast<uint32_t>(Features.to_ulong()),
        ResourceMonitoring::DegradationDirectionEnum::kDown,
        true);
    VerifyOrReturnError(gAmebaActivatedCarbonFilterInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaActivatedCarbonFilterInstance->Init();

    return CHIP_NO_ERROR;
}

void ActivatedCarbonFilterMonitoring::AmebaActivatedCarbonFilterInstanceShutdown(void)
{
    if (gAmebaActivatedCarbonFilterInstance != nullptr)
    {
        delete gAmebaActivatedCarbonFilterInstance;
        gAmebaActivatedCarbonFilterInstance = nullptr;
    }
}

void emberAfActivatedCarbonFilterMonitoringClusterInitCallback(chip::EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ActivatedCarbonFilterMonitoring::AmebaActivatedCarbonFilterDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaActivatedCarbonFilterDelegateInit Failed");
        return;
    }

    ret = ActivatedCarbonFilterMonitoring::AmebaActivatedCarbonFilterInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaActivatedCarbonInstanceInit Failed");
        return;
    }
}

// HEPA Filter Monitoring methods
void HepaFilterMonitoring::SetCondition(uint8_t value)
{
    Status status;
    if (GetAmebaHepaFilterInstance() != nullptr)
    {
        status = GetAmebaHepaFilterInstance()->UpdateCondition(value);
        if (status != Status::Success)
        {
            ChipLogProgress(DeviceLayer, "Update Condition Failed");
        }
        else
        {
            ChipLogProgress(DeviceLayer, "Update Condition to %d", value);
        }
    }
}

void HepaFilterMonitoring::SetChangeIndication(ResourceMonitoring::ChangeIndicationEnum aNewChangeIndication)
{
    Status status;
    if (GetAmebaHepaFilterInstance() != nullptr)
    {
        status = GetAmebaHepaFilterInstance()->UpdateChangeIndication(aNewChangeIndication);
        if (status != Status::Success)
        {
            ChipLogProgress(DeviceLayer, "Change Indication Failed");
        }
        else
        {
            ChipLogProgress(DeviceLayer, "Change Indication to %d", aNewChangeIndication);
        }
    }
}

ResourceMonitoring::Instance * HepaFilterMonitoring::GetAmebaHepaFilterInstance(void)
{
    return gAmebaHepaFilterInstance;
}

CHIP_ERROR HepaFilterMonitoring::AmebaHepaFilterInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaHepaFilterInstance == nullptr, CHIP_ERROR_INTERNAL);

    constexpr std::bitset<4> Features{
        static_cast<uint32_t>(ResourceMonitoring::Feature::kCondition)
    };

    auto * delegate = GetAmebaHepaFilterDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaHepaFilterInstance= new ResourceMonitoring::Instance(
        delegate,
        endpoint,
        HepaFilterMonitoring::Id,
        static_cast<uint32_t>(Features.to_ulong()),
        ResourceMonitoring::DegradationDirectionEnum::kDown,
        true);
    VerifyOrReturnError(gAmebaHepaFilterInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaHepaFilterInstance->Init();

    return CHIP_NO_ERROR;
}

void HepaFilterMonitoring::AmebaHepaFilterInstanceShutdown(void)
{
    if (gAmebaHepaFilterInstance != nullptr)
    {
        delete gAmebaHepaFilterInstance;
        gAmebaHepaFilterInstance = nullptr;
    }
}

void emberAfHepaFilterMonitoringClusterInitCallback(chip::EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = HepaFilterMonitoring::AmebaHepaFilterDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaHepaFilterDelegateInit Failed");
        return;
    }

    ret = HepaFilterMonitoring::AmebaHepaFilterInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaHepaInstanceInit Failed");
        return;
    }
}
