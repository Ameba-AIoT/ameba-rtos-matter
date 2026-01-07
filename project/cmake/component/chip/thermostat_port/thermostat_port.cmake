ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/thermostat

	${CHIP_DIR}/examples/thermostat/thermostat-common
	${CHIP_DIR}/examples/thermostat/ameba/main/include

)

ameba_list_append(private_sources

	# thermostat_port source files
	${MATTER_DRIVER_DIR}/device/thermostat_driver.cpp
	${MATTER_DRIVER_DIR}/device/thermostat_ui_driver.cpp
	${MATTER_EXAMPLE_DIR}/thermostat/example_matter_thermostat.cpp
	${MATTER_EXAMPLE_DIR}/thermostat/matter_drivers.cpp

)
