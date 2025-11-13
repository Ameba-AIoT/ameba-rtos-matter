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

 - [Bridge App](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/bridge/README.md) - build a bridge application that facilitates communication between Matter fabric and Non-Matter device.
 - [Bridge App with Dynamic Endpoint](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/bridge_dm/README.md) - this example extends the basic bridge functionality by incorporating dynamic endpoint management, enhancing flexibility in device connectivity
 - [Dishwasher](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/dishwasher/README.md) - Dishwasher device type implementation
 - [Fan](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/fan/README.md) - Fan device type implementation
 - [Generic Switch](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/generic_switch/README.md) - Generic Switch device type implementation
 - [Laundry Washer](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/laundry_washer/README.md) - Dishwasher device type implementation
 - [Dimmable Light](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/light/README.md) - Dimmable light device type implementation but can be customized to different lighting device.
 - [Dimmable Light with Dynamic Endpoint](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/light_dm/README.md) - this example extends the basic lighting functionality by incorporating dynamic endpoint management, enhancing flexibility in device connectivity
 - [Microwave Oven](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/microwave_oven/README.md) - Microwave oven device type implementation
 - [Refrigerator](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/refrigerator/README.md) - Refrigerator device type implementation
 - [Room Air-Conditioner](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/room_air_conditioner/README.md) - Room air-conditioner device type implementation
 - [Temperature Sensor](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/temperature_sensor/README.md) - Temperature Sensor device type implementation
 - [Thermostat](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/release/v1.4/examples/thermostat/README.md) - Thermostat device type implementation

### Porting Layer Example Memory Usage

The following table provides approximate memory usage for the Realtek Porting Layer example.

Example: light_port

#### AmebaZ2 (CM) Memory Usage
Image size (`flash_is.bin`): 1,262 KB

| Memory Region | Size (Bytes) | Size (KB) | Notes |
|----------------|--------------|-----------|-------|
| RAM | 114,528 | 111.8 | Internal SRAM usage (static data, stacks, etc.) |
| Heap | 171,320 | 167.3 | Dynamic allocation area |
| **Total** | **285,848** | **279.1** | Approximate total memory usage |

---

#### AmebaZ2plus (CM) Memory Usage
Image size (`flash_is.bin`): 1,250 KB

| Memory Region | Size (Bytes) | Size (KB) | Notes |
|----------------|--------------|-----------|-------|
| RAM | 114,528 | 111.8 | Internal SRAM usage (static data, stacks, etc.) |
| Heap | 170,104 | 166.1 | Dynamic allocation area |
| **Total** | **284,632** | **277.9** | Approximate total memory usage |

---

#### AmebaD (DM) Memory Usage
Image size (`km0_km4_image2.bin`): 1,388 KB

| Memory Region | Size (Bytes) | Size (KB) | Notes |
|----------------|--------------|-----------|-------|
| RAM | 173,856 | 169.8 | Internal SRAM usage (static data, stacks, etc.) |
| Heap | 215,008 | 210.0 | Dynamic allocation area |
| **Total** | **388,864** | **379.8** | Approximate total memory usage |