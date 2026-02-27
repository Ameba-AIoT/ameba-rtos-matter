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

#include <cstddef>
#include <cstdint>

#include <platform_stdlib.h>
#include <app/server/Server.h>
#include <app/server/OnboardingCodesUtil.h>
#include <credentials/CertificationDeclaration.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <lib/core/CHIPError.h>
#include <lib/support/Span.h>
#include <platform/Ameba/DiagnosticDataProviderImpl.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/CommissionableDataProvider.h>
#include <platform/DeviceInstanceInfoProvider.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>

#include <matter_api.h>
#include <matter_dcts.h>
#include <sys_api.h>
#include <wifi_conf.h>

using namespace chip;
using namespace ::chip::Credentials;
using namespace ::chip::DeviceLayer;

void matter_factory_reset(void)
{
    deinitPref();
    wifi_disconnect();
    sys_reset();
}

bool matter_server_is_commissioned(void)
{
    return (chip::Server::GetInstance().GetFabricTable().FabricCount() != 0);
}

void matter_get_fabric_indexes(uint16_t *pFabricIndexes, size_t bufSize)
{
    size_t i = 0;
    for (auto it = chip::Server::GetInstance().GetFabricTable().begin();
         it != chip::Server::GetInstance().GetFabricTable().end(); ++it) {
        if (bufSize < i) {
            // out of buffer space
            ChipLogError(DeviceLayer, "Returning... buffer too small");
            return;
        }
        ChipLogProgress(DeviceLayer, "Fabric Index = %d", it->GetFabricIndex());
        pFabricIndexes[i] = it->GetFabricIndex();
        i++;
    }
}

void matter_print_onboarding_codes(void)
{
#if CONFIG_NETWORK_LAYER_BLE
    auto flags = chip::RendezvousInformationFlags(chip::RendezvousInformationFlag::kBLE);
#else
    auto flags = chip::RendezvousInformationFlags(chip::RendezvousInformationFlag::kOnNetwork);
#endif /* CONFIG_NETWORK_LAYER_BLE */

    chip::PayloadContents payload;
    CHIP_ERROR err = GetPayloadContents(payload, flags);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "GetPayloadContents() failed: %" CHIP_ERROR_FORMAT, err.Format());
    }
#if CONFIG_USER_ACTION_REQUIRED
    payload.commissioningFlow = chip::CommissioningFlow::kUserActionRequired;
#endif
    PrintOnboardingCodes(payload);
}

uint8_t matter_get_manual_pairing_code(char *buf, size_t bufSize)
{
    if (bufSize < chip::QRCodeBasicSetupPayloadGenerator::kMaxQRCodeBase38RepresentationLength + 1) {
        ChipLogError(DeviceLayer, "Buffer too small for onboarding code!");
        return -1;
    }

    char payloadBuffer[chip::QRCodeBasicSetupPayloadGenerator::kMaxQRCodeBase38RepresentationLength + 1];
    chip::MutableCharSpan manualPairingCode(payloadBuffer);
    RendezvousInformationFlags rendezvousFlag = chip::RendezvousInformationFlags(chip::RendezvousInformationFlag::kBLE);
    PayloadContents payload;

    if (GetPayloadContents(payload, rendezvousFlag) != CHIP_NO_ERROR) {
        ChipLogError(DeviceLayer, "Failed to get onboarding payload contents");
        return -1;
    }

    if (GetManualPairingCode(manualPairingCode, payload) != CHIP_NO_ERROR) {
        ChipLogError(DeviceLayer, "Failed to generate manual pairing code!");
        return -1;
    }

    memcpy(buf, manualPairingCode.data(), manualPairingCode.size());

    return 0;
}

uint8_t matter_get_qr_code(char *buf, size_t bufSize)
{
    if (bufSize < chip::QRCodeBasicSetupPayloadGenerator::kMaxQRCodeBase38RepresentationLength + 1) {
        ChipLogError(DeviceLayer, "Buffer too small for onboarding code!");
        return -1;
    }

    char payloadBuffer[chip::QRCodeBasicSetupPayloadGenerator::kMaxQRCodeBase38RepresentationLength + 1];
    chip::MutableCharSpan qrCode(payloadBuffer);
    RendezvousInformationFlags rendezvousFlag = chip::RendezvousInformationFlags(chip::RendezvousInformationFlag::kBLE);
    PayloadContents payload;

    if (GetPayloadContents(payload, rendezvousFlag) != CHIP_NO_ERROR) {
        ChipLogError(DeviceLayer, "Failed to get onboarding payload contents");
        return -1;
    }

    if (GetQRCode(qrCode, payload) != CHIP_NO_ERROR) {
        ChipLogError(DeviceLayer, "Failed to generate qr code!");
        return -1;
    }

    memcpy(buf, qrCode.data(), qrCode.size());

    return 0;
}

