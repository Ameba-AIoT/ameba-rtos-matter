ameba_list_append(private_includes

    # ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/water_heater
)

ameba_list_append(private_sources

    # device energy management cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_energy_management_common_main.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_device_energy_management_delegate_impl.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_device_energy_management_manager.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_device_energy_management_mode.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_device_management_test_event_trigger.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_energy_time_utils.cpp

    # electrical energy measurement cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/electrical_energy_measurement/ameba_electrical_energy_measurement_test_event_trigger.cpp #test event trigger

    # electrical power measurement cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/electrical_power_measurement/ameba_electrical_power_measurement_delegate.cpp

    # power topology cluster (common)
    ${MATTER_DRIVER_DIR}/matter_drivers/power_topology/ameba_power_topology_delegate.cpp

    # water heater management cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_management_delegate.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_management_instance.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_management_main.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_management_manufacturer.cpp

    # water heater mode cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_mode_delegate.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_mode_instance.cpp

    # water heater src
    ${MATTER_EXAMPLE_DIR}/water_heater/example_matter_water_heater.cpp
    ${MATTER_EXAMPLE_DIR}/water_heater/matter_drivers.cpp
)
