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

#include <switch/ameba_switch_event.h>

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/clusters/switch-server/switch-server.h>
#include <app/util/attribute-storage.h>
#include <platform/PlatformManager.h>

#include <utility> // For std::pair
#include <memory>  // For std::unique_ptr

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Switch;
using namespace chip::DeviceLayer;

using namespace chip::app::Clusters::GenericSwitch::Event;
using chip::app::Clusters::GenericSwitch::Event::SwitchEventData;

void AmebaGenericSwitchEventHandler::HandleCommand(intptr_t context)
{
    auto* pair = reinterpret_cast<std::pair<AmebaGenericSwitchEventHandler*, SwitchEventData*>*>(context);

    AmebaGenericSwitchEventHandler* handler = pair->first;
    SwitchEventData* data = pair->second;

    switch (data->eventId)
    {
        case Events::SwitchLatched::Id:
            handler->OnSwitchLatchedHandler(data->endpointId, data->newPosition);
            break;
        case Events::InitialPress::Id:
            handler->OnSwitchInitialPressedHandler(data->endpointId, data->newPosition);
            break;
        case Events::LongPress::Id:
            handler->OnSwitchLongPressedHandler(data->endpointId, data->newPosition);
            break;
        case Events::ShortRelease::Id:
            handler->OnSwitchShortReleasedHandler(data->endpointId, data->previousPosition);
            break;
        case Events::LongRelease::Id:
            handler->OnSwitchLongReleasedHandler(data->endpointId, data->previousPosition);
            break;
        case Events::MultiPressOngoing::Id:
            handler->OnSwitchMultiPressOngoingHandler(data->endpointId, data->newPosition, data->count);
            break;
        case Events::MultiPressComplete::Id:
            handler->OnSwitchMultiPressCompleteHandler(data->endpointId, data->previousPosition, data->count);
            break;
        default:
            ChipLogError(NotSpecified, "Unhandled event ID: %d", data->eventId);
            break;
    }

    Platform::Delete(data);
    delete pair;
}

void AmebaGenericSwitchEventHandler::OnSwitchLatchedHandler(EndpointId endpoint, uint8_t newPosition)
{
    Protocols::InteractionModel::Status status = Switch::Attributes::CurrentPosition::Set(endpoint, newPosition);
    VerifyOrReturn(Protocols::InteractionModel::Status::Success == status,
                   ChipLogError(NotSpecified, "Failed to set CurrentPosition attribute"));
    ChipLogDetail(NotSpecified, "The latching switch is moved to a new position:%d", newPosition);

    chip::app::Clusters::SwitchServer::Instance().OnSwitchLatch(endpoint, newPosition);
}

void AmebaGenericSwitchEventHandler::OnSwitchInitialPressedHandler(EndpointId endpoint, uint8_t newPosition)
{
    Protocols::InteractionModel::Status status = Switch::Attributes::CurrentPosition::Set(endpoint, newPosition);
    VerifyOrReturn(Protocols::InteractionModel::Status::Success == status,
                   ChipLogError(NotSpecified, "Failed to set CurrentPosition attribute"));
    ChipLogDetail(NotSpecified, "The new position when the momentary switch starts to be pressed:%d", newPosition);

    chip::app::Clusters::SwitchServer::Instance().OnInitialPress(endpoint, newPosition);
}

void AmebaGenericSwitchEventHandler::OnSwitchLongPressedHandler(EndpointId endpoint, uint8_t newPosition)
{
    Protocols::InteractionModel::Status status = Switch::Attributes::CurrentPosition::Set(endpoint, newPosition);
    VerifyOrReturn(Protocols::InteractionModel::Status::Success == status,
                   ChipLogError(NotSpecified, "Failed to set CurrentPosition attribute"));
    ChipLogDetail(NotSpecified, "The new position when the momentary switch has been pressed for a long time:%d", newPosition);

    chip::app::Clusters::SwitchServer::Instance().OnLongPress(endpoint, newPosition);
}

