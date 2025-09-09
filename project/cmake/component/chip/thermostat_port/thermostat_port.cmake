ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/thermostat

	${CHIP_DIR}/examples/thermostat/thermostat-common
	${CHIP_DIR}/examples/thermostat/ameba/main/include

)

ameba_list_append(private_sources

	# porting layer source files
	${MATTER_DIR}/core/matter_core.cpp
	${MATTER_DIR}/core/matter_interaction.cpp

	# thermostat_port source files
	${MATTER_DIR}/drivers/device/thermostat_driver.cpp
	${MATTER_DIR}/drivers/device/thermostat_ui_driver.cpp
	${MATTER_EXAMPLE_DIR}/thermostat/example_matter_thermostat.cpp
	${MATTER_EXAMPLE_DIR}/thermostat/matter_drivers.cpp

)
