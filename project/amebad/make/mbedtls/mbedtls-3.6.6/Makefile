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
       $(DIR)/aesce.c \
       $(DIR)/aesni.c \
       $(DIR)/aria.c \
       $(DIR)/asn1parse.c \
       $(DIR)/asn1write.c \
       $(DIR)/base64.c \
       $(DIR)/bignum.c \
       $(DIR)/bignum_core.c \
       $(DIR)/bignum_mod.c \
       $(DIR)/bignum_mod_raw.c \
       $(DIR)/block_cipher.c \
       $(DIR)/camellia.c \
       $(DIR)/ccm.c \
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
       $(DIR)/ecp_curves_new.c \
       $(DIR)/entropy.c \
       $(DIR)/error.c \
       $(DIR)/gcm.c \
       $(DIR)/hkdf.c \
       $(DIR)/hmac_drbg.c \
       $(DIR)/lmots.c \
       $(DIR)/lms.c \
       $(DIR)/md.c \
       $(DIR)/md5.c \
       $(DIR)/memory_buffer_alloc.c \
       $(DIR)/mps_reader.c \
       $(DIR)/mps_trace.c \
       $(DIR)/nist_kw.c \
       $(DIR)/oid.c \
       $(DIR)/padlock.c \
       $(DIR)/pem.c \
       $(DIR)/pk.c \
       $(DIR)/pk_ecc.c \
       $(DIR)/pk_wrap.c \
       $(DIR)/pkcs12.c \
       $(DIR)/pkcs7.c \
       $(DIR)/pkcs5.c \
       $(DIR)/pkparse.c \
       $(DIR)/pkwrite.c \
       $(DIR)/platform.c \
       $(DIR)/platform_util.c \
       $(DIR)/poly1305.c \
       $(DIR)/psa_crypto.c \
       $(DIR)/psa_crypto_aead.c \
       $(DIR)/psa_crypto_cipher.c \
       $(DIR)/psa_crypto_client.c \
       $(DIR)/psa_crypto_driver_wrappers_no_static.c \
       $(DIR)/psa_crypto_ecp.c \
       $(DIR)/psa_crypto_ffdh.c \
       $(DIR)/psa_crypto_hash.c \
       $(DIR)/psa_crypto_mac.c \
       $(DIR)/psa_crypto_pake.c \
       $(DIR)/psa_crypto_rsa.c \
       $(DIR)/psa_crypto_se.c \
       $(DIR)/psa_crypto_slot_management.c \
       $(DIR)/psa_crypto_storage.c \
       $(DIR)/psa_its_file.c \
       $(DIR)/psa_util.c \
       $(DIR)/ripemd160.c \
       $(DIR)/rsa.c \
       $(DIR)/rsa_alt_helpers.c \
       $(DIR)/sha1.c \
       $(DIR)/sha3.c \
       $(DIR)/sha256.c \
       $(DIR)/sha512.c \
       $(DIR)/ssl_cache.c \
       $(DIR)/ssl_ciphersuites.c \
       $(DIR)/ssl_cli.c \
       $(DIR)/ssl_cookie.c \
       $(DIR)/ssl_debug_helpers_generated.c \
       $(DIR)/ssl_msg.c \
       $(DIR)/ssl_ticket.c \
       $(DIR)/ssl_tls.c \
       $(DIR)/ssl_tls12_client.c \
       $(DIR)/ssl_tls12_server.c \
       $(DIR)/ssl_tls13_client.c \
       $(DIR)/ssl_tls13_generic.c \
       $(DIR)/ssl_tls13_keys.c \
       $(DIR)/ssl_tls13_server.c \
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
