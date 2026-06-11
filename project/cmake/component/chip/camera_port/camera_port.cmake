ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_EXAMPLE_DIR}/camera

	${CHIP_DIR}/examples/camera-app/camera-common

	${c_CMPT_USB_DIR}/common
	${c_CMPT_USB_DIR}/host/core
	${c_CMPT_USB_DIR}/host/uvc

)

ameba_list_append(private_sources

	# camera_port source files
	${MATTER_DRIVER_DIR}/device/camera_driver.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/camera/ameba_camera.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/camera/ameba_camera_av_stream_manager.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/camera/ameba_camera_device.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/tls_certificate_management/ameba_tls_certificate_management_instance.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/tls_client_management/ameba_tls_client_management_instance.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/ameba_webrtc_libdatachannel.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/ameba_webrtc_provider_manager.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/ameba_webrtc_transport.cpp
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/library/ice/ameba_stun.c
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/library/ice/ameba_ice.c
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/library/libdatachannel/ameba_dtls.c
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/library/libdatachannel/ameba_datachannel.c
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/library/libdatachannel/ameba_srtp.c
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/library/webrtc/ameba_sdp.c
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/library/webrtc/ameba_rtp.c
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/library/webrtc/ameba_rtcp.c
	${MATTER_DRIVER_DIR}/matter_drivers/webrtc/library/webrtc/ameba_webrtc.c
	${MATTER_EXAMPLE_DIR}/camera/example_matter_camera.cpp
	${MATTER_EXAMPLE_DIR}/camera/matter_camera_command.cpp
	${MATTER_EXAMPLE_DIR}/camera/matter_drivers.cpp

)
