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

#include "app-common/zap-generated/ids/Attributes.h"
#include "app-common/zap-generated/ids/Clusters.h"
#include "app-common/zap-generated/ids/Commands.h"
#include "lib/core/CHIPError.h"

#include "controller/InvokeInteraction.h"
#include "controller/ReadInteraction.h"
#include <protocols/interaction_model/StatusCode.h>

#include <camera/ameba_camera_device.h>
#include <webrtc/ameba_webrtc_provider_manager.h>

// Matter-layer camera device instance, defined in matter_drivers.cpp.
extern Camera::CameraDevice gCameraDevice;

#if CONFIG_ENABLE_CHIP_SHELL
#include "lib/shell/Engine.h"
#include "lib/shell/commands/Help.h"

// Only pull in what this header actually uses unqualified (WebRTCProviderManager
// below). Avoid the broader chip/chip::app/chip::app::Clusters using-directives:
// this is a header, so they'd leak into every .cpp that includes it (e.g.
// matter_drivers.cpp) and chip::app::Clusters::Identify then collides with the
// global `struct Identify` from identify-server's CodegenIntegration.h.
using namespace chip;
using namespace chip::app::Clusters::WebRTCTransportProvider;

using Shell::Engine;
using Shell::shell_command_t;
using Shell::streamer_get;
using Shell::streamer_printf;

Engine sShellManualCameraSubCommands;

CHIP_ERROR ManualCameraCommandHelpHandler(int argc, char **argv)
{
    sShellManualCameraSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
    return CHIP_NO_ERROR;
}

CHIP_ERROR ManualCameraCommandHandler(int argc, char **argv)
{
    if (argc == 0) {
        return ManualCameraCommandHelpHandler(argc, argv);
    }

    return sShellManualCameraSubCommands.ExecCommand(argc, argv);
}

CHIP_ERROR ManualCameraSetWebRTCDeferredOfferCommandHandler(int argc, char **argv)
{
    if (argc == 0) {
        return ManualCameraCommandHelpHandler(argc, argv);
    }

    int enable = static_cast<int>(atoi(argv[0]));

    auto &providerManager = static_cast<WebRTCProviderManager &>(gCameraDevice.GetWebRTCProviderDelegate());
    providerManager.SetWebRTCDeferredOffer((enable == 1));

    return CHIP_NO_ERROR;
}
#endif // CONFIG_ENABLE_CHIP_SHELL
