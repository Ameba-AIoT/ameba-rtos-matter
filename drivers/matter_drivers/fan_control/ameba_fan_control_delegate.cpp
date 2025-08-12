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

#include <fan_control/ameba_fan_control_delegate.h>
#include <protocols/interaction_model/StatusCode.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::FanControl;
using namespace chip::app::Clusters::FanControl::Attributes;
using Protocols::InteractionModel::Status;

static AmebaFanControlDelegate * gAmebaFanControlDelegate = nullptr;

CHIP_ERROR AmebaFanControlDelegate::ReadPercentCurrent(AttributeValueEncoder & aEncoder)
{
    Percent ret = 0;
    uint8_t percentCurrent;

    Status status = PercentCurrent::Get(mEndpoint, &percentCurrent);
    if (status == Status::Success)
    {
        ret = percentCurrent;
    }

    return aEncoder.Encode(ret);
}

CHIP_ERROR AmebaFanControlDelegate::ReadSpeedCurrent(AttributeValueEncoder & aEncoder)
{
    Percent ret = 0;
    uint8_t speedCurrent;

    Status status = SpeedCurrent::Get(mEndpoint, &speedCurrent);
    if (status == Status::Success)
    {
        ret = speedCurrent;
    }

    return aEncoder.Encode(ret);
}

CHIP_ERROR AmebaFanControlDelegate::ReadPercentSetting(AttributeValueEncoder & aEncoder)
{
    Percent ret = 0;
    DataModel::Nullable<Percent> percentSetting;

    PercentSetting::Get(mEndpoint, percentSetting);
    if (!percentSetting.IsNull())
    {
        ret = percentSetting.Value();
    }

    return aEncoder.Encode(ret);
}

CHIP_ERROR AmebaFanControlDelegate::ReadSpeedSetting(AttributeValueEncoder & aEncoder)
{
    Percent ret = 0;
    DataModel::Nullable<uint8_t> speedSetting;

    SpeedSetting::Get(mEndpoint, speedSetting);
    if (!speedSetting.IsNull())
    {
        ret = speedSetting.Value();
    }

    return aEncoder.Encode(ret);
}

Status AmebaFanControlDelegate::HandleStep(StepDirectionEnum aDirection, bool aWrap, bool aLowestOff)
{
    ChipLogProgress(NotSpecified, "AmebaFanControlDelegate::HandleStep aDirection %d, aWrap %d, aLowestOff %d", to_underlying(aDirection),
                    aWrap, aLowestOff);

    VerifyOrReturnError(aDirection != StepDirectionEnum::kUnknownEnumValue, Status::InvalidCommand);

    uint8_t speedMax;
    SpeedMax::Get(mEndpoint, &speedMax);

    DataModel::Nullable<uint8_t> speedSetting;
    SpeedSetting::Get(mEndpoint, speedSetting);

    uint8_t newSpeedSetting = speedSetting.IsNull() ? 0 : speedSetting.Value();

    if (aDirection == StepDirectionEnum::kIncrease)
    {
        if (speedSetting.IsNull())
        {
            newSpeedSetting = 1;
        }
        else if (speedSetting.Value() < speedMax)
        {
            newSpeedSetting = static_cast<uint8_t>(speedSetting.Value() + 1);
        }
        else if (speedSetting.Value() == speedMax)
        {
            if (aWrap)
            {
                newSpeedSetting = aLowestOff ? 0 : 1;
            }
        }
    }
    else if (aDirection == StepDirectionEnum::kDecrease)
    {
        if (speedSetting.IsNull())
        {
            newSpeedSetting = aLowestOff ? 0 : 1;
        }
        else if ((speedSetting.Value() > 1) && (speedSetting.Value() <= speedMax))
        {
            newSpeedSetting = static_cast<uint8_t>(speedSetting.Value() - 1);
        }
        else if (speedSetting.Value() == 1)
        {
            if (aLowestOff)
            {
                newSpeedSetting = static_cast<uint8_t>(speedSetting.Value() - 1);
            }
            else if (aWrap)
            {
                newSpeedSetting = speedMax;
            }
        }
        else if (speedSetting.Value() == 0)
        {
            if (aWrap)
            {
                newSpeedSetting = speedMax;
            }
            else if (!aLowestOff)
            {
                newSpeedSetting = 1;
            }
        }
    }

    return SpeedSetting::Set(mEndpoint, newSpeedSetting);
}

CHIP_ERROR AmebaFanControlDelegate::Read(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder)
{
    VerifyOrDie(aPath.mClusterId == FanControl::Id);
    VerifyOrDie(aPath.mEndpointId == mEndpoint);

    switch (aPath.mAttributeId)
    {
    case SpeedCurrent::Id:
        return ReadSpeedCurrent(aEncoder);
    case PercentCurrent::Id:
        return ReadPercentCurrent(aEncoder);
    default:
        break;
    }
    return CHIP_NO_ERROR;
}

AmebaFanControlDelegate * FanControl::GetAmebaFanControlDelegate(void)
{
    return gAmebaFanControlDelegate;
}

CHIP_ERROR FanControl::AmebaFanControlDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaFanControlDelegate == nullptr, CHIP_ERROR_INTERNAL);

    gAmebaFanControlDelegate = new FanControl::AmebaFanControlDelegate(endpoint);

    VerifyOrReturnError(gAmebaFanControlDelegate != nullptr, CHIP_ERROR_INTERNAL);

    AttributeAccessInterfaceRegistry::Instance().Register(gAmebaFanControlDelegate);
    FanControl::SetDefaultDelegate(endpoint, gAmebaFanControlDelegate);

    return CHIP_NO_ERROR;
}

void FanControl::AmebaFanControlDelegateShutdown(void)
{
    if (gAmebaFanControlDelegate) {
        delete gAmebaFanControlDelegate;
        gAmebaFanControlDelegate = nullptr;
    }
}

void emberAfFanControlClusterInitCallback(chip::EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaFanControlDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaFanControlDelegateInit Failed");
        return;
    }
}

void emberAfFanControlClusterShutdownCallback(chip::EndpointId endpoint)
{
    AmebaFanControlDelegateShutdown();
}
