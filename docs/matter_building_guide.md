# Building Guide for Ameba Matter Solution

This general guide provides an overview of the Matter integration process and serves as a foundational resource for understanding how to build Matter on various ICs supported by Ameba.

- [Building Guides for Specific ICs](#building-guides-for-specific-ics)
- [Building Different Applications](#building-different-applications)

## Building Guides for Specific ICs

This general guide provides an overview of the Matter integration process and serves as a foundational resource for understanding how to build Matter on various ICs supported by Ameba.

- [AmebaZ2 Building Guide](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/main/docs/amebaz2_general_build.md) - Step-by-step instructions for building Matter on AmebaZ2.

- [AmebaD Building Guide](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/main/docs/amebad_general_build.md) - Step-by-step instructions for building Matter on AmebaD.

- [AmebaDplus Building Guide](amebadplus_general_build.md) - Step-by-step instructions for building Matter on AmebaDplus.

- [AmebaLite Building Guide](amebalite_general_build.md) - Step-by-step instructions for building Matter on AmebaLite.

- [AmebaSmart Building Guide](amebasmart_general_build.md) - Step-by-step instructions for building Matter on AmebaSmart.

**Notes**: For AmebaZ2plus, please refer to the AmebaZ2 Building Guide.

## Building Different Applications

Each device type has its standalone example, and here is how you can build the application.

- [Matter Application](#matter-application)
- [Matter Application Types](#matter-application-types)

### Matter Application

The following `make` commands correspond to different device types applicable in Realtek's SDK

#### AmebaZ2 (RTL8710C) and AmebaZ2Plus Commands:

```bash
make all_clusters / air_purifier / aircon_port / bridge_dm  / dishwasher_port / fan_port /
     generic_switch_port / laundrywasher_port / light / light_dm / light_port / light_switch /
     microwaveoven_port / refrigerator_port / temp_sensor_port / thermostat_port
```

#### AmebaD (RTL8721D) Commands:

```
make -C asdk all_clusters / air_purifier / aircon_port / bridge_dm / bridge_port / dishwasher_port / fan_port /
     generic_switch_port / laundrywasher_port / light / light_dm / light_port / light_switch /
     microwaveoven_port / refrigerator_port / temp_sensor_port / thermostat_port
```

#### AmebaDplus (RTL8721Dx) Commands:

```
make -C asdk all_clusters / air_purifier / aircon_port / bridge_dm / bridge_port / dishwasher_port / fan_port
             laundrywasher_port / light_dm / light_port / microwaveoven_port / refrigerator_port / thermostat_port
```

#### AmebaLite (RTL8720EA / RTL8726EA) Commands:

```
make -C asdk all_clusters / air_purifier / aircon_port / bridge_dm / bridge_port / dishwasher_port / fan_port
             laundrywasher_port / light_dm / light_port / microwaveoven_port / refrigerator_port / thermostat_port
```

#### AmebaSmart (RTL8730E) Commands:

```
make -C asdk all_clusters / air_purifier / aircon_port / bridge_dm / bridge_port / dishwasher_port / fan_port
             laundrywasher_port / light_dm / light_port / microwaveoven_port / refrigerator_port / thermostat_port
```

### Matter Application Types

There are two different types of Matter applications:

#### Connectedhomeip Repository Applications:
- Sourced from the connectedhomeip repository.
- Includes: `all_clusters`, `air_purifier`, `light`, and `light_switch`.

#### Realtek's Porting Layer Applications:
- Utilizes Realtek's Porting Layer for specific device integrations.
- Includes: `aircon_port`, `bridge_dm`, `dishwasher_port`, `laundrywasher_port`, `light_port`, `light_dm`, `microwaveoven_port`, `refrigerator_port`, `thermostat_port`.

Learn more about Realtek's Porting Layer Application [here](https://github.com/Ameba-AIoT/ameba-rtos-matter/blob/main/docs/matter_porting_layer_guide.md).
