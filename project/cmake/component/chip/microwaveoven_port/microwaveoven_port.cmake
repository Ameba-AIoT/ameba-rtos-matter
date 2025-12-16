ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/microwaveoven

)

ameba_list_append(private_sources

	# porting layer source files
	${MATTER_DIR}/core/matter_core.cpp
	${MATTER_DIR}/core/matter_interaction.cpp

	# microwaveoven_port source files
	${MATTER_DIR}/drivers/device/microwaveoven_driver.cpp
	${MATTER_DRIVER_DIR}/microwave_oven_control/ameba_microwave_oven_control_delegate.cpp
	${MATTER_DRIVER_DIR}/microwave_oven_control/ameba_microwave_oven_control_instance.cpp
	${MATTER_DRIVER_DIR}/microwave_oven_mode/ameba_microwave_oven_mode_delegate.cpp
	${MATTER_DRIVER_DIR}/microwave_oven_mode/ameba_microwave_oven_mode_instance.cpp
	${MATTER_DRIVER_DIR}/operational_state/ameba_operational_state_delegate.cpp
	${MATTER_DRIVER_DIR}/operational_state/ameba_operational_state_instance.cpp
	${MATTER_EXAMPLE_DIR}/microwaveoven/example_matter_microwave_oven.cpp
	${MATTER_EXAMPLE_DIR}/microwaveoven/matter_drivers.cpp

)
