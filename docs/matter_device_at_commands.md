# Device AT Commands

This page provides clear instructions for using AT commands on Realtek devices, specifically for factory reset and accessing the Matter shell (client).

To view all available AT Commands for Matter, enter `ATMH`

    Matter AT Commands List

        ATM$ : factory reset. (Usage: ATM$)
        ATM% : matter ota query image. (Usage: ATM%)
        ATM^ : matter ota apply update. (Usage: ATM^)
        ATMH : matter help. (Usage: ATMH)
        ATMS : matter client console. (Usage: ATMS=switch / ATMS=manual)

## Factory Reset

To perform a factory reset on the device:

    ATM$

## Matter Shell (Client) Commands

To interact with the Matter shell (client) via AT commands, use the following syntax:

### For AmebaZ2, AmebaZ2Plus, AmebaDplus, AmebaLite, and AmebaSmart

    ATMS=switch

### For AmebaD

    ATMS switch

**Note**: The AT command for AmebaD does not require the '=' sign.

### Examples:

Demostrating with `ATMS=switch`, please make sure to replace the '=' sign with space for AmebaD.

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

