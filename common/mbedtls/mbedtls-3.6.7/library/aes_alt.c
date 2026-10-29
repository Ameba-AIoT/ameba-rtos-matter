/*
 *  FIPS-197 compliant AES implementation
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */
/*
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  https://csrc.nist.gov/csrc/media/projects/cryptographic-standards-and-guidelines/documents/aes-development/rijndael-ammended.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#include "common.h"

#if defined(MBEDTLS_AES_ALT)

#include <string.h>

#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#include "ctr.h"

#include <hal_crypto.h>
#include "device_lock.h"

void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_aes_context));
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
    if (ctx == NULL) {
        return;
    }

    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_aes_context));
}

/*
 * AES key schedule (encryption)
 */
#if !defined(MBEDTLS_AES_SETKEY_ENC_ALT)
int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key,
                           unsigned int keybits)
{
    switch (keybits) {
        case 128: ctx->nr = 10; break;
#if !defined(MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH)
        case 192: ctx->nr = 12; break;
        case 256: ctx->nr = 14; break;
#endif /* !MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH */
        default: return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
    }

    ctx->rk = (uint8_t *) (((uint32_t) ctx->buf + 31) / 32 * 32);
    memcpy(ctx->rk, key, keybits / 8);

    return 0;
}
#endif /* !MBEDTLS_AES_SETKEY_ENC_ALT */

/*
 * AES key schedule (decryption)
 */
#if !defined(MBEDTLS_AES_SETKEY_DEC_ALT) && !defined(MBEDTLS_BLOCK_CIPHER_NO_DECRYPT)
int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key,
                           unsigned int keybits)
{
    int ret;

    /* Also checks keybits */
    if ((ret = mbedtls_aes_setkey_enc(ctx, key, keybits)) != 0) {
        goto exit;
    }

exit:

    return ret;
}
#endif /* !MBEDTLS_AES_SETKEY_DEC_ALT && !MBEDTLS_BLOCK_CIPHER_NO_DECRYPT */

/*
 * AES-ECB block encryption/decryption
 */
int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx,
                          int mode,
                          const unsigned char input[16],
                          unsigned char output[16])
{
    unsigned char key_buf[32 + 32 + 32], *key_buf_aligned;

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    device_mutex_lock(RT_DEV_LOCK_CRYPTO);
    key_buf_aligned = (unsigned char *) (((unsigned int) key_buf + 32) / 32 * 32);
    if(mode == MBEDTLS_AES_DECRYPT)
    {
        memcpy(key_buf_aligned, ctx->rk, ((ctx->nr - 6) * 4));
        rom_ssl_ram_map.hw_crypto_aes_ecb_init(key_buf_aligned, ((ctx->nr - 6) * 4));
        rom_ssl_ram_map.hw_crypto_aes_ecb_decrypt(input, 16, NULL, 0, output);
    }
    else
    {
        memcpy(key_buf_aligned, ctx->rk, ((ctx->nr - 6) * 4));
        rom_ssl_ram_map.hw_crypto_aes_ecb_init(key_buf_aligned,((ctx->nr - 6) * 4));
        rom_ssl_ram_map.hw_crypto_aes_ecb_encrypt(input, 16, NULL, 0, output);
    }
	device_mutex_unlock(RT_DEV_LOCK_CRYPTO);

	return 0;
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)

/*
 * AES-CBC buffer encryption/decryption
 */
