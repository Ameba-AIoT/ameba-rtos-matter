# General Guide for Ameba Matter Solution

## Realtek Matter Application Architecture

Realtek’s Matter platform solution has both Wi-Fi and BLE working on a single SoC. Initial development of Realtek does not
support Thread or Ethernet, and developers can decide whether to support BLE as Matter provides IP (Wi-Fi) commissioning.
Theoretically speaking, the support of Wi-Fi is mandatory.

## Getting Started

1. [Requirements](#requirements)
2. [Prerequisites](#prerequisites)
3. [Getting Repository and Building Matter for Supported ICs](#getting-repository-and-building-matter-for-supported-ics)

### Requirements
- Recommended to use Ubuntu 22.04 and above.

### Prerequisites
Before starting with Matter integration, ensure you have fulfilled the prerequisites as outlined in the [Matter Repository](https://github.com/project-chip/connectedhomeip/blob/master/guides/BUILDING.md#prerequisites).

### Getting Repository and Building Matter for Supported ICs

To begin building Matter for specific ICs, refer to the detailed instructions provided in the [Matter Building Guide](matter_building_guide.md). Each supported IC has its own building method documented there.
