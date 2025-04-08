#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#include <platform_opts.h>
#include <platform/platform_stdlib.h>
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include "platform_stdlib.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#include <dct.h>
#include <dct2.h>
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include "kv.h"
#endif

#include <chip_porting.h>

#if (CONFIG_ENABLE_DCT_ENCRYPTION) || (CONFIG_ENABLE_KV_ENCRYPTION)
#include <mbedtls/aes.h>
#endif

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#define DCT_BEGIN_ADDR_MATTER   MATTER_KVS_BEGIN_ADDR
#define MODULE_NUM              MATTER_KVS_MODULE_NUM
#define VARIABLE_NAME_SIZE      MATTER_KVS_VARIABLE_NAME_SIZE
#define VARIABLE_VALUE_SIZE     MATTER_KVS_VARIABLE_VALUE_SIZE

#define DCT_BEGIN_ADDR_MATTER2  MATTER_KVS_BEGIN_ADDR2
#define MODULE_NUM2             MATTER_KVS_MODULE_NUM2
#define VARIABLE_NAME_SIZE2     MATTER_KVS_VARIABLE_NAME_SIZE2
#define VARIABLE_VALUE_SIZE2    MATTER_KVS_VARIABLE_VALUE_SIZE2

#define ENABLE_BACKUP           MATTER_KVS_ENABLE_BACKUP
#define ENABLE_WEAR_LEVELING    MATTER_KVS_ENABLE_WEAR_LEVELING
#endif

#if (CONFIG_ENABLE_DCT_ENCRYPTION) || (CONFIG_ENABLE_KV_ENCRYPTION)
#if defined(MBEDTLS_CIPHER_MODE_CTR)
mbedtls_aes_context aes;

// key length 32 bytes for 256 bit encrypting, it can be 16 or 24 bytes for 128 and 192 bits encrypting mode
unsigned char key[] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#define DCT_REGION_1 0
#define DCT_REGION_2 1
#endif

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

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
int32_t dct_set_encrypted_variable(dct_handle_t *dct_handle, char *variable_name, char *variable_value, uint16_t variable_value_length, uint8_t region)
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
int32_t kv_set_encrypted_variable(char *variable_name, unsigned char *variable_value, int32_t variable_value_length)
#endif
{
    int32_t ret;
    char encrypted_data[VARIABLE_VALUE_SIZE2] = {0};

    // encrypt the variable value
    ret = kvs_encrypt(variable_value, variable_value_length, encrypted_data);
    if (ret != 0) {
        return MATTER_KVS_ERROR;
    }

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    // store into dct
    if (region == DCT_REGION_1) {
        ret = dct_set_variable_new(dct_handle, variable_name, encrypted_data, variable_value_length);
    } else if (region == DCT_REGION_2) {
        ret = dct_set_variable_new2(dct_handle, variable_name, encrypted_data, variable_value_length);
    }
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    // store into kv
    ret = rt_kv_set(variable_name, encrypted_data, variable_value_length);
#endif
    return ret;
}

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
int32_t dct_get_encrypted_variable(dct_handle_t *dct_handle, char *variable_name, char *buffer, uint16_t *buffer_size, uint8_t region)
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
int32_t kv_get_encrypted_variable(char *variable_name, unsigned char *buffer, int32_t buffer_size)
#endif
{
    int32_t ret;
    uint8_t encrypted_data[404] = {0};

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    // get the encrypted value from dct
    if (region == DCT_REGION_1) {
        ret = dct_get_variable_new(dct_handle, variable_name, encrypted_data, buffer_size);
    } else if (region == DCT_REGION_2) {
        ret = dct_get_variable_new2(dct_handle, variable_name, encrypted_data, buffer_size);
    }

    if (ret != DCT_SUCCESS) {
        return ret;
    }
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    // get the encrypted value from kv
    ret = rt_kv_get(variable_name, encrypted_data, buffer_size);
    if (ret <= 0) {
        return ret;
    }
#endif

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

#if defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
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
#endif

s32 initPref(void)
{
    s32 ret;
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    ret = dct_init(DCT_BEGIN_ADDR_MATTER, MODULE_NUM, VARIABLE_NAME_SIZE, VARIABLE_VALUE_SIZE, ENABLE_BACKUP, ENABLE_WEAR_LEVELING);
    if (ret != DCT_SUCCESS) {
        printf("dct_init failed with error: %d\n", ret);
    } else {
        printf("dct_init success\n");
    }

    ret = dct_init2(DCT_BEGIN_ADDR_MATTER2, MODULE_NUM2, VARIABLE_NAME_SIZE2, VARIABLE_VALUE_SIZE2, ENABLE_BACKUP, ENABLE_WEAR_LEVELING);
    if (ret != DCT_SUCCESS) {
        printf("dct_init2 failed with error: %d\n", ret);
    } else {
        printf("dct_init2 success\n");
    }
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    ret = rt_kv_init();
    if (ret < 0) {
        ret = MATTER_KVS_ERROR;
    } else {
        DiagPrintf("rt_kv_init success\r\n");
        ret = MATTER_KVS_SUCCESS;
    }
#endif

#if (CONFIG_ENABLE_DCT_ENCRYPTION) || (CONFIG_ENABLE_KV_ENCRYPTION)
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

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    ret = dct_format(DCT_BEGIN_ADDR_MATTER, MODULE_NUM, VARIABLE_NAME_SIZE, VARIABLE_VALUE_SIZE, ENABLE_BACKUP, ENABLE_WEAR_LEVELING);
    if (ret != DCT_SUCCESS) {
        printf("dct_init failed with error: %d\n", ret);
    } else {
        printf("dct_init success\n");
    }

    ret = dct_format2(DCT_BEGIN_ADDR_MATTER2, MODULE_NUM2, VARIABLE_NAME_SIZE2, VARIABLE_VALUE_SIZE2, ENABLE_BACKUP, ENABLE_WEAR_LEVELING);
    if (ret != DCT_SUCCESS) {
        printf("dct_init2 failed with error: %d\n", ret);
    } else {
        printf("dct_init2 success\n");
    }
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    ret = rt_kv_deinit();
    if (ret < 0) {
        ret = MATTER_KVS_ERROR;
    } else {
        DiagPrintf("deinitPref success\r\n");
        ret = MATTER_KVS_SUCCESS;
    }
#endif

#if (CONFIG_ENABLE_DCT_ENCRYPTION) || (CONFIG_ENABLE_KV_ENCRYPTION)
    // free aes context
    mbedtls_aes_free(&aes);
#endif

    return ret;
}

s32 registerPref(void)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    s32 ret;
    char ns[15];

    for (size_t i = 0; i < MODULE_NUM; i++) {
        snprintf(ns, 15, "matter_kvs1_%d", i + 1);

        ret = dct_register_module(ns);
        if (ret != DCT_SUCCESS) {
            printf("DCT1 modules registration failed\n");
            goto exit;
        } else {
            printf("dct_register_module %s success\n", ns);
        }
    }

exit:

    return ret;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    return MATTER_KVS_SUCCESS;
#endif
}

