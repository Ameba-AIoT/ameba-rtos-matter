# Matter Commissioning and Control Guide

## Test with [chip-tool](https://github.com/project-chip/connectedhomeip/tree/master/examples/chip-tool)
Use standalone chip-tool app(linux) to communicate with the device. The Chip-tool is a software utility designed to assist developers and manufacturers working on Matter-compatible devices.
It provides various functionalities to interact with Matter-enabled devices, such as commissioning, controlling, and monitoring their operations. 
The Chip-tool is an essential component for developers working on Matter devices as it allows them to validate their implementations, debug issues, and ensure interoperability with other Matter devices. 
Additionally, it assists in configuring and managing Matter networks, making it easier for developers to create robust and reliable smart home and IoT solutions.

As a commissioner, the Chip-tool facilitates the provisioning and configuration of Matter devices within a network. It assigns necessary network addresses, security keys, and other parameters to newly added devices, ensuring their seamless integration into the Matter network.

In order to send commands to a device, the device must be commissioned into the Matter network. The configuration state for chip-tool is stored in `/tmp/chip_tool_config.ini`; deleting this and other `.ini` files in `/tmp` can sometimes resolve issues due to stale configuration.

### Commission a device over BLE-WiFi

* Run CHIP-TOOL ble-wifi commissioning command `./chip-tool pairing ble-wifi <NODE_ID> <SSID> <PASSWORD> <SETUP_PIN_CODE> <DISCRIMINATOR>`
* For example: `./chip-tool pairing ble-wifi 12344321 testssid password 20202021 3840`

### Commission a device over IP (For devices already on the WiFi network)

* Connect Ameba to AP using `ATW0, ATW1, ATWC` commands
* Run CHIP-TOOL onnetwork commissioning command `./chip-tool pairing onnetwork <NODE_ID> <SETUP_PIN_CODE>`
* For example: `./chip-tool pairing onnetwork 12344321 20202021`

### Command for onoff cluster

Use PA_20 as output, connect a LED to this pin and GND.

* Run CHIP-TOOL on-off cluster command `./chip-tool onoff on 12344321 1`

* Run CHIP-TOOL on-off cluster command `./chip-tool onoff off 12344321 1`

Find more detail in [chip-tool](https://github.com/project-chip/connectedhomeip/tree/master/examples/chip-tool)

### Notice

The QR code will be provided when booting, for example:

```shell
chip[SVR] SetupQRCode: [MT:-24J042C00KA0648G00]
chip[SVR] Copy/paste the below URL in a browser to see the QR Code:
chip[SVR] https://dhrishi.github.io/connectedhomeip/qrcode.html?data=MT%3A-24J042C00KA0648G00
```
Copy/paste the URL in a browser and a scannable QR code will be displayed.

## Matter ESF (Enhanced Setup Flow)
Matter Enhanced Setup Flow, for short ESF, also known as Matter Terms and Condition, is an update to the core commissioning process. It now supports the display of a manufacturer’s terms and conditions (T&Cs) directly in the commissioner app, including enabling the collection of customer consent. These responses are transmitted to the device being set up, enabling the device to adjust functionalities as outlined in the T&Cs.

### Enable Matter ESF
Navigate to connectedhomeip folder and open the zapfile using zaptool

``` bash
cd connectedhomeip
source scripts/activate.sh
./scripts/tools/zap/run_zaptool.sh path/to/zapfile.zap
```

Navigate to `Endpoint 0`. Under `General` clusters, configure `General Commissioning` cluster. Enable the following attributes and commands:

- Attributes:
    - `TCAcceptedVersion (0x0005)`
    - `TCMinRequiredVersion (0x0006)`
    - `TCAcknowledgements (0x0007)`
    - `TCAcknowledgementsRequired (0x0008)`
    - `TCUpdateDeadline (0x0009)`
- Commands:
    - `SetTCAcknowledgements (0x06)`
    - `SetTCAcknowledgementsResponse (0x07)`

Save and close the zapfile.

Open `component/application/matter/project/cmake/flags/public_definitions_matter.cmake` and modify `CHIP_AMEBA_TC_REQUIRED_ACKNOWLEDGEMENTS` and `CHIP_AMEBA_TC_MIN_REQUIRED_VERSION` based on requirements, default is set to 1.

```cmake
...
if(CONFIG_MATTER_TC_EN)
    ameba_list_append(matter_defintions
        CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION=1
        CHIP_AMEBA_TC_REQUIRED_ACKNOWLEDGEMENTS=1
        CHIP_AMEBA_TC_MIN_REQUIRED_VERSION=1
    )
endif()
...
```

Navigate to the `amebaxxx_gcc_project` directory:

``` bash
cd ameba-rtos/amebaxxx_gcc_project/
```

Menuconfig for Matter ESF:
- To enable Matter ESF, select `CONFIG APPLICATION`, under `Matter Config`, enable `Enable Matter Terms and Condition`

```bash
python menuconfig.py
```

[Build Matter library and Ameba image](matter_building_guide.md)

### Commission a device when Matter ESF is enabled

* If Matter ESF is enabled, add the following two arguments to the CHIP-TOOL during pairing: `--tc-acknowledgements <CHIP_AMEBA_TC_REQUIRED_ACKNOWLEDGEMENTS> --tc-acknowledgements-version <CHIP_AMEBA_TC_MIN_REQUIRED_VERSION>`
* For example: `./chip-tool pairing ble-wifi 12344321 testssid password 20202021 3840 --tc-acknowledgements 1 --tc-acknowledgements-version 1`
