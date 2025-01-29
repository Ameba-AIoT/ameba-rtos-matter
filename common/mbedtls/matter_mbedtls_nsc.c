#include "cmsis.h"
#include "platform_stdlib.h"
#include "platform_opts_matter.h"
#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#else
#include "mbedtls/config.h"
#endif
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"
#include "mbedtls/pk.h"
#include "mbedtls/version.h"
#include "crypto_api.h"

#if defined(CONFIG_MATTER_SECURE) && CONFIG_MATTER_SECURE
#include "matter_utils.h"
#include <mbedtls/x509_csr.h>
#include "mbedtls/aes.h"

__weak const uint8_t kSecureDacPrivateKey[] = {
0xe6, 0xfe, 0xfc, 0xf7, 0x38, 0x1e, 0x01, 0x6e, 0x66, 0xa3, 0x09, 0xe6, 0x55, 0x20, 0x20, 0x1f,
0x85, 0x9d, 0xaa, 0x4a, 0xf3, 0x07, 0x92, 0x13, 0x86, 0x68, 0x63, 0x1c, 0xe5, 0xdc, 0xac, 0xd9,
};

unsigned char test_key[] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
unsigned char test_iv[] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

mbedtls_aes_context aes_ctx;
mbedtls_ecp_keypair DacKey;
mbedtls_ecp_keypair OpKey;

int keyInitialized = 0;

static void* _calloc(size_t count, size_t size)
{
	void *ptr = pvPortMalloc(count * size);
	if(ptr)	memset(ptr, 0, count * size);
	return ptr;
}

static int _random(void *p_rng, unsigned char *output, size_t output_len)
{
	/* To avoid gcc warnings */
	( void ) p_rng;

	static unsigned int seed = 0;
	if(seed == 0) {
#if defined(CONFIG_PLATFORM_8710C)
		crypto_random_generate((uint8_t *)&seed, sizeof(seed));
#elif (defined CONFIG_PLATFORM_8721D)
		extern u32 RandSeedTZ;
		seed = RandSeedTZ;
#endif
		srand(seed);
	}

	int rand_num = 0;
	while(output_len) {
		int r = rand();
		if(output_len > sizeof(int)) {
			memcpy(&output[rand_num], &r, sizeof(int));
			rand_num += sizeof(int);
			output_len -= sizeof(int);
		}
		else {
			memcpy(&output[rand_num], &r, output_len);
			rand_num += output_len;
			output_len = 0;
		}
	}

	return 0;
}

/**
 * @brief Clears the Key Pair associated with the specified Matter Key Type.
 * @param:  key_type: The type of Matter Key for which the key pair needs to be cleared. It can be either 
 *                    MATTER_DACKEY_KEY_TYPE for Device Attestation Certificate (DAC) key pair or 
 *                    MATTER_OPKEY_KEY_TYPE for Operational key pair.
 *
 * Upon successful execution, the key pair associated with the specified Matter Key Type is freed, and 
 * the 'keyInitialized' flag is set to 0 to indicate that the key pair is no longer initialized.
 */
int NS_ENTRY matter_secure_clear_keypair(matter_key_type key_type)
{
    if (keyInitialized)
    {
        switch (key_type)
        {
        case MATTER_DACKEY_KEY_TYPE:
            mbedtls_ecp_keypair_free(&DacKey);
            break;
        case MATTER_OPKEY_KEY_TYPE:
            mbedtls_ecp_keypair_free(&OpKey);
            break;
        default:
            break;
        }
        keyInitialized = 0;
    }
}

/**
 * @brief Calculate the SHA-256 hash for a message.
 * @param  msg: Pointer to the message for which the hash needs to be calculated.
 * @param  msg_size: Size of the message.
 * @param  out_buf: Pointer to the buffer where the SHA-256 hash will be stored.
 * @return  0 on success, negative value otherwise
 */
