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

#include <temperature_control/ameba_temperature_control_delegate.h>

using namespace chip;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::TemperatureControl;
using chip::Protocols::InteractionModel::Status;

static AmebaTemperatureControlDelegate * sAmebaTemperatureControlDelegate = nullptr;

CharSpan AmebaTemperatureControlDelegate::temperatureLevelOptions[] =
        { CharSpan("Hot", 3), CharSpan("Warm", 4), CharSpan("Freezing", 8) };

const AmebaTemperatureControlDelegate::EndpointPair AmebaTemperatureControlDelegate::supportedOptionsByEndpoints
    [MATTER_DM_TEMPERATURE_CONTROL_CLUSTER_SERVER_ENDPOINT_COUNT] = {
        EndpointPair(1, AmebaTemperatureControlDelegate::temperatureLevelOptions, 3) // Options for Endpoint 1
    };

uint8_t AmebaTemperatureControlDelegate::Size()
{
    for (auto & endpointPair : AmebaTemperatureControlDelegate::supportedOptionsByEndpoints)
    {
        if (endpointPair.mEndpointId == mEndpoint)
        {
            return endpointPair.mSize;
        }
    }
    return 0;
}

CHIP_ERROR AmebaTemperatureControlDelegate::Next(MutableCharSpan & item)
{
    for (auto & endpointPair : AmebaTemperatureControlDelegate::supportedOptionsByEndpoints)
    {
        if (endpointPair.mEndpointId == mEndpoint)
        {
            if (endpointPair.mSize > mIndex)
            {
                CHIP_ERROR err = CopyCharSpanToMutableCharSpan(endpointPair.mTemperatureLevels[mIndex], item);
                if (err != CHIP_NO_ERROR)
                {
                    ChipLogError(Zcl, "Error copying char span to mutable char span %s", ErrorStr(err));
                    return err;
                }
                mIndex++;
                return CHIP_NO_ERROR;
            }
        }
    }
    return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
}

AmebaTemperatureControlDelegate * TemperatureControl::GetAmebaTemperatureControlDelegate(void)
{
    return sAmebaTemperatureControlDelegate;
}

CHIP_ERROR TemperatureControl::AmebaTemperatureControlDelegateInit(chip::EndpointId endpoint)
{
    VerifyOrReturnError(sAmebaTemperatureControlDelegate == nullptr, CHIP_ERROR_INTERNAL);

    sAmebaTemperatureControlDelegate = new TemperatureControl::AmebaTemperatureControlDelegate;

    VerifyOrReturnError(sAmebaTemperatureControlDelegate != nullptr, CHIP_ERROR_INTERNAL);

    TemperatureControl::SetInstance(sAmebaTemperatureControlDelegate);

    return CHIP_NO_ERROR;
}

void TemperatureControl::AmebaTemperatureControlDelegateShutdown(void)
{
    if (sAmebaTemperatureControlDelegate) {
        delete sAmebaTemperatureControlDelegate;
        sAmebaTemperatureControlDelegate = nullptr;
    }
}

void emberAfTemperatureControlClusterInitCallback(chip::EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaTemperatureControlDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaTemperatureControlDelegateInit Failed");
        return;
    }
}

void emberAfTemperatureControlClusterShutdownCallback(chip::EndpointId endpoint)
{
    AmebaTemperatureControlDelegateShutdown();
}

