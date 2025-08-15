/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <platform_stdlib.h>
// These addresses are located right after VFS1
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
#define MATTER_FACTORY_DATA (0x08400000 - SPI_FLASH_BASE)
#elif defined(CONFIG_AMEBASMART)
#define MATTER_FACTORY_DATA (0x08644000 - SPI_FLASH_BASE)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <chip_porting.h>
#include <flash_api.h>
#include <ameba_factory.pb.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <device_lock.h>

#if defined(CONFIG_AMEBASMART) && defined(CONFIG_MATTER_SECURE) && CONFIG_MATTER_SECURE
#include <matter_rtk_svc_setup.h>
#endif

#if CONFIG_ENABLE_FACTORY_DATA_ENCRYPTION
#include <mbedtls/aes.h>
unsigned char test_key[] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
unsigned char test_iv[] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
mbedtls_aes_context aes_ctx;
#endif

static bool store_string_spake2_salt(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->cdata.spake2_salt.value, buffer, data_length);
    fdp->cdata.spake2_salt.len = data_length;

    return true;
}

static bool store_string_spake2_verifier(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->cdata.spake2_verifier.value, buffer, data_length);
    fdp->cdata.spake2_verifier.len = data_length;

    return true;
}

static bool store_string_dac_cert(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->dac.dac_cert.value, buffer, data_length);
    fdp->dac.dac_cert.len = data_length;

    return true;
}

static bool store_string_dac_key(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->dac.dac_key.value, buffer, data_length);
    fdp->dac.dac_key.len = data_length;

    return true;
}

static bool store_string_pai_cert(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->dac.pai_cert.value, buffer, data_length);
    fdp->dac.pai_cert.len = data_length;

    return true;
}

static bool store_string_cd(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->dac.cd.value, buffer, data_length);
    fdp->dac.cd.len = data_length;

    return true;
}

static bool store_string_vendor_name(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->dii.vendor_name.value, buffer, data_length);
    fdp->dii.vendor_name.len = data_length;

    return true;
}

static bool store_string_product_name(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->dii.product_name.value, buffer, data_length);
    fdp->dii.product_name.len = data_length;

    return true;
}

static bool store_string_hw_ver_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->dii.hw_ver_string.value, buffer, data_length);
    fdp->dii.hw_ver_string.len = data_length;

    return true;
}

static bool store_string_mfg_date(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->dii.mfg_date.value, buffer, data_length);
    fdp->dii.mfg_date.len = data_length;

    return true;
}

static bool store_string_serial_num(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->dii.serial_num.value, buffer, data_length);
    fdp->dii.serial_num.len = data_length;

    return true;
}

static bool store_string_rd_id_uid(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    FactoryData *fdp = *(FactoryData **) arg;
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    size_t data_length = stream->bytes_left;
    if (stream->bytes_left > sizeof(buffer) - 1) {
        return false;
    }

    if (!pb_read(stream, buffer, stream->bytes_left)) {
        return false;
    }

    memcpy(fdp->dii.rd_id_uid.value, buffer, data_length);
    fdp->dii.rd_id_uid.len = data_length;

    return true;
}

int32_t ReadFactory(uint8_t *buffer, uint16_t *pfactorydata_len)
{
    uint32_t ret;
    flash_t flash;
    uint32_t address = MATTER_FACTORY_DATA;
    uint8_t length_bytes = 2;

    // The first 2 bytes of the binary file is the length of the FactoryData
    ret = flash_stream_read(&flash, address, length_bytes, (uint8_t *)pfactorydata_len);

    // Check if factory data length is more than 4096
    // Which indicates that factory data is not flashed
    // Return to prevent reading from non-existent address
    if (*pfactorydata_len > 4096) {
        return -1;
    }

    // +2 offset to read the FactoryData
    ret = flash_stream_read(&flash, address + 2, *pfactorydata_len, buffer);

    return ret;
}

