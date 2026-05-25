ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/camera

	${CHIP_DIR}/examples/camera-app/camera-common

	${c_CMPT_USB_DIR}/common
	${c_CMPT_USB_DIR}/host/core
	${c_CMPT_USB_DIR}/host/uvc
)

ameba_list_append(private_sources

	# light_port source files
	${MATTER_DRIVER_DIR}/device/camera_driver.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/camera/ameba_camera.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/camera/ameba_camera_av_stream_manager.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/camera/ameba_camera_device.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/ameba_webrtc_libdatachannel.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/ameba_webrtc_provider_manager.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/ameba_webrtc_transport.cpp
	${MATTER_EXAMPLE_DIR}/camera/example_matter_camera.cpp
	${MATTER_EXAMPLE_DIR}/camera/matter_drivers.cpp

)
