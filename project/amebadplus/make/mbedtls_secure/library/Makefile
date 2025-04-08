
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(MBEDTLSDIR)
DIR_FWLIB = $(BASEDIR)/component/soc/amebadplus/fwlib/ram_km4
DIR_RAMMAP = $(BASEDIR)/component/ssl/mbedtls_ram_map

MODULE_IFLAGS += -DCONFIG_BUILD_SECURE=1
MODULE_IFLAGS += -I$(BASEDIR)/component/ssl/mbedtls_ram_map/rom
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/common/crashdump/include

vpath %.c $(DIR) $(shell find $(DIR) -type d) $(DIR_RAMMAP) $(shell find $(DIR_RAMMAP) -type d) $(DIR_FWLIB)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/library/aes.c			        \
       $(DIR)/library/asn1parse.c		    \
       $(DIR)/library/asn1write.c		    \
       $(DIR)/library/base64.c			    \
       $(DIR)/library/bignum.c			    \
       $(DIR)/library/ccm.c			        \
       $(DIR)/library/cipher.c			    \
       $(DIR)/library/cipher_wrap.c         \
       $(DIR)/library/constant_time.c       \
       $(DIR)/library/ctr_drbg.c            \
       $(DIR)/library/ecdsa.c			    \
       $(DIR)/library/ecp.c			        \
       $(DIR)/library/ecp_curves.c			\
       $(DIR)/library/md.c			        \
       $(DIR)/library/md5.c			        \
       $(DIR)/library/oid.c		            \
       $(DIR)/library/pkparse.c             \
       $(DIR)/library/pkcs5.c			    \
       $(DIR)/library/pkcs12.c			    \
       $(DIR)/library/pem.c			        \
       $(DIR)/library/pk.c			        \
       $(DIR)/library/pkwrite.c             \
       $(DIR)/library/pk_wrap.c			    \
       $(DIR)/library/platform.c		    \
       $(DIR)/library/platform_util.c		\
       $(DIR)/library/rsa.c			        \
       $(DIR)/library/rsa_internal.c		\
       $(DIR)/library/sha1.c  			    \
       $(DIR)/library/sha256.c			    \
       $(DIR)/library/sha512.c			    \
       $(DIR)/library/ssl_tls.c  		    \
       $(DIR)/library/x509_create.c         \
       $(DIR)/library/x509write_csr.c       \
       $(DIR_RAMMAP)/rom/rom_ssl_ram_map.c  \
       $(DIR_FWLIB)/ameba_app_start.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)

	
#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

%.o:%.c
	$(CC) -ffunction-sections $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
