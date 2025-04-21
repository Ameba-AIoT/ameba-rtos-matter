# Realtek's Matter Solution

- [Introduction ameba-matter](#introduction-ameba-matter)
- [Ameba RTOS Matter Repo Integration](#ameba-rtos-matter-repo-integration)
- [Supported Integrated Circuits (ICs)](#supported-integrated-circuits-ics)
- [Documentation](#documentation)
- [Release History](#release-history)
- [Important Notice](#important-notice)
- [Directory Structure Overview](#directory-structure-overview)

## Introduction ameba-matter

Welcome to the `ameba-rtos-matter` repository, your go-to resource for Realtek's Matter Solution. This repository is designed to provide a standardized interface for integrating Realtek's Matter Solution into embedded systems. By offering a unified and adaptable framework, `ameba-rtos-matter` simplifies the process of implementing Matter-compliant devices. Whether you're a developer, manufacturer, or researcher, this repository equips you with the tools and documentation needed to seamlessly integrate Matter into your projects.

## Ameba RTOS Matter Repo Integration

Learn how to integrate Realtek's Matter solution with the Ameba RTOS platform. Explore the integration details [here](https://github.com/Ameba-AIoT/ameba-matter/blob/main/docs/ameba_matter_integration.md)

## Important Notice

Please read the [Important Notice](https://github.com/Ameba-AIoT/ameba-matter/blob/main/docs/matter_important_notice.md) for important updates and announcements.

## Supported Integrated Circuits (ICs)

Feel free to explore our curated list of supported ICs below to find the best match for your project.

1. AmebaDplus (RTL8721Dx)
2. AmebaLite  (RTL8720EA / RTL8726EA)
3. AmebaSmart (RTL8730E)

Checkout the [Application Notes](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/main/docs/AN0204%20Realtek%20Matter%20application%20note.en.pdf) to learn more about Realtek's Matter solution.

## Documentation

1. [General Guide](https://github.com/Ameba-AIoT/ameba-matter/blob/main/docs/matter_general_guide.md) - A comprehensive overview of Matter integration and usage.

2. [Building Guide](https://github.com/Ameba-AIoT/ameba-matter/blob/main/docs/matter_building_guide.md) - Detailed instructions and guidelines for building Matter-compliant devices using the Ameba Matter solutions.

3. [Supported Device Types](https://github.com/Ameba-AIoT/ameba-matter/blob/main/docs/ameba_supported_device_types.md) - Comprehensive list detailing the types of devices compatible with Ameba Matter.

4. [AT Command Guide](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/main/docs/matter_device_at_commands.md) - AT commands for Matter.

5. [Commissioning and Control](https://github.com/Ameba-AIoT/ameba-matter/blob/main/docs/matter_commissioning_and_control_guide.md) - Focuses on commissioning procedures and control mechanisms within the Ameba Matter ecosystem.

6. [Matter Certification Guide](https://github.com/Ameba-AIoT/ameba-matter/blob/main/docs/matter_certification_guide.md) - Guide for Matter Certification.

7. [Factory Data Guide](https://github.com/Ameba-AIoT/ameba-matter/blob/main/tools/factorydata/README.md) - Information about Matter factory data.

8. [OTA Guide](https://github.com/Ameba-AIoT/ameba-matter/blob/main/tools/ota/README.md) - Guide for Over-The-Air (OTA) updates in Matter implementations.

9. [Tools Guide](https://github.com/Ameba-AIoT/ameba-matter/blob/main/docs/matter_tools_guide.md) - Overview of tools available for Matter development.

## Release History

**Matter v1.4 and v1.3** is associated with the following SDKs:
- AmebaRTOS SDK: [ameba-rtos](https://github.com/mikaelajiwidodo/ameba-rtos.git)

| Matter Version | ameba-rtos branch                   | ameba-rtos-matter version | connectedhomeip version |
|----------------|-------------------------------------|---------------------------|-------------------------|
| Matter v1.4    | ameba-rtos-v1.0/matter/release/v1.4 | ameba-rtos/release/v1.4   | v1.4-branch             |
| Matter v1.3    | ameba-rtos-v1.0/matter/release/v1.3 | ameba-rtos/release/v1.3   | (SHA) 70d9a614          |

## Directory Structure Overview

The directory structure is organized to facilitate development, documentation, and tooling for Matter integration on embedded systems.

| Directory    | Description                                                                                                    |
|--------------|----------------------------------------------------------------------------------------------------------------|
|  api         | Essential functions for interfacing with Matter, including device status checks and information retrieval such as vendor ID and product ID. |
|  docs        | Documentation files, including guides, references, and API documentation for Matter integration with the Ameba SDK. |
|  driver      | Device-specific drivers enabling smooth operation of Matter devices with the Ameba SDK, tailored to each device type's requirements. |
|  common      | Common code for Matter functionalities in the Ameba SDK, covering Bluetooth, Wi-Fi, time management, and data storage. Includes configuration headers for easy project integration. |
|  core        | Fundamental code for powering Matter in the Ameba SDK's Porting Layer, providing the foundation for seamless integration. |
|  example     | Code snippets and projects demonstrating Matter device usage in the Ameba SDK, organized into subfolders for various device types. |
|  project     | Makefiles and compilation scripts for Matter libraries within the Ameba SDK, categorized by different chip builds for various device types. |
|  tools       | Tools and utilities for development, testing, and deployment in the context of Matter integration with the Ameba SDK. |