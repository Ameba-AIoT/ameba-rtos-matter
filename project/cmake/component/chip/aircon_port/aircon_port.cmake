ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/aircon

)

ameba_list_append(private_sources

	# porting layer source files
	${MATTER_DIR}/core/matter_core.cpp
	${MATTER_DIR}/core/matter_interaction.cpp

	# aircon_port source files
	${MATTER_DIR}/drivers/device/room_aircon_driver.cpp
	${MATTER_DIR}/drivers/device/temp_hum_sensor_driver.cpp
	${MATTER_EXAMPLE_DIR}/aircon/example_matter_aircon.cpp
	${MATTER_EXAMPLE_DIR}/aircon/matter_drivers.cpp

)
