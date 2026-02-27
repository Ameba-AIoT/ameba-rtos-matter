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

#pragma once
/******************************************************
 *               Functions
 ******************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *               Utilities and Information
 ******************************************************/

/**
 * @brief  Perform device factory reset.
 */
void matter_factory_reset(void);

/**
 * @brief  Check if the Matter server is commissioned.
 * @return  true if the server is commissioned, false otherwise.
 */
bool matter_server_is_commissioned(void);

/**
 * @brief  Get the fabric indexes.
 * @param[out] pFabricIndexes: Pointer to the buffer where fabric indexes will be stored.
 * @param[in]  bufSize: Size of the buffer.
 */
void matter_get_fabric_indexes(uint16_t *pFabricIndexes, size_t bufSize);

/**
 * @brief  Print onboarding codes.
 */
void matter_print_onboarding_codes(void);

/**
 * @brief  Get the manual pairing code.
 * @param[out] buf: Pointer to the buffer where the manual pairing code will be stored.
 * @param[in]  bufSize: Size of the buffer.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_manual_pairing_code(char *buf, size_t bufSize);

/**
 * @brief  Get the QR code.
 * @param[out]  buf: Pointer to the buffer where the QR code will be stored.
 * @param[in]  bufSize: Size of the buffer.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_qr_code(char *buf, size_t bufSize);

/**
 * @brief  Open the basic commissioning window (BCM).
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_open_basic_commissioning_window(void);

/**
 * @brief  Close the basic commissioning window (BCM).
 * @return  0 if success; -1 if failed.
 */
void matter_close_basic_commissioning_window(void);

/******************************************************
 *               Device Attestation Credentials
 ******************************************************/

/**
 * @brief  Get the certificate declaration.
 * @param[out] buf       Buffer where the certificate declaration will be stored.
 * @param[in]  bufSize   Size of the buffer.
 * @param[out] outLen    Actual number of bytes written into buf.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_certificate_declaration(uint8_t *buf, size_t bufSize, size_t *outLen);

/**
 * @brief  Get the DAC certificate.
 * @param[out] buf       Buffer where the DAC certificate will be stored.
 * @param[in]  bufSize   Size of the buffer.
 * @param[out] outLen    Actual number of bytes written into buf.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_dac_cert(uint8_t *buf, size_t bufSize, size_t *outLen);

/**
 * @brief  Get the PAI certificate.
 * @param[out] buf       Buffer where the PAI certificate will be stored.
 * @param[in]  bufSize   Size of the buffer.
 * @param[out] outLen    Actual number of bytes written into buf.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_pai_cert(uint8_t *buf, size_t bufSize, size_t *outLen);

/**
 * @brief  Get the firmware information.
 * @param[out] buf       Buffer where the firmware information will be stored.
 * @param[in]  bufSize   Size of the buffer.
 * @param[out] outLen    Actual number of bytes written into buf.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_firmware_information(uint8_t *buf, size_t bufSize, size_t *outLen);

/******************************************************
 *               Commissionable Data
 ******************************************************/

/**
 * @brief  Get the setup discriminator.
 * @param[out] discriminator: Reference to the variable where the discriminator will be stored.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_setup_discriminator(uint16_t *discriminator);

/**
 * @brief  Get the SPAKE2+ iteration count.
 * @param[out]  iterationCount: Reference to the variable where the iteration count will be stored.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_spake2p_iteration_count(uint32_t *iterationCount);

/**
 * @brief  Get the SPAKE2P salt.
 * @param[out] buf       Buffer where the SPAKE2P salt will be stored.
 * @param[in]  bufSize   Size of the buffer.
 * @param[out] outLen    Actual number of bytes written into buf.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_spake2p_salt(uint8_t *buf, size_t bufSize, size_t *outLen);

/**
 * @brief  Get the SPAKE2P verifier.
 * @param[out] buf       Buffer where the SPAKE2P verifier will be stored.
 * @param[in]  bufSize   Size of the buffer.
 * @param[out] outLen    Actual number of bytes written into buf.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_spake2p_verifier(uint8_t *buf, size_t bufSize, size_t *outLen);

/**
 * @brief  Get the setup passcode.
 * @param[out]  passcode: Reference to the variable where the passcode will be stored.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_setup_passcode(uint32_t *passcode);

/******************************************************
 *               Device Instance Information
 ******************************************************/

/**
 * @brief  Get the vendor name.
 * @param[out] buf: Buffer where the vendor name will be stored.
 * @param[in]  bufSize: Size of the buffer.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_vendor_name(char *buf, size_t bufSize);

/**
 * @brief  Get the vendor ID.
 * @param[out]  vendorId: Reference to the variable where the vendor ID will be stored.
 * @return  CHIP_NO_ERROR if retrieve successfully.
 */
uint8_t matter_get_vendor_id(uint16_t *vendorId);

/**
 * @brief  Get the product name.
 * @param[out] buf: Buffer where the product name will be stored.
 * @param[in]  bufSize: Size of the buffer.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_product_name(char *buf, size_t bufSize);

/**
 * @brief  Get the product ID.
 * @param[out]  productId: Reference to the variable where the product ID will be stored.
 * @return  CHIP_NO_ERROR if retrieve successfully.
 */
uint8_t matter_get_product_id(uint16_t *productId);

/**
 * @brief  Get the part number.
 * @param[out] buf: Buffer where the part number will be stored.
 * @param[in]  bufSize: Size of the buffer.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_part_number(char *buf, size_t bufSize);

/**
 * @brief  Get the product URL.
 * @param[out] buf: Buffer where the product URL will be stored.
 * @param[in]  bufSize: Size of the buffer.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_product_url(char *buf, size_t bufSize);

/**
 * @brief  Get the product label.
 * @param[out] buf: Buffer where the product label will be stored.
 * @param[in]  bufSize: Size of the buffer.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_product_label(char *buf, size_t bufSize);

/**
 * @brief  Get the serial number.
 * @param[out] buf: Buffer where the serial number will be stored.
 * @param[in]  bufSize: Size of the buffer.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_serial_number(char *buf, size_t bufSize);

/**
 * @brief  Get the manufacturing date.
 * @param[out] year: Reference to the variable where the year will be stored.
 * @param[out] month: Reference to the variable where the month will be stored.
 * @param[out] day: Reference to the variable where the day will be stored.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_manufacturing_date(uint16_t *year, uint8_t *month, uint8_t *day);

/**
 * @brief  Get the hardware version.
 * @param[out] hardwareVersion: Reference to the variable where the hardware version will be stored.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_hardware_version(uint16_t *hardwareVersion);

/**
 * @brief  Get the hardware version string.
 * @param[out] buf: Buffer where the hardware version string will be stored.
 * @param[in]  bufSize: Size of the buffer.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_hardware_version_string(char *buf, size_t bufSize);

/**
 * @brief  Get the software version.
 * @param[out]  softwareVersion: Reference to the variable where the software version will be stored.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_software_version(uint32_t *softwareVersion);

/**
 * @brief  Get the software version string.
 * @param[out] buf: Buffer where the software version string will be stored.
 * @param[in]  bufSize: Size of the buffer.
 * @return  0 if success; -1 if failed.
 */
uint8_t matter_get_software_version_string(char *buf, size_t bufSize);

#ifdef __cplusplus
}
#endif