int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx,
                          int mode,
                          size_t length,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
	unsigned char key_buf[32 + 32 + 32], *key_buf_aligned;
    unsigned char iv_buf[32 + 16 + 32], *iv_buf_aligned, iv_tmp[16];
    size_t length_done = 0;

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    /* Nothing to do if length is zero. */
    if (length == 0) {
        return 0;
    }

    if (length % 16) {
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
    }

    device_mutex_lock(RT_DEV_LOCK_CRYPTO);

    if(length > 0)
    {
        key_buf_aligned = (unsigned char *) (((unsigned int) key_buf + 32) / 32 * 32);
        iv_buf_aligned = (unsigned char *) (((unsigned int) iv_buf + 32) / 32 * 32);

        memcpy(iv_buf_aligned, iv, 16);

        if(mode == MBEDTLS_AES_DECRYPT)
        {
            memcpy(key_buf_aligned, ctx->rk, ((ctx->nr - 6) * 4));
            rom_ssl_ram_map.hw_crypto_aes_cbc_init(key_buf_aligned, ((ctx->nr - 6) * 4));

            while((length - length_done) > RTL_CRYPTO_FRAGMENT)
            {
                memcpy(iv_tmp, (input + length_done + RTL_CRYPTO_FRAGMENT - 16), 16);
                rom_ssl_ram_map.hw_crypto_aes_cbc_decrypt(input + length_done, RTL_CRYPTO_FRAGMENT, iv_buf_aligned, 16, output + length_done);
                memcpy(iv_buf_aligned, iv_tmp, 16);
                length_done += RTL_CRYPTO_FRAGMENT;
            }

            memcpy(iv_tmp, (input + length - 16), 16);
            rom_ssl_ram_map.hw_crypto_aes_cbc_decrypt(input + length_done, length - length_done, iv_buf_aligned, 16, output + length_done);
            memcpy(iv, iv_tmp, 16);
        }
        else
        {
            memcpy(key_buf_aligned, ctx->rk, ((ctx->nr - 6) * 4));
            rom_ssl_ram_map.hw_crypto_aes_cbc_init(key_buf_aligned,((ctx->nr - 6) * 4));

            while((length - length_done) > RTL_CRYPTO_FRAGMENT)
            {
                rom_ssl_ram_map.hw_crypto_aes_cbc_encrypt(input + length_done, RTL_CRYPTO_FRAGMENT, iv_buf_aligned, 16, output + length_done);
                memcpy(iv_buf_aligned, (output + length_done + RTL_CRYPTO_FRAGMENT - 16), 16);
                length_done += RTL_CRYPTO_FRAGMENT;
            }
            rom_ssl_ram_map.hw_crypto_aes_cbc_encrypt(input + length_done, length - length_done, iv_buf_aligned, 16, output + length_done);
            memcpy(iv, (output + length - 16), 16);
        }
    }	
    device_mutex_unlock(RT_DEV_LOCK_CRYPTO);

	return 0;
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128(mbedtls_aes_context *ctx,
                             int mode,
                             size_t length,
                             size_t *iv_off,
                             unsigned char iv[16],
                             const unsigned char *input,
                             unsigned char *output)
{
    int c;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t n;

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    n = *iv_off;

    if (n > 15) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    if (mode == MBEDTLS_AES_DECRYPT) {
        while (length--) {
            if (n == 0) {
                ret = mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);
                if (ret != 0) {
                    goto exit;
                }
            }

            c = *input++;
            *output++ = (unsigned char) (c ^ iv[n]);
            iv[n] = (unsigned char) c;

            n = (n + 1) & 0x0F;
        }
    } else {
        while (length--) {
            if (n == 0) {
                ret = mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);
                if (ret != 0) {
                    goto exit;
                }
            }

            iv[n] = *output++ = (unsigned char) (iv[n] ^ *input++);

            n = (n + 1) & 0x0F;
        }
    }

    *iv_off = n;
    ret = 0;

exit:
    return ret;
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8(mbedtls_aes_context *ctx,
                           int mode,
                           size_t length,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char c;
    unsigned char ov[17];

    if (mode != MBEDTLS_AES_ENCRYPT && mode != MBEDTLS_AES_DECRYPT) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }
    while (length--) {
        memcpy(ov, iv, 16);
        ret = mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);
        if (ret != 0) {
            goto exit;
        }

        if (mode == MBEDTLS_AES_DECRYPT) {
            ov[16] = *input;
        }

        c = *output++ = (unsigned char) (iv[0] ^ *input++);

        if (mode == MBEDTLS_AES_ENCRYPT) {
            ov[16] = c;
        }

        memcpy(iv, ov + 1, 16);
    }
    ret = 0;

exit:
    return ret;
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
/*
 * AES-OFB (Output Feedback Mode) buffer encryption/decryption
 */
int mbedtls_aes_crypt_ofb(mbedtls_aes_context *ctx,
                          size_t length,
                          size_t *iv_off,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    int ret = 0;
    size_t n;

    n = *iv_off;

    if (n > 15) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    while (length--) {
        if (n == 0) {
            ret = mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);
            if (ret != 0) {
                goto exit;
            }
        }
        *output++ =  *input++ ^ iv[n];

        n = (n + 1) & 0x0F;
    }

    *iv_off = n;

exit:
    return ret;
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr(mbedtls_aes_context *ctx,
                          size_t length,
                          size_t *nc_off,
                          unsigned char nonce_counter[16],
                          unsigned char stream_block[16],
                          const unsigned char *input,
                          unsigned char *output)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    size_t offset = *nc_off;

    if (offset > 0x0F) {
        return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
    }

    for (size_t i = 0; i < length;) {
        size_t n = 16;
        if (offset == 0) {
            ret = mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, nonce_counter, stream_block);
            if (ret != 0) {
                goto exit;
            }
            mbedtls_ctr_increment_counter(nonce_counter);
        } else {
            n -= offset;
        }

        if (n > (length - i)) {
            n = (length - i);
        }
        mbedtls_xor(&output[i], &input[i], &stream_block[offset], n);
        // offset might be non-zero for the last block, but in that case, we don't use it again
        offset = 0;
        i += n;
    }

    // capture offset for future resumption
    *nc_off = (*nc_off + length) % 16;

    ret = 0;

exit:
    return ret;
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#endif /* MBEDTLS_AES_ALT */
