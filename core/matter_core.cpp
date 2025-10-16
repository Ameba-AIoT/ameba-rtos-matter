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

#include <stdlib.h>
#include <stdint.h>

#include <matter_core.h>
#include <matter_events.h>
#include <matter_interaction.h>
#include <matter_ota_initializer.h>
#if defined(CONFIG_ENABLE_AMEBA_DLOG) && (CONFIG_ENABLE_AMEBA_DLOG == 1)
#include <matter_fs.h>
#include <diagnostic_logs/ameba_logging_faultlog.h>
#include <diagnostic_logs/ameba_logging_redirect_handler.h>
#endif
#if defined(CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER) && (CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER == 1)
#include <matter_fabric_observer.h>
#endif
#if defined(CONFIG_ENABLE_AMEBA_OPHOURS) && (CONFIG_ENABLE_AMEBA_OPHOURS == 1)
#include <matter_device_utils.h>
#endif
#if defined(CONFIG_ENABLE_AMEBA_MDNS_FILTER) && (CONFIG_ENABLE_AMEBA_MDNS_FILTER == 1)
#include <matter_mdns_filter.h>
#endif
#if defined(CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION) && (CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION == 1)
#include <app/server/TermsAndConditionsManager.h>
#endif
#if CONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER
#include <test_event_trigger/AmebaTestEventTriggerDelegate.h>
#endif

#include <DeviceInfoProviderImpl.h>

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Commands.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/DeferredAttributePersistenceProvider.h>
#include <app/util/af-types.h>
#include <app/util/attribute-storage.h>
#include <app/util/basic-types.h>
#include <app/util/util.h>
#include <app/clusters/identify-server/identify-server.h>
#include <app/clusters/network-commissioning/network-commissioning.h>
#include <app/server/Dnssd.h>
#include <app/server/OnboardingCodesUtil.h>
#include <app/server/Server.h>

#include <credentials/DeviceAttestationCredsProvider.h>
#include <credentials/examples/DeviceAttestationCredsExample.h>

#include <lib/dnssd/Advertiser.h>
#include <platform/Ameba/AmebaUtils.h>

#include <platform/Ameba/AmebaConfig.h>
#include <platform/Ameba/FactoryDataProvider.h>
#include <platform/Ameba/NetworkCommissioningDriver.h>

#include <route_hook/ameba_route_hook.h>

#include <support/CHIPMem.h>
#include <support/CodeUtils.h>
#include <core/ErrorStr.h>

#if CONFIG_ENABLE_CHIP_SHELL
#include <shell/launch_shell.h>
#endif

#if CONFIG_ENABLE_AMEBA_CRYPTO
#include <platform/Ameba/crypto/AmebaPersistentStorageOperationalKeystore.h>
#endif

using namespace ::chip;
using namespace ::chip::app;
using namespace ::chip::DeviceLayer;

// Define a custom attribute persister which makes actual write of the selected attribute values
// to the non-volatile storage only when it has remained constant for 5 seconds. This is to reduce
// the flash wearout when the attribute changes frequently as a result of MoveToLevel command.
// DeferredAttribute object describes a deferred attribute, but also holds a buffer with a value to
// be written, so it must live so long as the DeferredAttributePersistenceProvider object.

DeferredAttribute gDeferredAttributeArray[] = {
    DeferredAttribute(ConcreteAttributePath(1 /* kLightEndpointId */, Clusters::LevelControl::Id, Clusters::LevelControl::Attributes::CurrentLevel::Id)),
    DeferredAttribute(ConcreteAttributePath(1 /* kLightEndpointId */, Clusters::ColorControl::Id, Clusters::ColorControl::Attributes::CurrentHue::Id)),
    DeferredAttribute(ConcreteAttributePath(1 /* kLightEndpointId */, Clusters::ColorControl::Id, Clusters::ColorControl::Attributes::CurrentSaturation::Id)),
    DeferredAttribute(ConcreteAttributePath(1 /* kLightEndpointId */, Clusters::ColorControl::Id, Clusters::ColorControl::Attributes::EnhancedCurrentHue::Id)),
    DeferredAttribute(ConcreteAttributePath(1 /* kLightEndpointId */, Clusters::ColorControl::Id, Clusters::ColorControl::Attributes::CurrentX::Id)),
    DeferredAttribute(ConcreteAttributePath(1 /* kLightEndpointId */, Clusters::ColorControl::Id, Clusters::ColorControl::Attributes::CurrentY::Id)),
};
DeferredAttributePersistenceProvider gDeferredAttributePersister(Server::GetInstance().GetDefaultAttributePersister(),
        Span<DeferredAttribute>(gDeferredAttributeArray, 6), System::Clock::Milliseconds32(5000));

