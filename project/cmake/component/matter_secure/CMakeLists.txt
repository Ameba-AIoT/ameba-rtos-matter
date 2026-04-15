if(CONFIG_AMEBARTOS_V1_2)
set(MATTER_DIR         ${EXAMPLEDIR})
elseif(CONFIG_AMEBARTOS_V1_1)
set(BASEDIR            ${c_BASEDIR})
set(MATTER_DIR         ${BASEDIR}/component/application/matter)
endif()
set(MATTER_MBEDTLS_DIR ${MATTER_DIR}/common/mbedtls)

##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

include(../../flags/public_includes_matter.cmake)
include(../../flags/public_definitions_matter.cmake)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries}) #default: whole-archived

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

# mbedtls secure library, latest v1.1 (+ matter patch) and v1.2 compile mbedtls in image3 by default
#------------------------------#
if(CONFIG_COMPILE_MBEDTLS_FROM_MATTER_SECURE)
include(mbedtls_secure.cmake)
endif()

# mbedtls secure library_s
#------------------------------#
ameba_list_append(private_sources
    ${MATTER_MBEDTLS_DIR}/matter_mbedtls_nsc.c
)

# To check freertos secure heap status, optional, only applicable for dplus and lite
#------------------------------#
if(CONFIG_AMEBADPLUS OR CONFIG_AMEBALITE)
    ameba_list_append(private_sources
        ${MATTER_DIR}/common/os/freertos_secure_additions.c
    )
endif()
ameba_list_append(private_definitions
    CONFIG_BUILD_SECURE=1
)
ameba_list_append(private_compile_options
    -ffunction-sections
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(matter_mbedtls_secure
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################