int NS_ENTRY matter_hash_sha256(const uint8_t * msg, size_t msg_size, uint8_t * out_buf)
{
    // Check if 'msg' or 'out_buf' pointers are nullptr
    if ((msg == NULL) || (out_buf == NULL))
    {
        printf("ERROR: %s nullptr \n\r", __FUNCTION__);
        return MATTER_INVALID_ARGUMENT;
    }

    // Calculate the SHA-256 hash for the input message
#if defined(MBEDTLS_VERSION_NUMBER) && (MBEDTLS_VERSION_NUMBER>=0x03010000)
    return mbedtls_sha256(msg, msg_size, out_buf, 0);
#else
    return mbedtls_sha256_ret(msg, msg_size, out_buf, 0);
#endif
}

/**
 * @brief Calculate the SHA-256 hash for a message and sign it using ECDSA.
 * @param  key_type: The type of key to be used for signing (MATTER_DACKEY_KEY_TYPE or MATTER_OPKEY_KEY_TYPE).
 * @param  msg: Pointer to the message to be signed.
 * @param  msg_size: Size of the message.
 * @param  signature: Pointer to the buffer where the signature will be stored.
 * @return  0 on success, negative value otherwise
 * 
 * Notes:
 *  - This function first calculates the SHA-256 hash of the input message.
 *  - It then sets up the ECDSA context with the specified key type (DacKey or OpKey).
 *  - The ECDSA signature is computed and stored in the 'signature' buffer.
 *  - Error handling includes printing error messages and returning appropriate error codes.
 */
int NS_ENTRY matter_secure_ecdsa_sign_msg(matter_key_type key_type, const unsigned char * msg, size_t msg_size, unsigned char *signature)
{
    int result;
    uint8_t digest[MATTER_SHA256_HASH_LENGTH];

    // Calculate the SHA-256 hash of the message
    memset(&digest[0], 0, sizeof(digest));

    result = matter_hash_sha256(msg, msg_size, &digest[0]);
    if (result != 0)
    {
        printf("ERROR: %s hash failed result=%d \n\r", __FUNCTION__, result);
        return MATTER_INVALID_ARGUMENT;
    }

#if defined(MBEDTLS_ECDSA_C)
    mbedtls_mpi r, s;
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    mbedtls_ecdsa_context ecdsa_ctxt;
    mbedtls_ecdsa_init(&ecdsa_ctxt);

    // Set up the ECDSA context with the specified key type
    switch (key_type)
    {
    case MATTER_DACKEY_KEY_TYPE:
        result = mbedtls_ecdsa_from_keypair(&ecdsa_ctxt, &DacKey);
        break;
    case MATTER_OPKEY_KEY_TYPE:
        result = mbedtls_ecdsa_from_keypair(&ecdsa_ctxt, &OpKey);
        break;
    }

    if (result != 0)
    {
        printf("ERROR: %s setup ECDSA failed result=%d \n\r", __FUNCTION__, result);
        return result;
    }

    // Compute the ECDSA signature
    result = mbedtls_ecdsa_sign(&ecdsa_ctxt.grp, &r, &s, &ecdsa_ctxt.d,
                                (unsigned char const *)digest, sizeof(digest), _random, NULL);
    if (result != 0)
    {
        printf("ERROR: %s ECDSA sign failed result=%d \n\r", __FUNCTION__, result);
        return result;
    }

    if (!(mbedtls_mpi_size(&r) <= MATTER_P256_FE_LENGTH) && (mbedtls_mpi_size(&s) <= MATTER_P256_FE_LENGTH))
    {
        return MATTER_ERROR_INTERNAL;
    }

    // Write the signature into the 'signature' buffer
    result = mbedtls_mpi_write_binary(&r, signature, MATTER_P256_FE_LENGTH);
    if (result != 0)
    {
        printf("ERROR: %s write binary failed result=%d \n\r", __FUNCTION__, result);
        return result;
    }

    result = mbedtls_mpi_write_binary(&s, signature + MATTER_P256_FE_LENGTH, MATTER_P256_FE_LENGTH);
    if (result != 0)
    {
        printf("ERROR: %s write binary failed result=%d \n\r", __FUNCTION__, result);
        return result;
    }

    mbedtls_ecdsa_free(&ecdsa_ctxt);
    mbedtls_mpi_free(&s);
    mbedtls_mpi_free(&r);
#else
    result = MATTER_NOT_IMPLEMENTED;
#endif
    return result;
}

