ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/fan

)

ameba_list_append(private_sources

	# fan_port source files
	${MATTER_DRIVER_DIR}/device/fan_driver.cpp
	${MATTER_EXAMPLE_DIR}/fan/example_matter_fan.cpp
	${MATTER_EXAMPLE_DIR}/fan/matter_drivers.cpp

)
