/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
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
#include "controller/InvokeInteraction.h"
#include "controller/ReadInteraction.h"
#include <app/clusters/boolean-state-configuration-server/CodegenIntegration.h>
#include <app/clusters/boolean-state-server/CodegenIntegration.h>
#include <protocols/interaction_model/StatusCode.h>

#if CONFIG_ENABLE_CHIP_SHELL
#include "lib/shell/Engine.h"
#include "lib/shell/commands/Help.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::BooleanState;

using Shell::Engine;
using Shell::shell_command_t;
using Shell::streamer_get;
using Shell::streamer_printf;

Engine sShellManualBoolStateSubCommands;

CHIP_ERROR ManualBoolCommandHelpHandler(int argc, char **argv)
{
    sShellManualBoolStateSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
    return CHIP_NO_ERROR;
}

CHIP_ERROR ManualBoolCommandHandler(int argc, char **argv)
{
    if (argc == 0) {
        return ManualBoolCommandHelpHandler(argc, argv);
    }

    return sShellManualBoolStateSubCommands.ExecCommand(argc, argv);
}

CHIP_ERROR ManualBoolSetStateCommandHandler(int argc, char **argv)
{
    if (argc == 0) {
        return ManualBoolCommandHelpHandler(argc, argv);
    }

    bool newState = static_cast<bool>(atoi(argv[0]));

    auto booleanState = BooleanState::FindClusterOnEndpoint(1);
    VerifyOrReturnError(booleanState != nullptr, CHIP_ERROR_NOT_FOUND);
    booleanState->SetStateValue(newState);
    ChipLogProgress(NotSpecified, "BooleanState changed to %s", newState ? "true" : "false");
    return CHIP_NO_ERROR;
}
#endif // CONFIG_ENABLE_CHIP_SHELL