app::Clusters::NetworkCommissioning::Instance
sWiFiNetworkCommissioningInstance(0 /* Endpoint Id */, &(NetworkCommissioning::AmebaWiFiDriver::GetInstance()));

chip::DeviceLayer::DeviceInfoProviderImpl gExampleDeviceInfoProvider;
chip::DeviceLayer::FactoryDataProvider mFactoryDataProvider;

#if defined(CONFIG_ENABLE_AMEBA_MDNS_FILTER) && (CONFIG_ENABLE_AMEBA_MDNS_FILTER == 1)
constexpr size_t kMaxPendingMdnsPackets = 10u;
chip::Inet::DropIfTooManyQueuedPacketsFilter sMdnsPacketFilter(kMaxPendingMdnsPackets);
#endif

#if CONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER
uint8_t sTestEventTriggerEnableKey[TestEventTriggerDelegate::kEnableKeyLength] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                                                                   0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
#endif

void matter_core_device_callback_internal(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case DeviceEventType::kInternetConnectivityChange:
#if CHIP_DEVICE_CONFIG_ENABLE_OTA_REQUESTOR
        static bool isOTAInitialized = false; // use this static variable to replace CheckInit()
#endif
        if (event->InternetConnectivityChange.IPv4 == kConnectivity_Established) {
            ChipLogProgress(DeviceLayer, "IPv4 Server ready...");
            chip::app::DnssdServer::Instance().StartServer();
        } else if (event->InternetConnectivityChange.IPv4 == kConnectivity_Lost) {
            ChipLogProgress(DeviceLayer, "Lost IPv4 connectivity...");
        }
        if (event->InternetConnectivityChange.IPv6 == kConnectivity_Established) {
            ChipLogProgress(DeviceLayer, "IPv6 Server ready...");
            chip::app::DnssdServer::Instance().StartServer();

#if CHIP_DEVICE_CONFIG_ENABLE_OTA_REQUESTOR
            // Init OTA requestor only when we have gotten IPv6 address
            if (!isOTAInitialized) {
                matter_ota_initializer();
                isOTAInitialized = true;
            }
#endif
        } else if (event->InternetConnectivityChange.IPv6 == kConnectivity_Lost) {
            ChipLogProgress(DeviceLayer, "Lost IPv6 connectivity...");
        }
        break;
    case DeviceEventType::kInterfaceIpAddressChanged:
        if ((event->InterfaceIpAddressChanged.Type == InterfaceIpChangeType::kIpV4_Assigned) ||
            (event->InterfaceIpAddressChanged.Type == InterfaceIpChangeType::kIpV6_Assigned)) {
            // MDNS server restart on any ip assignment: if link local ipv6 is configured, that
            // will not trigger a 'internet connectivity change' as there is no internet
            // connectivity. MDNS still wants to refresh its listening interfaces to include the
            // newly selected address.
            chip::app::DnssdServer::Instance().StartServer();
        }
        if (event->InterfaceIpAddressChanged.Type == InterfaceIpChangeType::kIpV6_Assigned) {
            ChipLogProgress(DeviceLayer, "Initializing route hook...");
            ameba_route_hook_init();
        }
        break;
    case DeviceEventType::kCommissioningComplete:
        ChipLogProgress(DeviceLayer, "Commissioning Complete");
        chip::DeviceLayer::Internal::AmebaUtils::SetCurrentProvisionedNetwork();
        break;
#if defined(CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER) && (CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER == 1)
    case DeviceEventType::kEvent_CommissioningSessionEstablishmentStarted:
        ChipLogProgress(DeviceLayer, "Commissioning Session has been established");
        break;
    case DeviceEventType::kEvent_CommissioningSessionStarted:
        ChipLogProgress(DeviceLayer, "Commissioning Session started");
        break;
    case DeviceEventType::kEvent_CommissioningSessionEstablishmentError:
        ChipLogProgress(DeviceLayer, "Commissioning Session established error");
        break;
    case DeviceEventType::kEvent_CommissioningSessionStopped:
        ChipLogProgress(DeviceLayer, "Commissioning Session stopped");
        break;
    case DeviceEventType::kEvent_CommissioningWindowOpened:
        ChipLogProgress(DeviceLayer, "Commissioning Window is opened");
        break;
    case DeviceEventType::kEvent_CommissioningWindowClosed:
        ChipLogProgress(DeviceLayer, "Commissioning Window is closed");
        break;
    case DeviceEventType::kEvent_FabricWillBeRemoved:
        ChipLogProgress(DeviceLayer, "Fabric removing");
        break;
    case DeviceEventType::kEvent_FabricRemoved:
        ChipLogProgress(DeviceLayer, "Fabric removed successfully");
        break;
    case DeviceEventType::kEvent_FabricCommitted:
        ChipLogProgress(DeviceLayer, "Fabric info committed");
        break;
    case DeviceEventType::kEvent_FabricUpdated:
        ChipLogProgress(DeviceLayer, "Fabric info updated");
        break;
#endif /* CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER */
    }
}

