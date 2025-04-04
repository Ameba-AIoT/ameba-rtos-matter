/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef __RTK_MATTER_KVS_H__
#define __RTK_MATTER_KVS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <wifi_conf.h>

enum{
  DCT_SUCCESS = 0,			/*!< success */
  DCT_ERROR = -1,				/*!< error */
  DCT_ERR_CRC = -2,			/*!< crc error */
  DCT_ERR_NO_SPACE = -3,		/*!< no space error */
  DCT_ERR_NO_MEMORY = -4,		/*!< alloc memory error */
  DCT_ERR_FLASH_RW = -5,		/*!< flash r/w error */
  DCT_ERR_NOT_FIND = -6,		/*!< not find error */
  DCT_ERR_INVALID = -7,		/*!< invalid operation error */
  DCT_ERR_SIZE_OVER = -8,		/*!< varialbe length over max size error */
  DCT_ERR_MODULE_BUSY = -9,	/*!< module mutex time out */
};

// Map DCT return code to MATTER KVS ERROR
#define KVS_SUCCESS      DCT_SUCCESS
#define KVS_ERROR        DCT_ERROR
#define KVS_ERR_NOT_FIND DCT_ERR_NOT_FIND
#define KVS_ERR_INVALID  DCT_ERR_INVALID

/*
 * @brief  Initialize Non-volatile Storage (NVS) for Key Value Storage (KVS).
 * @return  DCT_SUCCESS if initialized successfully.
 */
s32 initPref(void);

/*
 * @brief  Deinitialize Non-volatile Storage (NVS) for Key Value Storage (KVS).
 * @return  DCT_SUCCESS if deinitalized successfully.
 */
s32 deinitPref(void);

/*
 * @brief  Register NameSpace (delicated for Device Configuration Table (DCT)).
 *         In DCT, there are two regions DCT1 and DCT2.
 * @return  DCT_SUCCESS if registered successfully.
 */
s32 registerPref(void);
s32 registerPref2(void);

/*
 * @brief  Clear NameSpace (delicated for Device Configuration Table (DCT)).
 *         In DCT, there are two regions DCT1 and DCT2.
 * @return  DCT_SUCCESS if cleared successfully.
 */
s32 clearPref(void);
s32 clearPref2(void);

/*
 * @brief  Delete Key values from KVS.
 * @param[in]  domain: key namespace.
 * @param[in]  key: key name.
 * @return  DCT_SUCCESS if deleted successfully.
 */
s32 deleteKey(const char *domain, const char *key);

/*
 * @brief  Check if key values exist in KVS.
 * @param[in]  domain: key namespace.
 * @param[in]  key: key name.
 * @return  true if key exist, false otherwise.
 */
bool checkExist(const char *domain, const char *key);

/*
 * @brief  Write Key values into KVS.
 * @param[in]  domain: key namespace.
 * @param[in]  key: key name.
 * @param[in]  value: key value.
 * @param[in]  byteCount: key value size.
 * @return  DCT_SUCCESS if read successfully.
 */
s32 setPref_new(const char *domain, const char *key, u8 *value, size_t byteCount);

/*
 * @brief  Read a boolean value from KVS.
 * @param[in]  domain: key namespace.
 * @param[in]  key: key name.
 * @param[out] val: pointer to store key value.
 * @return  DCT_SUCCESS if read successfully.
 */
s32 getPref_bool_new(const char *domain, const char *key, u8 *val);

/*
 * @brief  Read a 32-bit value from KVS.
 * @param[in]  domain: key namespace.
 * @param[in]  key: key name.
 * @param[out] val: pointer to store key value.
 * @return  DCT_SUCCESS if read successfully.
 */
s32 getPref_u32_new(const char *domain, const char *key, u32 *val);

/*
 * @brief  Read a 64-bit value from KVS.
 * @param[in]  domain: key namespace.
 * @param[in]  key: key name.
 * @param[out] val: pointer to store key value.
 * @return  DCT_SUCCESS if read successfully.
 */
s32 getPref_u64_new(const char *domain, const char *key, u64 *val);

/*
 * @brief  Read a string value from KVS.
 * @param[in]  domain: key namespace.
 * @param[in]  key: key name.
 * @param[out] buf: buffer to store key value.
 * @param[in]  bufSize: size of the buffer.
 * @param[out] outLen: length of the key value.
 * @return  DCT_SUCCESS if read successfully.
 */
s32 getPref_str_new(const char *domain, const char *key, char *buf, size_t bufSize, size_t *outLen);

/*
 * @brief  Read a binary value from KVS.
 * @param[in]  domain: key namespace.
 * @param[in]  key: key name.
 * @param[out] buf: buffer to store key value.
 * @param[in]  bufSize: size of the buffer.
 * @param[out] outLen: length of the key value.
 * @return  DCT_SUCCESS if read successfully.
 */
s32 getPref_bin_new(const char *domain, const char *key, u8 *buf, size_t bufSize, size_t *outLen);

#ifdef __cplusplus
}
#endif

#endif /* __RTK_MATTER_KVS_H__ */
