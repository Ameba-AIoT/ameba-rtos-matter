#
# Copyright (c) 2015-2020, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

#
# Copyright (c) 2015-2020, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

MATTER_MBEDTLSDIR = $(MATTER_DIR)/common/mbedtls
MBEDTLS_DIR       = $(BASEDIR)/component/ssl/mbedtls-3.6.2

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

endif

# Add BL32_SOURCES for Matter Secure
BL32_SOURCES += ${MBEDTLS_SOURCES} \
                ${MATTER_DIR}/common/atf/service/matter_rtk_svc_setup.c

# Add WARNINGS for Matter Secure
WARNINGS_TEMP := ${WARNINGS} -Wno-maybe-uninitialized -Wno-unused-variable -Wno-unused-label
WARNINGS      := $(filter-out -Wmaybe-uninitialized, ${WARNINGS_TEMP})

# Add DEFINITIONS for Matter Secure
CONFIG_BUILD_SECURE	       := 1
CONFIG_MATTER_SECURE       := 1
CONFIG_PLATFORM_AMEBASMART := 1
$(eval $(call add_define,CONFIG_BUILD_SECURE))
$(eval $(call add_define,CONFIG_MATTER_SECURE))
$(eval $(call add_define,CONFIG_PLATFORM_AMEBASMART))


# Add INCLUDES for Matter Secure
INCLUDES += -I${MATTER_DIR}/common/port \
            -I${MATTER_DIR}/common/atf/include

# Provide additional lib to add srand and rand API
# LIBRAND_SRCS = ${MATTER_DIR}/common/atf/libc/rand.c
# $(eval $(call MAKE_LIB,rand))
BL32_LIBS += ${MATTER_DIR}/common/atf/libc/librand.a
LDLIBS    += ${PROJECT_DIR}/asdk/lib/application/lib_atf_mbedtls_secure.a
