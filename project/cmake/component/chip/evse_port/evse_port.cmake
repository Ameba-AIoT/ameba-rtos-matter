ameba_list_append(private_includes

    # ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/evse

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

    # evse cluster
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_charging_targets_mem_manager.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_fake_readings.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_delegate_impl.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_main.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_manager.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_manufacturer_impl.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_mode.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_targets_store.cpp
    ${MATTER_DRIVER_DIR}/matter_drivers/energy_evse/ameba_energy_evse_test_event_trigger.cpp

    # evse src
    ${MATTER_EXAMPLE_DIR}/evse/example_matter_evse.cpp
    ${MATTER_EXAMPLE_DIR}/evse/matter_drivers.cpp
)