/**
 * @brief Generate a Certificate Signing Request (CSR) for a CASE Session using the Operational Keypair.
 * @param  out_csr: Pointer to the buffer where the CSR will be stored.
 * @param  csr_length: Length of the CSR buffer.
 * @return  The length of the generated CSR on success, MATTER_ERROR_INTERNAL otherwise.
 */
int NS_ENTRY matter_secure_new_csr(uint8_t *out_csr, size_t csr_length)
{
    int result = 0;
    mbedtls_ecp_group_id group = MBEDTLS_ECP_DP_SECP256R1;

    matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);

    // Initialize a new Operational Keypair
    matter_secure_opkey_init_keypair();

#if defined(MBEDTLS_X509_CSR_WRITE_C)
    size_t out_length;

    mbedtls_x509write_csr csr;
    mbedtls_x509write_csr_init(&csr);

    // Set up the CSR with the Operational Keypair
    mbedtls_pk_context pk;
    pk.pk_info = mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY);
    pk.pk_ctx = &OpKey;
    if (pk.pk_info == NULL)
    {
        return MATTER_ERROR_INTERNAL;
    }

    mbedtls_x509write_csr_set_key(&csr, &pk);

    // Set the message digest algorithm to SHA-256
    mbedtls_x509write_csr_set_md_alg(&csr, MBEDTLS_MD_SHA256);

    // Set the subject name of the CSR
    result = mbedtls_x509write_csr_set_subject_name(&csr, "O=CSR");
    if (result != 0)
    {
        printf("Error: %s set subject failed, result=%d\n", __FUNCTION__, result);
        return MATTER_ERROR_INTERNAL;
    }

    // Generate the CSR and store it in the output buffer 'out_csr'
    result = mbedtls_x509write_csr_der(&csr, out_csr, csr_length, _random, NULL);
    if (result <= 0)
    {
        printf("Error: %s write csr der failed, length=%d \n\r", __FUNCTION__, result);
        return MATTER_ERROR_INTERNAL;
    }

    out_length = (size_t) result;
    result     = 0;

    // Check for CSR length
    if (out_length > csr_length)
    {
        printf("Error: %s length error, length=%d \n\r", __FUNCTION__, out_length);
        return MATTER_ERROR_INTERNAL;
    }

    if (csr_length != out_length)
    {
        // mbedTLS API writes the CSR at the end of the provided buffer.
        // Let's move it to the start of the buffer.
        size_t offset = csr_length - out_length;
        memmove(out_csr, &out_csr[offset], out_length);
    }

    csr_length = out_length;

exit:
    mbedtls_x509write_csr_free(&csr);

    return csr_length;
#else
    printf("Error: %s MBEDTLS_X509_CSR_WRITE_C is not enabled. CSR cannot be created \n\r", __FUNCTION__);
    return MATTER_NOT_IMPLEMENTED;
#endif
}

/**
 * @brief Generate a new Operational Keypair for a CASE Session.
 * @return  0 on success, negative value otherwise.
 * 
 * Notes:
 *  - The function initializes a new Operational Keypair structure ('OpKey') and clears any existing keypair of the same type.
 *  - It generates a new keypair using the elliptic curve specified by 'group' (SECP256R1).
 *  - If an error occurs during the key generation process, the function prints an error message and returns the error code.
 */
int NS_ENTRY matter_secure_opkey_init_keypair()
{
    int result = 0;
    mbedtls_ecp_group_id group = MBEDTLS_ECP_DP_SECP256R1;

    matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);

    // Initialize the Operational Keypair
    mbedtls_ecp_keypair_init(&OpKey);

    // Generate a new keypair using the specified elliptic curve group
    result = mbedtls_ecp_gen_key(group, &OpKey, _random, NULL);
    if (result != 0)
    {
        printf("Error: %s gen key failed, result=%d \n\r", __FUNCTION__, result);
        goto exit;
    }

    keyInitialized = 1;
    return result;

exit:
    matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);
    return result;
}

