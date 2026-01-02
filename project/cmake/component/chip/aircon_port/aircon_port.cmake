ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/aircon

)

ameba_list_append(private_sources

	# aircon_port source files
	${MATTER_DRIVER_DIR}/device/room_aircon_driver.cpp
	${MATTER_DRIVER_DIR}/device/temp_hum_sensor_driver.cpp
	${MATTER_EXAMPLE_DIR}/aircon/example_matter_aircon.cpp
	${MATTER_EXAMPLE_DIR}/aircon/matter_drivers.cpp

)
