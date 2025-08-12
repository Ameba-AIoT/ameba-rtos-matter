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

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ResourceMonitoring;
using namespace chip::app::Clusters::ActivatedCarbonFilterMonitoring;
using namespace chip::app::Clusters::HepaFilterMonitoring;
using chip::Protocols::InteractionModel::Status;

AmebaActivatedCarbonFilterMonitoringDelegate * gAmebaActivatedCarbonFilterDelegate = nullptr;
AmebaHepaFilterMonitoringDelegate * gAmebaHepaFilterDelegate = nullptr;
static AmebaImmutableReplacementProductListManager sReplacementProductListManager;

//-- Activated Carbon Filter Monitoring delegate methods
CHIP_ERROR AmebaActivatedCarbonFilterMonitoringDelegate::Init()
{
    GetInstance()->SetReplacementProductListManagerInstance(&sReplacementProductListManager);
    return CHIP_NO_ERROR;
}

Status AmebaActivatedCarbonFilterMonitoringDelegate::PreResetCondition()
{
    return Status::Success;
}

Status AmebaActivatedCarbonFilterMonitoringDelegate::PostResetCondition()
{
    return Status::Success;
}

// HEPA Filter Monitoring methods
CHIP_ERROR AmebaHepaFilterMonitoringDelegate::Init()
{
    GetInstance()->SetReplacementProductListManagerInstance(&sReplacementProductListManager);
    return CHIP_NO_ERROR;
}

Status AmebaHepaFilterMonitoringDelegate::PreResetCondition()
{
    return Status::Success;
}

Status AmebaHepaFilterMonitoringDelegate::PostResetCondition()
{
    return Status::Success;
}

CHIP_ERROR AmebaImmutableReplacementProductListManager::Next(ReplacementProductStruct & item)
{
    if (mIndex >= kReplacementProductListMaxSize)
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    switch (mIndex)
    {
    case 0: {
        item.SetProductIdentifierType(ResourceMonitoring::ProductIdentifierTypeEnum::kUpc);
        item.SetProductIdentifierValue(CharSpan::fromCharString("111112222233"));
        break;
    case 1:
        item.SetProductIdentifierType(ResourceMonitoring::ProductIdentifierTypeEnum::kGtin8);
        item.SetProductIdentifierValue(CharSpan::fromCharString("gtin8xxx"));
        break;
    case 2:
        item.SetProductIdentifierType(ResourceMonitoring::ProductIdentifierTypeEnum::kEan);
        item.SetProductIdentifierValue(CharSpan::fromCharString("4444455555666"));
        break;
    case 3:
        item.SetProductIdentifierType(ResourceMonitoring::ProductIdentifierTypeEnum::kGtin14);
        item.SetProductIdentifierValue(CharSpan::fromCharString("gtin14xxxxxxxx"));
        break;
    case 4:
        item.SetProductIdentifierType(ResourceMonitoring::ProductIdentifierTypeEnum::kOem);
        item.SetProductIdentifierValue(CharSpan::fromCharString("oem20xxxxxxxxxxxxxxx"));
        break;
    default:
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
        break;
    }
    }
    mIndex++;
    return CHIP_NO_ERROR;
}

AmebaActivatedCarbonFilterMonitoringDelegate *
ActivatedCarbonFilterMonitoring::GetAmebaActivatedCarbonFilterDelegate(void)
{
    return gAmebaActivatedCarbonFilterDelegate;
}

CHIP_ERROR ActivatedCarbonFilterMonitoring::AmebaActivatedCarbonFilterDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaActivatedCarbonFilterDelegate == nullptr, CHIP_ERROR_INTERNAL);

    constexpr std::bitset<4> Features{
        static_cast<uint32_t>(ResourceMonitoring::Feature::kCondition)
    };

    gAmebaActivatedCarbonFilterDelegate = new ActivatedCarbonFilterMonitoring::AmebaActivatedCarbonFilterMonitoringDelegate;

    VerifyOrReturnError(gAmebaActivatedCarbonFilterDelegate != nullptr, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

void ActivatedCarbonFilterMonitoring::AmebaActivatedCarbonFilterDelegateShutdown(void)
{
    if (gAmebaActivatedCarbonFilterDelegate != nullptr)
    {
        delete gAmebaActivatedCarbonFilterDelegate;
        gAmebaActivatedCarbonFilterDelegate = nullptr;
    }
}

AmebaHepaFilterMonitoringDelegate * HepaFilterMonitoring::GetAmebaHepaFilterDelegate(void)
{
    return gAmebaHepaFilterDelegate;
}

CHIP_ERROR HepaFilterMonitoring::AmebaHepaFilterDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaHepaFilterDelegate == nullptr, CHIP_ERROR_INTERNAL);

    constexpr std::bitset<4> Features{
        static_cast<uint32_t>(ResourceMonitoring::Feature::kCondition)
    };

    gAmebaHepaFilterDelegate = new HepaFilterMonitoring::AmebaHepaFilterMonitoringDelegate;

    VerifyOrReturnError(gAmebaHepaFilterDelegate != nullptr, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

void HepaFilterMonitoring::AmebaHepaFilterDelegateInit(void)
{
    if (gAmebaHepaFilterDelegate != nullptr)
    {
        delete gAmebaHepaFilterDelegate;
        gAmebaHepaFilterDelegate = nullptr;
    }
}