/**
 * @brief Encrypts the Operational Private Key using AES-CTR for storing into DCT.
 * @param  buf: Pointer to the buffer containing the original Operational Private Key.
 * @param  size: Size of the buffer containing the original Operational Private Key.
 * @return  0 on success, negative value otherwise.
 *
 * Notes:
 *  - Upon successful encryption, the encrypted Operational Private Key is copied back to the input buffer 'buf'.
 */
int NS_ENTRY matter_secure_encrypt_key(uint8_t *buf, size_t size)
{
    int result = 0;
    size_t nc_off = 0;
    unsigned char nonce_counter[16] = {0};
    unsigned char stream_block[16] = {0};

    // Initialize AES context
    mbedtls_aes_init(&aes_ctx);
    
    // Set encryption key
    mbedtls_aes_setkey_enc(&aes_ctx, test_key, 256);

    // Allocate memory for decrypted_privkey
    unsigned char *encrypted_privkey = (unsigned char*) pvPortMalloc(size);
    if (encrypted_privkey == NULL)
    {
        result = -1;
        goto exit;
    }

    // Initialize variables and copy IV
    memset(encrypted_privkey, 0, size);
    memcpy(nonce_counter, test_iv, sizeof(nonce_counter));

    // Decrypt the encrypted Operational private key using AES-CTR
    result = mbedtls_aes_crypt_ctr(&aes_ctx, size, &nc_off, nonce_counter, stream_block, buf, encrypted_privkey);
    if (result !=0)
    {
        printf("ERROR: %s privkey decrypt failed! result=%d \n\r", __FUNCTION__, result);
        goto exit; 
    }

    // Copy decrypted key back to input buffer
    memset(buf, 0, size);
    memcpy(buf, encrypted_privkey, size);

exit:
    if (encrypted_privkey)
    {
        vPortFree(encrypted_privkey);
    }

    mbedtls_aes_free(&aes_ctx);
    return result;
}

/**
 * @brief Retrieves the public key of the Operational Keypair.
 * @param  pubkey: Pointer to the buffer where the public key will be written.
 * @param  pubkey_size: Size of the buffer to accommodate the public key.
 * @return  0 on success, negative value otherwise.
 */
int NS_ENTRY matter_secure_get_opkey_pub(uint8_t * pubkey, size_t pubkey_size)
{
    int result = 0;
    size_t temp_size = 0;

    // Write the public key of the Operational Keypair into the buffer
    result = mbedtls_ecp_point_write_binary(&OpKey.grp, &OpKey.Q, MBEDTLS_ECP_PF_UNCOMPRESSED, 
                                            &temp_size, (unsigned char *) pubkey, pubkey_size);
    if (result != 0)
    {
        printf("ERROR: %s write public key failed, result=%d \n\r", __FUNCTION__, result);
        return result;
    }

    // Check if the size of the written public key matches the expected size
    if (temp_size != pubkey_size)
    {
        return -1;
    }

    return result;
}

/**
 * @brief Retrieves and encrypts the private key of the Operational Keypair.
 * @param  privkey: Pointer to the buffer where the encrypted private key will be written.
 * @param  privkey_size: Size of the buffer to accommodate the encrypted private key.
 * @return  0 on success, negative value otherwise.
 */
int NS_ENTRY matter_secure_get_opkey_priv(uint8_t * privkey, size_t privkey_size)
{
    int result = 0;

    // Retrieve the encrypted private key of the Operational Keypair and write it into the buffer 'privkey'
    result = mbedtls_mpi_write_binary(&OpKey.d, privkey, privkey_size);
    if (result != 0)
    {
        printf("ERROR: %s get private key failed! result=%d \n\r", __FUNCTION__, result);
        matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);
        goto exit;
    }

    // Decrypt the encrypted private key
    result = matter_secure_encrypt_key(privkey, privkey_size);
    if (result != 0)
    {
        printf("ERROR: %s encrypt private key failed! result=%d \n\r", __FUNCTION__, result);
        matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);
        goto exit;
    }

exit:

    return result;
}

