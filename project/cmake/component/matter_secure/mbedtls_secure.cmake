# mbedtls secure library
#------------------------------#
ameba_list_append(private_sources
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
        ${DIR}/library/hmac_drbg.c
        ${DIR}/library/rsa_alt_helpers.c
        ${DIR}/library/sha3.c
    )
endif()

# mbedtls secure hardware
#------------------------------#

if(CONFIG_AMEBASMART OR CONFIG_AMEBALITE OR CONFIG_AMEBADPLUS)
    ameba_list_append(private_sources
        ${DIR}/hw_alt_legacy/aes_alt.c
        ${DIR}/hw_alt_legacy/sha256_alt.c
        ${DIR}/hw_alt_legacy/sha512_alt.c
        ${DIR}/hw_alt_legacy/ssl_rom_to_ram_map.c
    )
else()
    ameba_list_append(private_sources
        ${DIR}/library/aes.c
        ${DIR}/library/sha256.c
        ${DIR}/library/sha512.c
        ${DIR}/library/psa_crypto_driver_wrappers_no_static.c
        ${DIR}/library/psa_crypto_ffdh.c
    )
endif()

ameba_list_append_if(CONFIG_AMEBASMART private_sources ${DIR}/hw_alt_legacy/ecc_alt.c)
ameba_list_append_if(CONFIG_AMEBALITE  private_sources ${DIR}/hw_alt_legacy/ecc_alt.c)
ameba_list_append_if(CONFIG_AMEBASMART private_sources ${DIR}/hw_alt_legacy/rsa_alt.c)