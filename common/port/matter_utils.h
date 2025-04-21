/********************************************************************************
 * @file    matter_utils.h
 * @author
 * @version
 * @brief   Utility API which includes Factory Data and Key protection.
 ********************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
********************************************************************************/

#ifndef _RTK_MATTER_UTILS_H_
#define _RTK_MATTER_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *               Factory Data Structure
 ******************************************************/

typedef struct
{
    uint8_t value[68];
    size_t len;
} FactoryDataString;

typedef struct
{
    uint8_t value[602];
    size_t len;
} CertDataString;

typedef struct
{
    uint8_t value[256];
    size_t len;
} VerifierDataString;

typedef struct  
{
    int passcode; 
    int discriminator;
    int spake2_it;
    FactoryDataString spake2_salt;
    VerifierDataString spake2_verifier;
} CommissionableData;

typedef struct 
{
    CertDataString dac_cert;
    FactoryDataString dac_key;
    CertDataString pai_cert;
    CertDataString cd;
} DeviceAttestationCredentials;

typedef struct 
{
    int vendor_id;
    FactoryDataString vendor_name;
    int product_id;
    FactoryDataString product_name;
    int hw_ver;
    FactoryDataString hw_ver_string; 
    FactoryDataString mfg_date;
    FactoryDataString serial_num;
    FactoryDataString rd_id_uid;
} DeviceInstanceInfo;

typedef struct
{
    CommissionableData cdata;
    DeviceAttestationCredentials dac;
    DeviceInstanceInfo dii;
} FactoryData;

/******************************************************
 *               Factory Data Functions
 ******************************************************/

/**
 * @brief  Read FactoryData from Flash.
 * @param[out]  buffer: Pointer to the factorydata read from Flash.
 * @param[out]  pfactorydata_len: Length of the factorydata.
 * @returns  0 on success, -1 otherwise.
 */
int32_t ReadFactory(uint8_t *buffer, uint16_t *pfactorydata_len);

/**
 * @brief  Decode FactoryData from Flash.
 * @param[in]  buffer: Pointer that contains the factorydata.
 * @param[out]  fdp: Pointer to the decoded factorydata.
 * @param[in]  data_len: Length of the factorydata.
 * @returns  1 on success, -1 otherwise.
 */
int32_t DecodeFactory(uint8_t *buffer, FactoryData *fdp, uint16_t data_len);

#if defined(CONFIG_MATTER_SECURE) && CONFIG_MATTER_SECURE
#include <mbedtls/ecp.h>

/******************************************************
 *               Matter Key Type
 ******************************************************/

typedef enum {
    MATTER_DACKEY_KEY_TYPE = 0,
    MATTER_OPKEY_KEY_TYPE,
} matter_key_type;

/******************************************************
 *               Matter Crypto Variables
 ******************************************************/

#define MATTER_PUBLIC_KEY_SIZE          65
#define MATTER_DAC_PRIVATE_KEY_LENGTH   32
#define MATTER_SHA256_HASH_LENGTH       32
#define MATTER_P256_FE_LENGTH           32

/******************************************************
 *               Matter Crypto Error Codes
 ******************************************************/

#define MATTER_ERROR_WELL_UNINITIALIZED 0x1C
#define MATTER_NOT_IMPLEMENTED          0x2D
#define MATTER_INVALID_ARGUMENT         0x2F
#define MATTER_ERROR_INTERNAL           0xAC

/******************************************************
 *               Matter Crypto Functions
 ******************************************************/

/**
 * @brief  Calculate the SHA-256 hash for a message (msg) and sign it using ECDSA,
 *         afterwards return the resulting signature (signature).
 * @param[in]  msg: Pointer to the message to be signed.
 * @param[in]  msg_size: Size of the message.
 * @param[out]  signature: Pointer to the buffer where the signature will be stored.
 * @returns  0 on success, negative value otherwise.
 */
int matter_ecdsa_sign_msg(const unsigned char *msg, size_t msg_size , unsigned char *signature);

/**
 * @brief Generate a Certificate Signing Request (CSR) for a CASE Session using the Operational Keypair.
 * @param[out]  out_csr: Pointer to the buffer where the CSR will be stored.
 * @param[in,out]  csr_length: On input, specifies the size of the out_csr buffer.
 *                             On output, returns the length of the generated CSR.
 * @return  The length of the generated CSR on success, negative value otherwise.
 */
size_t matter_gen_new_csr(uint8_t *out_csr, size_t csr_length);

/**
 * @brief Retrieves the public key of the Operational Keypair.
 * @param[out] pubkey: Pointer to the buffer where the public key will be written.
 * @param[in] pubkey_size: Size of the pubkey.
 * Returns:  0 on success, negative value otherwise.
 */
int matter_get_publickey(uint8_t *pubkey, size_t pubkey_size);

/**
 * @brief  Initialize Device Attestation Certificate (DAC) keypair and
 *         calculate the SHA-256 hash for a message (msg) and sign it using ECDSA,
 *         afterwards return the resulting signature (signature).
 * @param[in]  pub_buf: DAC public key extracted from certificate.
 * @param[in]  pub_size: Size of DAC public key.
 * @param[in]  msg: Pointer to the message to be signed.
 * @param[in]  msg_size: Size of the message.
 * @param[out]  signature: Pointer to the buffer where the signature will be stored.
 * @returns  0 on success, negative value otherwise.
 */
int matter_get_signature(uint8_t *pub_buf, size_t pub_size, const unsigned char *msg, size_t msg_size , unsigned char *signature);

/**
 * @brief Serializes the Operational Keypair.
 * @param[out] output_buf: Pointer to the buffer where the serialized data will be written.
 * @param[out] output_size: Size of the output buffer.
 * @returns  0 on success, negative value otherwise.
 */
int matter_serialize(uint8_t *output_buf, size_t output_size);

/**
 * @brief Prepare Operational Keypair.
 * @param[in]  buf: DAC keypair retrieved from NVS.
 * @param[in]  size: Size of DAC keypair.
 * @returns  0 on success, negative value otherwise.
 */
int matter_deserialize(uint8_t *buf, size_t size);
#endif /* configENABLE_TRUSTZONE */

#ifdef __cplusplus
}
#endif

#endif /* _RTK_MATTER_UTILS_H_ */
