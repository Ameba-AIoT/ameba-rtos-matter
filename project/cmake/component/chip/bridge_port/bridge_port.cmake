ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_DIR}/examples/bridge

	${CHIP_DIR}/examples/all-clusters-app/all-clusters-common
	${CHIP_DIR}/examples/bridge-app/bridge-common
	${CHIP_DIR}/examples/bridge-app/ameba/main/include

)

ameba_list_append(private_sources

	# porting layer source files
	${MATTER_DIR}/core/matter_core.cpp
	${MATTER_DIR}/core/matter_interaction.cpp

	# bridge_port source files
	${MATTER_DIR}/drivers/device/bridge_driver.cpp
	${MATTER_DRIVER_DIR}/actions/ameba_actions_delegate.cpp
	${MATTER_DRIVER_DIR}/actions/ameba_actions_server.cpp
	${MATTER_DIR}/examples/bridge/example_matter_bridge.cpp
	${MATTER_DIR}/examples/bridge/matter_drivers.cpp

)
