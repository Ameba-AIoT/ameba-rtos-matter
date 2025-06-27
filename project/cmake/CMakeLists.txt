if(CONFIG_MATTER_EN AND CONFIG_CORE_AS_AP)
##########################################################################################
## * MATTER DEFINITIONS
set(BASEDIR ${c_BASEDIR})

include(matter.cmake)

# matter general cmake
set(MATTER_INCLUDE        "${MATTER_CMAKEDIR}/matter.cmake")
set(GENERATE_ARGS_GN      "${MATTER_CMAKEDIR}/utilities/generate_args_gn.cmake")
set(GENERATE_CLUSTER_FILE "${MATTER_CMAKEDIR}/utilities/generate_cluster_file.cmake")


# MATTER ZAP, CLUSTER, AND OUTPUT_DIR LIST
set(ALL_CLUSTERS_FILE       "${CHIPDIR}/examples/all-clusters-app/ameba/build/chip/codegen/cluster-file.txt")
set(ALL_CLUSTERS_ZAP        "${CHIPDIR}/examples/all-clusters-app/all-clusters-common/all-clusters-app.zap")
set(ALL_CLUSTERS_OUTPUT_DIR "${CHIPDIR}/examples/all-clusters-app/ameba/build/chip")
set(LIGHTING_FILE           "${CHIPDIR}/examples/lighting-app/ameba/build/chip/codegen/cluster-file.txt")
set(LIGHTING_ZAP            "${CHIPDIR}/examples/lighting-app/lighting-common/lighting-app.zap")
set(LIGHTING_OUTPUT_DIR     "${CHIPDIR}/examples/lighting-app/ameba/build/chip")

# Set Matter files and directories based on the example
if("${MATTER_EXAMPLE}" STREQUAL "all_clusters")
	set(CLUSTER_FILE ${ALL_CLUSTERS_FILE})
	set(ZAP_FILE     ${ALL_CLUSTERS_ZAP})
	string(REPLACE ".zap" ".matter" MATTER_FILE "${ZAP_FILE}")
	set(OUTPUT_DIR ${ALL_CLUSTERS_OUTPUT_DIR})
	# file(REMOVE "${ALL_CLUSTERS_ZAP}")
	file(COPY "${MATTER_EXAMPLEDIR}/chiptest/all-clusters-app.zap" 
		 DESTINATION "${CHIPDIR}/examples/all-clusters-app/all-clusters-common/")
elseif("${MATTER_EXAMPLE}" STREQUAL "light_port")
	set(CLUSTER_FILE ${LIGHTING_FILE})
	set(ZAP_FILE     ${LIGHTING_ZAP})
	string(REPLACE ".zap" ".matter" MATTER_FILE "${ZAP_FILE}")
	set(OUTPUT_DIR ${LIGHTING_OUTPUT_DIR})
endif()
set(CODEGEN_DIR "${OUTPUT_DIR}/codegen")
set(ARGS_GN "${OUTPUT_DIR}/args.gn")

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

include(public_includes_matter.cmake)
include(public_definitions_matter.cmake)
ameba_list_append(lib_chip_core ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_core.a)
ameba_list_append(lib_chip_main ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_main.a)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${lib_chip_main}) #default: whole-archived
ameba_global_library(p_NO_WHOLE_ARCHIVE ${lib_chip_core})

# Generate cluster file during setup
if(DEFINED MATTER_EXAMPLE)
if(NOT EXISTS ${CLUSTER_FILE})
if(EXISTS ${ZAP_FILE})
	message(STATUS "${ZAP_FILE} exists")
	execute_process(COMMAND ${CMAKE_COMMAND} -DCODEGEN_DIR=${CODEGEN_DIR} -DCHIPDIR=${CHIPDIR} -DMATTER_TOOLDIR=${MATTER_TOOLDIR} -DMATTER_FILE=${MATTER_FILE} -DZAP_FILE=${ZAP_FILE} -DCLUSTER_FILE=${CLUSTER_FILE} -P ${GENERATE_CLUSTER_FILE})
else()
	message(FATAL_ERROR "${ZAP_FILE} does not exist.")
endif()
endif()
endif()

