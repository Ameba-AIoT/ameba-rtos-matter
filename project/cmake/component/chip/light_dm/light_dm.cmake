ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/light_dm

	${CHIP_DIR}/examples/lighting-app/lighting-common
	${CHIP_DIR}/examples/lighting-app/ameba/main/include

)

ameba_list_append(private_sources

	# light_dm source files
	${MATTER_DRIVER_DIR}/device/led_driver.cpp
	${MATTER_EXAMPLE_DIR}/light_dm/example_matter_light.cpp
	${MATTER_EXAMPLE_DIR}/light_dm/matter_drivers.cpp

)
