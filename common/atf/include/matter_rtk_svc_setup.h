/*
 * Copyright (C) 2020 Realtek Semiconductor Corp.  All Rights Reserved.
 */

#ifndef MATTER_RTK_SVC_SETUP_H
#define MATTER_RTK_SVC_SETUP_H

#include <stdint.h>
#include <matter_utils.h> // get matter_key_type

/* Matter Secure APIs */
void matter_secure_mbedtls_init(uint32_t random_seed);
int matter_secure_dac_init_keypair(uint8_t *pub_buf, size_t pub_size);
int matter_secure_ecdsa_sign_msg(matter_key_type key_type, const unsigned char *msg, size_t msg_size, unsigned char *signature);
int matter_secure_get_opkey(uint8_t *buf, size_t size);
int matter_secure_get_opkey_pub(uint8_t *pubkey, size_t pubkey_size);
int matter_secure_new_csr(uint8_t *out_csr, size_t csr_length);
int matter_secure_serialize(uint8_t *output_buf, size_t output_size);

/* SMC function IDs for Matter Secure queries */
#define MATTER_SECURE_MBEDTLS_INIT     0x82000002
#define MATTER_SECURE_DAC_INIT_KEYPAIR 0x82000003
#define MATTER_SECURE_ECDSA_SIGN_MSG   0x82000004
#define MATTER_SECURE_GET_OPKEY        0x82000005
#define MATTER_SECURE_GET_OPKEY_PUB    0x82000006
#define MATTER_SECURE_NEW_CSR          0x82000007
#define MATTER_SECURE_SERIALIZE        0x82000008

#endif /* MATTER_RTK_SVC_SETUP_H */
