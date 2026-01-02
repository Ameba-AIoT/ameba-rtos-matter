ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/dishwasher

)

ameba_list_append(private_sources

	# dishwasher_port source files
	${MATTER_DRIVER_DIR}/device/dishwasher_driver.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/dishwasher_mode/ameba_dishwasher_mode_delegate.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/dishwasher_mode/ameba_dishwasher_mode_instance.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/operational_state/ameba_operational_state_delegate.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/operational_state/ameba_operational_state_instance.cpp
	${MATTER_EXAMPLE_DIR}/dishwasher/example_matter_dishwasher.cpp
	${MATTER_EXAMPLE_DIR}/dishwasher/matter_drivers.cpp

)
