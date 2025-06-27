
ameba_list_append(public_includes
	${MATTER_DIR}/api
	${MATTER_DIR}/common/atcmd
	${MATTER_DIR}/common/file_system/kv
	${MATTER_DIR}/common/lwip/api
	${MATTER_DIR}/common/lwip/include
	${MATTER_DIR}/common/port
	${MATTER_DIR}/common/os
	${MATTER_DIR}/common/include
	${MATTER_DIR}/common/protobuf
	${MATTER_DIR}/common/protobuf/nanopb
	${MATTER_DIR}/common/mbedtls
	${MATTER_DIR}/common/wifi
	
	${BASEDIR}/component/at_cmd
	
	${FREERTOSDIR}/include/

	#lwip_default_hooks
	${MATTER_DIR}/common/lwip/lwip_v2.1.2/port/realtek/include/
)

if(CONFIG_AMEBADPLUS)
	ameba_list_append(public_includes
		${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/non_secure
		${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/secure
	)
elseif(CONFIG_AMEBALITE)
	ameba_list_append(public_includes
		${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/non_secure
		${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/secure
	)
elseif(CONFIG_AMEBASMART)
	ameba_list_append(public_includes
		${FREERTOSDIR}/portable/GCC/ARM_CA7
	)
	if(CONFIG_MATTER_SECURE_EN)
		ameba_list_append(public_includes
			${MATTER_DIR}/common/atf/include
		)
	endif()
endif()

if(CONFIG_BT_MATTER_ADAPTER)
	ameba_list_append(public_includes
		${MATTER_DIR}/common/bluetooth/bt_matter_adapter
	)
endif()
if(CONFIG_BLE_MATTER_ADAPTER)
	ameba_list_append(public_includes
		${MATTER_DIR}/common/bluetooth
		${MATTER_DIR}/common/bluetooth/ble_matter_adapter_peripheral
	)
endif()
