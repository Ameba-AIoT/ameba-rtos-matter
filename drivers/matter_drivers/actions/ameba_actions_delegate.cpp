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

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Actions;
using namespace chip::app::Clusters::Actions::Attributes;
using namespace chip::Protocols::InteractionModel;

namespace {
std::unique_ptr<AmebaActionsDelegateImpl> sAmebaActionsDelegateImpl = nullptr;
}

CHIP_ERROR AmebaActionsDelegateImpl::ReadActionAtIndex(uint16_t index, ActionStructStorage & action)
{
    if (index >= kActionList.size())
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    action = kActionList[index];
    return CHIP_NO_ERROR;
}

CHIP_ERROR AmebaActionsDelegateImpl::ReadEndpointListAtIndex(uint16_t index, EndpointListStorage & epList)
{
    if (index >= kEndpointList.size())
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    epList = kEndpointList[index];
    return CHIP_NO_ERROR;
}

bool AmebaActionsDelegateImpl::HaveActionWithId(uint16_t actionId, uint16_t & actionIndex)
{
    for (size_t i = 0; i < kEndpointList.size(); i++)
    {
        if (kActionList[i].actionID == actionId)
        {
            actionIndex = (uint16_t) i;
            return true;
        }
    }
    return false;
}

Status AmebaActionsDelegateImpl::HandleInstantAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

Status AmebaActionsDelegateImpl::HandleInstantActionWithTransition(uint16_t actionId, uint16_t transitionTime,
                                                              Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

Status AmebaActionsDelegateImpl::HandleStartAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

Status AmebaActionsDelegateImpl::HandleStartActionWithDuration(uint16_t actionId, uint32_t duration, Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

Status AmebaActionsDelegateImpl::HandleStopAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

Status AmebaActionsDelegateImpl::HandlePauseAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

Status AmebaActionsDelegateImpl::HandlePauseActionWithDuration(uint16_t actionId, uint32_t duration, Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

Status AmebaActionsDelegateImpl::HandleResumeAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

Status AmebaActionsDelegateImpl::HandleEnableAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

Status AmebaActionsDelegateImpl::HandleEnableActionWithDuration(uint16_t actionId, uint32_t duration, Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

Status AmebaActionsDelegateImpl::HandleDisableAction(uint16_t actionId, Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

Status AmebaActionsDelegateImpl::HandleDisableActionWithDuration(uint16_t actionId, uint32_t duration, Optional<uint32_t> invokeId)
{
    return Status::NotFound;
}

AmebaActionsDelegateImpl * Actions::GetAmebaActionsDelegate(void)
{
    return sAmebaActionsDelegateImpl.get();
}

CHIP_ERROR Actions::AmebaActionsDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(sAmebaActionsDelegateImpl == nullptr, CHIP_ERROR_INTERNAL);

    sAmebaActionsDelegateImpl = std::make_unique<Actions::AmebaActionsDelegateImpl>();

    VerifyOrReturnError(sAmebaActionsDelegateImpl != nullptr, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

void Actions::AmebaActionsDelegateShutdown(void)
{
    sAmebaActionsDelegateImpl.reset();
}
