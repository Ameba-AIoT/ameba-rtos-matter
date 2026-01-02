ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/laundrywasher

)

ameba_list_append(private_sources

	# laundrywasher_port source files
	${MATTER_DRIVER_DIR}/device/washer_driver.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/laundry_washer_controls/ameba_laundry_washer_controls_delegate.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/laundry_washer_mode/ameba_laundry_washer_mode_delegate.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/laundry_washer_mode/ameba_laundry_washer_mode_instance.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/operational_state/ameba_operational_state_delegate.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/operational_state/ameba_operational_state_instance.cpp
	${MATTER_EXAMPLE_DIR}/laundrywasher/example_matter_laundrywasher.cpp
	${MATTER_EXAMPLE_DIR}/laundrywasher/matter_drivers.cpp

)
