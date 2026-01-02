ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/bridge

	${CHIP_DIR}/examples/all-clusters-app/all-clusters-common
	${CHIP_DIR}/examples/bridge-app/bridge-common
	${CHIP_DIR}/examples/bridge-app/ameba/main/include

)

ameba_list_append(private_sources

	# bridge_port source files
	${MATTER_DRIVER_DIR}/device/bridge_driver.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/actions/ameba_actions_delegate.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/actions/ameba_actions_server.cpp
	${MATTER_EXAMPLE_DIR}/bridge/example_matter_bridge.cpp
	${MATTER_EXAMPLE_DIR}/bridge/matter_drivers.cpp

)
