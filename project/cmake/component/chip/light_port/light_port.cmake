ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/light

	${CHIP_DIR}/examples/lighting-app/lighting-common
	${CHIP_DIR}/examples/lighting-app/ameba/main/include
)

ameba_list_append(private_sources

	# porting layer source files
	${MATTER_DIR}/core/matter_core.cpp
	${MATTER_DIR}/core/matter_interaction.cpp

	# light_port source files
	${MATTER_DIR}/drivers/device/led_driver.cpp
	${MATTER_EXAMPLE_DIR}/light/example_matter_light.cpp
	${MATTER_EXAMPLE_DIR}/light/matter_drivers.cpp

)
