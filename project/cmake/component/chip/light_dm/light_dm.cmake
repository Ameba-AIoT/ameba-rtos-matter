ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/light_dm

	${CHIP_DIR}/examples/lighting-app/lighting-common
	${CHIP_DIR}/examples/lighting-app/ameba/main/include

)

ameba_list_append(private_sources

	# porting layer source files
	${MATTER_DIR}/core/matter_core.cpp
	${MATTER_DIR}/core/matter_interaction.cpp

	# porting layer dynamic endpoint src files
	${MATTER_DIR}/core/matter_data_model.cpp
	${MATTER_DIR}/core/matter_data_model_presets.cpp

	# light_dm source files
	${MATTER_DIR}/drivers/device/led_driver.cpp
	${MATTER_EXAMPLE_DIR}/light_dm/example_matter_light.cpp
	${MATTER_EXAMPLE_DIR}/light_dm/matter_drivers.cpp

)
