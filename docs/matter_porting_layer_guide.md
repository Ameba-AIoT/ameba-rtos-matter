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

 - [Room Air-Conditioner](../examples/aircon/README.md) - Room air-conditioner device type implementation
 - [Bridge App](../examples/bridge/README.md) - build a bridge application that facilitates communication between Matter fabric and Non-Matter device.
 - [Bridge App with Dynamic Endpoint](../examples/bridge_dm/README.md) - this example extends the basic bridge functionality by incorporating dynamic endpoint management, enhancing flexibility in device connectivity
 - [Dish Washer](../examples/dishwasher/README.md) - Dish Washer device type implementation.
 - [Energy Management](../examples/energy_management/README.md) - Energy management device type implementation.
 - [Fan](../examples/fan/README.md) - Fan device type implementation
 - [Laundry Washer](../examples/laundrywasher/README.md) - Dishwasher device type implementation
 - [Dimmable Light](../examples/light/README.md) - Dimmable light device type implementation but can be customized to different lighting device.
 - [Dimmable Light with Dynamic Endpoint](../examples/light_dm/README.md) - this example extends the basic lighting functionality by incorporating dynamic endpoint management, enhancing flexibility in device connectivity
 - [Microwave Oven](../examples/microwaveoven/README.md) - Microwave oven device type implementation
 - [Refrigerator](../examples/refrigerator/README.md) - Refrigerator device type implementation
 - [Thermostat](../examples/thermostat/README.md) - Thermostat device type implementation

### Porting Layer Example Memory Usage

The following table provides approximate memory usage for the Realtek Porting Layer example.
Example: light_port

#### AmebaDplus Memory Usage
##### Ameba RTOS v1.1 (CMake build)
Image size (`km4_boot_all.bin` + `km0_km4_app.bin`): 1,858 KB

| Memory Region  | Size (Bytes) | Size (KB) | Notes                                           |
|----------------|--------------|-----------|-------------------------------------------------|
| RAM            | 129,092      | 126.1     | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 134,400      | 131.3     | Dynamic allocation area                         |
| **Total**      | **263,492**  | **257.4** | Approximate total memory usage                  |

##### Ameba RTOS v1.0 (Makefile build)
Image size (`km4_boot_all.bin` + `km0_km4_app.bin`): 1,608 KB

| Memory Region  | Size (Bytes) | Size (KB) | Notes                                           |
|----------------|--------------|-----------|-------------------------------------------------|
| RAM            | 188,540      | 184.1     | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 127,776      | 124.8     | Dynamic allocation area                         |
| **Total**      | **316,316**  | **308.9** | Approximate total memory usage                  |

#### AmebaLite Memory Usage
##### Ameba RTOS v1.1 (CMake build)
Image size (`km4_boot_all.bin` + `kr4_km4_app.bin`): 1,725 KB

| Memory Region  | Size (Bytes) | Size (KB) | Notes                                           |
|----------------|--------------|-----------|-------------------------------------------------|
| RAM            | 135,440      | 132.3     | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 140,960      | 137.6     | Dynamic allocation area                         |
| **Total**      | **276,400**  | **269.9** | Approximate total memory usage                  |

##### Ameba RTOS v1.0 (Makefile build)
Image size (`km4_boot_all.bin` + `kr4_km4_app.bin`): 1,683 KB

| Memory Region  | Size (Bytes) | Size (KB) | Notes                                           |
|----------------|--------------|-----------|-------------------------------------------------|
| RAM            | 203,140      | 198.4     | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 136,416      | 133.2     | Dynamic allocation area                         |
| **Total**      | **339,556**  | **331.6** | Approximate total memory usage                  |

#### AmebaSmart Memory Usage
##### Ameba RTOS v1.1 (CMake build)
Image size (`km4_boot_all.bin` + `km0_km4_ca32_app.bin`): 3,080 KB

| Memory Region  | Size (Bytes) | Size (KB) | Notes                                           |
|----------------|--------------|-----------|-------------------------------------------------|
| RAM            | 152,972      | 149.4     | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 174,272      | 170.2     | Dynamic allocation area                         |
| **Total**      | **327,244**  | **319.6** | Approximate total memory usage                  |

##### Ameba RTOS v1.0 (Makefile build)
Image size (`km4_boot_all.bin` + `km0_km4_ca32_app.bin`): 2,924 KB

| Memory Region  | Size (Bytes) | Size (KB) | Notes                                           |
|----------------|--------------|-----------|-------------------------------------------------|
| RAM            | 153,996      | 150.4     | Internal SRAM usage (static data, stacks, etc.) |
| Heap           | 161,344      | 157.6     | Dynamic allocation area                         |
| **Total**      | **315,340**  | **308.0** | Approximate total memory usage                  |
