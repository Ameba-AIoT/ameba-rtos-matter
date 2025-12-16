ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_DIR}/examples/dishwasher

)

ameba_list_append(private_sources

	# porting layer source files
	${MATTER_DIR}/core/matter_core.cpp
	${MATTER_DIR}/core/matter_interaction.cpp

	# dishwasher_port source files
	${MATTER_DIR}/drivers/device/dishwasher_driver.cpp
	${MATTER_DRIVER_DIR}/dishwasher_mode/ameba_dishwasher_mode_delegate.cpp
	${MATTER_DRIVER_DIR}/dishwasher_mode/ameba_dishwasher_mode_instance.cpp
	${MATTER_DRIVER_DIR}/operational_state/ameba_operational_state_delegate.cpp
	${MATTER_DRIVER_DIR}/operational_state/ameba_operational_state_instance.cpp
	${MATTER_EXAMPLE_DIR}/dishwasher/example_matter_dishwasher.cpp
	${MATTER_EXAMPLE_DIR}/dishwasher/matter_drivers.cpp

)
