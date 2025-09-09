ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${CHIP_DIR}/examples/platform/ameba/observer
	${CHIP_DIR}/examples/lighting-app/lighting-common
	${CHIP_DIR}/examples/lighting-app/ameba/main/include

)

ameba_list_append(private_sources

	# light source files
	${CHIP_DIR}/examples/lighting-app/ameba/main/chipinterface.cpp
	${CHIP_DIR}/examples/lighting-app/ameba/main/DeviceCallbacks.cpp
	${CHIP_DIR}/examples/lighting-app/ameba/main/CHIPDeviceManager.cpp
	${CHIP_DIR}/examples/lighting-app/ameba/main/Globals.cpp
	${CHIP_DIR}/examples/lighting-app/ameba/main/LEDWidget.cpp

)