uint8_t matter_open_basic_commissioning_window(void)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    CommissioningWindowManager *mgr = &(chip::Server::GetInstance().GetCommissioningWindowManager());

    if (mgr != NULL) {
        chip::DeviceLayer::PlatformMgr().LockChipStack();
        err = mgr->OpenBasicCommissioningWindow();
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

void matter_close_basic_commissioning_window(void)
{
    CommissioningWindowManager *mgr = &(chip::Server::GetInstance().GetCommissioningWindowManager());

    if (mgr != NULL) {
        chip::DeviceLayer::PlatformMgr().LockChipStack();
        mgr->CloseCommissioningWindow();
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    }

    return;
}


uint8_t matter_get_certificate_declaration(uint8_t *buf, size_t bufSize, size_t *outLen)
{
    if (bufSize < chip::Credentials::kMaxCMSSignedCDMessage) {
        ChipLogError(DeviceLayer, "buffer size is smaller than %d.", chip::Credentials::kMaxCMSSignedCDMessage);
        return -1;
    }

    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    MutableByteSpan outBuffer(buf, bufSize);
    DeviceAttestationCredentialsProvider *dacProvider = chip::Credentials::GetDeviceAttestationCredentialsProvider();

    if (dacProvider != NULL) {
        err = dacProvider->GetCertificationDeclaration(outBuffer);
        if (err == CHIP_NO_ERROR) {
            *outLen = outBuffer.size();
            return 0;
        }
    }

    return -1;
}

uint8_t matter_get_dac_cert(uint8_t *buf, size_t bufSize, size_t *outLen)
{
    if (bufSize < chip::Credentials::kMaxDERCertLength) {
        ChipLogError(DeviceLayer, "buffer size is smaller than %d.", chip::Credentials::kMaxDERCertLength);
        return -1;
    }

    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    MutableByteSpan outBuffer(buf, bufSize);
    DeviceAttestationCredentialsProvider *dacProvider = chip::Credentials::GetDeviceAttestationCredentialsProvider();

    if (dacProvider != NULL) {
        err = dacProvider->GetDeviceAttestationCert(outBuffer);
        if (err == CHIP_NO_ERROR) {
            *outLen = outBuffer.size();
            return 0;
        }
    }

    return -1;
}

uint8_t matter_get_pai_cert(uint8_t *buf, size_t bufSize, size_t *outLen)
{
    if (bufSize < chip::Credentials::kMaxDERCertLength) {
        ChipLogError(DeviceLayer, "buffer size is smaller than %d.", chip::Credentials::kMaxDERCertLength);
        return -1;
    }

    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    MutableByteSpan outBuffer(buf, bufSize);
    DeviceAttestationCredentialsProvider *dacProvider = chip::Credentials::GetDeviceAttestationCredentialsProvider();

    if (dacProvider != NULL) {
        err = dacProvider->GetProductAttestationIntermediateCert(outBuffer);
        if (err == CHIP_NO_ERROR) {
            *outLen = outBuffer.size();
            return 0;
        }
    }

    return -1;
}

uint8_t matter_get_firmware_information(uint8_t *buf, size_t bufSize, size_t *outLen)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    MutableByteSpan outBuffer(buf, bufSize);
    DeviceAttestationCredentialsProvider *dacProvider = chip::Credentials::GetDeviceAttestationCredentialsProvider();

    if (dacProvider != NULL) {
        err = dacProvider->GetFirmwareInformation(outBuffer);
        if (err == CHIP_NO_ERROR) {
            *outLen = outBuffer.size();
            return 0;
        }
    }

    return -1;
}

uint8_t matter_get_setup_discriminator(uint16_t *discriminator)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    CommissionableDataProvider *cdProvider = chip::DeviceLayer::GetCommissionableDataProvider();

    if (cdProvider != NULL) {
        err = cdProvider->GetSetupDiscriminator(*discriminator);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_spake2p_iteration_count(uint32_t *iterationCount)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    CommissionableDataProvider *cdProvider = chip::DeviceLayer::GetCommissionableDataProvider();

    if (cdProvider != NULL) {
        err = cdProvider->GetSpake2pIterationCount(*iterationCount);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_spake2p_salt(uint8_t *buf, size_t bufSize, size_t *outLen)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    MutableByteSpan saltBuf(buf, bufSize);
    CommissionableDataProvider *cdProvider = chip::DeviceLayer::GetCommissionableDataProvider();

    if (cdProvider != NULL) {
        err = cdProvider->GetSpake2pSalt(saltBuf);
        if (err == CHIP_NO_ERROR) {
            *outLen = saltBuf.size();
            return 0;
        }
    }

    return -1;
}

uint8_t matter_get_spake2p_verifier(uint8_t *buf, size_t bufSize, size_t *outLen)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    MutableByteSpan verifierBuf(buf, bufSize);
    CommissionableDataProvider *cdProvider = chip::DeviceLayer::GetCommissionableDataProvider();

    if (cdProvider != NULL) {
        err = cdProvider->GetSpake2pVerifier(verifierBuf, bufSize);
        if (err == CHIP_NO_ERROR) {
            *outLen = verifierBuf.size();
            return 0;
        }
    }

    return -1;
}