s32 registerPref2(void)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    s32 ret;
    char ns[15];

    for (size_t i = 0; i < MODULE_NUM2; i++) {
        snprintf(ns, 15, "matter_kvs2_%d", i + 1);

        ret = dct_register_module2(ns);
        if (ret != DCT_SUCCESS) {
            printf("DCT2 modules registration failed\n");
            goto exit;
        } else {
            printf("dct_register_module2 %s success\n", ns);
        }
    }

exit:

    return ret;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    return MATTER_KVS_SUCCESS;
#endif
}

s32 clearPref(void)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    s32 ret;
    char ns[15];

    for (size_t i = 0; i < MODULE_NUM; i++) {
        snprintf(ns, 15, "matter_kvs1_%d", i + 1);

        ret = dct_unregister_module(ns);
        if (ret != DCT_SUCCESS) {
            printf("DCT1 modules unregistration failed\n");
            goto exit;
        } else {
            printf("dct_unregister_module %s success\n", ns);
        }
    }

exit:

    return ret;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    return MATTER_KVS_SUCCESS;
#endif
}

s32 clearPref2(void)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    s32 ret;
    char ns[15];

    for (size_t i = 0; i < MODULE_NUM2; i++) {
        snprintf(ns, 15, "matter_kvs2_%d", i + 1);

        ret = dct_unregister_module2(ns);
        if (ret != DCT_SUCCESS) {
            printf("DCT2 modules unregistration failed\n");
            goto exit;
        } else {
            printf("dct_unregister_module2 %s success\n", ns);
        }
    }

exit:

    return ret;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    return MATTER_KVS_SUCCESS;
#endif
}