int32_t DecodeFactory(uint8_t *buffer, FactoryData *fdp, uint16_t data_len)
{
    int32_t ret = 1;
    pb_istream_t stream;
    FactoryDataProvider FDP = FactoryDataProvider_init_zero;

#if CONFIG_ENABLE_FACTORY_DATA_ENCRYPTION
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_enc(&aes_ctx, test_key, 256);
    // decrypt the factorydata
    size_t nc_off = 0;
    unsigned char nonce_counter[16] = {0};
    unsigned char stream_block[16] = {0};
    size_t iv_offset = 0;

    unsigned char *decrypted_output = (unsigned char *) pvPortMalloc(data_len);
    if (decrypted_output == NULL) {
        ret = -1;
        goto encrypt_exit;
    }

    memcpy(nonce_counter, test_iv, sizeof(nonce_counter));
    ret = mbedtls_aes_crypt_ctr(&aes_ctx, data_len, &nc_off, nonce_counter, stream_block, buffer, decrypted_output);
    if (ret != 0) {
        ret = -1;
        goto encrypt_exit;
    } else {
        ret = 1;
        memcpy(buffer, decrypted_output, data_len);
    }

encrypt_exit:

    vPortFree(decrypted_output);
    mbedtls_aes_free(&aes_ctx);

#endif /* CONFIG_ENABLE_FACTORY_DATA_ENCRYPTION */

    stream = pb_istream_from_buffer(buffer, data_len);

    // Set the callbacks for these fields
    // Once decoding is done, the decoded data will be passed to the callback functions and handled there
    // TODO: Combine all the callbacks into a single switch case callback
    FDP.cdata.spake2_salt.value.funcs.decode = &store_string_spake2_salt;
    FDP.cdata.spake2_verifier.value.funcs.decode = &store_string_spake2_verifier;
    FDP.dac.dac_cert.value.funcs.decode = &store_string_dac_cert;
    FDP.dac.dac_key.value.funcs.decode = &store_string_dac_key;
    FDP.dac.pai_cert.value.funcs.decode = &store_string_pai_cert;
    FDP.dac.cd.value.funcs.decode = &store_string_cd;
    FDP.dii.vendor_name.value.funcs.decode = &store_string_vendor_name;
    FDP.dii.product_name.value.funcs.decode = &store_string_product_name;
    FDP.dii.hw_ver_string.value.funcs.decode = &store_string_hw_ver_string;
    FDP.dii.mfg_date.value.funcs.decode = &store_string_mfg_date;
    FDP.dii.serial_num.value.funcs.decode = &store_string_serial_num;
    FDP.dii.rd_id_uid.value.funcs.decode = &store_string_rd_id_uid;

    // Pass in fdp as an argument to the callback
    FDP.cdata.spake2_salt.value.arg = fdp;
    FDP.cdata.spake2_verifier.value.arg = fdp;
    FDP.dac.dac_cert.value.arg = fdp;
    FDP.dac.dac_key.value.arg = fdp;
    FDP.dac.pai_cert.value.arg = fdp;
    FDP.dac.cd.value.arg = fdp;
    FDP.dii.vendor_name.value.arg = fdp;
    FDP.dii.product_name.value.arg = fdp;
    FDP.dii.hw_ver_string.value.arg = fdp;
    FDP.dii.mfg_date.value.arg = fdp;
    FDP.dii.serial_num.value.arg = fdp;
    FDP.dii.rd_id_uid.value.arg = fdp;

    if (!pb_decode(&stream, FactoryDataProvider_fields, &FDP)) {
        ret = -1;
        goto exit;
    }

    // We handle the integer fields here, don't need for callbacks
    fdp->cdata.passcode = FDP.cdata.passcode;
    fdp->cdata.discriminator = FDP.cdata.discriminator;
    fdp->cdata.spake2_it = FDP.cdata.spake2_it;
    fdp->dii.vendor_id = FDP.dii.vendor_id;
    fdp->dii.product_id = FDP.dii.product_id;
    fdp->dii.hw_ver = FDP.dii.hw_ver;

exit:
    return ret;
}

