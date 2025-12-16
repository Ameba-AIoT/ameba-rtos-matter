if(CONFIG_MATTER_EN)
##########################################################################################
## * MATTER DEFINITIONS
set(BASEDIR ${c_BASEDIR})

#Set everything in matter.cmake
include(matter.cmake)

message(STATUS "CLUSTER_FILE :${CLUSTER_FILE}")
message(STATUS "ZAP_FILE     :${ZAP_FILE}")
message(STATUS "MATTER_FILE  :${MATTER_FILE}")
message(STATUS "OUTPUT_DIR   :${OUTPUT_DIR}")

##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

set(matter_defintions)
set(lib_chip_core)
set(lib_chip_main)

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

include(flags/public_includes_matter.cmake)
include(flags/public_definitions_matter.cmake)
ameba_list_append(lib_chip_core ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_core.a)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(p_NO_WHOLE_ARCHIVE ${lib_chip_core})

# Generate cluster file during setup
if(DEFINED MATTER_EXAMPLE)
if(NOT EXISTS ${CLUSTER_FILE})
if(EXISTS ${ZAP_FILE})
	message(STATUS "${ZAP_FILE} exists")
	execute_process(COMMAND ${CMAKE_COMMAND} -DCODEGEN_DIR=${CODEGEN_DIR} -DCHIP_DIR=${CHIP_DIR} -DMATTER_TOOL_DIR=${MATTER_TOOL_DIR} -DMATTER_FILE=${MATTER_FILE} -DZAP_FILE=${ZAP_FILE} -DCLUSTER_FILE=${CLUSTER_FILE} -P ${GENERATE_CLUSTER_FILE})
else()
	message(FATAL_ERROR "${ZAP_FILE} does not exist.")
endif()
endif()
endif()

# generate cluster file again after ninja clean_matter_libs is run
add_custom_command(
	OUTPUT ${CLUSTER_FILE}
	COMMAND ${CMAKE_COMMAND} -DCODEGEN_DIR=${CODEGEN_DIR} -DCHIP_DIR=${CHIP_DIR} -DMATTER_TOOL_DIR=${MATTER_TOOL_DIR} -DMATTER_FILE=${MATTER_FILE} -DZAP_FILE=${ZAP_FILE} -DCLUSTER_FILE=${CLUSTER_FILE} -P ${GENERATE_CLUSTER_FILE}
)

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

# get compile definitions from global interface and put it in LIB_CHIP_CORE_FLAGS
get_target_property(GLOBAL_INTERFACE_DEFINITIONS ${c_MCU_PROJ_CONFIG} INTERFACE_COMPILE_DEFINITIONS)
# get include directories from global interface and put it in LIB_CHIP_CORE_INC_PATH
get_target_property(GLOBAL_INTERFACE_INCLUDES    ${c_MCU_PROJ_CONFIG} INTERFACE_INCLUDE_DIRECTORIES)
ameba_list_append(private_compile_options
	-Wno-register
)

# Add flags, includes, and source files based on the example 
if(DEFINED MATTER_EXAMPLE AND MATTER_EXAMPLE)
	message(STATUS "MATTER_EXAMPLE = ${MATTER_EXAMPLE}")
	if(EXISTS ${MATTER_CMAKE_DIR}/component/chip/${MATTER_EXAMPLE})
		if(EXISTS ${MATTER_CMAKE_DIR}/component/chip/${MATTER_EXAMPLE}/${MATTER_EXAMPLE}.cmake)
			message(STATUS "Found ${MATTER_EXAMPLE} include project")
			include(${MATTER_CMAKE_DIR}/component/chip/matter_main_sources.cmake)
			include(${MATTER_CMAKE_DIR}/component/chip/${MATTER_EXAMPLE}/${MATTER_EXAMPLE}.cmake)
		else()
			message(FATAL_ERROR "Found ${MATTER_EXAMPLE} include project but ${MATTER_EXAMPLE}.cmake not exist")
		endif()
	else()
		message(FATAL_ERROR "${MATTER_EXAMPLE} Not Found")
	endif()
endif()

# Component private part, user config end
#------------------------------#

# Generate args.gn during setup
# TARGET_COMPILE_FLAGS
set(TARGET_COMPILE_FLAGS)
foreach(item IN LISTS c_GLOBAL_COMMON_COMPILE_C_OPTIONS)
	set(TARGET_COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}\"${item}\",")
endforeach()
foreach(item IN LISTS c_GLOBAL_MCU_COMPILE_C_OPTIONS)
	set(TARGET_COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}\"${item}\",")
endforeach()
set(TARGET_COMPILE_FLAGS "${TARGET_COMPILE_FLAGS}\"-Wno-register\",")
# INCLUDE_FLAGS
set(TARGET_INCLUDE_FLAGS)
foreach(item IN LISTS LIB_CHIP_CORE_INC_PATH)
	set(TARGET_INCLUDE_FLAGS "${TARGET_INCLUDE_FLAGS}\"-I${item}\",")
endforeach()
#DEFINE_FLAGS
set(TARGET_DEFINE_FLAGS)
foreach(item IN LISTS LIB_CHIP_CORE_FLAGS)
	set(TARGET_DEFINE_FLAGS "${TARGET_DEFINE_FLAGS}\"-D${item}\",")
endforeach()
set(TARGET_DEFINE_FLAGS "${TARGET_DEFINE_FLAGS}\"-Drtw_get_random_bytes=TRNG_get_random_bytes\",")