# generate cluster file again after clean_matter_libs is run
add_custom_command(
	OUTPUT ${CLUSTER_FILE}
	COMMAND ${CMAKE_COMMAND} -DCODEGEN_DIR=${CODEGEN_DIR} -DCHIPDIR=${CHIPDIR} -DMATTER_TOOLDIR=${MATTER_TOOLDIR} -DMATTER_FILE=${MATTER_FILE} -DZAP_FILE=${ZAP_FILE} -DCLUSTER_FILE=${CLUSTER_FILE} -P ${GENERATE_CLUSTER_FILE}
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

# Add flags, includes, and source files based on the example 
if(DEFINED MATTER_EXAMPLE AND MATTER_EXAMPLE)
	message(STATUS "MATTER_EXAMPLE = ${MATTER_EXAMPLE}")
	if(EXISTS ${MATTER_CMAKEDIR}/chip/${MATTER_EXAMPLE})
		if(EXISTS ${MATTER_CMAKEDIR}/chip/${MATTER_EXAMPLE}/${MATTER_EXAMPLE}.cmake)
			message(STATUS "Found ${MATTER_EXAMPLE} include project")
			include(${MATTER_CMAKEDIR}/chip/${MATTER_EXAMPLE}/${MATTER_EXAMPLE}.cmake)
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

set(TARGET_CFLAGS_C  "${TARGET_COMPILE_FLAGS}${TARGET_DEFINE_FLAGS}${TARGET_INCLUDE_FLAGS}")
set(TARGET_CFLAGS_CC "${TARGET_COMPILE_FLAGS}${TARGET_DEFINE_FLAGS}${TARGET_INCLUDE_FLAGS}")

execute_process(COMMAND ${CMAKE_COMMAND} -DCHIP_ENABLE_OTA_REQUESTOR=${CHIP_ENABLE_OTA_REQUESTOR} -DCHIP_ENABLE_CHIPOBLE=${CHIP_ENABLE_CHIPOBLE} -DCHIP_ENABLE_IPV4=${CHIP_ENABLE_IPV4} -DCHIP_ENABLE_TC=${CHIP_ENABLE_TC} -DOUTPUT_DIR=${OUTPUT_DIR} -DCHIPDIR=${CHIPDIR} -DTARGET_CFLAGS_C=${TARGET_CFLAGS_C} -DTARGET_CFLAGS_CC=${TARGET_CFLAGS_CC} -P ${GENERATE_ARGS_GN})

# generate args.gn file again after clean_matter_libs is run
add_custom_command(
	OUTPUT  ${ARGS_GN}
	DEPENDS ${CLUSTER_FILE}
	COMMAND ${CMAKE_COMMAND} -DCHIP_ENABLE_OTA_REQUESTOR=${CHIP_ENABLE_OTA_REQUESTOR} -DCHIP_ENABLE_CHIPOBLE=${CHIP_ENABLE_CHIPOBLE} -DCHIP_ENABLE_IPV4=${CHIP_ENABLE_IPV4} -DCHIP_ENABLE_TC=${CHIP_ENABLE_TC} -DOUTPUT_DIR=${OUTPUT_DIR} -DCHIPDIR=${CHIPDIR} -DTARGET_CFLAGS_C=\"${TARGET_CFLAGS_C}\" -DTARGET_CFLAGS_CC=\"${TARGET_CFLAGS_CC}\" -DREGEN_ARGS_GN=1 -P ${GENERATE_ARGS_GN}
)

# Compiling Configurations
# ameba_app_library(chip_main)
# target_sources(${CURRENT_LIB_NAME} PRIVATE ${LIB_CHIP_MAIN_SOURCES})
# target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${LIB_CHIP_MAIN_FLAGS})
# target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${LIB_CHIP_MAIN_INC_PATH})

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_external_app_library(chip_main
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

	COMMAND ${CMAKE_COMMAND} -E make_directory ${CHIPDIR}/config/ameba/components/chip
	COMMAND ${CMAKE_COMMAND} -E chdir ${CHIPDIR}/config/ameba/components/chip gn gen --check --fail-on-unused-args ${OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E chdir ${CHIPDIR}/config/ameba/components/chip ninja -C ${OUTPUT_DIR} :ameba
	COMMAND ${CMAKE_COMMAND} -E copy ${OUTPUT_DIR}/lib/* ${c_SDK_LIB_APPLICATION_DIR}/
	COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_LIB_APPLICATION_DIR}/libCHIP.a ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_core.a
)

# lib_chip_core is based on GENERATE_NINJA
add_custom_target(
	lib_chip_core
	DEPENDS ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_core.a
)

# before compiling lib_chip_main, make sure lib_chip_core is compiled
add_dependencies(
	${c_CURRENT_TARGET_NAME}
	lib_chip_core
)

# matter clean target
add_custom_target(
	clean_matter_libs
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${OUTPUT_DIR}
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_core.a
	COMMAND ${CMAKE_COMMAND} -E echo cleaned lib_chip_core
	COMMAND ${CMAKE_COMMAND} -E rm -rf ${c_SDK_LIB_APPLICATION_DIR}/lib_chip_main.a
	COMMAND ${CMAKE_COMMAND} -E echo cleaned lib_chip_main
)

ameba_add_subdirectory(application matter_application)

endif() #CONFIG_MATTER_EN AND CONFIG_CORE_AS_AP