#if defined(CONFIG_MATTER_SECURE) && CONFIG_MATTER_SECURE
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
#define MATTER_SECURE_CONTEXT_STACK_SIZE 4096
extern int NS_ENTRY secure_mbedtls_platform_set_calloc_free(void);
extern int NS_ENTRY matter_secure_dac_init_keypair(uint8_t *pub_buf, size_t pub_size);
extern int NS_ENTRY matter_secure_ecdsa_sign_msg(matter_key_type key_type, const unsigned char *msg, size_t msg_size, unsigned char *signature);
extern int NS_ENTRY matter_secure_get_opkey(uint8_t *buf, size_t size);
extern int NS_ENTRY matter_secure_get_opkey_pub(uint8_t *pubkey, size_t pubkey_size);
extern int NS_ENTRY matter_secure_new_csr(uint8_t *out_csr, size_t csr_length);
extern int NS_ENTRY matter_secure_serialize(uint8_t *output_buf, size_t output_size);
extern void NS_ENTRY vMatterPrintSecureHeapStatus(void);
static bool matter_secure_context_created = false;

void matter_create_secure_context(void)
{
    if (matter_secure_context_created) {
        return;
    }

    rtos_create_secure_context(MATTER_SECURE_CONTEXT_STACK_SIZE);
    secure_mbedtls_platform_set_calloc_free();

    matter_secure_context_created = true;
}

#elif defined(CONFIG_AMEBASMART)

static bool matter_secure_mbedtls_initialized = false;
struct arm_smccc_res {
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
};
/**
 * struct arm_smccc_quirk - Contains quirk information
 * @id: quirk identification
 * @state: quirk specific information
 * @a6: Qualcomm quirk entry for returning post-smc call contents of a6
 */
struct arm_smccc_quirk {
    int	id;
    union {
        unsigned long a6;
    } state;
};
extern void __arm_smccc_smc(unsigned long a0, unsigned long a1,
                            unsigned long a2, unsigned long a3, unsigned long a4,
                            unsigned long a5, unsigned long a6, unsigned long a7,
                            struct arm_smccc_res *res, struct arm_smccc_quirk *quirk);

#define arm_smccc_smc(...) __arm_smccc_smc(__VA_ARGS__, NULL)
static unsigned long invoke_matter_secure_smc(
        unsigned long arg0, unsigned long arg1,
        unsigned long arg2, unsigned long arg3)
{
    struct arm_smccc_res res;
    printf("invoke_matter_secure_smc: matter_secure_function_id(0x%x)\n", arg0);
    arm_smccc_smc(RTK_SMC_MATTER_SECURE, arg0, arg1, arg2, arg3, 0, 0, 0, &res);
    printf("invoke_matter_secure_smc: res.a0(0x%x) \n", res.a0);
    return res.a0;
}

#define matter_secure_mbedtls_init(random_seed)                       invoke_matter_secure_smc(MATTER_SECURE_SMC_MBEDTLS_INIT, random_seed, 0, 0)
#define matter_secure_dac_init_keypair(pub_buf, pub_size)             invoke_matter_secure_smc(MATTER_SECURE_SMC_DAC_INIT_KEYPAIR, pub_buf, pub_size, 0)
#define matter_secure_dackey_ecdsa_sign_msg(msg, msg_size, signature) invoke_matter_secure_smc(MATTER_SECURE_SMC_DACKEY_ECDSA_SIGN_MSG, msg, msg_size, signature)
#define matter_secure_opkey_ecdsa_sign_msg(msg, msg_size, signature)  invoke_matter_secure_smc(MATTER_SECURE_SMC_OPKEY_ECDSA_SIGN_MSG, msg, msg_size, signature)
#define matter_secure_get_opkey(buf, size)                            invoke_matter_secure_smc(MATTER_SECURE_SMC_GET_OPKEY, buf, size, 0)
#define matter_secure_get_opkey_pub(pubkey, pubkey_size)              invoke_matter_secure_smc(MATTER_SECURE_SMC_GET_OPKEY_PUB, pubkey, pubkey_size, 0)
#define matter_secure_new_csr(out_csr, csr_length)                    invoke_matter_secure_smc(MATTER_SECURE_SMC_NEW_CSR, out_csr, csr_length, 0)
#define matter_secure_serialize(output_buf, output_size)              invoke_matter_secure_smc(MATTER_SECURE_SMC_SERIALIZE, output_buf, output_size, 0)

