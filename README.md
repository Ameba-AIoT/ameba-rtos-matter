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

Learn how to integrate Realtek's Matter solution with the Ameba RTOS platform. Explore the integration details [here](docs/ameba_matter_integration.md)

## Important Notice

Please read the [Important Notice](docs/matter_important_notice.md) for important updates and announcements.

## Supported Integrated Circuits (ICs)

Feel free to explore our curated list of supported ICs below to find the best match for your project.

| Platform Name         | Chipset(s)                        |
|-----------------------|-----------------------------------|
| AmebaZ2 / AmebaZ2plus | RTL8720CM                         |
| AmebaD                | RTL8721DM                         |
| AmebaDplus            | RTL8721Dx                         |
| AmebaLite             | RTL8720E / RTL8726E / RTL8713E    |
| AmebaSmart            | RTL8730E                          |

> ⚠️ **Notice**: This `ameba-rtos/release/v1.5` branch supports **AmebaDplus**, **AmebaLite**, and **AmebaSmart**.
>
> For details, examples, and support targeting **AmebaZ2**, **AmebaZ2plus** and **AmebaD**, please switch to the appropriate release branches:
>
> - [release/v1.5](https://github.com/Ameba-AIoT/ameba-rtos-matter/tree/release/v1.5)
> - [release/v1.4.2](https://github.com/Ameba-AIoT/ameba-rtos-matter/tree/release/v1.4.2)
> - [release/v1.4](https://github.com/Ameba-AIoT/ameba-rtos-matter/tree/release/v1.4)
> - [release/v1.3](https://github.com/Ameba-AIoT/ameba-rtos-matter/tree/release/v1.3)

Checkout the [Application Notes](docs/AN0204%20Realtek%20Matter%20application%20note.en.pdf) to learn more about Realtek's Matter solution.

## Documentation

1. [General Guide](docs/matter_general_guide.md) - A comprehensive overview of Matter integration and usage.

2. [Building Guide](docs/matter_building_guide.md) - Detailed instructions and guidelines for building Matter-compliant devices using the Ameba Matter solutions.

3. [Supported Device Types](docs/ameba_supported_device_types.md) - Comprehensive list detailing the types of devices compatible with Ameba Matter.

4. [AT Command Guide](docs/matter_device_at_commands.md) - AT commands for Matter.

5. [Commissioning and Control](docs/matter_commissioning_and_control_guide.md) - Focuses on commissioning procedures and control mechanisms within the Ameba Matter ecosystem.

6. [Matter Certification Guide](docs/matter_certification_guide.md) - Guide for Matter Certification.

7. [Factory Data Guide](tools/factorydata/README.md) - Information about Matter factory data.

8. [OTA Guide](tools/ota/README.md) - Guide for Over-The-Air (OTA) updates in Matter implementations.

9. [Tools Guide](docs/matter_tools_guide.md) - Overview of tools available for Matter development.

## Release History

**Matter v1.5** is associated with the following SDKs:
- AmebaRTOS SDK: ameba-rtos_v1.1 + integrated patch + matter_v1.5 patch, please contact our FAE to get the SDKs.

| Matter Version |    ameba-rtos version   | ameba-rtos-matter version | connectedhomeip version |
|----------------|-------------------------|---------------------------|-------------------------|
| Matter v1.5    | v1.1 + integrated patch | ameba-rtos/release/v1.5   | v1.5-branch             |

**Matter v1.4 and v1.3** is associated with the following SDKs:
- AmebaRTOS SDK: [ameba-rtos:release/v1.0+matter](https://github.com/Ameba-AIoT/ameba-rtos/tree/release/v1.0%2Bmatter)

| Matter Version | ameba-rtos branch   | ameba-rtos-matter version | connectedhomeip version |
|----------------|---------------------|---------------------------|-------------------------|
| Matter v1.4    | release/v1.0+matter | ameba-rtos/release/v1.4   | v1.4-branch             |
| Matter v1.3    | release/v1.0+matter | ameba-rtos/release/v1.3   | (SHA) 70d9a614          |

## Directory Structure Overview

The directory structure is organized to facilitate development, documentation, and tooling for Matter integration on embedded systems.

| Directory    | Description                                                                                                    |
|--------------|----------------------------------------------------------------------------------------------------------------|
|  api         | Essential functions for interfacing with Matter, including device status checks and information retrieval such as vendor ID and product ID. |
|  docs        | Documentation files, including guides, references, and API documentation for Matter integration with the Ameba SDK. |
|  drivers     | Device-specific drivers enabling smooth operation of Matter devices with the Ameba SDK, tailored to each device type's requirements. |
|  common      | Common code for Matter functionalities in the Ameba SDK, covering Bluetooth, Wi-Fi, time management, and data storage. Includes configuration headers for easy project integration. |
|  core        | Fundamental code for powering Matter in the Ameba SDK's Porting Layer, providing the foundation for seamless integration. |
|  examples    | Code snippets and projects demonstrating Matter device usage in the Ameba SDK, organized into subfolders for various device types. |
|  project     | Makefiles and compilation scripts for Matter libraries within the Ameba SDK, categorized by different chip builds for various device types. |
|  tools       | Tools and utilities for development, testing, and deployment in the context of Matter integration with the Ameba SDK. |