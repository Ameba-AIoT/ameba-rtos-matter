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

#include <actions/ameba_actions_delegate.h>
#include <actions/ameba_actions_server.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Actions;

namespace {
std::unique_ptr<ActionsServer> sAmebaActionsServer = nullptr;
} // namespace

ActionsServer * Actions::GetAmebaActionsServer(void)
{
    return sAmebaActionsServer.get();
}

CHIP_ERROR Actions::AmebaActionsServerInit(EndpointId endpoint)
{
    VerifyOrReturnError(sAmebaActionsServer == nullptr, CHIP_ERROR_INTERNAL);

    auto * delegate = GetAmebaActionsDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    sAmebaActionsServer = std::make_unique<Actions::ActionsServer>(endpoint, *delegate);
    VerifyOrReturnError(sAmebaActionsServer != nullptr, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

void Actions::AmebaActionsServerShutdown(void)
{
    if (sAmebaActionsServer != nullptr)
    {
        sAmebaActionsServer->Shutdown();
        sAmebaActionsServer.reset();
    }
}

void emberAfActionsClusterInitCallback(chip::EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaActionsDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaActionsDelegateInit Failed");
        return;
    }

    ret = AmebaActionsServerInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaActionsServerInit Failed");
        return;
    }
}

void emberAfActionsClusterShutdownCallback(chip::EndpointId endpoint)
{
    AmebaActionsServerShutdown();
    AmebaActionsDelegateShutdown();
}
