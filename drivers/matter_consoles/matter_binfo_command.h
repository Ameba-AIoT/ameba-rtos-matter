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
#include <app/clusters/basic-information/BasicInformationCluster.h>
#include <app/clusters/basic-information/CodegenIntegration.h>
#include <protocols/interaction_model/StatusCode.h>

#if CONFIG_ENABLE_CHIP_SHELL
#include "lib/shell/Engine.h"
#include "lib/shell/commands/Help.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::BasicInformation;

using Shell::Engine;
using Shell::shell_command_t;
using Shell::streamer_get;
using Shell::streamer_printf;

Engine sShellManualBinfoStateSubCommands;

CHIP_ERROR ManualBinfoCommandHelpHandler(int argc, char **argv)
{
    sShellManualBinfoStateSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
    return CHIP_NO_ERROR;
}

CHIP_ERROR ManualBinfoCommandHandler(int argc, char **argv)
{
    if (argc == 0) {
        return ManualBinfoCommandHelpHandler(argc, argv);
    }

    return sShellManualBinfoStateSubCommands.ExecCommand(argc, argv);
}

CHIP_ERROR ManualBinfoConfigVerCommandHandler(int argc, char **argv)
{
    auto cluster = app::Clusters::BasicInformation::GetClusterInstance();
    VerifyOrReturnError(cluster != nullptr, CHIP_ERROR_NOT_FOUND);
    return cluster->IncreaseConfigurationVersion();
}
#endif // CONFIG_ENABLE_CHIP_SHELL
