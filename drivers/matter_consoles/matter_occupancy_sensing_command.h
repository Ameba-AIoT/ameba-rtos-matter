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
#include <app/clusters/occupancy-sensor-server/occupancy-hal.h>
#include <app/clusters/occupancy-sensor-server/occupancy-sensor-server.h>

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
    if (argc != 1)
    {
        return ManualOccupancySensingCommandHelpHandler(argc, argv);
    }
    Protocols::InteractionModel::Status status;
    status = chip::app::Clusters::OccupancySensing::Attributes::Occupancy::Set(1, (uint8_t) atoi(argv[0]));
    if (status != Protocols::InteractionModel::Status::Success)
    {
        ChipLogError(DeviceLayer, "ManualOCCSetOccupancyCommandHandler Error!");
        return CHIP_ERROR_INTERNAL;
    }
    return CHIP_NO_ERROR;
}

#endif // CONFIG_ENABLE_CHIP_SHELL