void matter_core_init_server(intptr_t context)
{
    xTaskHandle task_to_notify = reinterpret_cast<xTaskHandle>(context);
    // Init ZCL Data Model and CHIP App Server
    static chip::CommonCaseDeviceServerInitParams initParams;
    initParams.InitializeStaticResourcesBeforeServerInit();

#if CONFIG_ENABLE_AMEBA_CRYPTO
    ChipLogProgress(DeviceLayer, "platform crypto enabled!");
    static chip::AmebaPersistentStorageOperationalKeystore sAmebaPersistentStorageOpKeystore;
    VerifyOrDie((sAmebaPersistentStorageOpKeystore.Init(initParams.persistentStorageDelegate)) == CHIP_NO_ERROR);
    initParams.operationalKeystore = &sAmebaPersistentStorageOpKeystore;
#endif

#if defined(CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER) && (CONFIG_ENABLE_AMEBA_FABRIC_OBSERVER == 1)
    static AmebaObserver sAmebaObserver;
    initParams.appDelegate = &sAmebaObserver;
#endif

#if CONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER
    static AmebaTestEventTriggerDelegate sTestEventTriggerDelegate{ ByteSpan(sTestEventTriggerEnableKey) };
    initParams.testEventTriggerDelegate = &sTestEventTriggerDelegate;
#endif

    chip::Server::GetInstance().Init(initParams);

#if defined(CONFIG_ENABLE_AMEBA_MDNS_FILTER) && (CONFIG_ENABLE_AMEBA_MDNS_FILTER == 1)
    chip::Inet::UDPEndPointImplLwIP::SetQueueFilter(&sMdnsPacketFilter);
#endif

    gExampleDeviceInfoProvider.SetStorageDelegate(&Server::GetInstance().GetPersistentStorage());
    // TODO: Use our own DeviceInfoProvider
    chip::DeviceLayer::SetDeviceInfoProvider(&gExampleDeviceInfoProvider);
    SetAttributePersistenceProvider(&gDeferredAttributePersister);

#if defined(CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION) && (CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION == 1)
    const Optional<app::TermsAndConditions> termsAndConditions = Optional<app::TermsAndConditions>(
        app::TermsAndConditions(CHIP_AMEBA_TC_REQUIRED_ACKNOWLEDGEMENTS, CHIP_AMEBA_TC_MIN_REQUIRED_VERSION));
    PersistentStorageDelegate & persistentStorageDelegate = Server::GetInstance().GetPersistentStorage();
    chip::app::TermsAndConditionsManager::GetInstance()->Init(&persistentStorageDelegate, termsAndConditions);
#endif

    sWiFiNetworkCommissioningInstance.Init();

    // We only have network commissioning on endpoint 0.
    // TODO: configure the endpoint
    emberAfEndpointEnableDisable(0xFFFE, false);

    if (RTW_SUCCESS != matter_wifi_is_connected_to_ap()) {
        // QR code will be used with CHIP Tool
        PrintOnboardingCodes(chip::RendezvousInformationFlags(chip::RendezvousInformationFlag::kBLE));
    }

#if CONFIG_ENABLE_CHIP_SHELL
    InitBindingHandler();
#endif

    xTaskNotifyGive(task_to_notify);
}

