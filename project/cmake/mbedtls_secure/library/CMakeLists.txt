set(DIR        ${MBEDTLSDIR})
set(DIR_RAMMAP ${BASEDIR}/component/ssl/mbedtls_ram_map)
set(DIR_FWLIB  ${BASEDIR}/component/soc/amebadplus/fwlib/ram_km4)

set(MODULE_C_DEFINES CONFIG_BUILD_SECURE=1)

set(MODULE_IFLAGS
    ${BASEDIR}/component/ssl/mbedtls_ram_map/rom
    ${BASEDIR}/component/soc/common/crashdump/include
)
set(
    CSRC
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
    # ${DIR}/library/rsa_internal.c
    ${DIR}/library/sha1.c
    ${DIR}/library/sha256.c
    ${DIR}/library/sha512.c
    ${DIR}/library/ssl_tls.c
    ${DIR}/library/x509_create.c
    ${DIR}/library/x509write_csr.c
    ${DIR_RAMMAP}/rom/rom_ssl_ram_map.c
    ${DIR_FWLIB}/ameba_app_start.c
)

set_source_files_properties(${CSRC} DIRECTORY ${PROJECTDIR}/asdk/make_secure PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
set_source_files_properties(${CSRC} DIRECTORY ${PROJECTDIR}/asdk/make_secure PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")
target_sources(target_img3 PRIVATE ${CSRC})