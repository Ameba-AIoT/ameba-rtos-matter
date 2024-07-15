include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/common/application/matter/common/mbedtls/$(MBEDTLS_VERSION)
DIR_RAMMAP = $(BASEDIR)/component/common/network/ssl/ssl_ram_map

MODULE_IFLAGS += -DCONFIG_BUILD_NONSECURE=1

vpath %.c $(DIR) $(shell find $(DIR) -type d) $(DIR_RAMMAP) $(shell find $(DIR_RAMMAP) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/aes.c \
       $(DIR)/aesni.c \
       $(DIR)/arc4.c \
       $(DIR)/aria.c \
       $(DIR)/asn1parse.c \
       $(DIR)/asn1write.c \
       $(DIR)/base64.c \
       $(DIR)/bignum.c \
       $(DIR)/blowfish.c \
       $(DIR)/camellia.c \
       $(DIR)/ccm.c \
       $(DIR)/certs.c \
       $(DIR)/chacha20.c \
       $(DIR)/chachapoly.c \
       $(DIR)/cipher.c \
       $(DIR)/cipher_wrap.c \
       $(DIR)/cmac.c \
       $(DIR)/constant_time.c \
       $(DIR)/ctr_drbg.c \
       $(DIR)/debug.c \
       $(DIR)/des.c \
       $(DIR)/dhm.c \
       $(DIR)/ecdh.c \
       $(DIR)/ecdsa.c \
       $(DIR)/ecjpake.c \
       $(DIR)/ecp.c \
       $(DIR)/ecp_curves.c \
       $(DIR)/entropy.c \
       $(DIR)/entropy_poll.c \
       $(DIR)/error.c \
       $(DIR)/gcm.c \
       $(DIR)/havege.c \
       $(DIR)/hkdf.c \
       $(DIR)/hmac_drbg.c \
       $(DIR)/md2.c \
       $(DIR)/md4.c \
       $(DIR)/md5.c \
       $(DIR)/md.c \
       $(DIR)/memory_buffer_alloc.c \
       $(DIR)/nist_kw.c \
       $(DIR)/oid.c \
       $(DIR)/padlock.c \
       $(DIR)/pem.c \
       $(DIR)/pk.c \
       $(DIR)/pkcs11.c \
       $(DIR)/pkcs12.c \
       $(DIR)/pkcs5.c \
       $(DIR)/pkparse.c \
       $(DIR)/pk_wrap.c \
       $(DIR)/pkwrite.c \
       $(DIR)/platform.c \
       $(DIR)/platform_util.c \
       $(DIR)/poly1305.c \
       $(DIR)/ripemd160.c \
       $(DIR)/rsa.c \
       $(DIR)/rsa_internal.c \
       $(DIR)/sha1.c \
       $(DIR)/sha256.c \
       $(DIR)/sha512.c \
       $(DIR)/ssl_cache.c \
       $(DIR)/ssl_ciphersuites.c \
       $(DIR)/ssl_cli.c \
       $(DIR)/ssl_cookie.c \
       $(DIR)/ssl_msg.c \
       $(DIR)/ssl_srv.c \
       $(DIR)/ssl_ticket.c \
       $(DIR)/ssl_tls.c \
       $(DIR)/threading.c \
       $(DIR)/timing.c \
       $(DIR)/version.c \
       $(DIR)/version_features.c \
       $(DIR)/x509.c \
       $(DIR)/x509_create.c \
       $(DIR)/x509_crl.c \
       $(DIR)/x509_crt.c \
       $(DIR)/x509_csr.c \
       $(DIR)/x509write_crt.c \
       $(DIR)/x509write_csr.c \
       $(DIR)/xtea.c \
       $(DIR_RAMMAP)/ssl_ram_map.c \

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
all: CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
