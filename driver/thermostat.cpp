#include <thermostat.h>

#include <support/logging/CHIPLogging.h>

void MatterThermostat::Init(void)
{
    // init thermostat driver code
}

void MatterThermostat::deInit(void)
{
    // deinit thermostat driver code
}

void MatterThermostat::Do(void)
{
    // implement thermostat action here
    ChipLogProgress(DeviceLayer, "Thermostat action");
}
