## * This scrpt is used to make mbedtls library for atf, in order to avoid building error in atf
## *    step 1. make lib_atf_mbedtls.a here
## *    step 2. link lib_atf_mbedtls.a to bl1,bl2,bl32 in ${c_CMPT_SOC_DIR}/atf/Makefile
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
ameba_list_append(private_definitions
    CONFIG_BUILD_SECURE=1
    CONFIG_PLATFORM_AMEBASMART=1
    MBEDTLS_CONFIG_FILE="matter_mbedtls_config.h"
    CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN=${CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN}
    CONFIG_MBEDTLS_SSL_OUT_CONTENT_LEN=${CONFIG_MBEDTLS_SSL_OUT_CONTENT_LEN}
)

# to compatiable with component/soc/amebasmart/atf/Makefile
# ameba_list_append(private_compile_options
#     -g
#     -march=armv8-a -mfpu=neon -mfloat-abi=softfp
#     -mthumb -mno-unaligned-access
#     -Werror -Wall -Wmissing-include-dirs -Wunused
#     -Wdisabled-optimization -Wvla -Wshadow
#     -Wno-unused-parameter -Wunused-but-set-variable
#     -Wpacked-bitfield-compat -Wshift-overflow=2
#     -Wlogical-op -Wno-implicit-function-declaration
#     -Wno-maybe-uninitialized -Wno-unused-variable
#     -Wno-unused-label -Wno-error=deprecated-declarations
#     -Wno-error=cpp
#     -ffunction-sections -fdata-sections -ffreestanding
#     -fno-builtin -fno-common -Os -std=gnu99 -fno-stack-protector
# )

ameba_list_append(private_compile_options
    -g
    -march=armv8-a -mfpu=neon -mfloat-abi=softfp
    -mthumb -mno-unaligned-access
    -Wall -Wmissing-include-dirs -Wunused
    -Wdisabled-optimization -Wvla -Wshadow
    -Wno-unused-parameter -Wunused-but-set-variable
    -Wpacked-bitfield-compat -Wshift-overflow=2
    -Wlogical-op -Wno-implicit-function-declaration
    -Wno-maybe-uninitialized -Wno-unused-variable
    -Wno-unused-label -Wno-error=deprecated-declarations
    -Wno-error=cpp
    -ffunction-sections -fdata-sections -ffreestanding
    -fno-builtin -fno-common -Os -std=gnu99 -fno-stack-protector
)

set(MBEDTLS_DIR ${c_CMPT_SSL_DIR}/mbedtls-3.6.2)
#ameba-rtos includes
ameba_list_append(private_includes
    ${MBEDTLS_DIR}/include
    ${MBEDTLS_DIR}/include/mbedtls
    ${MBEDTLS_DIR}/library
    ${c_CMPT_SSL_DIR}/mbedtls_ram_map/rom
    ${c_SOC_PROJECT_DIR}/menuconfig/project_ap
)
#ameba-rtos-matter includes
ameba_list_append(private_includes
    ${MATTER_DIR}/common/os
    ${MATTER_DIR}/common/port
    ${MATTER_DIR}/common/mbedtls
    ${MATTER_DIR}/common/atf/include
)
#mbedtls sources
ameba_list_append(private_sources
    ${MBEDTLS_DIR}/library/aes.c
    ${MBEDTLS_DIR}/library/asn1parse.c
    ${MBEDTLS_DIR}/library/asn1write.c
    ${MBEDTLS_DIR}/library/base64.c
    ${MBEDTLS_DIR}/library/bignum.c
    ${MBEDTLS_DIR}/library/bignum_core.c
    ${MBEDTLS_DIR}/library/bignum_mod.c
    ${MBEDTLS_DIR}/library/bignum_mod_raw.c
    ${MBEDTLS_DIR}/library/ccm.c
    ${MBEDTLS_DIR}/library/cipher.c
    ${MBEDTLS_DIR}/library/cipher_wrap.c
    ${MBEDTLS_DIR}/library/constant_time.c
    ${MBEDTLS_DIR}/library/ctr_drbg.c
    ${MBEDTLS_DIR}/library/des.c
    ${MBEDTLS_DIR}/library/ecdsa.c
    ${MBEDTLS_DIR}/library/ecp.c
    ${MBEDTLS_DIR}/library/ecp_curves.c
    ${MBEDTLS_DIR}/library/md.c
    ${MBEDTLS_DIR}/library/md5.c
    ${MBEDTLS_DIR}/library/oid.c
    ${MBEDTLS_DIR}/library/pkparse.c
    ${MBEDTLS_DIR}/library/pkcs5.c
    ${MBEDTLS_DIR}/library/pkcs12.c
    ${MBEDTLS_DIR}/library/pem.c
    ${MBEDTLS_DIR}/library/pk.c
    ${MBEDTLS_DIR}/library/pkwrite.c
    ${MBEDTLS_DIR}/library/pk_ecc.c
    ${MBEDTLS_DIR}/library/pk_wrap.c
    ${MBEDTLS_DIR}/library/platform.c
    ${MBEDTLS_DIR}/library/platform_util.c
    ${MBEDTLS_DIR}/library/rsa.c
    ${MBEDTLS_DIR}/library/rsa_alt_helpers.c
    ${MBEDTLS_DIR}/library/sha1.c
    ${MBEDTLS_DIR}/library/sha3.c
    ${MBEDTLS_DIR}/library/sha256.c
    ${MBEDTLS_DIR}/library/sha512.c
    ${MBEDTLS_DIR}/library/ssl_tls.c
    ${MBEDTLS_DIR}/library/x509_create.c
    ${MBEDTLS_DIR}/library/x509write_csr.c
)
#matter modified memory_buffer_alloc.c to print debugger
ameba_list_append(private_sources
    ${c_CMPT_MATTER_DIR}/common/mbedtls/mbedtls-3.6.2/library/memory_buffer_alloc.c
)
#matter_mbedtls source
ameba_list_append(private_sources
    ${MATTER_DIR}/common/mbedtls/matter_mbedtls_nsc.c
)
                
# As MBEDTLS_CONFIG_FILE is defined as INTERFACE_COMPILE_DEFINITIONS of {c_PROJ_CONFIG},
# use ameba_target_add to avoid MBEDTLS_CONFIG_FILE redefined error, {c_PROJ_CONFIG} will not be linked
ameba_target_add(atf_mbedtls_secure
    p_WRAP_TARGET_NAME
    p_PREFIX lib_
    p_OUTPUT_PATH ${c_SDK_LIB_APPLICATION_DIR}
    p_OUTPUT_NAME atf_mbedtls_secure
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)

add_custom_target(
	lib_atf_mbedtls_secure.a
	DEPENDS ${c_SDK_LIB_APPLICATION_DIR}/lib_atf_mbedtls_secure.a
)
