/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/

#pragma once

#include <app/server/AppDelegate.h>
#include <app/server/Server.h>
#include <credentials/FabricTable.h>

using namespace ::chip;
using namespace ::chip::app;

#if defined(CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER) && (CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER)

class AmebaObserver : public AppDelegate, public FabricTable::Delegate
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

    void FabricWillBeRemoved(const FabricTable & fabricTable, chip::FabricIndex fabricIndex) override
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_FabricWillBeRemoved);
    }

    void OnFabricRemoved(const FabricTable & fabricTable, FabricIndex fabricIndex) override
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_FabricRemoved);
    }

    void OnFabricCommitted(const FabricTable & fabricTable, chip::FabricIndex fabricIndex) override
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_FabricCommitted);
    }

    void OnFabricUpdated(const FabricTable & fabricTable, chip::FabricIndex fabricIndex) override
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kEvent_FabricUpdated);
    }
};

#endif /* CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER */
