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

#include <laundry_dryer_controls/ameba_laundry_dryer_controls_delegate.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::LaundryDryerControls;

static AmebaLaundryDryerControlsDelegate * gAmebaLaundryDryerControlsDelegate = nullptr;

const DrynessLevelEnum AmebaLaundryDryerControlsDelegate::supportedDrynessLevelOptions[] = {
    DrynessLevelEnum::kLow,
    DrynessLevelEnum::kNormal,
    DrynessLevelEnum::kMax
};

CHIP_ERROR AmebaLaundryDryerControlsDelegate::GetSupportedDrynessLevelAtIndex(size_t index, DrynessLevelEnum & supportedDrynessLevel)
{
    if (index >= MATTER_ARRAY_SIZE(supportedDrynessLevelOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    supportedDrynessLevel = supportedDrynessLevelOptions[index];
    return CHIP_NO_ERROR;
}

AmebaLaundryDryerControlsDelegate * LaundryDryerControls::GetAmebaLaundryDryerControlsDelegate(void)
{
    return gAmebaLaundryDryerControlsDelegate;
}

CHIP_ERROR LaundryDryerControls::AmebaLaundryDryerControlsDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaLaundryDryerControlsDelegate == nullptr, CHIP_ERROR_INTERNAL);

    gAmebaLaundryDryerControlsDelegate = new AmebaLaundryDryerControlsDelegate();

    VerifyOrReturnError(gAmebaLaundryDryerControlsDelegate != nullptr, CHIP_ERROR_INTERNAL);

    LaundryDryerControlsServer::SetDefaultDelegate(endpoint, gAmebaLaundryDryerControlsDelegate);

    return CHIP_NO_ERROR;
}

void LaundryDryerControls::AmebaLaundryDryerControlsDelegateShutdown(void)
{
    if (gAmebaLaundryDryerControlsDelegate) {
        delete gAmebaLaundryDryerControlsDelegate;
        gAmebaLaundryDryerControlsDelegate = nullptr;
    }
}

void emberAfLaundryDryerControlsClusterInitCallback(chip::EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaLaundryDryerControlsDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaLaundryDryerControlsDelegateInit Failed");
        return;
    }
}

void emberAfLaundryDryerControlsClusterShutdownCallback(chip::EndpointId endpoint)
{
    AmebaLaundryDryerControlsDelegateShutdown();
}
