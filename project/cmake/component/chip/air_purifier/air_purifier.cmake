ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${CHIP_DIR}/examples/platform/ameba/observer
	${CHIP_DIR}/examples/air-purifier-app/air-purifier-common
	${CHIP_DIR}/examples/air-purifier-app/air-purifier-common/include
	${CHIP_DIR}/examples/air-purifier-app/ameba/main/include

)

ameba_list_append(private_sources

	# air_purifier_app source files
	${CHIP_DIR}/examples/air-purifier-app/ameba/main/chipinterface.cpp
	${CHIP_DIR}/examples/air-purifier-app/ameba/main/DeviceCallbacks.cpp
	${CHIP_DIR}/examples/air-purifier-app/ameba/main/CHIPDeviceManager.cpp
	${CHIP_DIR}/examples/air-purifier-app/air-purifier-common/src/air-purifier-manager.cpp
	${CHIP_DIR}/examples/air-purifier-app/air-purifier-common/src/air-quality-sensor-manager.cpp
	${CHIP_DIR}/examples/air-purifier-app/air-purifier-common/src/filter-delegates.cpp
	${CHIP_DIR}/examples/air-purifier-app/air-purifier-common/src/thermostat-manager.cpp

)
