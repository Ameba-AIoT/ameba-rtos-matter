set(BASEDIR ${c_BASEDIR})

set(DIR                ${c_MBEDTLS_DIR})
set(MATTER_DIR         ${BASEDIR}/component/application/matter)
set(MATTER_MBEDTLS_DIR ${MATTER_DIR}/common/mbedtls)

##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

include(../public_includes_matter.cmake)
include(../public_definitions_matter.cmake)

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

# mbedtls secure library
#------------------------------#
ameba_list_append(private_sources
    ${DIR}/library/aes.c
    ${DIR}/library/asn1parse.c
    ${DIR}/library/asn1write.c
    ${DIR}/library/base64.c
    ${DIR}/library/bignum.c
    ${DIR}/library/ccm.c
    ${DIR}/library/cipher.c
    ${DIR}/library/cipher_wrap.c
    ${DIR}/library/constant_time.c
    ${DIR}/library/ctr_drbg.c
    ${DIR}/library/ecdsa.c
    ${DIR}/library/ecp.c
    ${DIR}/library/ecp_curves.c
    ${DIR}/library/md.c
    ${DIR}/library/md5.c
    ${DIR}/library/oid.c
    ${DIR}/library/pkparse.c
    ${DIR}/library/pkcs5.c
    ${DIR}/library/pkcs12.c
    ${DIR}/library/pem.c
    ${DIR}/library/pk.c
    ${DIR}/library/pkwrite.c
    ${DIR}/library/pk_wrap.c
    ${DIR}/library/platform.c
    ${DIR}/library/platform_util.c
    ${DIR}/library/rsa.c
    ${DIR}/library/sha1.c
    ${DIR}/library/sha256.c
    ${DIR}/library/sha512.c
    ${DIR}/library/ssl_tls.c
    ${DIR}/library/x509_create.c
    ${DIR}/library/x509write_csr.c
)
if(CONFIG_MBEDTLS_V_3_6_2_ENABLE)
    ameba_list_append(private_sources
        ${DIR}/library/bignum_core.c
        ${DIR}/library/bignum_mod.c
        ${DIR}/library/bignum_mod_raw.c
        ${DIR}/library/des.c
        ${DIR}/library/pk_ecc.c
        ${DIR}/library/rsa_alt_helpers.c
        ${DIR}/library/sha3.c
    )
endif()

# mbedtls secure library_s
#------------------------------#
ameba_list_append(private_sources
    ${MATTER_MBEDTLS_DIR}/matter_mbedtls_nsc.c
)
if(CONFIG_MBEDTLS_V_3_6_2_ENABLE)
    ameba_list_append(private_sources
        ${MATTER_MBEDTLS_DIR}/mbedtls-3.6.2/library_s/mbedtls_nsc.c
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
