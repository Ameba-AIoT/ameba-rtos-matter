/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
 *    All rights reserved.
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
#include <app/clusters/occupancy-sensor-server/CodegenIntegration.h>
#include <app/clusters/occupancy-sensor-server/OccupancySensingCluster.h>

#if CONFIG_ENABLE_CHIP_SHELL
#include "lib/shell/Engine.h"
#include "lib/shell/commands/Help.h"
#endif // ENABLE_CHIP_SHELL

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OccupancySensing;
using namespace chip::app::Clusters::OccupancySensing::Structs;

#if CONFIG_ENABLE_CHIP_SHELL
using Shell::Engine;
using Shell::shell_command_t;
using Shell::streamer_get;
using Shell::streamer_printf;

Engine sShellManualOccupancySensingSubCommands;
#endif // defined(ENABLE_CHIP_SHELL)

#if CONFIG_ENABLE_CHIP_SHELL

CHIP_ERROR ManualOccupancySensingCommandHelpHandler(int argc, char ** argv)
{
    sShellManualOccupancySensingSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);

    printf("Occupancy values:\n");
    printf("    0: Unoccupied\n");
    printf("    1: kOccupied\n\n");

    return CHIP_NO_ERROR;
}

CHIP_ERROR ManualOccupancySensingCommandHandler(int argc, char ** argv)
{
    if (argc == 0)
    {
        return ManualOccupancySensingCommandHelpHandler(argc, argv);
    }
    return sShellManualOccupancySensingSubCommands.ExecCommand(argc, argv);
}


CHIP_ERROR ManualOccupancySensingSetOccupancyCommandHandler(int argc, char ** argv)
{
    if (argc != 1) {
        return ManualOccupancySensingCommandHelpHandler(argc, argv);
    }

    chip::app::Clusters::OccupancySensingCluster * cluster = OccupancySensing::FindClusterOnEndpoint(1);
    if (cluster == nullptr) {
        return CHIP_ERROR_INTERNAL;
    }
    cluster->SetOccupancy(atoi(argv[0]));
    return CHIP_NO_ERROR;
}

#endif // CONFIG_ENABLE_CHIP_SHELL
