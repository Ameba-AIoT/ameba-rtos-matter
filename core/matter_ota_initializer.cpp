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
#include <matter_ota_initializer.h>
#include <app/clusters/ota-requestor/DefaultOTARequestorStorage.h>
#include <app/clusters/ota-requestor/CodegenIntegration.h>
#include <app/clusters/ota-requestor/BDXDownloader.h>
#include <app/clusters/ota-requestor/DefaultOTARequestor.h>
#include <app/clusters/ota-requestor/DefaultOTARequestorDriver.h>
#include <platform/Ameba/AmebaOTAImageProcessor.h>

using namespace chip;
using namespace chip::DeviceLayer;

namespace {
DefaultOTARequestor gRequestorCore;
DefaultOTARequestorStorage gRequestorStorage;
DefaultOTARequestorDriver gRequestorUser;
BDXDownloader gDownloader;
AmebaOTAImageProcessor gImageProcessor;
} // namespace

void matter_ota_initializer()
{
    SetRequestorInstance(&gRequestorCore);
    gRequestorStorage.Init(chip::Server::GetInstance().GetPersistentStorage());
    // Set server instance used for session establishment
    CHIP_ERROR err = gRequestorCore.Init(chip::Server::GetInstance(), gRequestorStorage, gRequestorUser, gDownloader,
                                         GetOTARequestorAttributes(), GetDefaultOTARequestorEventGenerator());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(SoftwareUpdate, "Failed to initialize OTA Requestor: %" CHIP_ERROR_FORMAT, err.Format());
        return;
    }

    gImageProcessor.SetOTADownloader(&gDownloader);
    // Connect the Downloader and Image Processor objects
    gDownloader.SetImageProcessorDelegate(&gImageProcessor);
    gRequestorUser.Init(&gRequestorCore, &gImageProcessor);
}
