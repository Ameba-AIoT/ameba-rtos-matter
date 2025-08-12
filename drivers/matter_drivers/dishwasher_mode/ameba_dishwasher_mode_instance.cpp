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

#include <dishwasher_mode/ameba_dishwasher_mode_delegate.h>
#include <dishwasher_mode/ameba_dishwasher_mode_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::DishwasherMode;
using chip::Protocols::InteractionModel::Status;

static ModeBase::Instance * gAmebaDishwasherModeInstance = nullptr;

ModeBase::Instance * DishwasherMode::GetAmebaDishwasherModeInstance(void)
{
    return gAmebaDishwasherModeInstance;
}

CHIP_ERROR DishwasherMode::AmebaDishwasherModeInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaDishwasherModeInstance == nullptr, CHIP_ERROR_INTERNAL);

    auto * delegate = GetAmebaDishwasherModeDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaDishwasherModeInstance = new ModeBase::Instance(delegate, endpoint, DishwasherMode::Id, 0x0);
    VerifyOrReturnError(gAmebaDishwasherModeInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaDishwasherModeInstance->Init();

    return CHIP_NO_ERROR;
}

void DishwasherMode::AmebaDishwasherModeInstanceShutdown(void)
{
    if (gAmebaDishwasherModeInstance != nullptr)
    {
        gAmebaDishwasherModeInstance->Shutdown();
        delete gAmebaDishwasherModeInstance;
        gAmebaDishwasherModeInstance = nullptr;
    }
}

void emberAfDishwasherModeClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaDishwasherModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaDishwasherModeDelegateInit Failed");
        return;
    }

    ret = AmebaDishwasherModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaDishwasherModeInstanceInit Failed");
        return;
    }
}

void emberAfDishwasherModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaDishwasherModeInstanceShutdown();
    AmebaDishwasherModeDelegateShutdown();
}
