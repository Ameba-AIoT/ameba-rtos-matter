/********************************************************************************
 * @file    matter_ota.h
 * @author
 * @version
 * @brief   OTA API to support Matter OTA feature.
 ********************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
********************************************************************************/

#ifndef __RTK_MATTER_OTA_H__
#define __RTK_MATTER_OTA_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  Matter OTA error number.
  */
enum {
    OTA_SUCCESS = 1,            /**< Matter OTA success */
    OTA_ERROR = -1,             /**< Matter OTA error */
};

/**
 * @brief  Get total header size.
 * @return Total header size.
 */
uint8_t matter_ota_get_total_header_size(void);

/**
 * @brief  Get current header size from Matter OTA firmware.
 * @return Current header size.
 */
uint8_t matter_ota_get_current_header_size(void);

/**
 * @brief  Prepare partition for Matter OTA firmware.
 *         Set the address for the new firmware and clear all buffers data.
 */
void matter_ota_prepare_partition(void);

/**
 * @brief  Store Matter OTA firmware header.
 *         Firmware header will only be included in the first frame, record it down.
 * @param[in]  data: Pointer to the firmware header data.
 * @param[in]  size: Size of the firmware header.
 * @return OTA_SUCCESS if store successful; OTA_ERROR otherwise
 */
int8_t matter_ota_store_header(uint8_t *data, uint32_t size);

/**
 * @brief  Write/Store 4KB of Matter OTA firmware data into flash.
 *         If data exceeds 4KB, indicate as overflow, save into matter_ota_buffer, and wait for the next data.
 * @param[in]  data: Pointer to the firmware data.
 * @param[in]  size: Size of the firmware data.
 * @return OTA_SUCCESS if write successful; OTA_ERROR otherwise
 */
int8_t matter_ota_flash_burst_write(uint8_t *data, uint32_t size);

/**
 * @brief  Write/Store the remaining data of Matter OTA firmware.
 *         After the last frame of Matter OTA, check for remaining data using matter_ota_buffer_size, 
 *         if there is remaining data, write it into flash.
 * @return OTA_SUCCESS if write successful; OTA_ERROR otherwise
 */
int8_t matter_ota_flush_last(void);

/**
 * @brief  Update signature after receiving the last frame.
 *         Update the signature to acknowledge the new firmware,
 *         and after device reboot, the new firmware will take effect.
 * @return OTA_SUCCESS if update successful; OTA_ERROR otherwise
 */
int8_t matter_ota_update_signature(void);

/**
 * @brief  Reset device after Matter OTA completes.
 */
void matter_ota_platform_reset(void);

/**
 * @brief  Create an abort task when Matter OTA terminates or is aborted.
 */
void matter_ota_create_abort_task(void);

#ifdef __cplusplus
}
#endif

#endif /* __RTK_MATTER_OTA_H__ */

