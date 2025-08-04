#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
#include "cmsis.h"
#include "platform_stdlib.h"
#include "ameba_crypto_api.h"
#include "FreeRTOS.h"
#endif
#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#else
#include "mbedtls/config.h"
#endif
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"
#include "mbedtls/pk.h"
#include "mbedtls/version.h"

#if defined(CONFIG_SSL_CLIENT_PRIVATE_IN_TZ) && (CONFIG_SSL_CLIENT_PRIVATE_IN_TZ == 1)
extern const char *client_key_s;
#endif

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)

static void *_calloc(size_t count, size_t size)
{
    void *ptr = pvPortMalloc(count * size);
    if (ptr)	{
        memset(ptr, 0, count * size);
    }
    return ptr;
}

#define _free		vPortFree

#elif defined(CONFIG_AMEBASMART)

#define _calloc mbedtls_calloc
#define _free   mbedtls_free

#endif //defined(CONFIG_BUILD_SECURE)

static int _random(void *p_rng, unsigned char *output, size_t output_len)
{
    /* To avoid gcc warnings */
    (void) p_rng;

    static unsigned int seed = 0;
    if (seed == 0) {
        extern u32 RandSeedTZ;
        seed = RandSeedTZ;
        srand(seed);
    }

    int rand_num = 0;
    while (output_len) {
        int r = rand();
        if (output_len > sizeof(int)) {
            memcpy(&output[rand_num], &r, sizeof(int));
            rand_num += sizeof(int);
            output_len -= sizeof(int);
        } else {
            memcpy(&output[rand_num], &r, output_len);
            rand_num += output_len;
            output_len = 0;
        }
    }

    return 0;
}

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
#if defined(__ICCARM__)
void (__cmse_nonsecure_call *ns_device_mutex_lock)(uint32_t) = NULL;
void (__cmse_nonsecure_call *ns_device_mutex_unlock)(uint32_t) = NULL;
#else
void __attribute__((cmse_nonsecure_call))(*ns_device_mutex_lock)(uint32_t) = NULL;
void __attribute__((cmse_nonsecure_call))(*ns_device_mutex_unlock)(uint32_t) = NULL;
#endif

IMAGE3_ENTRY_SECTION
void NS_ENTRY secure_set_ns_device_lock(
    void (*device_mutex_lock_func)(uint32_t),
    void (*device_mutex_unlock_func)(uint32_t))
{
#if defined(__ICCARM__)
    ns_device_mutex_lock = cmse_nsfptr_create((void (__cmse_nonsecure_call *)(uint32_t)) device_mutex_lock_func);
    ns_device_mutex_unlock = cmse_nsfptr_create((void (__cmse_nonsecure_call *)(uint32_t)) device_mutex_unlock_func);
#else
    ns_device_mutex_lock = cmse_nsfptr_create((void __attribute__((cmse_nonsecure_call))(*)(uint32_t)) device_mutex_lock_func);
    ns_device_mutex_unlock = cmse_nsfptr_create((void __attribute__((cmse_nonsecure_call))(*)(uint32_t)) device_mutex_unlock_func);
#endif
}

IMAGE3_ENTRY_SECTION
int NS_ENTRY secure_mbedtls_platform_set_calloc_free(void)
{
#if defined(MBEDTLS_VERSION_NUMBER) && ( MBEDTLS_VERSION_NUMBER==0x02100300 || MBEDTLS_VERSION_NUMBER==0x021C0000)
    mbedtls_platform_setup(NULL);
#endif
    return 	mbedtls_platform_set_calloc_free(_calloc, _free);
}
#endif

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
IMAGE3_ENTRY_SECTION
void NS_ENTRY secure_mbedtls_ssl_conf_rng(mbedtls_ssl_config *conf, void *p_rng)
#elif defined(CONFIG_AMEBASMART)
void secure_mbedtls_ssl_conf_rng(mbedtls_ssl_config *conf, void *p_rng)
#endif
{
    mbedtls_ssl_conf_rng(conf, _random, p_rng);
}

