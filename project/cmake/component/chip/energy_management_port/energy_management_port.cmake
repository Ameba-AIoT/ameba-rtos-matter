ameba_list_append(private_includes

    # ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/energy_management

    ${CHIP_DIR}/examples/energy-management-app/energy-management-common
    ${CHIP_DIR}/examples/energy-management-app/ameba/main/include

)

ameba_list_append(private_sources

    # device energy management cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_device_energy_management_delegate_impl.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_device_energy_management_manager.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_device_energy_management_mode.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_energy_management_common_main.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_device_management_test_event_trigger.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/device_energy_management/ameba_energy_time_utils.cpp

    # electrical energy measurement cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/electrical_energy_measurement/ameba_electrical_energy_measurement_test_event_trigger.cpp #test event trigger

    # electrical power measurement cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/electrical_power_measurement/ameba_electrical_power_measurement_delegate.cpp

    # energy evse cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_charging_targets_mem_manager.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_fake_readings.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_delegate_impl.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_main.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_manager.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_manufacturer_impl.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_mode.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_targets_store.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_test_event_trigger.cpp

    # power topology cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/power_topology/ameba_power_topology_delegate.cpp

    # water heater management cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_management_delegate.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_management_instance.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_management_main.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_management_manufacturer.cpp

    # water heater mode cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_mode_delegate.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/water_heater/ameba_water_heater_mode_instance.cpp

    # energy management src
    ${MATTER_EXAMPLE_DIR}/energy_management/example_matter_energy_management.cpp
    ${MATTER_EXAMPLE_DIR}/energy_management/matter_drivers.cpp

)