s32 deleteKey(const char *domain, const char *key)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    dct_handle_t handle;
    s32 ret;
    char ns[15];

    // Loop over DCT1 modules
    for (size_t i = 0; i < MODULE_NUM; i++) {
        snprintf(ns, 15, "matter_kvs1_%d", i + 1);

        ret = dct_open_module(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }

        ret = dct_delete_variable(&handle, (char *)key);
        if (ret == DCT_SUCCESS) { // return success once deleted
            dct_close_module(&handle);
            goto exit;
        }

        dct_close_module(&handle);
    }

    // Loop over DCT2 modules
    for (size_t i = 0; i < MODULE_NUM2; i++) {
        snprintf(ns, 15, "matter_kvs2_%d", i + 1);

        ret = dct_open_module2(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module2(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }

        ret = dct_delete_variable2(&handle, (char *)key);
        if (ret == DCT_SUCCESS) { // return success once deleted
            dct_close_module2(&handle);
            goto exit;
        }

        dct_close_module2(&handle);
    }

exit:

    return ret;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
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
#endif
}

bool checkExist(const char *domain, const char *key)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    dct_handle_t handle;
    s32 ret;
    uint16_t len = 0;
    char ns[15];

    u8 *str = malloc(sizeof(u8) * VARIABLE_VALUE_SIZE2); // use the bigger buffer size

    // Loop over DCT1 modules
    for (size_t i = 0; i < MODULE_NUM; i++) {
        snprintf(ns, 15, "matter_kvs1_%d", i + 1);

        ret = dct_open_module(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }

        len = sizeof(u32);
        ret = dct_get_variable_new(&handle, (char *)key, (char *)str, &len);
        if (ret == DCT_SUCCESS) {
            printf("checkExist key=%s found.\n", key);
            dct_close_module(&handle);
            goto exit;
        }

        len = sizeof(u64);
        ret = dct_get_variable_new(&handle, (char *)key, (char *)str, &len);
        if (ret == DCT_SUCCESS) {
            printf("checkExist key=%s found.\n", key);
            dct_close_module(&handle);
            goto exit;
        }

        dct_close_module(&handle);
    }

    // Loop over DCT2 modules
    for (size_t i = 0; i < MODULE_NUM2; i++) {
        snprintf(ns, 15, "matter_kvs2_%d", i + 1);

        ret = dct_open_module2(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module2(%s) failed with error : %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }

        len = VARIABLE_VALUE_SIZE2;
        ret = dct_get_variable_new2(&handle, (char *)key, (char *)str, &len);
        if (ret == DCT_SUCCESS) {
            printf("checkExist key=%s found.\n", key);
            dct_close_module2(&handle);
            goto exit;
        }

        dct_close_module2(&handle);
    }

exit:

    free(str);
    return (ret == DCT_SUCCESS) ? true : false;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    s32 ret, keyLen = strlen(key);
    char *tempKey;

    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
    ret = rt_kv_get_length(tempKey);

    if (ret > 0) {
        DiagPrintf("checkExist key=%s found.\n", tempKey);
    }

    free(tempKey);
    return (ret > 0) ? true : false;
#endif
}