void AmebaGenericSwitchEventHandler::OnSwitchShortReleasedHandler(EndpointId endpoint, uint8_t previousPosition)
{
    Protocols::InteractionModel::Status status = Switch::Attributes::CurrentPosition::Set(endpoint, 0);
    VerifyOrReturn(Protocols::InteractionModel::Status::Success == status,
                   ChipLogError(NotSpecified, "Failed to reset CurrentPosition attribute"));

    chip::app::Clusters::SwitchServer::Instance().OnShortRelease(endpoint, previousPosition);
}

void AmebaGenericSwitchEventHandler::OnSwitchLongReleasedHandler(EndpointId endpoint, uint8_t previousPosition)
{
    Protocols::InteractionModel::Status status = Switch::Attributes::CurrentPosition::Set(endpoint, 0);
    VerifyOrReturn(Protocols::InteractionModel::Status::Success == status,
                   ChipLogError(NotSpecified, "Failed to reset CurrentPosition attribute"));

    chip::app::Clusters::SwitchServer::Instance().OnLongRelease(endpoint, previousPosition);
}

void AmebaGenericSwitchEventHandler::OnSwitchMultiPressOngoingHandler(EndpointId endpoint, uint8_t newPosition, uint8_t count)
{
    Protocols::InteractionModel::Status status = Switch::Attributes::CurrentPosition::Set(endpoint, newPosition);
    VerifyOrReturn(Protocols::InteractionModel::Status::Success == status,
                   ChipLogError(NotSpecified, "Failed to set CurrentPosition attribute"));

    chip::app::Clusters::SwitchServer::Instance().OnMultiPressOngoing(endpoint, newPosition, count);
}

void AmebaGenericSwitchEventHandler::OnSwitchMultiPressCompleteHandler(EndpointId endpoint, uint8_t previousPosition, uint8_t count)
{
    Protocols::InteractionModel::Status status = Switch::Attributes::CurrentPosition::Set(endpoint, 0);
    VerifyOrReturn(Protocols::InteractionModel::Status::Success == status,
                   ChipLogError(NotSpecified, "Failed to reset CurrentPosition attribute"));

    chip::app::Clusters::SwitchServer::Instance().OnMultiPressComplete(endpoint, previousPosition, count);
}

void AmebaGenericSwitchEventHandler::TriggerCommand(SwitchEventData data)
{
    auto temp = std::make_unique<SwitchEventData>(data);
    auto pair = std::make_unique<std::pair<AmebaGenericSwitchEventHandler*, SwitchEventData*>>(this, temp.release());

    chip::DeviceLayer::PlatformMgr().ScheduleWork(AmebaGenericSwitchEventHandler::HandleCommand, reinterpret_cast<intptr_t>(pair.release()));
}

// SwitchLatch class methods

AmebaGenericSwitchEventHandler::SwitchLatch::SwitchLatch(AmebaGenericSwitchEventHandler* handler)
    : mHandler(handler) {}

void AmebaGenericSwitchEventHandler::SwitchLatch::Set(EndpointId ep, uint8_t value)
{
    ChipLogProgress(DeviceLayer, "SwitchLatch Set endpoint%d, value=%x", ep, value);
    SwitchEventData data;
    data.endpointId = ep;
    data.clusterId = chip::app::Clusters::Switch::Id;
    data.eventId = Events::SwitchLatched::Id;
    data.newPosition = value;

    mHandler->TriggerCommand(data);
}

// SwitchInitialPress class methods

AmebaGenericSwitchEventHandler::SwitchInitialPress::SwitchInitialPress(AmebaGenericSwitchEventHandler* handler)
    : mHandler(handler) {}

