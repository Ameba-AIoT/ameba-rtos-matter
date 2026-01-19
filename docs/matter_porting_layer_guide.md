# Matter Application with Realtek Porting Layer

The porting layer facilitates the development of custom Matter applications by providing core Matter APIs and supporting the integration of peripheral drivers.

### Directory Structure

| Directory | Description |
| --------- | ----------- |
| common    | Contains common files and utilities essential for all Matter applications. |
| core      | Provides core Matter APIs for seamless integration into various hardware platforms. |
| driver    | Houses peripheral drivers utilized by Matter application callbacks. |
| example   | Includes Matter application examples categorized by device types for reference and customization. |

### Designing Custom Matter Applications with the Porting Layer

To create your own custom Matter application using the porting layer:

1. **Common and Core Modules**: No modifications are required in the `common` or `core` directories as they provide foundational APIs and utilities.

2. **Peripheral Drivers**: Add your peripheral driver code in the `driver` directory. Refer to existing driver files for guidance.

3. **Creating or Modifying Examples**:
   - Use the `example` directory to create a new custom example or modify an existing one.
   - Your custom example should include at least the following files:
     - Main task implementation (e.g., `light/example_matter_light.cpp`).
     - Driver interface implementation (e.g., `light/matter_drivers.cpp`).
     - Makefiles necessary to build the Matter libraries.

4. **Readme and Documentation**: Refer to the examples' readme files for detailed instructions on integrating and customizing your Matter application.

This structure and approach simplify the process of building and extending Matter-compliant applications on Ameba, ensuring flexibility and ease of integration with peripheral hardware.

### Porting Layer Example Documentations

 - [Room Air-Conditioner](../example/aircon/README.md) - Room air-conditioner device type implementation
 - [Bridge App](../example/bridge/README.md) - build a bridge application that facilitates communication between Matter fabric and Non-Matter device.
 - [Bridge App with Dynamic Endpoint](../example/bridge_dm/README.md) - this example extends the basic bridge functionality by incorporating dynamic endpoint management, enhancing flexibility in device connectivity
 - [Dish Washer](../example/dishwasher/README.md) - Dish Washer device type implementation.
 - [Fan](../example/fan/README.md) - Fan device type implementation
 - [Laundry Washer](../example/laundrywasher/README.md) - Dishwasher device type implementation
 - [Dimmable Light](../example/light/README.md) - Dimmable light device type implementation but can be customized to different lighting device.
 - [Dimmable Light with Dynamic Endpoint](../example/light_dm/README.md) - this example extends the basic lighting functionality by incorporating dynamic endpoint management, enhancing flexibility in device connectivity
 - [Microwave Oven](../example/microwaveoven/README.md) - Microwave oven device type implementation
 - [Refrigerator](../example/refrigerator/README.md) - Refrigerator device type implementation
 - [Thermostat](../example/thermostat/README.md) - Thermostat device type implementation

### Porting Layer Example Memory Usage

The following table provides approximate memory usage for the Realtek Porting Layer example.
Example: light_port

#### AmebaDplus Memory Usage
Image size (`km4_boot_all.bin` + `km0_km4_app.bin`): 1,591 KB

| Memory Region  | Size (Bytes) | Size (KB) | Notes                                           |
|----------------|--------------|-----------|-------------------------------------------------|
| RAM            | 188,412      | 184.0     | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 131,232      | 128.2     | Dynamic allocation area                         |
| **Total**      | **319,644**  | **312.2** | Approximate total memory usage                  |

#### AmebaLite Memory Usage
Image size (`km4_boot_all.bin` + `kr4_km4_app.bin`): 1,670 KB

| Memory Region  | Size (Bytes) | Size (KB) | Notes                                           |
|----------------|--------------|-----------|-------------------------------------------------|
| RAM            | 203,060      | 198.3     | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 137,952      | 134.7     | Dynamic allocation area                         |
| **Total**      | **341,012**  | **333.0** | Approximate total memory usage                  |

#### AmebaSmart Memory Usage
Image size (`km4_boot_all.bin` + `km0_km4_ca32_app.bin`): 2,908 KB

| Memory Region  | Size (Bytes) | Size (KB) | Notes                                           |
|----------------|--------------|-----------|-------------------------------------------------|
| RAM            | 153,820      | 150.2     | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 169,024      | 165.1     | Dynamic allocation area                         |
| **Total**      | **322,844**  | **315.3** | Approximate total memory usage                  |
