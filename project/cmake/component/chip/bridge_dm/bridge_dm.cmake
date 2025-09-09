ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_DIR}/examples/bridge_dm

)

ameba_list_append(private_sources

	# porting layer source files
	${MATTER_DIR}/core/matter_core.cpp
	${MATTER_DIR}/core/matter_interaction.cpp

	# porting layer dynamic endpoint src files
	${MATTER_DIR}/core/matter_data_model.cpp
	${MATTER_DIR}/core/matter_data_model_presets.cpp

	# bridge_app_port_dm source files
	${MATTER_DIR}/drivers/device/bridge_dm_driver.cpp
	${MATTER_DIR}/examples/bridge_dm/example_matter_bridge.cpp
	${MATTER_DIR}/examples/bridge_dm/matter_drivers.cpp

)