set(TARGET_CFLAGS_C  "${TARGET_COMPILE_FLAGS}${TARGET_DEFINE_FLAGS}${TARGET_INCLUDE_FLAGS}")
set(TARGET_CFLAGS_CC "${TARGET_COMPILE_FLAGS}${TARGET_DEFINE_FLAGS}${TARGET_INCLUDE_FLAGS}")

execute_process(COMMAND ${CMAKE_COMMAND} -DCHIP_ENABLE_OTA_REQUESTOR=${CHIP_ENABLE_OTA_REQUESTOR} -DCHIP_ENABLE_CHIPOBLE=${CHIP_ENABLE_CHIPOBLE} -DCHIP_ENABLE_IPV4=${CHIP_ENABLE_IPV4} -DCHIP_ENABLE_TC=${CHIP_ENABLE_TC} -DCHIP_ENABLE_SHELL=${CHIP_ENABLE_SHELL} -DOUTPUT_DIR=${OUTPUT_DIR} -DCHIP_DIR=${CHIP_DIR} -DTARGET_CFLAGS_C=${TARGET_CFLAGS_C} -DTARGET_CFLAGS_CC=${TARGET_CFLAGS_CC} -P ${GENERATE_ARGS_GN})

# generate args.gn file again after ninja clean_matter_libs is run
add_custom_command(
	OUTPUT  ${ARGS_GN}
	DEPENDS ${CLUSTER_FILE}
	COMMAND ${CMAKE_COMMAND} -DCHIP_ENABLE_OTA_REQUESTOR=${CHIP_ENABLE_OTA_REQUESTOR} -DCHIP_ENABLE_CHIPOBLE=${CHIP_ENABLE_CHIPOBLE} -DCHIP_ENABLE_IPV4=${CHIP_ENABLE_IPV4} -DCHIP_ENABLE_TC=${CHIP_ENABLE_TC} -DCHIP_ENABLE_SHELL=${CHIP_ENABLE_SHELL} -DOUTPUT_DIR=${OUTPUT_DIR} -DCHIP_DIR=${CHIP_DIR} -DTARGET_CFLAGS_C=\"${TARGET_CFLAGS_C}\" -DTARGET_CFLAGS_CC=\"${TARGET_CFLAGS_CC}\" -DREGEN_ARGS_GN=1 -P ${GENERATE_ARGS_GN}
)

# Compiling Configurations
# ameba_app_library(chip_main)
# target_sources(${CURRENT_LIB_NAME} PRIVATE ${LIB_CHIP_MAIN_SOURCES})
# target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${LIB_CHIP_MAIN_FLAGS})
# target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${LIB_CHIP_MAIN_INC_PATH})

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(chip_main
    p_OUTPUT_PATH ${c_SDK_LIB_APPLICATION_DIR}
    p_OUTPUT_NAME chip_main
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)

add_custom_command(
	OUTPUT ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_core.a

	DEPENDS ${ARGS_GN}

	COMMAND ${CMAKE_COMMAND} -E make_directory ${CHIP_DIR}/config/ameba/components/chip
	COMMAND ${CMAKE_COMMAND} -E chdir ${CHIP_DIR}/config/ameba/components/chip gn gen --check --fail-on-unused-args ${OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E chdir ${CHIP_DIR}/config/ameba/components/chip ninja -C ${OUTPUT_DIR} :ameba
	COMMAND ${CMAKE_COMMAND} -E copy ${OUTPUT_DIR}/lib/* ${c_SDK_LIB_APPLICATION_DIR}/
	COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_LIB_APPLICATION_DIR}/libCHIP.a ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_core.a
)

# set lib_chip_core as target for lib_chip_main
add_custom_target(
	lib_chip_core
	DEPENDS ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_core.a
)

# before compiling lib_chip_main.a, make sure lib_chip_core.a is compiled
ameba_target_depend(
	${c_CURRENT_TARGET_NAME}
	lib_chip_core
)

# matter clean target
add_custom_target(
	clean_matter_libs

	# Clean all output dirs
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${ALL_CLUSTERS_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${AIR_PURIFIER_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${BRIDGE_PORT_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${ENERGY_MANAGEMENT_PORT_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${LIGHTING_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${THERMOSTAT_PORT_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${AIRCON_PORT_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${BRIDGE_DM_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${DISHWASHER_PORT_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${FAN_PORT_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${LAUNDRYWASHER_PORT_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${LIGHTING_DM_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${MICROWAVEOVEN_PORT_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${REFRIGERATOR_PORT_OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E echo "cleaned Matter output directories"

	# Clean all .matter files
	COMMAND ${CMAKE_COMMAND} -E rm -f ${ALL_CLUSTERS_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${AIR_PURIFIER_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${BRIDGE_PORT_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${ENERGY_MANAGEMENT_PORT_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${LIGHTING_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${THERMOSTAT_PORT_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${AIRCON_PORT_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${BRIDGE_DM_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${DISHWASHER_PORT_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${FAN_PORT_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${LAUNDRYWASHER_PORT_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${LIGHTING_DM_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${MICROWAVEOVEN_PORT_MATTER}
	COMMAND ${CMAKE_COMMAND} -E rm -f ${REFRIGERATOR_PORT_MATTER}
	COMMAND ${CMAKE_COMMAND} -E echo "cleaned .matter files"

	# Clean matter library object
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_core.a
	COMMAND ${CMAKE_COMMAND} -E echo "cleaned lib_chip_core"
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_main.a
	COMMAND ${CMAKE_COMMAND} -E echo "cleaned lib_chip_main"
)

# Other Matter component subdirectories
ameba_add_subdirectory(component/application)
if(CONFIG_AMEBASMART)
	ameba_add_subdirectory(component/atf_mbedtls_secure)
endif()

endif() #CONFIG_MATTER_EN