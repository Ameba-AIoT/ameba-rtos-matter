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

#include <controller/InvokeInteraction.h>
#include <controller/ReadInteraction.h>
#include <app/clusters/soil-measurement-server/SoilMeasurementCluster.h>
#include <soil_measurement/ameba_soil_measurement_instance.h>

#if CONFIG_ENABLE_CHIP_SHELL
#include <lib/shell/Engine.h>
#include <lib/shell/commands/Help.h>
#endif // ENABLE_CHIP_SHELL

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::SoilMeasurement;

#if CONFIG_ENABLE_CHIP_SHELL
using Shell::Engine;
using Shell::shell_command_t;
using Shell::streamer_get;
using Shell::streamer_printf;

Engine sShellManualSoilSubCommands;
#endif // defined(ENABLE_CHIP_SHELL)

#if CONFIG_ENABLE_CHIP_SHELL

CHIP_ERROR ManualSoilCommandHelpHandler(int argc, char ** argv)
{
    sShellManualSoilSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
    return CHIP_NO_ERROR;
}

CHIP_ERROR ManualSoilCommandHandler(int argc, char ** argv)
{
    if (argc == 0)
    {
        return ManualSoilCommandHelpHandler(argc, argv);
    }
    return sShellManualSoilSubCommands.ExecCommand(argc, argv);
}

CHIP_ERROR ManualSoilChangeMoistureCommandHandler(int argc, char ** argv)
{
    if (argc != 1)
    {
        return ManualSoilCommandHelpHandler(argc, argv);
    }

    // Convert input argument (moisture value) to uint8_t
    uint8_t moistureValue = static_cast<uint8_t>(atoi(argv[0]));

    chip::app::Clusters::SoilMeasurement::Attributes::SoilMoistureMeasuredValue::TypeInfo::Type soilMoistureMeasuredValue(moistureValue);

    // Call the function to set the moisture value
    CHIP_ERROR err = GetAmebaSoilMeasurementInstance()->Cluster().SetSoilMoistureMeasuredValue(soilMoistureMeasuredValue);

    if (err != CHIP_NO_ERROR)
    {
        return err;
    }

    return CHIP_NO_ERROR;
}

#endif // CONFIG_ENABLE_CHIP_SHELL