#if defined(CONFIG_SSL_CLIENT_PRIVATE_IN_TZ) && (CONFIG_SSL_CLIENT_PRIVATE_IN_TZ == 1)
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
IMAGE3_ENTRY_SECTION
uint8_t *NS_ENTRY secure_mbedtls_pk_parse_key(void)
#elif defined(CONFIG_AMEBASMART)
uint8_t *secure_mbedtls_pk_parse_key(void)
#endif
{

    mbedtls_pk_context *client_pk = (mbedtls_pk_context *) mbedtls_calloc(1, sizeof(mbedtls_pk_context));

    if (client_pk) {
        mbedtls_pk_init(client_pk);

        if (mbedtls_pk_parse_key(client_pk, (unsigned char const *)client_key_s, strlen(client_key_s) + 1, NULL, 0) != 0) {
            printf("\n\r ERROR: mbedtls_pk_parse_key \n\r");
            goto error;
        }
    } else {
        printf("\n\r ERROR: mbedtls_calloc \n\r");
        goto error;
    }

    return client_pk;
error:
    if (client_pk) {
        mbedtls_pk_free(client_pk);
        mbedtls_free(client_pk);
    }

    return NULL;
}
#endif

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
IMAGE3_ENTRY_SECTION
void NS_ENTRY secure_mbedtls_pk_free(mbedtls_pk_context *pk)
#elif defined(CONFIG_AMEBASMART)
void secure_mbedtls_pk_free(mbedtls_pk_context *pk)
#endif
{
    mbedtls_pk_free(pk);
    mbedtls_free(pk);
}

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
IMAGE3_ENTRY_SECTION
int NS_ENTRY secure_mbedtls_pk_can_do(const mbedtls_pk_context *ctx, mbedtls_pk_type_t type)
#elif defined(CONFIG_AMEBASMART)
int secure_mbedtls_pk_can_do(const mbedtls_pk_context *ctx, mbedtls_pk_type_t type)
#endif
{
    return mbedtls_pk_can_do(ctx, type);
}

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
IMAGE3_ENTRY_SECTION
unsigned char NS_ENTRY secure_mbedtls_ssl_sig_from_pk(mbedtls_pk_context *pk)
#elif defined(CONFIG_AMEBASMART)
unsigned char secure_mbedtls_ssl_sig_from_pk(mbedtls_pk_context *pk)
#endif
{
#if defined(MBEDTLS_RSA_C)
    if (mbedtls_pk_can_do(pk, MBEDTLS_PK_RSA)) {
        return (MBEDTLS_SSL_SIG_RSA);
    }
#endif
#if defined(MBEDTLS_ECDSA_C)
    if (mbedtls_pk_can_do(pk, MBEDTLS_PK_ECDSA)) {
        return (MBEDTLS_SSL_SIG_ECDSA);
    }
#endif
    return (MBEDTLS_SSL_SIG_ANON);
}

struct secure_mbedtls_pk_sign_param {
    mbedtls_pk_context *ctx;
    mbedtls_md_type_t md_alg;
    unsigned char *hash;
    size_t hash_len;
    unsigned char *sig;
    size_t *sig_len;
    int (*f_rng)(void *, unsigned char *, size_t);
    void *p_rng;
};

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
IMAGE3_ENTRY_SECTION
int NS_ENTRY secure_mbedtls_pk_sign(struct secure_mbedtls_pk_sign_param *param)
#elif defined(CONFIG_AMEBASMART)
int secure_mbedtls_pk_sign(struct secure_mbedtls_pk_sign_param *param)
#endif
{

    return mbedtls_pk_sign(param->ctx, param->md_alg, param->hash, param->hash_len,
                           param->sig, param->sig_len, _random, param->p_rng);
}

#if defined(ENABLE_AMAZON_COMMON)
#include "mbedtls/threading_alt.h"
#include "mbedtls/threading.h"

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBALITE)
IMAGE3_ENTRY_SECTION
mbedtls_pk_type_t NS_ENTRY secure_mbedtls_pk_get_type(const mbedtls_pk_context *ctx)
#elif defined(CONFIG_AMEBASMART)
mbedtls_pk_type_t secure_mbedtls_pk_get_type(const mbedtls_pk_context *ctx)
#endif
{
    return mbedtls_pk_get_type(ctx);
}

void s_mbedtls_mutex_init(mbedtls_threading_mutex_t *mutex)
{

}

void s_mbedtls_mutex_free(mbedtls_threading_mutex_t *mutex)
{

}

int s_mbedtls_mutex_lock(mbedtls_threading_mutex_t *mutex)
{
    __disable_irq();
    return 0;
}

int s_mbedtls_mutex_unlock(mbedtls_threading_mutex_t *mutex)
{
    __enable_irq();
    return 0;
}

void (*mbedtls_mutex_init)(mbedtls_threading_mutex_t *) = s_mbedtls_mutex_init;
void (*mbedtls_mutex_free)(mbedtls_threading_mutex_t *) = s_mbedtls_mutex_free;
int (*mbedtls_mutex_lock)(mbedtls_threading_mutex_t *) = s_mbedtls_mutex_lock;
int (*mbedtls_mutex_unlock)(mbedtls_threading_mutex_t *) = s_mbedtls_mutex_unlock;
#endif
