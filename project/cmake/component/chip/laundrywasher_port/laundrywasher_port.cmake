ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_DIR}/examples/laundrywasher

)

ameba_list_append(private_sources

	# porting layer source files
	${MATTER_DIR}/core/matter_core.cpp
	${MATTER_DIR}/core/matter_interaction.cpp

	# laundrywasher_port source files
	${MATTER_DIR}/drivers/device/washer_driver.cpp
	${MATTER_DRIVER_DIR}/laundry_washer_controls/ameba_laundry_washer_controls_delegate.cpp
	${MATTER_DRIVER_DIR}/laundry_washer_mode/ameba_laundry_washer_mode_delegate.cpp
	${MATTER_DRIVER_DIR}/laundry_washer_mode/ameba_laundry_washer_mode_instance.cpp
	${MATTER_DRIVER_DIR}/operational_state/ameba_operational_state_delegate.cpp
	${MATTER_DRIVER_DIR}/operational_state/ameba_operational_state_instance.cpp
	${MATTER_EXAMPLE_DIR}/laundrywasher/example_matter_laundrywasher.cpp
	${MATTER_EXAMPLE_DIR}/laundrywasher/matter_drivers.cpp

)
