#
# Copyright (c) 2015-2020, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

ifneq (${MATTER_SECURE_MBEDTLS_MK},1)
MATTER_SECURE_MBEDTLS_MK := 1

# MBEDTLS_DIR must be set to the mbed TLS main directory (it must contain
# the 'include' and 'library' subdirectories).
ifeq (${MBEDTLS_DIR},)
  $(error Error: MBEDTLS_DIR not set)
endif

MBEDTLS_INC = -I${MATTER_MBEDTLSDIR} \
              -I${MBEDTLS_DIR}/include \
              -I${MBEDTLS_DIR}/include/mbedtls \
              -I${MBEDTLS_DIR}/library \
              -I${MATTER_DIR}/common/os \
              -I${MATTER_DIR}/common/port \
              -I${BASEDIR}/component/ssl/mbedtls_ram_map/rom

# Specify mbed TLS configuration file
MBEDTLS_CONFIG_FILE	:=	"<matter_mbedtls_config.h>"
$(eval $(call add_define,MBEDTLS_CONFIG_FILE))

MBEDTLS_SOURCES += ${MATTER_DIR}/common/atf/mbedtls_secure/mbedtls_secure.c

LIBMBEDTLS_SRCS:= $(addprefix ${MBEDTLS_DIR}/library/, \
                    aes.c \
                    asn1parse.c \
                    asn1write.c \
                    base64.c \
                    bignum.c \
                    ccm.c \
                    cipher.c \
                    cipher_wrap.c \
                    constant_time.c \
                    ctr_drbg.c \
                    ecdsa.c \
                    ecp.c \
                    ecp_curves.c \
                    md.c \
                    md5.c \
                    memory_buffer_alloc.c \
                    oid.c \
                    pkparse.c \
                    pkcs5.c \
                    pkcs12.c \
                    pem.c \
                    pk.c \
                    pkwrite.c \
                    pk_wrap.c \
                    platform.c \
                    platform_util.c \
                    rsa.c \
                    sha1.c \
                    sha256.c \
                    sha512.c \
                    ssl_tls.c \
                    x509_create.c \
                    x509write_csr.c \
                    )

ifeq ($(MBEDTLS_MATTER_V_2_28_1_ENABLE), y)
LIBMBEDTLS_SRCS+= $(addprefix ${MBEDTLS_DIR}/library/, \
                    rsa_internal.c \
                    )
else ifeq ($(MBEDTLS_MATTER_V_3_6_1_ENABLE), y)
LIBMBEDTLS_SRCS+= $(addprefix ${MBEDTLS_DIR}/library/, \
                    bignum_core.c \
                    bignum_mod.c \
                    bignum_mod_raw.c \
                    rsa_alt_helpers.c \
                    sha3.c \
                    )
endif

LIBMBEDTLS_SRCS += ${MATTER_MBEDTLSDIR}/matter_mbedtls_nsc.c \
                   ${MBEDTLS_DIR}/library_s/mbedtls_nsc.c

$(eval $(call MAKE_LIB,mbedtls))

endif
