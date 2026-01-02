ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/refrigerator

)

ameba_list_append(private_sources

	# refrigerator_port source files
	${MATTER_DRIVER_DIR}/device/refrigerator_driver.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/refrigerator_mode/ameba_refrigerator_mode_delegate.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/refrigerator_mode/ameba_refrigerator_mode_instance.cpp
	${MATTER_EXAMPLE_DIR}/refrigerator/example_matter_refrigerator.cpp
	${MATTER_EXAMPLE_DIR}/refrigerator/matter_drivers.cpp

)
