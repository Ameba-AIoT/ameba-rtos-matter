# Matter Wi-Fi FullMAC Support

For more information about Ameba Wi-Fi Card (WHC) Mode, please refer to the official documentation:

https://aiot.realmcu.com/en/latest/rtos/whc/introduction/index.html

## Configuration

### Prerequisites

Under the `ameba-rtos-matter` directory, apply the Matter configuration and then launch Menuconfig:

```bash
matter_apply_conf
ameba.py menuconfig
```

### Enable WHC Device Mode

Navigate to:

```text
CONFIG WHC INTF
    WHC MODE
        ( ) WHC_IPC
        ( ) SINGLE CORE
        (X) WHC_DEV
        ( ) WHC_HOST

    HW INTERFACE
        ---> Select any supported interface

    WHC Config
        [*] Support WHC CMD PATH
        [ ] Support WHC WIFI API PATH
        [*] Support WHC SUPPLICANT OFFLOAD
        [*] Support WHC DEV TCPIP KEEPALIVE
        [ ] Support Download to SRAM Only
```

### Enable Bluetooth

Navigate to:

```text
CONFIG BT
    [*] Enable BT
        HW Interface (Internal) --->
    [ ] BT Advanced Feature
    [ ] BT Example Demo
```

> **Important:** Set **HW Interface** to **Internal**.

### Enable Matter BLE Adapter

Navigate to:

```text
CONFIG APPLICATION
    Entry to Matter Config --->
        Matter Config --->
            [*] Enable Matter
                Ameba RTOS version (Ameba RTOS v1.2) --->

            [*] BLE Matter Adapter
            [ ] Enable BLE Matter Multi Adv
            [ ] Enable Matter IPv4
            [ ] Enable Matter Terms and Condition
```

> **Important:** Enable **BLE Matter Adapter** to allow Matter commissioning over Bluetooth LE.

---

## Demonstration

This demonstration is based on **WPAoD (WPA Supplicant on Device)**.

### Test Environment

- **WHC Host:** Raspberry Pi
- **WHC Device:** AmebaGreen2
  - Functions as both the **WHC Device** and the **Matter Controllee**

### Notes

- The Matter stack runs on the WHC Device.
- Although validation has only been performed on AmebaGreen2, the implementation is expected to work on all supported ICs.

## API Overview

### `whc_matter_dev_downlink_hdl`

Handles events received from the WHC Host.

When triggered, this handler:

1. Receives an event posted by the Host.
2. Forwards the event to the Matter downlink path.
3. Sends the corresponding command to the Matter application for processing.

### `whc_matter_dev_uplink_hdl`

Handles commands received from the Matter Controller.

When triggered, this handler:

1. Receives commands or attribute updates from the Matter Controller.
2. Posts the corresponding event to the WHC Host.
3. Allows the Host to perform the required actions based on the received command.

Please check `examples/doorlock/matter_drivers.cpp` on how to include `whc_matter_dev_uplink_hdl`.

## Data Flow

### Downlink Path

```text
WHC Host
    │
    ▼
whc_matter_dev_downlink_hdl()
    │
    ▼
Matter Downlink Event
    │
    ▼
Matter Application
```

### Uplink Path

```text
Matter Controller
    │
    ▼
Matter Attribute / Command
    │
    ▼
whc_matter_dev_uplink_hdl()
    │
    ▼
WHC Host
```
