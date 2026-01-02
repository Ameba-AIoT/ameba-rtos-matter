ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/bridge_dm

)

ameba_list_append(private_sources

	# bridge_app_port_dm source files
	${MATTER_DRIVER_DIR}/device/bridge_dm_driver.cpp
	${MATTER_EXAMPLE_DIR}/bridge_dm/example_matter_bridge.cpp
	${MATTER_EXAMPLE_DIR}/bridge_dm/matter_drivers.cpp

)
