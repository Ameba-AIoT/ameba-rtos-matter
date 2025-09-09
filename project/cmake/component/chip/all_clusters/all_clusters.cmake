ameba_list_append(private_includes

    # ${GLOBAL_INTERFACE_INCLUDES} #not needed

    ${CHIP_DIR}/examples/platform/ameba/observer
    ${CHIP_DIR}/examples/all-clusters-app/ameba/main/include
    ${MATTER_DIR}/examples/chiptest

)

ameba_list_append(private_sources

    # all-clusters-app clusters source files
    # actions cluster
    ${MATTER_DRIVER_DIR}/actions/ameba_actions_delegate.cpp
    ${MATTER_DRIVER_DIR}/actions/ameba_actions_server.cpp

    # air quality cluster
    ${MATTER_DRIVER_DIR}/air_quality/ameba_air_quality_instance.cpp

    # concentration measurement cluster
    ${MATTER_DRIVER_DIR}/concentration_measurement/ameba_concentration_measurement_instance.cpp

    # device energy management cluster
    ${MATTER_DRIVER_DIR}/device_energy_management/ameba_device_energy_management_delegate_impl.cpp
    ${MATTER_DRIVER_DIR}/device_energy_management/ameba_device_energy_management_manager.cpp
    ${MATTER_DRIVER_DIR}/device_energy_management/ameba_device_energy_management_mode.cpp
    ${MATTER_DRIVER_DIR}/device_energy_management/ameba_energy_management_common_main.cpp
    ${MATTER_DRIVER_DIR}/device_energy_management/ameba_device_management_test_event_trigger.cpp
    ${MATTER_DRIVER_DIR}/device_energy_management/ameba_energy_time_utils.cpp

    # dishwasher alarm cluster
    ${MATTER_DRIVER_DIR}/dishwasher_alarm/ameba_dishwasher_alarm_delegate.cpp
    ${MATTER_DRIVER_DIR}/dishwasher_alarm/ameba_dishwasher_alarm_instance.cpp

    # dishwasher mode cluster
    ${MATTER_DRIVER_DIR}/dishwasher_mode/ameba_dishwasher_mode_delegate.cpp
    ${MATTER_DRIVER_DIR}/dishwasher_mode/ameba_dishwasher_mode_instance.cpp

    # eletrical power measurement cluster
    ${MATTER_DRIVER_DIR}/electrical_power_measurement/ameba_electrical_power_measurement_delegate.cpp

    # energy evse cluster
    ${MATTER_DRIVER_DIR}/energy_evse/ameba_charging_targets_mem_manager.cpp
    ${MATTER_DRIVER_DIR}/energy_evse/ameba_fake_readings.cpp
    ${MATTER_DRIVER_DIR}/energy_evse/ameba_energy_evse_delegate_impl.cpp
    ${MATTER_DRIVER_DIR}/energy_evse/ameba_energy_evse_main.cpp
    ${MATTER_DRIVER_DIR}/energy_evse/ameba_energy_evse_manager.cpp
    ${MATTER_DRIVER_DIR}/energy_evse/ameba_energy_evse_manufacturer_impl.cpp
    ${MATTER_DRIVER_DIR}/energy_evse/ameba_energy_evse_mode.cpp
    ${MATTER_DRIVER_DIR}/energy_evse/ameba_energy_evse_stub.cpp
    ${MATTER_DRIVER_DIR}/energy_evse/ameba_energy_evse_targets_store.cpp
    ${MATTER_DRIVER_DIR}/energy_evse/ameba_energy_evse_test_event_trigger.cpp
    ${MATTER_DRIVER_DIR}/energy_evse/ameba_energy_preference_delegate.cpp

    # fan control cluster
    ${MATTER_DRIVER_DIR}/fan_control/ameba_fan_control_delegate.cpp
    ${MATTER_DRIVER_DIR}/fan_control/ameba_fan_control_manager.cpp

    # laundry dryer control cluster
    ${MATTER_DRIVER_DIR}/laundry_dryer_controls/ameba_laundry_dryer_controls_delegate.cpp

    # laundry washer control cluster
    ${MATTER_DRIVER_DIR}/laundry_washer_controls/ameba_laundry_washer_controls_delegate.cpp

    # laundry washer mode cluster
    ${MATTER_DRIVER_DIR}/laundry_washer_mode/ameba_laundry_washer_mode_delegate.cpp
    ${MATTER_DRIVER_DIR}/laundry_washer_mode/ameba_laundry_washer_mode_instance.cpp

    # microwave oven controls cluster
    ${MATTER_DRIVER_DIR}/microwave_oven_control/ameba_microwave_oven_control_delegate.cpp
    ${MATTER_DRIVER_DIR}/microwave_oven_control/ameba_microwave_oven_control_instance.cpp

    # microwave oven mode cluster
    ${MATTER_DRIVER_DIR}/microwave_oven_mode/ameba_microwave_oven_mode_delegate.cpp
    ${MATTER_DRIVER_DIR}/microwave_oven_mode/ameba_microwave_oven_mode_instance.cpp

    # mode select cluster
    ${MATTER_DRIVER_DIR}/mode_select/ameba_mode_select_manager.cpp

    # occupancy sensing cluster
    ${MATTER_DRIVER_DIR}/occupancy_sensing/ameba_occupancy_sensing_instance.cpp

    # operational state cluster
    ${MATTER_DRIVER_DIR}/operational_state/ameba_operational_state_delegate.cpp
    ${MATTER_DRIVER_DIR}/operational_state/ameba_operational_state_instance.cpp

    # oven mode cluster
    ${MATTER_DRIVER_DIR}/oven_mode/ameba_oven_mode_delegate.cpp
    ${MATTER_DRIVER_DIR}/oven_mode/ameba_oven_mode_instance.cpp

    # oven operational state cluster
    ${MATTER_DRIVER_DIR}/oven_cavity_operational_state/ameba_oven_cavity_operational_state_delegate.cpp
    ${MATTER_DRIVER_DIR}/oven_cavity_operational_state/ameba_oven_cavity_operational_state_instance.cpp

    # power topology cluster
    ${MATTER_DRIVER_DIR}/power_topology/ameba_power_topology_delegate.cpp

    # resource monitoring cluster
    ${MATTER_DRIVER_DIR}/resource_monitoring/ameba_resource_monitoring_delegate.cpp
    ${MATTER_DRIVER_DIR}/resource_monitoring/ameba_resource_monitoring_instance.cpp

    # refrigerator mode cluster
    ${MATTER_DRIVER_DIR}/refrigerator_mode/ameba_refrigerator_mode_delegate.cpp
    ${MATTER_DRIVER_DIR}/refrigerator_mode/ameba_refrigerator_mode_instance.cpp

    # rvc clean mode cluster
    ${MATTER_DRIVER_DIR}/rvc_clean_mode/ameba_rvc_clean_mode_delegate.cpp
    ${MATTER_DRIVER_DIR}/rvc_clean_mode/ameba_rvc_clean_mode_instance.cpp

    # rvc run mode cluster
    ${MATTER_DRIVER_DIR}/rvc_run_mode/ameba_rvc_run_mode_delegate.cpp
    ${MATTER_DRIVER_DIR}/rvc_run_mode/ameba_rvc_run_mode_instance.cpp

    # rvc operational state cluster
    ${MATTER_DRIVER_DIR}/rvc_operational_state/ameba_rvc_operational_state_delegate.cpp
    ${MATTER_DRIVER_DIR}/rvc_operational_state/ameba_rvc_operational_state_instance.cpp

    # smoke co alarm cluster
    ${MATTER_DRIVER_DIR}/smoke_co_alarm/ameba_smoke_co_alarm_test_event.cpp
    ${MATTER_DRIVER_DIR}/smoke_co_alarm/ameba_smoke_co_alarm_manager.cpp

    # soil measurement cluster
    ${MATTER_DRIVER_DIR}/soil_measurement/ameba_soil_measurement_instance.cpp

    # switch cluster
    ${MATTER_DRIVER_DIR}/switch/ameba_switch_event.cpp

    # temperature controls cluster
    ${MATTER_DRIVER_DIR}/temperature_control/ameba_temperature_control_delegate.cpp

    # valve control cluster
    ${MATTER_DRIVER_DIR}/valve_control/ameba_valve_control_delegate.cpp

    # water heater management cluster
    ${MATTER_DRIVER_DIR}/water_heater/ameba_water_heater_management_delegate.cpp
    ${MATTER_DRIVER_DIR}/water_heater/ameba_water_heater_management_instance.cpp
    ${MATTER_DRIVER_DIR}/water_heater/ameba_water_heater_management_main.cpp
    ${MATTER_DRIVER_DIR}/water_heater/ameba_water_heater_management_manufacturer.cpp

    # water heater mode cluster
    ${MATTER_DRIVER_DIR}/water_heater/ameba_water_heater_mode_delegate.cpp
    ${MATTER_DRIVER_DIR}/water_heater/ameba_water_heater_mode_instance.cpp

    ${MATTER_DIR}/drivers/matter_consoles/matter_command.cpp
    ${MATTER_DIR}/core/matter_attribute_callbacks.cpp
    ${MATTER_DIR}/examples/chiptest/ameba_main_task.cpp

    ${CHIP_DIR}/examples/all-clusters-app/ameba/main/chipinterface.cpp
    ${CHIP_DIR}/examples/all-clusters-app/ameba/main/BindingHandler.cpp
    ${CHIP_DIR}/examples/all-clusters-app/ameba/main/DeviceCallbacks.cpp
    ${CHIP_DIR}/examples/all-clusters-app/ameba/main/SmokeCOAlarmManager.cpp
    ${CHIP_DIR}/examples/all-clusters-app/ameba/main/CHIPDeviceManager.cpp
    ${CHIP_DIR}/examples/all-clusters-app/ameba/main/Globals.cpp
    ${CHIP_DIR}/examples/all-clusters-app/ameba/main/LEDWidget.cpp

)