/**
 * @brief Prepare Operational Keypair
 * 
 * When the function SignWithStoredOpKey() is called, it retrieves the Operational Keypair from the key storage. 
 * The buffer 'buf' will contain the Operationalpublic key and the encrypted private key of the Operational Keypair.
 * 
 * Whenever the key storage reads and requires the Operational Keypair, it invokes matter_secure_get_opkey():
 *    1. It copies the public key from the buffer 'buf' into the Opkey
 *    2. It decrypts the private key and copies the decrypted private key into the Opkey.
 * 
 * It's important to note that the decrypted private key remains within the secure context and is not exposed to 
 * non-secure environments. The Operational Keypair (Opkey) is exclusively used within a secure context.
 */
int NS_ENTRY matter_secure_get_opkey(uint8_t *buf, size_t size)
{
    int result = 0;
    size_t nc_off = 0;
    unsigned char nonce_counter[16] = {0};
    unsigned char stream_block[16] = {0};

    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_enc(&aes_ctx, test_key, 256);

    //initialize decrypted_privkey for store decrypted private key
    unsigned char *decrypted_privkey = (unsigned char*) pvPortMalloc(MATTER_P256_FE_LENGTH);
    if (decrypted_privkey == NULL)
    {
        result = -1;
        goto exit;
    }

    unsigned char *pubkey = (unsigned char*) pvPortMalloc(MATTER_PUBLIC_KEY_SIZE);
    if (pubkey == NULL)
    {
        result = -1;
        goto exit;
    }

    //clears decrypted_privkey and pubkey
    memset(decrypted_privkey, 0, MATTER_P256_FE_LENGTH);
    memset(pubkey, 0, MATTER_PUBLIC_KEY_SIZE);

    //copy operational public key from buf into pubkey
    memcpy(pubkey, buf, MATTER_PUBLIC_KEY_SIZE);
    //copy IV into nonce_counter
    memcpy(nonce_counter, test_iv, sizeof(nonce_counter));

    //decrypt operational private key
    result = mbedtls_aes_crypt_ctr(&aes_ctx, MATTER_P256_FE_LENGTH, &nc_off, nonce_counter, stream_block, buf + MATTER_PUBLIC_KEY_SIZE, decrypted_privkey);
    if (result !=0)
    {
        printf("ERROR: %s decryption failed! result=%d \n\r", __FUNCTION__, result);
        goto exit; 
    }

    //Initialize Opkey
    matter_secure_clear_keypair(MATTER_OPKEY_KEY_TYPE);
    mbedtls_ecp_keypair_init(&OpKey);
    keyInitialized = 1;

    //set Operational keypair ecp group as MBEDTLS_ECP_DP_SECP256R1
    result = mbedtls_ecp_group_load(&OpKey.grp, MBEDTLS_ECP_DP_SECP256R1);
    if (result != 0)
    {
        printf("ERROR: %s load grp failed! result=%d \n\r", result);
        goto exit; 
    }

    // set Opkey public key
    result = mbedtls_ecp_point_read_binary(&OpKey.grp, &OpKey.Q, (const unsigned char *)pubkey, MATTER_PUBLIC_KEY_SIZE);
    if (result != 0)
    {
        printf("ERROR: %s set pubkey failed! result=%d \n\r", result);
        goto exit; 
    }

    // set decrypted priv key into Opkey
    result = mbedtls_mpi_read_binary(&OpKey.d, decrypted_privkey, MATTER_P256_FE_LENGTH);
    if (result != 0)
    {
        printf("ERROR: %s set privkey failed! result=%d \n\r", result);
        goto exit; 
    }

exit:
    if (decrypted_privkey)
    {
        vPortFree(decrypted_privkey);
    }
    if (pubkey)
    {
        vPortFree(pubkey);
    }

    mbedtls_aes_free(&aes_ctx);
    return result;
}

