ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_DIR}/examples/fan

)

ameba_list_append(private_sources

	# porting layer source files
	${MATTER_DIR}/core/matter_core.cpp
	${MATTER_DIR}/core/matter_interaction.cpp

	# fan_port source files
	${MATTER_DIR}/drivers/device/fan_driver.cpp
	${MATTER_DIR}/examples/fan/example_matter_fan.cpp
	${MATTER_DIR}/examples/fan/matter_drivers.cpp

)