s32 setPref_new(const char *domain, const char *key, u8 *value, size_t byteCount)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    dct_handle_t handle;
    s32 ret;
    char ns[15];

    if (byteCount <= 64) {
        // Loop over DCT1 modules
        for (size_t i = 0; i < MODULE_NUM; i++) {
            snprintf(ns, 15, "matter_kvs1_%d", i + 1);

            ret = dct_open_module(&handle, ns);
            if (ret != DCT_SUCCESS) {
                printf("%s : dct_open_module(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
                goto exit;
            }

            if (dct_remain_variable(&handle) > 0) {
#if CONFIG_ENABLE_DCT_ENCRYPTION
                ret = dct_set_encrypted_variable(&handle, (char *)key, value, byteCount, DCT_REGION_1);
#else
                ret = dct_set_variable_new(&handle, (char *)key, (char *)value, (uint16_t)byteCount);
#endif
                if (ret != DCT_SUCCESS) {
                    printf("%s : dct_set_variable(%s) failed with error: %d\n", __FUNCTION__, key, ret);
                    dct_close_module(&handle);
                    goto exit;
                }
                dct_close_module(&handle);
                break;
            }
            dct_close_module(&handle);
        }
    } else {
        // Loop over DCT2 modules
        for (size_t i = 0; i < MODULE_NUM2; i++) {
            snprintf(ns, 15, "matter_kvs2_%d", i + 1);

            ret = dct_open_module2(&handle, ns);
            if (ret != DCT_SUCCESS) {
                printf("%s : dct_open_module2(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
                goto exit;
            }

            if (dct_remain_variable2(&handle) > 0) {
#if CONFIG_ENABLE_DCT_ENCRYPTION
                ret = dct_set_encrypted_variable(&handle, (char *)key, value, byteCount, DCT_REGION_2);
#else
                ret = dct_set_variable_new2(&handle, (char *)key, (char *)value, (uint16_t)byteCount);
#endif
                if (ret != DCT_SUCCESS) {
                    printf("%s : dct_set_variable2(%s) failed with error: %d\n", __FUNCTION__, key, ret);
                    dct_close_module2(&handle);
                    goto exit;
                }
                dct_close_module2(&handle);
                break;
            }
            dct_close_module2(&handle);
        }
    }

exit:

    return ret;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
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
#endif
}

s32 getPref_bool_new(const char *domain, const char *key, u8 *val)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    dct_handle_t handle;
    s32 ret;
    uint16_t len = sizeof(u8);
    char ns[15];

    // Loop over DCT1 modules
    for (size_t i = 0; i < MODULE_NUM; i++) {
        snprintf(ns, 15, "matter_kvs1_%d", i + 1);

        ret = dct_open_module(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }
#if CONFIG_ENABLE_DCT_ENCRYPTION
        ret = dct_get_encrypted_variable(&handle, (char *)key, (char *)val, &len, DCT_REGION_1);
#else
        ret = dct_get_variable_new(&handle, (char *)key, (char *)val, &len);
#endif
        if (ret == DCT_SUCCESS) {
            dct_close_module(&handle);
            goto exit;
        }
        dct_close_module(&handle);
    }

    // Loop over DCT2 modules
    for (size_t i = 0; i < MODULE_NUM2; i++) {
        snprintf(ns, 15, "matter_kvs2_%d", i + 1);
        ret = dct_open_module2(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module2(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }
#if CONFIG_ENABLE_DCT_ENCRYPTION
        ret = dct_get_encrypted_variable(&handle, (char *)key, (char *)val, &len, DCT_REGION_2);
#else
        ret = dct_get_variable_new2(&handle, (char *)key, (char *)val, &len);
#endif
        if (ret == DCT_SUCCESS) {
            dct_close_module2(&handle);
            goto exit;
        }
        dct_close_module2(&handle);
    }

exit:
    return ret;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
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
#endif
}

s32 getPref_u32_new(const char *domain, const char *key, u32 *val)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    dct_handle_t handle;
    s32 ret;
    uint16_t len = sizeof(u32);
    char ns[15];

    // Loop over DCT1 modules
    for (size_t i = 0; i < MODULE_NUM; i++) {
        snprintf(ns, 15, "matter_kvs1_%d", i + 1);

        ret = dct_open_module(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }
#if CONFIG_ENABLE_DCT_ENCRYPTION
        ret = dct_get_encrypted_variable(&handle, (char *)key, (char *)val, &len, DCT_REGION_1);
#else
        ret = dct_get_variable_new(&handle, (char *)key, (char *)val, &len);
#endif
        if (ret == DCT_SUCCESS) {
            dct_close_module(&handle);
            goto exit;
        }
        dct_close_module(&handle);
    }

    // Loop over DCT2 modules
    for (size_t i = 0; i < MODULE_NUM2; i++) {
        snprintf(ns, 15, "matter_kvs2_%d", i + 1);

        ret = dct_open_module2(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module2(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }
#if CONFIG_ENABLE_DCT_ENCRYPTION
        ret = dct_get_encrypted_variable(&handle, (char *)key, (char *)val, &len, DCT_REGION_2);
#else
        ret = dct_get_variable_new2(&handle, (char *)key, (char *)val, &len);
#endif
        if (ret == DCT_SUCCESS) {
            dct_close_module2(&handle);
            goto exit;
        }
        dct_close_module2(&handle);
    }

exit:
    return ret;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
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
#endif
}

s32 getPref_u64_new(const char *domain, const char *key, u64 *val)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    dct_handle_t handle;
    s32 ret;
    uint16_t len = sizeof(u64);
    char ns[15];

    // Loop over DCT1 modules
    for (size_t i = 0; i < MODULE_NUM; i++) {
        snprintf(ns, 15, "matter_kvs1_%d", i + 1);

        ret = dct_open_module(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }
#if CONFIG_ENABLE_DCT_ENCRYPTION
        ret = dct_get_encrypted_variable(&handle, (char *)key, (char *)val, &len, DCT_REGION_1);
#else
        ret = dct_get_variable_new(&handle, (char *)key, (char *)val, &len);
#endif
        if (ret == DCT_SUCCESS) {
            dct_close_module(&handle);
            goto exit;
        }
        dct_close_module(&handle);
    }

    // Loop over DCT2 modules
    for (size_t i = 0; i < MODULE_NUM2; i++) {
        snprintf(ns, 15, "matter_kvs2_%d", i + 1);

        ret = dct_open_module2(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module2(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }
#if CONFIG_ENABLE_DCT_ENCRYPTION
        ret = dct_get_encrypted_variable(&handle, (char *)key, (char *)val, &len, DCT_REGION_2);
#else
        ret = dct_get_variable_new2(&handle, (char *)key, (char *)val, &len);
#endif
        if (ret == DCT_SUCCESS) {
            dct_close_module2(&handle);
            goto exit;
        }
        dct_close_module2(&handle);
    }

exit:
    return ret;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
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
#endif
}

s32 getPref_str_new(const char *domain, const char *key, char *buf, size_t bufSize, size_t *outLen)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    dct_handle_t handle;
    s32 ret;
    char ns[15];
    uint16_t *len = (uint16_t *)(&bufSize);

    // Loop over DCT1 modules
    for (size_t i = 0; i < MODULE_NUM; i++) {
        snprintf(ns, 15, "matter_kvs1_%d", i + 1);

        ret = dct_open_module(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }
#if CONFIG_ENABLE_DCT_ENCRYPTION
        ret = dct_get_encrypted_variable(&handle, (char *)key, buf, len, DCT_REGION_1);
#else
        ret = dct_get_variable_new(&handle, (char *)key, buf, len);
#endif
        if (ret == DCT_SUCCESS) {
            dct_close_module(&handle);
            *outLen = bufSize;
            goto exit;
        }
        dct_close_module(&handle);
    }

    // Loop over DCT2 modules
    for (size_t i = 0; i < MODULE_NUM2; i++) {
        snprintf(ns, 15, "matter_kvs2_%d", i + 1);

        ret = dct_open_module2(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module2(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }
#if CONFIG_ENABLE_DCT_ENCRYPTION
        ret = dct_get_encrypted_variable(&handle, (char *)key, buf, len, DCT_REGION_2);
#else
        ret = dct_get_variable_new2(&handle, (char *)key, buf, len);
#endif
        if (ret == DCT_SUCCESS) {
            dct_close_module2(&handle);
            *outLen = bufSize;
            goto exit;
        }
        dct_close_module2(&handle);
    }

exit:
    return ret;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    s32 ret, keyLen = strlen(key);
    char *tempKey;

    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
    memset(buf, 0, bufSize);
    *outLen = (size_t)rt_kv_get_length(tempKey);
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
#endif
}

s32 getPref_bin_new(const char *domain, const char *key, u8 *buf, size_t bufSize, size_t *outLen)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    dct_handle_t handle;
    s32 ret;
    char ns[15];
    uint16_t *len = (uint16_t *)(&bufSize);

    // Loop over DCT1 modules
    for (size_t i = 0; i < MODULE_NUM; i++) {
        snprintf(ns, 15, "matter_kvs1_%d", i + 1);

        ret = dct_open_module(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }
#if CONFIG_ENABLE_DCT_ENCRYPTION
        ret = dct_get_encrypted_variable(&handle, (char *)key, (char *)buf, len, DCT_REGION_1);
#else
        ret = dct_get_variable_new(&handle, (char *)key, (char *)buf, len);
#endif
        if (ret == DCT_SUCCESS) {
            dct_close_module(&handle);
            *outLen = bufSize;
            goto exit;
        }
        dct_close_module(&handle);
    }

    // Loop over DCT2 modules
    for (size_t i = 0; i < MODULE_NUM2; i++) {
        snprintf(ns, 15, "matter_kvs2_%d", i + 1);

        ret = dct_open_module2(&handle, ns);
        if (ret != DCT_SUCCESS) {
            printf("%s : dct_open_module2(%s) failed with error: %d\n", __FUNCTION__, ns, ret);
            goto exit;
        }
#if CONFIG_ENABLE_DCT_ENCRYPTION
        ret = dct_get_encrypted_variable(&handle, (char *)key, (char *)buf, len, DCT_REGION_2);
#else
        ret = dct_get_variable_new2(&handle, (char *)key, (char *)buf, len);
#endif
        if (ret == DCT_SUCCESS) {
            dct_close_module2(&handle);
            *outLen = bufSize;
            goto exit;
        }
        dct_close_module2(&handle);
    }

exit:
    return ret;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    s32 ret, keyLen = strlen(key);
    char *tempKey;

    tempKey = malloc(keyLen + 1);
    modifyKey(tempKey, key, keyLen);
    memset(buf, 0, bufSize);
    *outLen = (size_t)rt_kv_get_length(tempKey);
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
#endif
}

#ifdef __cplusplus
}
#endif
