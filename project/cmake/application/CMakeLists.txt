set(BASEDIR ${c_BASEDIR})

set(DIR         ${BASEDIR}/component/application/matter)
set(ATCMDDIR    ${BASEDIR}/component/application/matter/common/atcmd)
set(BTDIR       ${BASEDIR}/component/application/matter/common/bluetooth)
set(LWIPDIR     ${BASEDIR}/component/application/matter/common/lwip)
set(MBEDDIR     ${BASEDIR}/component/application/matter/common/mbedtls)
set(PORTDIR     ${BASEDIR}/component/application/matter/common/port)
set(PROTOBUFDIR ${BASEDIR}/component/application/matter/common/protobuf)
set(NANOPBDIR   ${BASEDIR}/component/application/matter/common/protobuf/nanopb)
set(EXAMPLEDIR  ${BASEDIR}/component/application/matter/examples)
set(CHIPTESTDIR ${BASEDIR}/component/application/matter/examples/chiptest)

##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

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

ameba_list_append(private_sources
    ${ATCMDDIR}/atcmd_matter.c

    # Matter BLE
    ${BTDIR}/matter_blemgr_common.c

    # Matter KV
    ${PORTDIR}/matter_kv.c

    # Matter LwIP
    ${LWIPDIR}/lwip_v2.1.2/port/realtek/hooks/lwip_default_hooks.c

    # Matter Common code porting
    ${PORTDIR}/matter_kvs.c
    ${PORTDIR}/matter_ota.c
    ${PORTDIR}/matter_timers.c
    ${PORTDIR}/matter_utils.c
    ${PORTDIR}/matter_wifis.c
    ${PORTDIR}/matter_lwip.c

    # Matter Factory data
    ${PROTOBUFDIR}/ameba_factory.pb.c
    ${NANOPBDIR}/pb_common.c
    ${NANOPBDIR}/pb_decode.c
    ${NANOPBDIR}/pb_encode.c

    # Matter Example
    ${CHIPTESTDIR}/example_matter.c
    ${EXAMPLEDIR}/matter_example_entry.c
)

ameba_list_append(private_includes

    ${MBEDTLSDIR}/library
    ${BASEDIR}/component/application/matter/examples
)
# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(matter_application
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
