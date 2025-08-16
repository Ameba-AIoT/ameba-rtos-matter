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

#include <laundry_washer_controls/ameba_laundry_washer_controls_delegate.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::LaundryWasherControls;

static AmebaLaundryWasherControlsDelegate * gAmebaLaundryWasherControlsDelegate = nullptr;

const CharSpan AmebaLaundryWasherControlsDelegate::spinSpeedsNameOptions[] = {
    CharSpan::fromCharString("Off"),
    CharSpan::fromCharString("Low"),
    CharSpan::fromCharString("Medium"),
    CharSpan::fromCharString("High"),
};

const NumberOfRinsesEnum AmebaLaundryWasherControlsDelegate::supportRinsesOptions[] = {
    NumberOfRinsesEnum::kNormal,
    NumberOfRinsesEnum::kExtra,
};

CHIP_ERROR AmebaLaundryWasherControlsDelegate::GetSpinSpeedAtIndex(size_t index, MutableCharSpan & spinSpeed)
{
    if (index >= MATTER_ARRAY_SIZE(spinSpeedsNameOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    return chip::CopyCharSpanToMutableCharSpan(AmebaLaundryWasherControlsDelegate::spinSpeedsNameOptions[index], spinSpeed);
}

CHIP_ERROR AmebaLaundryWasherControlsDelegate::GetSupportedRinseAtIndex(size_t index, NumberOfRinsesEnum & supportedRinse)
{
    if (index >= MATTER_ARRAY_SIZE(supportRinsesOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    supportedRinse = AmebaLaundryWasherControlsDelegate::supportRinsesOptions[index];
    return CHIP_NO_ERROR;
}

AmebaLaundryWasherControlsDelegate * LaundryWasherControls::GetAmebaLaundryWasherControlsDelegate(void)
{
    return gAmebaLaundryWasherControlsDelegate;
}

CHIP_ERROR LaundryWasherControls::AmebaLaundryWasherControlsDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaLaundryWasherControlsDelegate == nullptr, CHIP_ERROR_INTERNAL);

    gAmebaLaundryWasherControlsDelegate = new AmebaLaundryWasherControlsDelegate();

    VerifyOrReturnError(gAmebaLaundryWasherControlsDelegate != nullptr, CHIP_ERROR_INTERNAL);

    LaundryWasherControlsServer::SetDefaultDelegate(endpoint, gAmebaLaundryWasherControlsDelegate);

    return CHIP_NO_ERROR;
}

void LaundryWasherControls::AmebaLaundryWasherControlsDelegateShutdown(void)
{
    if (gAmebaLaundryWasherControlsDelegate)
    {
        delete gAmebaLaundryWasherControlsDelegate;
        gAmebaLaundryWasherControlsDelegate = nullptr;
    }
}

void emberAfLaundryWasherControlsClusterInitCallback(chip::EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaLaundryWasherControlsDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaLaundryWasherControlsDelegateInit Failed");
        return;
    }
}

void emberAfLaundryWasherControlsClusterShutdownCallback(chip::EndpointId endpoint)
{
    AmebaLaundryWasherControlsDelegateShutdown();
}
