# Device AT Commands

This page provides clear instructions for using AT commands on Realtek devices, specifically for factory reset and accessing the Matter shell (client).

## Factory Reset

To perform a factory reset on the device:

    ATM$

## Matter Shell (Client) Commands

To interact with the Matter shell (client) via AT commands, use the following syntax:

    ATMS=switch

### Examples:

#### Turn On/Off Command:

`ATMS=switch onoff on`

#### Level Control Command:

`ATMS=switch levelcontrol movetolevel 100 0 0 0`

#### Color Control Command:

`ATMS=switch colorcontrol movetohue 100 0 0 0 0`

#### Thermostat Command:

`ATMS=switch thermostat SPRL 0 0`

#### Read Attribute

`ATMS=switch <cluster> read <attribute>`