CHIP_ERROR matter_core_init(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
#if defined(CONFIG_ENABLE_AMEBA_DLOG) && (CONFIG_ENABLE_AMEBA_DLOG == 1)
    err = ChipError(0, NULL, 0);
    auto &instance = AmebaLogRedirectHandler::GetInstance();
#endif
    err = Platform::MemoryInit();
    SuccessOrExit(err);

    // Initialize the CHIP stack.
    err = PlatformMgr().InitChipStack();
    SuccessOrExit(err);

#if defined(CONFIG_ENABLE_AMEBA_DLOG) && (CONFIG_ENABLE_AMEBA_DLOG == 1)
    if (instance.GetAmebaLogSubsystemInited()) {
        instance.RegisterAmebaErrorFormatter(); // only register the custom error formatter if the log subsystem was inited.
    }
#endif

    err = mFactoryDataProvider.Init();
    if (err != CHIP_NO_ERROR) {
        ChipLogError(DeviceLayer, "Error initializing FactoryData!");
        ChipLogError(DeviceLayer, "Check if you have flashed it correctly!");
    }

    SetCommissionableDataProvider(&mFactoryDataProvider);
    SetDeviceAttestationCredentialsProvider(&mFactoryDataProvider);
    SetDeviceInstanceInfoProvider(&mFactoryDataProvider);

    if (CONFIG_NETWORK_LAYER_BLE) {
        ConnectivityMgr().SetBLEAdvertisingEnabled(true);
    }

#if defined(CONFIG_ENABLE_AMEBA_OPHOURS) && (CONFIG_ENABLE_AMEBA_OPHOURS == 1)
    matter_op_hours();
#endif

    // Start a task to run the CHIP Device event loop.
    err = PlatformMgr().StartEventLoopTask();
    SuccessOrExit(err);

    // Register a function to receive events from the CHIP device layer.  Note that calls to
    // this function will happen on the CHIP event loop thread, not the app_main thread.
    PlatformMgr().AddEventHandler(matter_core_device_callback_internal, reinterpret_cast<intptr_t>(NULL));

    // PlatformMgr().ScheduleWork(matter_core_init_server, 0);
    PlatformMgr().ScheduleWork(matter_core_init_server, reinterpret_cast<intptr_t>(xTaskGetCurrentTaskHandle()));
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

exit:
    return err;
}

CHIP_ERROR matter_core_start(void)
{
#if defined(CONFIG_ENABLE_AMEBA_DLOG) && (CONFIG_ENABLE_AMEBA_DLOG == 1)
    fault_handler_override(matter_fault_log, matter_bt_log);
    int res = matter_fs_init();

    /* init flash fs and read existing fault log into fs */
    if (res == 0) {
        ChipLogProgress(DeviceLayer, "Matter FlashFS Initialized");
    }

    // register log redirection
    auto &instance = AmebaLogRedirectHandler::GetInstance();
    instance.InitAmebaLogSubsystem();
#endif

    matter_set_autoreconnect(0);

    return matter_core_init();
}
