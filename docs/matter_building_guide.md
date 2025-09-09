# Building Guide for Ameba Matter Solution

This general guide provides an overview of the Matter integration process and serves as a foundational resource for understanding how to build Matter on various ICs supported by Ameba.

- [Building Guides for Specific ICs](#building-guides-for-specific-ics)
- [Building Different Applications](#building-different-applications)

## Building Guides for Specific ICs

This general guide provides an overview of the Matter integration process and serves as a foundational resource for understanding how to build Matter on various ICs supported by Ameba.

- [AmebaDplus Building Guide](amebadplus_general_build.md) - Step-by-step instructions for building Matter on AmebaDplus.

- [AmebaLite Building Guide](amebalite_general_build.md) - Step-by-step instructions for building Matter on AmebaLite.

- [AmebaSmart Building Guide](amebasmart_general_build.md) - Step-by-step instructions for building Matter on AmebaSmart.

## Building Different Applications

Each device type has its standalone example, and here is how you can build the application.

- [Matter Application](#matter-application)
- [Matter Application Types](#matter-application-types)

### Matter Application

The following `cmake` commands correspond to different device types applicable in Realtek's SDK

```bash
python build.py -D MATTER_EXAMPLE=all_clusters / MATTER_EXAMPLE=air_purifier / MATTER_EXAMPLE=aircon_port
                   MATTER_EXAMPLE=bridge_dm / MATTER_EXAMPLE=bridge_port / MATTER_EXAMPLE=dishwasher_port
                   MATTER_EXAMPLE=fan_port / MATTER_EXAMPLE=laundrywasher_port / MATTER_EXAMPLE=light_dm
                   MATTER_EXAMPLE=light_port / MATTER_EXAMPLE=microwaveoven_port / MATTER_EXAMPLE=refrigerator_port
                   MATTER_EXAMPLE=thermostat_port
```

### Matter Application Types
There are two different types of Matter applications:

#### Connectedhomeip Repository Applications:
- Sourced from the connectedhomeip repository.
- Includes: `all_clusters`, `air_purifier`, `light`

#### Realtek's Porting Layer Applications:
- Utilizes Realtek's Porting Layer for specific device integrations.
- Includes: `aircon_port`, `bridge_dm`, `dishwasher_port`, `laundrywasher_port`, `light_port`, `light_dm`, `microwaveoven_port`, `refrigerator_port`, `thermostat_port`.

Learn more about Realtek's Porting Layer Application [here](matter_porting_layer_guide.md).
