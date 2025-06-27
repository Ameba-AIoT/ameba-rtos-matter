#include "platform_stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "kv.h"
#include "matter_kv.h"

#include <chip_porting.h>

#if (CONFIG_ENABLE_KV_ENCRYPTION)
#if defined(MBEDTLS_CIPHER_MODE_CTR)

#include <mbedtls/aes.h>

mbedtls_aes_context aes;

// key length 32 bytes for 256 bit encrypting, it can be 16 or 24 bytes for 128 and 192 bits encrypting mode
unsigned char key[] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

int32_t kvs_encrypt(unsigned char *input_to_encrypt, int input_len, unsigned char *encrypt_output)
{
    size_t nc_off = 0;

    unsigned char nonce_counter[16] = {0};
    unsigned char stream_block[16] = {0};

    return mbedtls_aes_crypt_ctr(&aes, input_len, &nc_off, nonce_counter, stream_block, input_to_encrypt, encrypt_output);
}

int32_t kvs_decrypt(unsigned char *input_to_decrypt, int input_len, unsigned char *decrypt_output)
{
    size_t nc_off1 = 0;
    unsigned char nonce_counter1[16] = {0};
    unsigned char stream_block1[16] = {0};

    return mbedtls_aes_crypt_ctr(&aes, input_len, &nc_off1, nonce_counter1, stream_block1, input_to_decrypt, decrypt_output);
}

int32_t kv_set_encrypted_variable(char *variable_name, unsigned char *variable_value, int32_t variable_value_length)
{
    int32_t ret;
    char encrypted_data[VARIABLE_VALUE_SIZE2] = {0};

    // encrypt the variable value
    ret = kvs_encrypt(variable_value, variable_value_length, encrypted_data);
    if (ret != 0) {
        return MATTER_KVS_ERROR;
    }

    // store into kv
    ret = rt_kv_set(variable_name, encrypted_data, variable_value_length);
    return ret;
}

int32_t kv_get_encrypted_variable(char *variable_name, unsigned char *buffer, int32_t buffer_size)
{
    int32_t ret;
    uint8_t encrypted_data[404] = {0};

    // get the encrypted value from kv
    ret = rt_kv_get(variable_name, encrypted_data, buffer_size);
    if (ret <= 0) {
        return ret;
    }

    // decrypt the encrypted value
    ret = kvs_decrypt(encrypted_data, *buffer_size, buffer);
    if (ret != 0) {
        return MATTER_KVS_ERROR;
    }

    return ret;
}

#else
#error "MBEDTLS_CIPHER_MODE_CTR must be enabled to perform KVS flash encryption"
#endif /* MBEDTLS_CIPHER_MODE_CTR */
#endif

void modifyKey(char *tempKey, const char *key, int keyLen)
{
    memcpy(tempKey, key, keyLen);
    tempKey[keyLen] = '\0';
    for (int i = 0; i < keyLen; i++) { // substitute slash with dash, slash will cause error during assignment
        if (tempKey[i] == '/') {
            tempKey[i] = '-';
        }
    }
}

s32 initPref(void)
{
    s32 ret;
    ret = rt_kv_init();
    if (ret < 0) {
        ret = MATTER_KVS_ERROR;
    } else {
        DiagPrintf("rt_kv_init success\r\n");
        ret = MATTER_KVS_SUCCESS;
    }

#if (CONFIG_ENABLE_KV_ENCRYPTION)
    // Initialize mbedtls aes context and set encryption key
    mbedtls_aes_init(&aes);
    if (mbedtls_aes_setkey_enc(&aes, key, 256) != 0) {
        return MATTER_KVS_ERROR;
    }
#endif

    return ret;
}

s32 deinitPref(void)
{
    s32 ret;

    ret = rt_kv_deinit();
    if (ret < 0) {
        ret = MATTER_KVS_ERROR;
    } else {
        DiagPrintf("deinitPref success\r\n");
        ret = MATTER_KVS_SUCCESS;
    }

#if (CONFIG_ENABLE_KV_ENCRYPTION)
    // free aes context
    mbedtls_aes_free(&aes);
#endif

    return ret;
}

s32 registerPref(void)
{
    return MATTER_KVS_SUCCESS;
}

s32 registerPref2(void)
{
    return MATTER_KVS_SUCCESS;
}

s32 clearPref(void)
{
    return MATTER_KVS_SUCCESS;
}

s32 clearPref2(void)
{
    return MATTER_KVS_SUCCESS;
}

s32 deleteKey(const char *domain, const char *key)
{
    s32 ret, keyLen = strlen(key);
    char *tempKey;

    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
    ret = rt_kv_delete(tempKey);

    if (ret != 0) { //remove success code is 0
        if (ret == -2) {
            ret = MATTER_KVS_ERR_NOT_FIND;
        } else {
            ret = MATTER_KVS_ERROR;
        }
    } else {
        DiagPrintf("deleteKey %s success.\r\n", tempKey);
        ret = MATTER_KVS_SUCCESS;
    }

    free(tempKey);
    return ret;
}

bool checkExist(const char *domain, const char *key)
{
    s32 ret, keyLen = strlen(key);
    char *tempKey;

    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
    ret = rt_kv_size(tempKey);

    if (ret > 0) {
        DiagPrintf("checkExist key=%s found.\n", tempKey);
    }

    free(tempKey);
    return (ret > 0) ? true : false;
}

