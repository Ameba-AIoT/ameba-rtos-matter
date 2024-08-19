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

#include "matter_command.h"
#include "matter_dishwasher_alarm_command.h"
#include "matter_occupancy_sensing_command.h"
#include "matter_operational_state_command.h"
#include "matter_refrigerator_alarm_command.h"
#include "matter_rvc_modes_command.h"
#include "matter_switch_command.h"

#include "app/server/Server.h"
#include "platform/CHIPDeviceLayer.h"
#include <lib/support/CodeUtils.h>

#if CONFIG_ENABLE_CHIP_SHELL
#include "lib/shell/Engine.h"
#include "lib/shell/commands/Help.h"
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

CHIP_ERROR ManualOperationHelpHandler(int argc, char ** argv)
{
    sShellManualOperationSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
    return CHIP_NO_ERROR;
}

CHIP_ERROR ManualOperationCommandHandler(int argc, char ** argv)
{
    if (argc == 0)
    {
        return ManualOperationHelpHandler(argc, argv);
    }

    return sShellManualOperationSubCommands.ExecCommand(argc, argv);
}

/**
 * @brief configures switch matter shell
 *
 */
static void RegisterManualOperationCommands()
{

    static const shell_command_t sManualDishWasherAlarmSubCommands[] = {
        { &ManualDishWasherAlarmCommandHelpHandler, "help", "Usage: manual dishalm <subcommand>" },
        { &ManualDishWasherAlarmSetRaiseCommandHandler, "raise", "raise Usage: manual dishalm raise" },
        { &ManualDishWasherAlarmSetLowerCommandHandler, "lower", "lower Usage: manual dishalm lower" },
    };

    static const shell_command_t sManualOperationSubCommands[] = {
        { &ManualOperationHelpHandler, "help", "Usage: manual <subcommand>" },
        { &ManualDishWasherAlarmCommandHandler, "dishalm", " Usage: manual dishalm <subcommand>" },
        { &ManualOccupancySensingCommandHandler, "occ", " Usage: manual occ <subcommand>" },
        { &ManualOperationalStateCommandHandler, "opstate", " Usage: manual opstate <subcommand>" },
        { &ManualOvenCavityOperationalStateCommandHandler, "oven-opstate", " Usage: manual dishalm <subcommand>" },
        { &ManualRefrigeratorAlarmCommandHandler, "refalm", " Usage: manual refalm <subcommand>" },
        { &ManualRVCCommandHandler, "rvc", " Usage: manual rvc <subcommand>" },
        { &ManualSwitchCommandHandler, "switch", " Usage: manual switch <subcommand>" },
    };

    static const shell_command_t sManualOccupancySensingSubCommands[] = {
        { &ManualOccupancySensingCommandHelpHandler, "help", "Usage: manual occ <subcommand>" },
        { &ManualOccupancySensingSetOccupancyCommandHandler, "set-occupancy", "set-occupancy Usage: manual occ set-occupancy <state>" },
    };

    static const shell_command_t sManualOperationalStateSubCommands[] = {
        { &ManualOperationalStateCommandHelpHandler, "help", "Usage: manual opstate <subcommand>" },
        { &ManualOperationalStateSetStateCommandHandler, "set-state", "set-state Usage: manual opstate set-state <state>" },
        { &ManualOperationalStateSetErrorCommandHandler, "set-error", "set-error Usage: manual opstate set-error <error>" },
    };

    static const shell_command_t sManualOvenCavityOperationalStateSubCommands[] = {
        { &ManualOvenCavityOperationalStateCommandHelpHandler, "help", "Usage: manual oven-opstate <subcommand>" },
        { &ManualOvenCavityOperationalStateSetStateCommandHandler, "set-state",
          "set-state Usage: manual oven-opstate set-state <state>" },
        { &ManualOvenCavityOperationalStateSetErrorCommandHandler, "set-error",
          "set-error Usage: manual oven-opstate set-error <error>" },
    };

    static const shell_command_t sManualRefrigeratorAlarmStateSubCommands[] = {
        { &ManualRefrigeratorAlarmCommandHelpHandler, "help", "Usage: manual refalm <subcommand>" },
        { &ManualRefrigeratorAlarmDoorOpenCommandHandler, "door-open", "door-open Usage: manual refalm door-open" },
        { &ManualRefrigeratorAlarmDoorCloseCommandHandler, "door-close", "door-close Usage: manual refalm door-close" },
        { &ManualRefrigeratorAlarmSuppressCommandHandler, "suppress-alarm", "suppress-alarm Usage: manual refalm suppress-alarm" },
    };

    static const shell_command_t sManualRVCSubCommands[] = {
        { &ManualRVCCommandHelpHandler, "help", "Usage: manual rvc <subcommand>" },
        { &ManualRVCCleanModeCommandHandler, "cleanmode", "Usage: manual rvc cleanmode <subcommand>" },
        { &ManualRVCOperationalStateCommandHandler, "opstate", "Usage: manual rvc opstate <subcommand>" },
        { &ManualRVCRunModeCommandHandler, "runmode", "Usage: manual rvc runmode <subcommand>" },
    };

    static const shell_command_t sManualRVCCleanModeSubCommands[] = {
        { &ManualRVCCleanModeCommandHelpHandler, "help", "Usage: manual rvc cleanmode <subcommand>" },
        { &ManualRVCCleanModeSetModeCommandHandler, "set-mode", "set-mode Usage: manual rvc cleanmode set-mode <mode>" },
    };

    static const shell_command_t sManualRVCOperationalStateSubCommands[] = {
        { &ManualRVCOperationalStateCommandHelpHandler, "help", "Usage: manual rvc opstate <subcommand>" },
        { &ManualRVCOperationalStateSetStateCommandHandler, "set-state", "set-state Usage: manual rvc opstate set-state <state>" },
        { &ManualRVCOperationalStateSetErrorCommandHandler, "set-error", "set-error Usage: manual rvc opstate set-error <error>" },
    };

    static const shell_command_t sManualRVCRunModeSubCommands[] = {
        { &ManualRVCRunModeCommandHelpHandler, "help", "Usage: manual rvc runmode <subcommand>" },
        { &ManualRVCRunModeSetModeCommandHandler, "set-mode", "set-mode Usage: manual rvc runmode set-mode <mode>" },
    };

    static const shell_command_t sManualSwitchSubCommands[] = {
        { &ManualSwitchCommandHelpHandler, "help", "Usage: manual switch <subcommand>" },
        { &ManualSwitchInitialPressCommandHandler, "initial-press", "initial-press Usage: manual switch initial-press <subcommand>" },
    };

    static const shell_command_t sManualOperationCommand = { &ManualOperationCommandHandler, "manual",
                                                             "Manual Operation commands. Usage: manual <subcommand>" };

    // Register commands
    sShellManualOperationSubCommands.RegisterCommands(sManualOperationSubCommands, ArraySize(sManualOperationSubCommands));

    sShellManualDishWasherAlarmStateSubCommands.RegisterCommands(sManualDishWasherAlarmSubCommands, ArraySize(sManualDishWasherAlarmSubCommands));
    sShellManualOccupancySensingSubCommands.RegisterCommands(sManualOccupancySensingSubCommands, ArraySize(sManualOccupancySensingSubCommands));
    sShellManualOperationalStateSubCommands.RegisterCommands(sManualOperationalStateSubCommands, ArraySize(sManualOperationalStateSubCommands));

    sShellManualOvenCavityOperationalStateSubCommands.RegisterCommands(sManualOvenCavityOperationalStateSubCommands, ArraySize(sManualOvenCavityOperationalStateSubCommands));

    sShellManualRefrigeratorAlarmStateSubCommands.RegisterCommands(sManualRefrigeratorAlarmStateSubCommands, ArraySize(sManualRefrigeratorAlarmStateSubCommands));

    sShellManualRVCSubCommands.RegisterCommands(sManualRVCSubCommands, ArraySize(sManualRVCSubCommands));
    sShellManualRVCCleanModeSubCommands.RegisterCommands(sManualRVCCleanModeSubCommands, ArraySize(sManualRVCCleanModeSubCommands));
    sShellManualRVCOperationalStateSubCommands.RegisterCommands(sManualRVCOperationalStateSubCommands, ArraySize(sManualRVCOperationalStateSubCommands));
    sShellManualRVCRunModeSubCommands.RegisterCommands(sManualRVCRunModeSubCommands, ArraySize(sManualRVCRunModeSubCommands));

    sShellManualSwitchSubCommands.RegisterCommands(sManualSwitchSubCommands, ArraySize(sManualSwitchSubCommands));

    Engine::Root().RegisterCommands(&sManualOperationCommand, 1);
}
#endif // ENABLE_CHIP_SHELL

} // namespace

/********************************************************
 * Switch functions
 *********************************************************/

CHIP_ERROR InitManualOperation()
{
#if CONFIG_ENABLE_CHIP_SHELL
    RegisterManualOperationCommands();
#endif
    return CHIP_NO_ERROR;
}