void matter_init_matter_secure_mbedtls(void)
{
    uint32_t random_seed;
    if (matter_secure_mbedtls_initialized) {
        return;
    }
    // Generate random seed for the secure world
    TRNG_get_random_bytes(&random_seed, sizeof(random_seed));
    matter_secure_mbedtls_init(random_seed);
    matter_secure_mbedtls_initialized = true;
}

void matter_secure_print_mbedtls_heap_status(void)
{
    if (!matter_secure_mbedtls_initialized) {
        matter_init_matter_secure_mbedtls();
    }
    invoke_matter_secure_smc(MATTER_SECURE_SMC_MBEDTLS_HEAP_STATUS, 0, 0, 0);
}

#endif

int matter_get_signature(uint8_t *pub_buf, size_t pub_size, const unsigned char *msg, size_t msg_size, unsigned char *signature)
{
    int result = 0;

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
    matter_create_secure_context();
#elif defined(CONFIG_AMEBASMART)
    matter_init_matter_secure_mbedtls();
#endif

    result = matter_secure_dac_init_keypair(pub_buf, pub_size);
    if (result != 0) {
        printf("Error: %s DAC init failed, result=%d\n", __FUNCTION__, result);
        goto exit;
    }

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
    result = matter_secure_ecdsa_sign_msg(MATTER_DACKEY_KEY_TYPE, msg, msg_size, signature);
#elif defined(CONFIG_AMEBASMART)
    result = matter_secure_dackey_ecdsa_sign_msg(msg, msg_size, signature);
#endif
    if (result != 0) {
        printf("Error: %s ecdsa sign failed, result=%d\n", __FUNCTION__, result);
        goto exit;
    }

exit:
    return result;
}

int matter_ecdsa_sign_msg(const unsigned char *msg, size_t msg_size, unsigned char *signature)
{
    int result = 0;

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
    result = matter_secure_ecdsa_sign_msg(MATTER_OPKEY_KEY_TYPE, msg, msg_size, signature);
#elif defined(CONFIG_AMEBASMART)
    result = matter_secure_opkey_ecdsa_sign_msg(msg, msg_size, signature);
#endif
    if (result != 0) {
        printf("ERROR: %s get signature failed, result=%d\n", __FUNCTION__, result);
    }

    return result;
}

int matter_get_publickey(uint8_t *pubkey, size_t pubkey_size)
{
    int result = 0;

    result = matter_secure_get_opkey_pub(pubkey, pubkey_size);
    if (result != 0) {
        printf("ERROR: %s get public key failed, result=%d\n", __FUNCTION__, result);
    }

    return result;
}

size_t matter_gen_new_csr(uint8_t *out_csr, size_t csr_length)
{
    int result = 0;

    result = matter_secure_new_csr(out_csr, csr_length);
    if (result <= 0) {
        printf("ERROR: %s create csr failed, return=%d\n", __FUNCTION__, result);
    }

    return result;
}

int matter_serialize(uint8_t *output_buf, size_t output_size)
{
    int result = 0;

    result = matter_secure_serialize(output_buf, output_size);
    if (result != 0) {
        printf("ERROR: %s serialize failed, result=%d\n", __FUNCTION__, result);
    }

    return result;
}

int matter_deserialize(uint8_t *buf, size_t size)
{
    int result = 0;

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
    matter_create_secure_context();
#elif defined(CONFIG_AMEBASMART)
    matter_init_matter_secure_mbedtls();
#endif

    result = matter_secure_get_opkey(buf, size);
    if (result != 0) {
        printf("ERROR: %s decrypt failed, result=%d\n", __FUNCTION__, result);
        return result;
    }

    return result;
}

void matter_check_secure_heap_status(void)
{
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
    vMatterPrintSecureHeapStatus();
#elif defined(CONFIG_AMEBASMART)
    matter_secure_print_mbedtls_heap_status();
#endif
}
#endif /* configENABLE_TRUSTZONE */

#ifdef __cplusplus
}
#endif