s32 setPref_new(const char *domain, const char *key, u8 *value, size_t byteCount)
{
    s32 ret, keyLen = strlen(key);
    char *tempKey;

    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
#if CONFIG_ENABLE_KV_ENCRYPTION
    ret = kv_set_encrypted_variable(tempKey, value, (int32_t)byteCount);
#else
    ret = rt_kv_set(tempKey, value, (int32_t)byteCount);
#endif

    if (ret <= 0) { //0 is inclusive because 0 bytes were written
        ret = MATTER_KVS_ERROR;
    } else {
        DiagPrintf("setPref_new %s success, write %d bytes.\r\n", tempKey, byteCount);
        ret = MATTER_KVS_SUCCESS;
    }

    free(tempKey);
    return ret;
}

s32 getPref_bool_new(const char *domain, const char *key, u8 *val)
{
    s32 ret, keyLen = strlen(key);
    char *tempKey;

    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
#if CONFIG_ENABLE_KV_ENCRYPTION
    ret = kv_get_encrypted_variable(tempKey, val, (int32_t)sizeof(u8));
#else
    ret = rt_kv_get(tempKey, val, (int32_t)sizeof(u8));
#endif

    if (ret <= 0) { //0 is inclusive because 0 bytes were read
        if (ret == -1) {
            ret = MATTER_KVS_ERR_NOT_FIND;
        } else {
            ret = MATTER_KVS_ERROR;
        }
    } else {
        DiagPrintf("getPref_bool_new %s success, read %d bytes.\r\n", tempKey, sizeof(u8));
        ret = MATTER_KVS_SUCCESS;
    }

    free(tempKey);
    return ret;
}

s32 getPref_u32_new(const char *domain, const char *key, u32 *val)
{
    s32 ret, keyLen = strlen(key);
    char *tempKey;

    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
#if CONFIG_ENABLE_KV_ENCRYPTION
    ret = kv_get_encrypted_variable(tempKey, (uint8_t *)val, (int32_t)sizeof(u32));
#else
    ret = rt_kv_get(tempKey, val, (int32_t)sizeof(u32));
#endif

    if (ret <= 0) { //0 is inclusive because 0 bytes were read
        if (ret == -1) {
            ret = MATTER_KVS_ERR_NOT_FIND;
        } else {
            ret = MATTER_KVS_ERROR;
        }
    } else {
        DiagPrintf("getPref_u32_new %s success, read %d bytes.\r\n", tempKey, sizeof(u32));
        ret = MATTER_KVS_SUCCESS;
    }

    free(tempKey);
    return ret;
}

s32 getPref_u64_new(const char *domain, const char *key, u64 *val)
{
    s32 ret, keyLen = strlen(key);
    char *tempKey;

    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
#if CONFIG_ENABLE_KV_ENCRYPTION
    ret = kv_get_encrypted_variable(tempKey, (uint8_t *)val, (int32_t)sizeof(u64));
#else
    ret = rt_kv_get(tempKey, val, (int32_t)sizeof(u64));
#endif

    if (ret <= 0) { //0 is inclusive because 0 bytes were read
        if (ret == -1) {
            ret = MATTER_KVS_ERR_NOT_FIND;
        } else {
            ret = MATTER_KVS_ERROR;
        }
    } else {
        DiagPrintf("getPref_u64_new %s success, read %d bytes.\r\n", tempKey, sizeof(u64));
        ret = MATTER_KVS_SUCCESS;
    }

    free(tempKey);
    return ret;
}

s32 getPref_str_new(const char *domain, const char *key, char *buf, size_t bufSize, size_t *outLen)
{
    s32 ret, keyLen = strlen(key);
    char *tempKey;

    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
    memset(buf, 0, bufSize);
    *outLen = (size_t)rt_kv_size(tempKey);
#if CONFIG_ENABLE_KV_ENCRYPTION
    ret = kv_get_encrypted_variable(tempKey, (uint8_t *)buf, (int32_t) * outLen);
#else
    ret = rt_kv_get(tempKey, buf, (int32_t) * outLen);
#endif

    if (ret <= 0) { //0 is inclusive because 0 bytes were read
        if (ret == -1) {
            ret = MATTER_KVS_ERR_NOT_FIND;
        } else {
            ret = MATTER_KVS_ERROR;
        }
    } else {
        DiagPrintf("getPref_str_new %s success, read %d bytes.\r\n", tempKey, bufSize);
        ret = MATTER_KVS_SUCCESS;
    }

    free(tempKey);
    return ret;
}

s32 getPref_bin_new(const char *domain, const char *key, u8 *buf, size_t bufSize, size_t *outLen)
{
    s32 ret, keyLen = strlen(key);
    char *tempKey;

    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
    memset(buf, 0, bufSize);
    *outLen = (size_t)rt_kv_size(tempKey);
#if CONFIG_ENABLE_KV_ENCRYPTION
    ret = kv_get_encrypted_variable(tempKey, buf, (int32_t) * outLen);
#else
    ret = rt_kv_get(tempKey, buf, (int32_t) * outLen);
#endif

    if (ret <= 0) { //0 is inclusive because 0 bytes were read
        if (ret == -1) {
            ret = MATTER_KVS_ERR_NOT_FIND;
        } else {
            ret = MATTER_KVS_ERROR;
        }
    } else {
        DiagPrintf("getPref_bin_new %s success, read %d bytes.\r\n", tempKey, bufSize);
        ret = MATTER_KVS_SUCCESS;
    }

    free(tempKey);
    return ret;
}

#ifdef __cplusplus
}
#endif