uint8_t matter_get_setup_passcode(uint32_t *passcode)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    CommissionableDataProvider *cdProvider = chip::DeviceLayer::GetCommissionableDataProvider();

    if (cdProvider != NULL) {
        err = cdProvider->GetSetupPasscode(*passcode);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_vendor_name(char *buf, size_t bufSize)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    DeviceInstanceInfoProvider *diiProvider = chip::DeviceLayer::GetDeviceInstanceInfoProvider();

    if (diiProvider != NULL) {
        err = diiProvider->GetVendorName(buf, bufSize);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_vendor_id(uint16_t *vendorId)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    DeviceInstanceInfoProvider *diiProvider = chip::DeviceLayer::GetDeviceInstanceInfoProvider();

    if (diiProvider != NULL) {
        err = diiProvider->GetVendorId(*vendorId);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_product_name(char *buf, size_t bufSize)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    DeviceInstanceInfoProvider *diiProvider = chip::DeviceLayer::GetDeviceInstanceInfoProvider();

    if (diiProvider != NULL) {
        err = diiProvider->GetProductName(buf, bufSize);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_product_id(uint16_t *productId)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    DeviceInstanceInfoProvider *diiProvider = chip::DeviceLayer::GetDeviceInstanceInfoProvider();

    if (diiProvider != NULL) {
        err = diiProvider->GetProductId(*productId);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_part_number(char *buf, size_t bufSize)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    DeviceInstanceInfoProvider *diiProvider = chip::DeviceLayer::GetDeviceInstanceInfoProvider();

    if (diiProvider != NULL) {
        err = diiProvider->GetPartNumber(buf, bufSize);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_product_url(char *buf, size_t bufSize)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    DeviceInstanceInfoProvider *diiProvider = chip::DeviceLayer::GetDeviceInstanceInfoProvider();

    if (diiProvider != NULL) {
        err = diiProvider->GetProductURL(buf, bufSize);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_product_label(char *buf, size_t bufSize)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    DeviceInstanceInfoProvider *diiProvider = chip::DeviceLayer::GetDeviceInstanceInfoProvider();

    if (diiProvider != NULL) {
        err = diiProvider->GetProductLabel(buf, bufSize);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_serial_number(char *buf, size_t bufSize)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    DeviceInstanceInfoProvider *diiProvider = chip::DeviceLayer::GetDeviceInstanceInfoProvider();

    if (diiProvider != NULL) {
        err = diiProvider->GetSerialNumber(buf, bufSize);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_manufacturing_date(uint16_t *year, uint8_t *month, uint8_t *day)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    DeviceInstanceInfoProvider *diiProvider = chip::DeviceLayer::GetDeviceInstanceInfoProvider();

    if (diiProvider != NULL) {
        err = diiProvider->GetManufacturingDate(*year, *month, *day);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_hardware_version(uint16_t *hardwareVersion)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    DeviceInstanceInfoProvider *diiProvider = chip::DeviceLayer::GetDeviceInstanceInfoProvider();

    if (diiProvider != NULL) {
        err = diiProvider->GetHardwareVersion(*hardwareVersion);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_hardware_version_string(char *buf, size_t bufSize)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    DeviceInstanceInfoProvider *diiProvider = chip::DeviceLayer::GetDeviceInstanceInfoProvider();

    if (diiProvider != NULL) {
        err = diiProvider->GetHardwareVersionString(buf, bufSize);
    }

    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_software_version(uint32_t *softwareVersion)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    err = chip::DeviceLayer::ConfigurationMgr().GetSoftwareVersion(*softwareVersion);
    return (err == CHIP_NO_ERROR) ? 0 : -1;
}

uint8_t matter_get_software_version_string(char *buf, size_t bufSize)
{
    CHIP_ERROR err = CHIP_ERROR_INTERNAL;
    err = chip::DeviceLayer::ConfigurationMgr().GetSoftwareVersionString(buf, bufSize);
    return (err == CHIP_NO_ERROR) ? 0 : -1;
}
