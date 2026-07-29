/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
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

#include <app/server/AppDelegate.h>
#include <app/server/Server.h>
#include <credentials/FabricTable.h>
#if defined(CHIP_CONFIG_ENABLE_ICD_SERVER) && (CHIP_CONFIG_ENABLE_ICD_SERVER == 1)
#include <app/icd/server/ICDStateObserver.h>
#endif
#if defined(CONFIG_ENABLE_AMEBA_TIME_SYNC) && (CONFIG_ENABLE_AMEBA_TIME_SYNC == 1)
#include <app/clusters/time-synchronization-server/CodegenIntegration.h>
#include <time_synchronization/ameba_time_sync_delegate.h>
#endif

using namespace ::chip;
using namespace ::chip::app;

#if defined(CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER) && (CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER == 1)

class AmebaObserver : public AppDelegate, public FabricTable::Delegate
#if defined(CHIP_CONFIG_ENABLE_ICD_SERVER) && (CHIP_CONFIG_ENABLE_ICD_SERVER == 1)
    , public ICDStateObserver
#endif
{
public:
    void OnCommissioningSessionEstablishmentStarted(void)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_CommissioningSessionEstablishmentStarted);
    }

    void OnCommissioningSessionStarted(void)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_CommissioningSessionStarted);
    }

    void OnCommissioningSessionEstablishmentError(CHIP_ERROR err) override
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_CommissioningSessionEstablishmentError);
    }

    void OnCommissioningSessionStopped(void)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_CommissioningSessionStopped);
    }

    void OnCommissioningWindowOpened(void)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_CommissioningWindowOpened);
    }

    void OnCommissioningWindowClosed(void)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_CommissioningWindowClosed);
    }

    void FabricWillBeRemoved(const FabricTable &fabricTable, chip::FabricIndex fabricIndex) override
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_FabricWillBeRemoved);
    }

    void OnFabricRemoved(const FabricTable &fabricTable, FabricIndex fabricIndex) override
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_FabricRemoved);
    }

    void OnFabricCommitted(const FabricTable &fabricTable, chip::FabricIndex fabricIndex) override
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_FabricCommitted);
    }

    void OnFabricUpdated(const FabricTable &fabricTable, chip::FabricIndex fabricIndex) override
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_FabricUpdated);
    }
#if defined(CHIP_CONFIG_ENABLE_ICD_SERVER) && (CHIP_CONFIG_ENABLE_ICD_SERVER == 1)
    void OnEnterActiveMode() override
    {
        // Request high-performance WiFi mode (disable power-save)
        matter_wifi_set_powersave_mode(0, 0);
        ChipLogProgress(DeviceLayer, "ICD: Entering Active Mode");
#if defined(CONFIG_ENABLE_AMEBA_TIME_SYNC) && (CONFIG_ENABLE_AMEBA_TIME_SYNC == 1)
        // Try to resync time every active session
        auto *delegate = chip::app::Clusters::TimeSynchronization::GetDefaultDelegate();
        if (delegate != nullptr) {
            static_cast<chip::app::Clusters::TimeSynchronization::AmebaTimeSyncDelegate *>(delegate)->TryPeriodicResync();
        }
#endif
    }
    void OnEnterIdleMode() override
    {
        // Allow WiFi to use DTIM-based sleep
        matter_wifi_set_powersave_mode(1, 1);
        ChipLogProgress(DeviceLayer, "ICD: Entering Idle Mode");
    }
    void OnTransitionToIdle() override
    {
        // Called before the transition to idle; can be used to flush pending operations
        ChipLogProgress(DeviceLayer, "ICD: Transitioning to Idle");
    }
    void OnICDModeChange() override
    {
        // Called when the ICD mode changes (SIT <-> LIT)
        ChipLogProgress(DeviceLayer, "ICD: Mode Changed");
    }
#endif
};

#endif /* CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER */
