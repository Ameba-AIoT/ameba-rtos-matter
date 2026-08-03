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
#include "matter_camera_command.h"

#include "app/server/Server.h"
#include "platform/CHIPDeviceLayer.h"
#include <lib/support/CodeUtils.h>

#if CONFIG_ENABLE_CHIP_SHELL
#include "lib/shell/Engine.h"
#include "lib/shell/commands/Help.h"
#include <shell/launch_shell.h>
#endif // ENABLE_CHIP_SHELL

using namespace chip;
using namespace chip::app;

#if CONFIG_ENABLE_CHIP_SHELL
using Shell::Engine;
using Shell::shell_command_t;
using Shell::streamer_get;
using Shell::streamer_printf;

Engine sShellManualOperationSubCommands;
#endif // defined(ENABLE_CHIP_SHELL)

namespace {
#if CONFIG_ENABLE_CHIP_SHELL

/********************************************************
 * Manual Operation shell functions
 *********************************************************/

CHIP_ERROR ManualOperationHelpHandler(int argc, char **argv)
{
    sShellManualOperationSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
    return CHIP_NO_ERROR;
}

CHIP_ERROR ManualOperationCommandHandler(int argc, char **argv)
{
    if (argc == 0) {
        return ManualOperationHelpHandler(argc, argv);
    }

    return sShellManualOperationSubCommands.ExecCommand(argc, argv);
}

/**
 * @brief configures switch matter shell
 *
 */
static void RegisterCameraManualOperationCommands()
{
    static const shell_command_t sManualOperationSubCommands[] = {
        { &ManualOperationHelpHandler, "help", "Usage: manual <subcommand>" },
        { &ManualCameraCommandHandler, "camera", "Usage: manual camera <subcommand>" },
    };

    static const shell_command_t sManualCameraSubCommands[] = {
        { &ManualCameraCommandHelpHandler, "help", "Usage: manual camera <subcommand>" },
        { &ManualCameraSetWebRTCDeferredOfferCommandHandler, "set-webrtc-deffered-offer", "Usage: manual camera set-webrtc-deffered-offer <enable>" },
    };

    static const shell_command_t sManualOperationCommand = { &ManualOperationCommandHandler, "manual",
                                                             "Manual Operation commands. Usage: manual <subcommand>"
                                                           };

    // Register commands
    sShellManualOperationSubCommands.RegisterCommands(sManualOperationSubCommands, MATTER_ARRAY_SIZE(sManualOperationSubCommands));

    sShellManualCameraSubCommands.RegisterCommands(sManualCameraSubCommands, MATTER_ARRAY_SIZE(sManualCameraSubCommands));
    Engine::Root().RegisterCommands(&sManualOperationCommand, 1);
}
#endif // ENABLE_CHIP_SHELL

} // namespace

/********************************************************
 * Switch functions
 *********************************************************/

CHIP_ERROR InitCameraManualOperation()
{
#if CONFIG_ENABLE_CHIP_SHELL
    RegisterCameraManualOperationCommands();
    chip::LaunchShell();
#endif
    return CHIP_NO_ERROR;
}