void AmebaGenericSwitchEventHandler::SwitchInitialPress::Set(EndpointId ep, uint8_t value)
{
    ChipLogProgress(DeviceLayer, "SwitchInitialPress Set endpoint%d, value=%x", ep, value);
    SwitchEventData data;
    data.endpointId = ep;
    data.clusterId = chip::app::Clusters::Switch::Id;
    data.eventId = Events::InitialPress::Id;
    data.newPosition = value;

    mHandler->TriggerCommand(data);
}

// SwitchLongPress class methods

AmebaGenericSwitchEventHandler::SwitchLongPress::SwitchLongPress(AmebaGenericSwitchEventHandler* handler)
    : mHandler(handler) {}

void AmebaGenericSwitchEventHandler::SwitchLongPress::Set(EndpointId ep, uint8_t value)
{
    ChipLogProgress(DeviceLayer, "SwitchLongPress Set endpoint%d, value=%x", ep, value);
    SwitchEventData data;
    data.endpointId = ep;
    data.clusterId = chip::app::Clusters::Switch::Id;
    data.eventId = Events::LongPress::Id;
    data.newPosition = value;

    mHandler->TriggerCommand(data);
}

// SwitchShortRelease class methods

AmebaGenericSwitchEventHandler::SwitchShortRelease::SwitchShortRelease(AmebaGenericSwitchEventHandler* handler)
    : mHandler(handler) {}

void AmebaGenericSwitchEventHandler::SwitchShortRelease::Set(EndpointId ep, uint8_t value)
{
    ChipLogProgress(DeviceLayer, "SwitchShortRelease Set endpoint%d, value=%x", ep, value);
    SwitchEventData data;
    data.endpointId = ep;
    data.clusterId = chip::app::Clusters::Switch::Id;
    data.eventId = Events::ShortRelease::Id;
    data.previousPosition = value;

    mHandler->TriggerCommand(data);
}

// SwitchLongRelease class methods

AmebaGenericSwitchEventHandler::SwitchLongRelease::SwitchLongRelease(AmebaGenericSwitchEventHandler* handler)
    : mHandler(handler) {}

void AmebaGenericSwitchEventHandler::SwitchLongRelease::Set(EndpointId ep, uint8_t value)
{
    ChipLogProgress(DeviceLayer, "SwitchLongRelease Set endpoint%d, value=%x", ep, value);
    SwitchEventData data;
    data.endpointId = ep;
    data.clusterId = chip::app::Clusters::Switch::Id;
    data.eventId = Events::LongRelease::Id;
    data.previousPosition = value;

    mHandler->TriggerCommand(data);
}

// SwitchMultiPressOngoing class methods

AmebaGenericSwitchEventHandler::SwitchMultiPressOngoing::SwitchMultiPressOngoing(AmebaGenericSwitchEventHandler* handler)
    : mHandler(handler) {}

void AmebaGenericSwitchEventHandler::SwitchMultiPressOngoing::Set(EndpointId ep, uint8_t value)
{
    ChipLogProgress(DeviceLayer, "SwitchMultiPressOngoing Set endpoint%d, value=%x", ep, value);
    SwitchEventData data;
    data.endpointId = ep;
    data.clusterId = chip::app::Clusters::Switch::Id;
    data.eventId = Events::MultiPressOngoing::Id;
    data.newPosition = value;

    mHandler->TriggerCommand(data);
}

// SwitchMultiPressComplete class methods

AmebaGenericSwitchEventHandler::SwitchMultiPressComplete::SwitchMultiPressComplete(AmebaGenericSwitchEventHandler* handler)
    : mHandler(handler) {}

void AmebaGenericSwitchEventHandler::SwitchMultiPressComplete::Set(EndpointId ep, uint8_t value)
{
    ChipLogProgress(DeviceLayer, "SwitchMultiPressComplete Set endpoint%d, value=%x", ep, value);
    SwitchEventData data;
    data.endpointId = ep;
    data.clusterId = chip::app::Clusters::Switch::Id;
    data.eventId = Events::MultiPressComplete::Id;
    data.newPosition = value;

    mHandler->TriggerCommand(data);
}