/**
 * @brief Serializes the Operational Keypair for storage in the Device Configuration Table (DCT).
 * @param  output_buf: Pointer to the buffer where the serialized data will be written.
 * @param  output_size: Size of the output buffer.
 * @return  0 on success, negative value otherwise.
 * 
 * Notes:
 *  - The Operational Keypair (Opkey) is assumed to have been previously generated and initialized.
 *  - This function first retrieves the public and private keys of the Operational Keypair.
 *  - It then writes the public key followed by the encrypted private key into the output buffer.
 *  - If an error occurs during the retrieval or serialization process, the function prints an error message,
 *    clears the Operational Keypair, and returns the error code.
 */
int NS_ENTRY matter_secure_serialize(uint8_t *output_buf, size_t output_size)
{
    int result = 0;
    uint8_t privkey[MATTER_P256_FE_LENGTH];
    uint8_t pubkey[MATTER_PUBLIC_KEY_SIZE];

    // Get the public key of the Operational Keypair
    result = matter_secure_get_opkey_pub(pubkey, sizeof(pubkey));
    if (result != 0)
    {
        printf("ERROR: %s get public key failed! result=%d \n\r", __FUNCTION__, result);
        goto exit;
    }

    // Get the private key of the Operational Keypair
    result = matter_secure_get_opkey_priv(privkey, sizeof(privkey));
    if (result != 0)
    {
        printf("ERROR: %s get private key failed! result=%d \n\r", __FUNCTION__, result);
        goto exit;
    }

    // Write the public key to the output buffer
    memcpy(output_buf, pubkey, sizeof(pubkey));

    // Write the encrypted private key to the output buffer
    memcpy(output_buf + sizeof(pubkey), privkey, sizeof(privkey));

exit:
    return result;
}

/**
 * @brief Deserialize Device Attestation Certificate (DAC) keypair.
 * @param  pub_buf: Pointer to the buffer containing the public key.
 * @param  pub_size: Size of the public key buffer.
 * @return  0 on success, negative value otherwise.
 */
int NS_ENTRY matter_secure_deserialize(uint8_t *pub_buf, size_t pub_size)
{
    int result = 0;

    // Initialize the DAC keypair structure
    mbedtls_ecp_keypair_init(&DacKey);

    keyInitialized = 1;

    // Set the ECP group for the DAC keypair to MBEDTLS_ECP_DP_SECP256R1
    result = mbedtls_ecp_group_load(&DacKey.grp, MBEDTLS_ECP_DP_SECP256R1);
    if (result != 0)
    {
        printf("ERROR: %s load grp failed! result=%d \n\r", __FUNCTION__, result);
        matter_secure_clear_keypair(MATTER_DACKEY_KEY_TYPE);
        goto exit;
    }

    // Set the DAC public key into DacKey
    result = mbedtls_ecp_point_read_binary(&DacKey.grp, &DacKey.Q, (const unsigned char *)pub_buf, pub_size);
    if (result != 0)
    {
        printf("ERROR: %s set public key failed! result=%d \n\r", __FUNCTION__, result);
        matter_secure_clear_keypair(MATTER_DACKEY_KEY_TYPE);
        goto exit;
    }

    // Set the DAC private key into DacKey
    result = mbedtls_mpi_read_binary(&DacKey.d, kSecureDacPrivateKey, MATTER_DAC_PRIVATE_KEY_LENGTH);
    if (result != 0)
    {
        printf("ERROR: %s set private key failed! result=%d \n\r", __FUNCTION__, result);
        matter_secure_clear_keypair(MATTER_DACKEY_KEY_TYPE);
        goto exit;
    }

exit:
    return result;
}

/**
 * @brief Initialize Device Attestation Certificate (DAC) keypair
 * @param  pub_buf: Pointer to the buffer containing the public key.
 * @param  pub_size: Size of the public key buffer.
 * @return  0 on success, negative value otherwise.
 */
int NS_ENTRY matter_secure_dac_init_keypair(uint8_t *pub_buf, size_t pub_size)
{
    unsigned char *output_buffer;
    int result = 0;

    // Deserialize the DAC keypair
    result = matter_secure_deserialize(pub_buf, pub_size);
    if (result != 0) {
        printf("Error: %s deserialize failed result=%d \n\r", __FUNCTION__, result);
        return result;
    }

    return result;
}
#endif /* CONFIG_MATTER_SECURE */
