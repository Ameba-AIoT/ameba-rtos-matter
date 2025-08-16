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

#pragma once

#include <app-common/zap-generated/cluster-objects.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/clusters/switch-server/switch-server.h>

namespace chip {
namespace app {
namespace Clusters {
namespace GenericSwitch {
namespace Event {

class AmebaGenericSwitchEventHandler;

struct SwitchEventData
{
    chip::EndpointId endpointId = 1;
    chip::ClusterId clusterId;
    chip::AttributeId attributeId;
    chip::CommandId commandId;
    chip::EventId eventId;
    uint8_t newPosition;
    uint8_t previousPosition;
    uint8_t count;
};

class AmebaGenericSwitchEventHandler
{
public:
    AmebaGenericSwitchEventHandler() = default;

    static void HandleCommand(intptr_t context);

    void TriggerCommand(SwitchEventData data);

    void OnSwitchLatchedHandler(chip::EndpointId endpoint, uint8_t newPosition);
    void OnSwitchInitialPressedHandler(chip::EndpointId endpoint, uint8_t newPosition);
    void OnSwitchLongPressedHandler(chip::EndpointId endpoint, uint8_t newPosition);
    void OnSwitchShortReleasedHandler(chip::EndpointId endpoint, uint8_t previousPosition);
    void OnSwitchLongReleasedHandler(chip::EndpointId endpoint, uint8_t previousPosition);
    void OnSwitchMultiPressOngoingHandler(chip::EndpointId endpoint, uint8_t newPosition, uint8_t count);
    void OnSwitchMultiPressCompleteHandler(chip::EndpointId endpoint, uint8_t previousPosition, uint8_t count);

    // Nested classes for switch events
    class SwitchLatch
    {
    public:
        explicit SwitchLatch(AmebaGenericSwitchEventHandler* handler);
        void Set(chip::EndpointId ep, uint8_t value);

    private:
        AmebaGenericSwitchEventHandler* mHandler;
    };

    class SwitchInitialPress
    {
    public:
        explicit SwitchInitialPress(AmebaGenericSwitchEventHandler* handler);
        void Set(chip::EndpointId ep, uint8_t value);

    private:
        AmebaGenericSwitchEventHandler* mHandler;
    };

    class SwitchLongPress
    {
    public:
        explicit SwitchLongPress(AmebaGenericSwitchEventHandler* handler);
        void Set(chip::EndpointId ep, uint8_t value);

    private:
        AmebaGenericSwitchEventHandler* mHandler;
    };

    class SwitchShortRelease
    {
    public:
        explicit SwitchShortRelease(AmebaGenericSwitchEventHandler* handler);
        void Set(chip::EndpointId ep, uint8_t value);

    private:
        AmebaGenericSwitchEventHandler* mHandler;
    };

    class SwitchLongRelease
    {
    public:
        explicit SwitchLongRelease(AmebaGenericSwitchEventHandler* handler);
        void Set(chip::EndpointId ep, uint8_t value);

    private:
        AmebaGenericSwitchEventHandler* mHandler;
    };

    class SwitchMultiPressOngoing
    {
    public:
        explicit SwitchMultiPressOngoing(AmebaGenericSwitchEventHandler* handler);
        void Set(chip::EndpointId ep, uint8_t value);

    private:
        AmebaGenericSwitchEventHandler* mHandler;
    };

    class SwitchMultiPressComplete
    {
    public:
        explicit SwitchMultiPressComplete(AmebaGenericSwitchEventHandler* handler);
        void Set(chip::EndpointId ep, uint8_t value);

    private:
        AmebaGenericSwitchEventHandler* mHandler;
    };

private:
    AmebaGenericSwitchEventHandler* mEventHandler = nullptr;
};

} // namespace Event
} // namespace GenericSwitch
} // namespace Clusters
} // namespace app
} // namespace chip