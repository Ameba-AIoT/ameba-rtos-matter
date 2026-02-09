ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/doorlock

)

ameba_list_append(private_sources

	# doorlock_port source files
	${MATTER_DRIVER_DIR}/device/doorlock_driver.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/door_lock/ameba_lock_endpoint.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/door_lock/ameba_lock_manager.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/door_lock/ameba_zcl_door_lock_callbacks.cpp
	${MATTER_EXAMPLE_DIR}/doorlock/example_matter_doorlock.cpp
	${MATTER_EXAMPLE_DIR}/doorlock/matter_drivers.cpp

)
