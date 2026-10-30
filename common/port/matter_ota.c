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
#include <platform_stdlib.h>
#include <stdbool.h>
#include <flash_api.h>
#include <sys_api.h>
#include <device_lock.h>
#include <chip_porting.h>
#include <FreeRTOS.h>
#include <task.h>
#include <ameba_ota.h>
#if defined(CONFIG_AMEBASMART)
#include <ameba_spinand.h>
#if defined(CONFIG_AMEBARTOS_V1_0) || defined(CONFIG_AMEBARTOS_V1_1)
#include <ftl_nand.h>
#endif
#endif
#if !defined(CONFIG_AMEBARTOS_V1_0) && !defined(CONFIG_AMEBARTOS_V1_1)
#include <ota_api.h> // for ameba-rtos v1.2 and above
#endif
#if defined(CONFIG_OTA_FW_SIGN_VERIFY) && (CONFIG_OTA_FW_SIGN_VERIFY == 1)
#include <ota_verify.h>
#endif

static const char *const TAG = "MATTER_OTA";

#define MATTER_OTA_SECTOR_SIZE 4096
#define MATTER_OTA_HEADER_SIZE 32

// For Ameba OTA header
#define MATTER_OTA_OFF_HDR_NUM      4
#define MATTER_OTA_SUB_HDR_OFFSET   8
#define MATTER_OTA_OFF_SIGNATURE    (MATTER_OTA_SUB_HDR_OFFSET + 0)
#define MATTER_OTA_OFF_IMG_HDR_LEN  (MATTER_OTA_SUB_HDR_OFFSET + 4)
#define MATTER_OTA_OFF_CHECKSUM     (MATTER_OTA_SUB_HDR_OFFSET + 8)
#define MATTER_OTA_OFF_IMG_LEN      (MATTER_OTA_SUB_HDR_OFFSET + 12)
#define MATTER_OTA_OFF_IMG_ID       (MATTER_OTA_SUB_HDR_OFFSET + 20)

#define MATTER_OTA_SIGNATURE_LEN    3 /* "OTA" prefix of the image signature */
#define MATTER_OTA_SUB_HDR_SIZE     24 /* ImgHdrLen the SDK emits */
#define MATTER_OTA_HDR_NUM          1  /* ota_all.bin carries a single image */
#define MATTER_OTA_IMG_ID_APP       OTA_IMGID_APP

static flash_t matter_ota_flash;

#if defined(CONFIG_AMEBARTOS_V1_0) || defined(CONFIG_AMEBARTOS_V1_1)
typedef update_ota_target_hdr ameba_ota_hdr_t;
typedef Manifest_TypeDef ameba_ota_manifest_t;

ota_context matterCtx = {0};
update_ota_ctrl_info matterOtaCtrl = {0};
update_ota_target_hdr matterOtaTargetHdr = {0};
#define MATTER_OTA_MANIFEST_SIZE    ((uint32_t)sizeof(ameba_ota_manifest_t))
#else
typedef ota_hdr_manager_t ameba_ota_hdr_t;
typedef ota_manifest_t ameba_ota_manifest_t;

ota_context_t matterCtx = {0};
ota_download_ctrl_t matterOtaCtrl = {0};
ota_hdr_manager_t matterOtaTargetHdr = {0};
#define MATTER_OTA_MANIFEST_SIZE    ((uint32_t)sizeof(ameba_ota_manifest_t))
#endif

_Static_assert(MATTER_OTA_MANIFEST_SIZE <= MATTER_OTA_SECTOR_SIZE,
               "manifest header must fit in the first flash sector");

static bool matter_ota_first_sector_written = false;
static uint32_t matter_ota_flash_sector_base;
static uint32_t matter_ota_new_firmware_addr_start;
static uint32_t matter_ota_new_firmware_addr_end;

static uint8_t matter_ota_header[MATTER_OTA_HEADER_SIZE];
static uint8_t matter_ota_header_size           = 0;    // variable to track size of ota header
static uint16_t matter_ota_buffer_size          = 0;    // variable to track size of buffer
static uint8_t *matter_ota_buffer;                      // buffer to receive 4KB of data

static bool matter_ota_header_parsed            = false;
static uint32_t matter_ota_expected_checksum    = 0;
static uint32_t matter_ota_expected_image_len   = 0;
static uint32_t matter_ota_image_bytes          = 0;    // image bytes received so far
static uint32_t matter_ota_image_checksum       = 0;    // running sum over those bytes

static uint8_t matter_ota_manifest[MATTER_OTA_MANIFEST_SIZE];
static uint32_t matter_ota_manifest_size        = 0;    // manifest bytes received so far

static const char *kOTACompleted = "ota_completed";

// SPI NAND Flash APIs for AmebaSmart
#if defined(CONFIG_AMEBASMART)
static const char *const TAG_NAND = "MATTER_OTA_NAND";

#define MATTER_OTA_NAND_FLASH_DEBUG 0
extern uint32_t IMG_ADDR[OTA_IMGID_MAX][2];
static uint32_t boot_from_nor;

// For NAND, first sector will be written at last together with manifest update
static uint8_t matter_ota_nand_first_sector[MATTER_OTA_SECTOR_SIZE] = {0};

// Bad blocks checking
#define MATTER_OTA_NAND_BBT_UNCHECKED  0
#define MATTER_OTA_NAND_BBT_GOOD       1
#define MATTER_OTA_NAND_BBT_BAD        2

static uint8_t  *matter_ota_nand_bbt = NULL;
static uint32_t matter_ota_nand_region_block_id_start;
static uint32_t matter_ota_nand_region_block_id_end;
static uint32_t matter_ota_nand_region_block_length;

static uint8_t matter_ota_nand_block_status(uint32_t block_id)
{
    uint32_t bbt_index = block_id - matter_ota_nand_region_block_id_start;
    if (bbt_index >= matter_ota_nand_region_block_length) {
        RTK_LOGI(TAG_NAND, "block_id 0x%x out of OTA region\n", block_id);
        return MATTER_OTA_NAND_BBT_UNCHECKED;
    }

    if (matter_ota_nand_bbt != NULL) {
        if (matter_ota_nand_bbt[bbt_index] != MATTER_OTA_NAND_BBT_UNCHECKED) {
            return matter_ota_nand_bbt[bbt_index];
        }
    }

    // unchecked read OOB spare byte 0+1, same method as NAND_CHECK_IS_BAD_BLOCK
    uint8_t oob[4];
    NAND_Page_Read(NAND_BLOCK_ID_TO_PAGE_ADDR(block_id), NAND_PAGE_SIZE_MAIN, 4, oob);
    if (oob[0] != 0xFF || oob[1] != 0xFF) {
        RTK_LOGI(TAG_NAND, "Block 0x%x is bad (OOB=0x%02x 0x%02x)\n", block_id, oob[0], oob[1]);
        return MATTER_OTA_NAND_BBT_BAD;
    }

    return MATTER_OTA_NAND_BBT_GOOD;
}

static void matter_ota_nand_create_bbt(void)
{
    // Get page address and block id of start and end address of the target OTA region
    uint32_t page_addr_start              = NAND_ADDR_TO_PAGE_ADDR(matter_ota_new_firmware_addr_start);
    uint32_t page_addr_end                = NAND_ADDR_TO_PAGE_ADDR(matter_ota_new_firmware_addr_end);
    matter_ota_nand_region_block_id_start = NAND_PAGE_ADDR_TO_BLOCK_ID(page_addr_start);
    matter_ota_nand_region_block_id_end   = NAND_PAGE_ADDR_TO_BLOCK_ID(page_addr_end);
    matter_ota_nand_region_block_length   = matter_ota_nand_region_block_id_end - matter_ota_nand_region_block_id_start + 1;

    // Dynamically assign the BBT depends on the OTA region size
    matter_ota_nand_bbt = (uint8_t *) rtos_mem_malloc(matter_ota_nand_region_block_length);
    if (matter_ota_nand_bbt == NULL) {
        RTK_LOGE(TAG_NAND, "matter_ota_nand_bbt is null\n");
        return;
    }
    memset(matter_ota_nand_bbt, MATTER_OTA_NAND_BBT_UNCHECKED, matter_ota_nand_region_block_length);

    // Init the BBT
    RTK_LOGI(TAG_NAND, "Scanning the NAND blocks...\n");
    for (int bbt_index = 0; bbt_index < matter_ota_nand_region_block_length; bbt_index++) {
        matter_ota_nand_bbt[bbt_index] = matter_ota_nand_block_status(matter_ota_nand_region_block_id_start + bbt_index);
    }
    RTK_LOGI(TAG_NAND, "Scanning finished!\n");
}

static void matter_ota_nand_free_bbt(void)
{
    if (matter_ota_nand_bbt != NULL) {
        rtos_mem_free(matter_ota_nand_bbt);
        matter_ota_nand_bbt = NULL;
    }
}

static int8_t matter_ota_nand_skip_bad_block(uint32_t *addr)
{
    uint32_t block_id = NAND_PAGE_ADDR_TO_BLOCK_ID(NAND_ADDR_TO_PAGE_ADDR(*addr));

    // Enter the loop only if the the block is within the target OTA region and if it is BB
    while (block_id <= matter_ota_nand_region_block_id_end &&
           matter_ota_nand_block_status(block_id) == MATTER_OTA_NAND_BBT_BAD) {
        // Keep looping until it arrives at the good block
        *addr     += NAND_BLOCK_PAGE_CNT * NAND_PAGE_SIZE_MAIN; // advance one full block
        block_id += 1;
    }

    // Return error if there is no more good blocks in the new OTA region
    if (block_id > matter_ota_nand_region_block_id_end) {
        return OTA_ERROR;
    }

    return OTA_SUCCESS;
}

static void matter_ota_nand_mark_bad_block(uint32_t block_id)
{
    uint8_t oob[2] = {0};

    // Physically mark it as bad block
    RTK_LOGI(TAG_NAND, "Block 0x%x Mark BB!\n", block_id);
    if (NAND_Page_Write(NAND_BLOCK_ID_TO_PAGE_ADDR(block_id), NAND_PAGE_SIZE_MAIN, 2, oob)) {
        RTK_LOGE(TAG_NAND, "Block 0x%x Mark BB Fail!\n", block_id);
    }

    // Update the BBT
    if (matter_ota_nand_bbt != NULL) {
        matter_ota_nand_bbt[block_id - matter_ota_nand_region_block_id_start] = MATTER_OTA_NAND_BBT_BAD;
    }
}

static int8_t matter_ota_nand_flash_program(uint32_t StartAddr, uint32_t DataLen, uint8_t *pData)
{
    // Flash
    uint32_t WriteAddr   = StartAddr;
    uint32_t DataLenLeft = DataLen;
    uint8_t *pNewData    = pData;
    // Pages
    uint32_t PageAddr, ByteAddr, ByteLen, BlockId;
    uint32_t page_begin = NAND_ADDR_TO_PAGE_ADDR(StartAddr);
    uint32_t page_end   = NAND_ADDR_TO_PAGE_ADDR(StartAddr + DataLen - 1);
    uint32_t page_cnt   = page_end - page_begin + 1;
    // Check Pages
#if MATTER_OTA_NAND_FLASH_DEBUG
    uint32_t check_page_mismatch = 0;
    uint8_t  check_page[NAND_PAGE_SIZE] = {0};
#endif // MATTER_OTA_NAND_FLASH_DEBUG
    while (page_cnt) {
        PageAddr = NAND_ADDR_TO_PAGE_ADDR(WriteAddr);
        BlockId  = NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr);
        ByteAddr = NAND_ADDR_TO_BYTE_ADDR(WriteAddr);
        ByteLen  = (page_cnt == 1) ? DataLenLeft : (NAND_PAGE_SIZE_MAIN - ByteAddr);

        if (NAND_Page_Write(PageAddr, ByteAddr, ByteLen, pNewData)) {
            // Bad block detected, mark the current block as bad, abort OTA!
            RTK_LOGE(TAG_NAND, "Block 0x%x Page 0x%x Program Fail!\n", BlockId, PageAddr & NAND_BLOCK_PAGE_MASK);
            matter_ota_nand_mark_bad_block(BlockId);
            return OTA_ERROR;
        }

#if MATTER_OTA_NAND_FLASH_DEBUG
        if (NAND_Page_Read(PageAddr, ByteAddr, ByteLen, check_page)) {
            // Bad block detected, mark the current block as bad, abort OTA!
            RTK_LOGE(TAG_NAND, "Block 0x%x Page 0x%x Read Fail!\n", BlockId, PageAddr & NAND_BLOCK_PAGE_MASK);
            matter_ota_nand_mark_bad_block(BlockId);
            return OTA_ERROR;
        } else {
            for (int i = 0; i < ByteLen; i++) {
                if (check_page[i] != *(pNewData + i)) {
                    check_page_mismatch += 1;
                }
            }
        }
#endif // MATTER_OTA_NAND_FLASH_DEBUG

        WriteAddr = WriteAddr + ByteLen;
        DataLenLeft = DataLenLeft - ByteLen;
        pNewData += ByteLen;
        page_cnt--;
    }

#if MATTER_OTA_NAND_FLASH_DEBUG
    if (check_page_mismatch > 0) {
        RTK_LOGD(TAG_NAND, "Total mismatch is %d\n", check_page_mismatch);
    }
#endif // MATTER_OTA_NAND_FLASH_DEBUG

    return OTA_SUCCESS;
}

static void matter_ota_nand_flash_erase_new_ota_region()
{
    for (int block_id = matter_ota_nand_region_block_id_start; block_id <= matter_ota_nand_region_block_id_end; block_id++) {
        if (matter_ota_nand_block_status(block_id) == MATTER_OTA_NAND_BBT_BAD) {
            // never erase a bad block (preserve marker)
            continue;
        } else if (NAND_Erase(NAND_BLOCK_ID_TO_PAGE_ADDR(block_id))) {
            // Bad block detected, mark the current block as bad, continue erasing the other blocks
            RTK_LOGE(TAG_NAND, "Block 0x%x Erase Fail!\n", block_id);
            matter_ota_nand_mark_bad_block(block_id);
        } else {
            RTK_LOGI(TAG_NAND, "Block 0x%x Erased Successfully!\n", block_id);
        }
    }
}

static uint32_t matter_ota_nand_flash_update_manifest(
                ameba_ota_hdr_t *pOtaTgtHdr,
                uint32_t ota_target_index,
                int index)
{
    // OTA
    uint32_t addr;
    ameba_ota_manifest_t *manifest = NULL;

    // Pages
    uint32_t PageAddr, ByteAddr, ByteLen, BlockId;

#if OTA_CLEAR_PATTERN
    u8 ota_old_index = ota_target_index ^ 1;
    u8 empty_sig[8] = {0x0};
#endif

    addr     = IMG_ADDR[pOtaTgtHdr->FileImgHdr[index].ImgID][ota_target_index];
    manifest = &pOtaTgtHdr->Manifest[index];
#if MATTER_OTA_NAND_FLASH_DEBUG
    matter_ota_nand_flash_check_manifest_pattern("manifest->Pattern", manifest->Pattern);
#endif // MATTER_OTA_NAND_FLASH_DEBUG

    RTK_LOGI(TAG_NAND, "update addr: 0x%08x\n", (unsigned int)addr);
#if defined(CONFIG_AMEBARTOS_V1_0) || defined(CONFIG_AMEBARTOS_V1_1)
    RTK_LOGI(TAG_NAND, "update version major: %d, minor: %d\n", manifest->MajorImgVer, manifest->MinorImgVer);
#else
    RTK_LOGI(TAG_NAND, "update version major: %d, minor: %d\n", manifest->MajorKeyVer, manifest->MinorKeyVer);
#endif

    PageAddr = NAND_ADDR_TO_PAGE_ADDR(addr - SPI_FLASH_BASE);
    BlockId  = NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr);
    ByteAddr = NAND_ADDR_TO_BYTE_ADDR(addr - SPI_FLASH_BASE);
    ByteLen  = MATTER_OTA_SECTOR_SIZE;

    /*write the manifest, write pattern finally*/
    memcpy(matter_ota_nand_first_sector + 8,
           (u8 *)manifest + 8,
           sizeof(ameba_ota_manifest_t) - 8);
    memcpy(matter_ota_nand_first_sector, (u8 *)manifest->Pattern, 8);

#if OTA_CLEAR_PATTERN
    if (strncmp("OTA", (const char *)pOtaTgtHdr->FileImgHdr[index].Signature, 3) == 0) {
        addr = IMG_ADDR[pOtaTgtHdr->FileImgHdr[index].ImgID][ota_old_index];
    } else {
        matter_ota_nand_free_bbt();
        return 0;
    }

    /*clear the old FW pattern to 0 finally*/
    RTK_LOGI(TAG_NAND, "ImgID: %lu, clear the old FW pattern, addr: 0x%08x\n", pOtaTgtHdr->FileImgHdr[index].ImgID, (unsigned int)addr);
    memcpy(matter_ota_nand_first_sector, empty_sig, 8);
#endif

#if MATTER_OTA_NAND_FLASH_DEBUG
    matter_ota_nand_flash_check_manifest_pattern("matter_ota_nand_first_sector before write", matter_ota_nand_first_sector);
#endif // MATTER_OTA_NAND_FLASH_DEBUG
    // Immediately abort if first sector is bad block
    if (matter_ota_nand_block_status(matter_ota_nand_region_block_id_start) == MATTER_OTA_NAND_BBT_BAD) {
        RTK_LOGE(TAG_NAND, "CRITICAL ERROR: FIRST BLOCK IS BAD BLOCK, MANIFEST MAY NOT BE UPDATED!!!!!\n");
        matter_ota_nand_free_bbt();
        return 0;
    }
    if (matter_ota_nand_flash_program(addr - SPI_FLASH_BASE, ByteLen, matter_ota_nand_first_sector) != OTA_SUCCESS) {
        // Bad block detected, block is marked bad inside the API, return 0
        RTK_LOGE(TAG_NAND, "Block 0x%x Page 0x%x Program Fail!\n", BlockId, PageAddr & NAND_BLOCK_PAGE_MASK);
        matter_ota_nand_free_bbt();
        return 0;
    } else {
        RTK_LOGI(TAG_NAND, "Update OTA success!\n");
    }

#if MATTER_OTA_NAND_FLASH_DEBUG
    if (NAND_Page_Read(PageAddr, ByteAddr, NAND_PAGE_SIZE, matter_ota_nand_first_sector)) {
        // Bad block detected, mark the starting block as bad.
        RTK_LOGE(TAG_NAND, "Block 0x%x Page 0x%x Read Fail!\n", BlockId, PageAddr & NAND_BLOCK_PAGE_MASK);
        matter_ota_nand_mark_bad_block(BlockId);
        matter_ota_nand_free_bbt();
        return 0;
    }
    matter_ota_nand_flash_check_manifest_pattern("matter_ota_nand_first_sector after read", matter_ota_nand_first_sector);
#endif // MATTER_OTA_NAND_FLASH_DEBUG

    matter_ota_nand_free_bbt();
    return 1;
}

#if MATTER_OTA_NAND_FLASH_DEBUG
static void matter_ota_nand_flash_check_manifest_pattern(char *var_name, uint8_t *buf)
{
    RTK_LOGD(TAG_NAND, "\n %s, manifest contains: ", var_name);
    for (int i = 0; i < 8; i++) {
        RTK_LOGD(NOTAG, "0x%02x ", *(buf + i));
    }
    RTK_LOGD(NOTAG, "\n");
}
#endif // MATTER_OTA_NAND_FLASH_DEBUG
#endif // CONFIG_AMEBASMART

#if defined(CONFIG_OTA_FW_SIGN_VERIFY) && (CONFIG_OTA_FW_SIGN_VERIFY == 1)
/*
 * Signed OTA image layout (see tools/scripts/image_process/op_ota_sign.py):
 *
 * [0]                       72-byte signature block ("RSIG" + r||s)
 * [72]                      32-byte Ameba OTA container header
 * [104]                     application image
 *
 * The signature covers the container header plus the application image, i.e.
 * everything after the signature block. The signature block is stripped here
 * and never reaches flash, so what lands in the slot is exactly what the
 * unsigned path would have written.
 */
static bool matter_ota_sig_block_parsed = false;
static uint8_t matter_ota_sig_block[OTA_SIG_BLOCK_LEN];
static uint32_t matter_ota_sig_block_size = 0;
static uint8_t matter_ota_sig[OTA_SIG_P256_SIG_LEN];

/* Raw container header, signed as the first verification segment */
static uint8_t matter_ota_sig_hdr[OTA_HEADER_LEN + OTA_SUB_HEADER_LEN * OTA_IMGID_MAX]
__attribute__((aligned(4)));
static uint32_t matter_ota_sig_hdr_size = 0;

/* Slot the image was written to, needed to locate it again for verification */
static uint8_t matter_ota_sig_slot_idx = 0;

static uint32_t matter_ota_sig_block_take(const uint8_t *data, uint32_t size)
{
    uint32_t take;

    if (matter_ota_sig_block_size >= OTA_SIG_BLOCK_LEN) {
        return 0;
    }

    take = OTA_SIG_BLOCK_LEN - matter_ota_sig_block_size;
    if (take > size) {
        take = size;
    }

    memcpy(&matter_ota_sig_block[matter_ota_sig_block_size], data, take);
    matter_ota_sig_block_size += take;

    if (matter_ota_sig_block_size < OTA_SIG_BLOCK_LEN) {
        return take;
    }

    if (ota_sig_parse_block(matter_ota_sig_block, OTA_SIG_BLOCK_LEN, matter_ota_sig) != OTA_OK) {
        RTK_LOGE(TAG, "OTA image is not signed or its signature block is invalid\n");
        return take;
    }

    matter_ota_sig_block_parsed = true;
    return take;
}

static int8_t matter_ota_sig_get_pubkey(uint32_t *addr, uint32_t *len)
{
    if (ota_pubkey_get_addr(addr, len) != OTA_OK) {
        RTK_LOGE(TAG, "No valid OTA public key in the OTA_PUBKEY region; "
                 "program ota_pubkey_area.bin there before using signed OTA\n");
        return OTA_ERROR;
    }
    return OTA_SUCCESS;
}

static int8_t matter_ota_verify_signature(void)
{
    ota_sig_seg_t segs[2];
    uint32_t pubkey_addr = 0;
    uint32_t pubkey_len = 0;

    if (!matter_ota_sig_block_parsed) {
        RTK_LOGE(TAG, "No OTA signature block was received; refusing unsigned image\n");
        return OTA_ERROR;
    }

    if (matter_ota_sig_hdr_size == 0) {
        RTK_LOGE(TAG, "Ameba OTA header was never received\n");
        return OTA_ERROR;
    }

    if (matter_ota_sig_get_pubkey(&pubkey_addr, &pubkey_len) != OTA_SUCCESS) {
        return OTA_ERROR;
    }

    segs[0].region   = OTA_SIG_REGION_RAM;
    segs[0].len      = matter_ota_sig_hdr_size;
    segs[0].head     = matter_ota_sig_hdr;
    segs[0].head_len = matter_ota_sig_hdr_size;

#if defined(CONFIG_AMEBARTOS_V1_0) || defined(CONFIG_AMEBARTOS_V1_1)
    segs[1].region = (matterOtaCtrl.targetIdx == OTA_INDEX_1) ? IMG_APP_OTA1 : IMG_APP_OTA2;
#else
    segs[1].region = (matter_ota_sig_slot_idx == OTA_INDEX_1) ? IMG_APP_OTA1 : IMG_APP_OTA2;
#endif
    segs[1].len      = matter_ota_expected_image_len;
    segs[1].head     = matter_ota_manifest;
    segs[1].head_len = MATTER_OTA_MANIFEST_SIZE;

    if (ota_verify_firmware_sig(segs, 2, matter_ota_sig, OTA_SIG_P256_SIG_LEN,
                                pubkey_addr, pubkey_len) != OTA_OK) {
        RTK_LOGE(TAG, "OTA image signature verification failed\n");
        return OTA_ERROR;
    }

    RTK_LOGI(TAG, "OTA image signature verified (%u bytes)\n",
             (unsigned int)matter_ota_expected_image_len);

    return OTA_SUCCESS;
}
#endif /* CONFIG_OTA_FW_SIGN_VERIFY */

uint8_t matter_ota_get_total_header_size(void)
{
    uint8_t size = 0;
#if defined(CONFIG_OTA_FW_SIGN_VERIFY) && (CONFIG_OTA_FW_SIGN_VERIFY == 1)
    size = MATTER_OTA_HEADER_SIZE + OTA_SIG_BLOCK_LEN;
#else
    size = MATTER_OTA_HEADER_SIZE;
#endif
    RTK_LOGI(TAG, "total header size(%d)\n", size);
    return size;
}

uint8_t matter_ota_get_current_header_size(void)
{
    uint8_t size = 0;
#if defined(CONFIG_OTA_FW_SIGN_VERIFY) && (CONFIG_OTA_FW_SIGN_VERIFY == 1)
    size = matter_ota_sig_block_size + matter_ota_header_size;
#else
    size =  matter_ota_header_size;
#endif
    RTK_LOGI(TAG, "current header size(%d)\n", size);
    return size;
}

void matter_ota_prepare_partition(void)
{
    matter_ota_buffer = (uint8_t *)malloc(MATTER_OTA_SECTOR_SIZE);
    if (matter_ota_buffer == NULL) {
        RTK_LOGE(TAG, "matter_ota_buffer malloc failed\n");
        return;
    }

    matter_ota_first_sector_written = false;

    memset(matter_ota_buffer, 0, MATTER_OTA_SECTOR_SIZE);
    memset(matter_ota_header, 0, sizeof(matter_ota_header));
    matter_ota_header_size = 0;
    matter_ota_buffer_size = 0;

    matter_ota_header_parsed = false;
    matter_ota_expected_checksum = 0;
    matter_ota_expected_image_len = 0;
    matter_ota_image_bytes = 0;
    matter_ota_image_checksum = 0;
    memset(matter_ota_manifest, 0, sizeof(matter_ota_manifest));
    matter_ota_manifest_size = 0;

#if defined(CONFIG_OTA_FW_SIGN_VERIFY) && (CONFIG_OTA_FW_SIGN_VERIFY == 1)
    matter_ota_sig_block_parsed = false;
    memset(matter_ota_sig_block, 0, sizeof(matter_ota_sig_block));
    matter_ota_sig_block_size = 0;
    memset(matter_ota_sig, 0, sizeof(matter_ota_sig));
    memset(matter_ota_sig_hdr, 0, sizeof(matter_ota_sig_hdr));
    matter_ota_sig_hdr_size = 0;
#endif

    memset(&matterCtx, 0, sizeof(matterCtx));
    memset(&matterOtaCtrl, 0, sizeof(matterOtaCtrl));
    memset(&matterOtaTargetHdr, 0, sizeof(matterOtaTargetHdr));

#if defined(CONFIG_AMEBARTOS_V1_0) || defined(CONFIG_AMEBARTOS_V1_1)
    matterCtx.otactrl = &matterOtaCtrl;
    matterCtx.otaTargetHdr = &matterOtaTargetHdr;

    matterCtx.otactrl->ImgId = OTA_IMGID_APP;
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_1].ImgID = OTA_IMGID_APP;
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_2].ImgID = OTA_IMGID_APP;

    if (ota_get_cur_index(matterCtx.otactrl->ImgId) == OTA_INDEX_1) {
        matterCtx.otactrl->index = OTA_INDEX_1;
        matterCtx.otactrl->targetIdx = OTA_INDEX_2;
        flash_get_layout_info(IMG_APP_OTA2, &matter_ota_new_firmware_addr_start, &matter_ota_new_firmware_addr_end);
    } else {
        matterCtx.otactrl->index = OTA_INDEX_2;
        matterCtx.otactrl->targetIdx = OTA_INDEX_1;
        flash_get_layout_info(IMG_APP_OTA1, &matter_ota_new_firmware_addr_start, &matter_ota_new_firmware_addr_end);
    }
#if defined(CONFIG_OTA_FW_SIGN_VERIFY) && (CONFIG_OTA_FW_SIGN_VERIFY == 1)
    matter_ota_sig_slot_idx = matterCtx.otactrl->targetIdx;
#endif
    matterCtx.otactrl->FlashAddr       = matter_ota_new_firmware_addr_start;
#else
    matterCtx.otaCtrl = &matterOtaCtrl;
    matterCtx.otaHdrManager = &matterOtaTargetHdr;

    /* only ImgID is read by ota_update_manifest(). */
    matterCtx.otaCtrl->ImgId = OTA_IMGID_APP;
    matterCtx.otaHdrManager->FileImgHdr[OTA_INDEX_1].ImgID = OTA_IMGID_APP;
    matterCtx.otaHdrManager->FileImgHdr[OTA_INDEX_2].ImgID = OTA_IMGID_APP;

    if (ota_get_cur_index(matterCtx.otaCtrl->ImgId) == OTA_INDEX_1) {
        matterCtx.otaCtrl->index = OTA_INDEX_1;
        matterCtx.otaCtrl->slotIdx = OTA_INDEX_2;
        flash_get_layout_info(IMG_APP_OTA2, &matter_ota_new_firmware_addr_start, &matter_ota_new_firmware_addr_end);
    } else {
        matterCtx.otaCtrl->index = OTA_INDEX_2;
        matterCtx.otaCtrl->slotIdx = OTA_INDEX_1;
        flash_get_layout_info(IMG_APP_OTA1, &matter_ota_new_firmware_addr_start, &matter_ota_new_firmware_addr_end);
    }
#if defined(CONFIG_OTA_FW_SIGN_VERIFY) && (CONFIG_OTA_FW_SIGN_VERIFY == 1)
    matter_ota_sig_slot_idx = matterCtx.otaCtrl->slotIdx;
#endif
    matterCtx.otaCtrl->FlashAddr       = matter_ota_new_firmware_addr_start;
#endif

    matter_ota_new_firmware_addr_start = matter_ota_new_firmware_addr_start - SPI_FLASH_BASE;
    matter_ota_new_firmware_addr_end   = matter_ota_new_firmware_addr_end   - SPI_FLASH_BASE;
    matter_ota_flash_sector_base       = matter_ota_new_firmware_addr_start; // Note that the new fw address must be multiples of 4KB

#if defined(CONFIG_AMEBASMART)
#if defined(CONFIG_AMEBARTOS_V1_0) || defined(CONFIG_AMEBARTOS_V1_1)
    boot_from_nor = SYSCFG_BootFromNor();
#else
    boot_from_nor = SYSCFG_OTP_BootFromNor();
#endif
    if (!boot_from_nor) { // NAND
        matter_ota_nand_create_bbt();
        matter_ota_nand_flash_erase_new_ota_region();
        if (matter_ota_nand_block_status(matter_ota_nand_region_block_id_start) == MATTER_OTA_NAND_BBT_BAD) {
            RTK_LOGE(TAG_NAND, "CRITICAL ERROR: FIRST BLOCK IS BAD BLOCK, OTA MAY NOT PROCEED!!!!!\n");
        }
    }
#endif
}

static uint32_t matter_ota_read_le32(const uint8_t *buf)
{
    return (uint32_t)buf[0] | ((uint32_t)buf[1] << 8) |
           ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
}

static int8_t matter_ota_parse_header(void)
{
    if (matter_ota_header_size != MATTER_OTA_HEADER_SIZE) {
        RTK_LOGE(TAG, "Ameba OTA header incomplete (%u/%u bytes)\n",
                 (unsigned int)matter_ota_header_size, (unsigned int)MATTER_OTA_HEADER_SIZE);
        return OTA_ERROR;
    }

    if (matter_ota_read_le32(&matter_ota_header[MATTER_OTA_OFF_HDR_NUM]) != MATTER_OTA_HDR_NUM) {
        RTK_LOGE(TAG, "Ameba OTA header declares %u images, expected %u\n",
                 (unsigned int)matter_ota_read_le32(&matter_ota_header[MATTER_OTA_OFF_HDR_NUM]),
                 (unsigned int)MATTER_OTA_HDR_NUM);
        return OTA_ERROR;
    }

    if (memcmp(&matter_ota_header[MATTER_OTA_OFF_SIGNATURE], "OTA", MATTER_OTA_SIGNATURE_LEN) != 0) {
        RTK_LOGE(TAG, "Ameba OTA image signature is not \"OTA...\"\n");
        return OTA_ERROR;
    }

    if (matter_ota_read_le32(&matter_ota_header[MATTER_OTA_OFF_IMG_HDR_LEN]) != MATTER_OTA_SUB_HDR_SIZE) {
        RTK_LOGE(TAG, "Ameba OTA image header length is %u, expected %u\n",
                 (unsigned int)matter_ota_read_le32(&matter_ota_header[MATTER_OTA_OFF_IMG_HDR_LEN]),
                 (unsigned int)MATTER_OTA_SUB_HDR_SIZE);
        return OTA_ERROR;
    }

    if (matter_ota_read_le32(&matter_ota_header[MATTER_OTA_OFF_IMG_ID]) != MATTER_OTA_IMG_ID_APP) {
        RTK_LOGE(TAG, "Ameba OTA image ID is %u, expected %u (application)\n",
                 (unsigned int)matter_ota_read_le32(&matter_ota_header[MATTER_OTA_OFF_IMG_ID]),
                 (unsigned int)MATTER_OTA_IMG_ID_APP);
        return OTA_ERROR;
    }

    matter_ota_expected_image_len = matter_ota_read_le32(&matter_ota_header[MATTER_OTA_OFF_IMG_LEN]);
    matter_ota_expected_checksum = matter_ota_read_le32(&matter_ota_header[MATTER_OTA_OFF_CHECKSUM]);

    uint32_t slot_size = matter_ota_new_firmware_addr_end - matter_ota_new_firmware_addr_start + 1;
    if (matter_ota_expected_image_len > slot_size) {
        RTK_LOGE(TAG, "Ameba OTA image length %u exceeds the %u byte target slot\n",
                 (unsigned int)matter_ota_expected_image_len, (unsigned int)slot_size);
        return OTA_ERROR;
    }

    matter_ota_header_parsed = true;

#if defined(CONFIG_OTA_FW_SIGN_VERIFY) && (CONFIG_OTA_FW_SIGN_VERIFY == 1)
    /* Keep the raw bytes: the signature covers the header exactly as received */
    memcpy(matter_ota_sig_hdr, matter_ota_header, MATTER_OTA_HEADER_SIZE);
    matter_ota_sig_hdr_size = MATTER_OTA_HEADER_SIZE;
#endif

    RTK_LOGI(TAG, "Ameba OTA header OK: image length %u, checksum 0x%08x\n",
             (unsigned int)matter_ota_expected_image_len,
             (unsigned int)matter_ota_expected_checksum);

    return OTA_SUCCESS;
}

int8_t matter_ota_store_header(uint8_t *data, uint32_t size)
{
#if defined(CONFIG_OTA_FW_SIGN_VERIFY) && (CONFIG_OTA_FW_SIGN_VERIFY == 1)
    /* Drop the leading signature block before the container header is stored */
    uint32_t taken = matter_ota_sig_block_take(data, size);

    data += taken;
    size -= taken;

    if (size == 0) {
        /* The whole chunk was signature block; the header starts later */
        return OTA_SUCCESS;
    }
#endif

    if (size + matter_ota_header_size > MATTER_OTA_HEADER_SIZE) {
        return OTA_ERROR;
    }

    memcpy(&(matter_ota_header[matter_ota_header_size]), data, size);
    matter_ota_header_size += size;

    if (matter_ota_header_size == MATTER_OTA_HEADER_SIZE) {
        return matter_ota_parse_header();
    }

    return OTA_SUCCESS;
}

int8_t matter_ota_flash_burst_write(uint8_t *data, uint32_t size)
{
    if (size == 0) {
        return OTA_SUCCESS;
    }

    for (uint32_t i = 0; i < size; i++) {
        matter_ota_image_checksum += data[i];
    }
    matter_ota_image_bytes += size;

    if (matter_ota_manifest_size < MATTER_OTA_MANIFEST_SIZE) {
        RTK_LOGI(TAG, "Write Manifest size(%d / %d)...\n", matter_ota_manifest_size, MATTER_OTA_MANIFEST_SIZE);
        uint32_t take = MATTER_OTA_MANIFEST_SIZE - matter_ota_manifest_size;
        if (take > size) {
            take = size;
        }
        memcpy(matter_ota_manifest + matter_ota_manifest_size, data, take);
        matter_ota_manifest_size += take;
        data += take;
        size -= take;

        if (size == 0) {
            return OTA_SUCCESS;
        }
    }

    bool overflow = false;
    uint32_t sectorBase = matter_ota_flash_sector_base;
    uint32_t writeLength = MATTER_OTA_SECTOR_SIZE;
    int16_t bufferRemainSize = (int16_t)(MATTER_OTA_SECTOR_SIZE - matter_ota_buffer_size);

    if (!matter_ota_first_sector_written) {
        sectorBase += MATTER_OTA_MANIFEST_SIZE;
        writeLength -= MATTER_OTA_MANIFEST_SIZE;
        bufferRemainSize -= MATTER_OTA_MANIFEST_SIZE;
    }

    if (bufferRemainSize >= size) {
        memcpy(matter_ota_buffer + matter_ota_buffer_size, data, size);
        matter_ota_buffer_size += size;
    } else {
        memcpy(matter_ota_buffer + matter_ota_buffer_size, data, bufferRemainSize);
        matter_ota_buffer_size += bufferRemainSize;
        overflow = true;
        size -= bufferRemainSize;
    }

    if (matter_ota_buffer_size == writeLength) {
#if defined(CONFIG_AMEBASMART)
        if (boot_from_nor) { // NOR
            // buffer is full, time to erase sector and write buffer data to flash
            flash_erase_sector(&matter_ota_flash, matter_ota_flash_sector_base);
            flash_burst_write(&matter_ota_flash, sectorBase, writeLength, matter_ota_buffer);
        } else { // NAND
            if (!matter_ota_first_sector_written) {
                // For NAND, first sector will be written at last together with manifest update
                memcpy(matter_ota_nand_first_sector + MATTER_OTA_MANIFEST_SIZE, matter_ota_buffer, writeLength);
                // Immediately abort OTA if first sector is bad block
                if (matter_ota_nand_block_status(matter_ota_nand_region_block_id_start) == MATTER_OTA_NAND_BBT_BAD) {
                    RTK_LOGE(TAG_NAND, "CRITICAL ERROR: FIRST BLOCK IS BAD BLOCK, OTA MAY NOT PROCEED!!!!!\n");
                    return OTA_ERROR;
                }
            } else {
                if (matter_ota_nand_flash_program(sectorBase, writeLength, matter_ota_buffer) != OTA_SUCCESS) {
                    // Bad block detected, abort OTA!
                    return OTA_ERROR;
                }
            }
        }
#else
        // buffer is full, time to erase sector and write buffer data to flash
        flash_erase_sector(&matter_ota_flash, matter_ota_flash_sector_base);
        flash_burst_write(&matter_ota_flash, sectorBase, writeLength, matter_ota_buffer);
#endif
        if (!matter_ota_first_sector_written) {
            matter_ota_first_sector_written = true;
        }

        matter_ota_flash_sector_base += MATTER_OTA_SECTOR_SIZE; // point to next sector

#if defined(CONFIG_AMEBASMART)
        if (!boot_from_nor) {
            uint32_t current_page = NAND_ADDR_TO_PAGE_ADDR(matter_ota_flash_sector_base);
            // Only do checking if it is the first page of a block
            if ((current_page & NAND_BLOCK_PAGE_MASK) == 0) {
                // It will not skip block(s) if it is a good block
                if (matter_ota_nand_skip_bad_block(&matter_ota_flash_sector_base) != OTA_SUCCESS) {
                    RTK_LOGE(TAG_NAND, "No good block left in OTA region!\n");
                    return OTA_ERROR;
                }
            }
        }
#endif

        memset(matter_ota_buffer, 0, MATTER_OTA_SECTOR_SIZE); // clear buffer after writing
        matter_ota_buffer_size = 0;
    }

    if (overflow) { // write remaining data into the newly cleared buffer
        // TODO: what if it overflows twice?
        memcpy(matter_ota_buffer + matter_ota_buffer_size, data + bufferRemainSize, size);
        matter_ota_buffer_size += size;
    }

    return OTA_SUCCESS;
}

int8_t matter_ota_flush_last(void)
{
    if (matter_ota_manifest_size != MATTER_OTA_MANIFEST_SIZE) {
        RTK_LOGE(TAG, "Ameba OTA manifest header incomplete (%u/%u bytes)\n",
                 (unsigned int)matter_ota_manifest_size, (unsigned int)MATTER_OTA_MANIFEST_SIZE);
        return OTA_ERROR;
    }

    if (matter_ota_buffer_size > 0) {
        uint32_t sectorBase = matter_ota_flash_sector_base;
        if (!matter_ota_first_sector_written) {
            sectorBase += MATTER_OTA_MANIFEST_SIZE;
        }
#if defined(CONFIG_AMEBASMART)
        if (boot_from_nor) { // NOR
            flash_erase_sector(&matter_ota_flash, matter_ota_flash_sector_base);
            flash_burst_write(&matter_ota_flash, sectorBase, matter_ota_buffer_size, matter_ota_buffer);
        } else { // NAND
            if (matter_ota_nand_flash_program(sectorBase, matter_ota_buffer_size, matter_ota_buffer) != OTA_SUCCESS) {
                // Bad block detected, abort OTA!
                return OTA_ERROR;
            }
        }
#else
        flash_erase_sector(&matter_ota_flash, matter_ota_flash_sector_base);
        flash_burst_write(&matter_ota_flash, sectorBase, matter_ota_buffer_size, matter_ota_buffer);
#endif

        matter_ota_flash_sector_base += MATTER_OTA_SECTOR_SIZE; // point to next sector
        memset(matter_ota_buffer, 0, MATTER_OTA_SECTOR_SIZE); // clear buffer after writing
        matter_ota_buffer_size = 0;
    }

    free(matter_ota_buffer);
    matter_ota_buffer = NULL;

    return OTA_SUCCESS;
}

static int8_t matter_ota_verify_image(void)
{
    if (!matter_ota_header_parsed) {
        RTK_LOGE(TAG, "Ameba OTA header was never received\n");
        return OTA_ERROR;
    }

    if (matter_ota_image_bytes != matter_ota_expected_image_len) {
        RTK_LOGE(TAG, "Ameba OTA image length mismatch: got %u, header declares %u\n",
                 (unsigned int)matter_ota_image_bytes,
                 (unsigned int)matter_ota_expected_image_len);
        return OTA_ERROR;
    }

    if (matter_ota_image_checksum != matter_ota_expected_checksum) {
        RTK_LOGE(TAG, "Ameba OTA image checksum mismatch: computed 0x%08x, header declares 0x%08x\n",
                 (unsigned int)matter_ota_image_checksum,
                 (unsigned int)matter_ota_expected_checksum);
        return OTA_ERROR;
    }

    RTK_LOGI(TAG, "Ameba OTA image verified: %u bytes, checksum 0x%08x\n",
             (unsigned int)matter_ota_image_bytes,
             (unsigned int)matter_ota_image_checksum);

    return OTA_SUCCESS;
}

int8_t matter_ota_update_signature(void)
{
    if (matter_ota_verify_image() != OTA_SUCCESS) {
        return OTA_ERROR;
    }

#if defined(CONFIG_OTA_FW_SIGN_VERIFY) && (CONFIG_OTA_FW_SIGN_VERIFY == 1)
    if (matter_ota_verify_signature() != OTA_SUCCESS) {
        return OTA_ERROR;
    }
#endif

#if defined(CONFIG_AMEBARTOS_V1_0) || defined(CONFIG_AMEBARTOS_V1_1)
    memcpy(&(matterCtx.otaTargetHdr->Manifest[matterCtx.otactrl->index]), matter_ota_manifest, MATTER_OTA_MANIFEST_SIZE);
#if defined(CONFIG_AMEBASMART)
    if (boot_from_nor) { // NOR
        if (!ota_update_manifest(matterCtx.otaTargetHdr, matterCtx.otactrl->targetIdx, matterCtx.otactrl->index)) {
            return OTA_ERROR;
        }
    } else { // NAND
        if (!matter_ota_nand_flash_update_manifest(matterCtx.otaTargetHdr, matterCtx.otactrl->targetIdx, matterCtx.otactrl->index)) {
            return OTA_ERROR;
        }
    }
#else
    if (!ota_update_manifest(matterCtx.otaTargetHdr, matterCtx.otactrl->targetIdx, matterCtx.otactrl->index)) {
        return OTA_ERROR;
    }
#endif // CONFIG_AMEBAXXX
#else
    memcpy(&(matterCtx.otaHdrManager->Manifest[matterCtx.otaCtrl->index]), matter_ota_manifest, MATTER_OTA_MANIFEST_SIZE);
#if defined(CONFIG_AMEBASMART)
    if (boot_from_nor) { // NOR
        if (ota_storage_update_manifest(matterCtx.otaHdrManager, matterCtx.otaCtrl->slotIdx, matterCtx.otaCtrl->index) != OTA_OK) {
            return OTA_ERROR;
        }
    } else { // NAND
        if (!matter_ota_nand_flash_update_manifest(matterCtx.otaHdrManager, matterCtx.otaCtrl->slotIdx, matterCtx.otaCtrl->index)) {
            return OTA_ERROR;
        }
    }
#else
    if (ota_storage_update_manifest(matterCtx.otaHdrManager, matterCtx.otaCtrl->slotIdx, matterCtx.otaCtrl->index) != OTA_OK) {
        return OTA_ERROR;
    }
#endif // CONFIG_AMEBAXXX
#endif // CONFIG_AMEBARTOS_V1_X
    return OTA_SUCCESS;
}

uint8_t matter_get_ota_completed_value(void)
{
    uint8_t value = 0;
    getPref_bool_new(kOTACompleted, kOTACompleted, &value);
    return value;
}

void matter_ota_platform_reset(void)
{
    uint8_t value = 1;

    deleteKey(kOTACompleted, kOTACompleted);

    if (setPref_new(kOTACompleted, kOTACompleted, &value, sizeof(value)) != DCT_SUCCESS) {
        RTK_LOGI(TAG, "[%s] set persist storage failed\n", __FUNCTION__);
        return;
    }

    rtos_time_delay_ms(100);
    sys_reset();
}

static void matter_ota_abort_task(void *pvParameters)
{
    uint32_t newFWBlkSize = 0;

    if (matter_ota_new_firmware_addr_start != 0 &&
        matter_ota_flash_sector_base >= matter_ota_new_firmware_addr_start) {
        uint32_t written   = matter_ota_flash_sector_base - matter_ota_new_firmware_addr_start;
        uint32_t slot_size = matter_ota_new_firmware_addr_end - matter_ota_new_firmware_addr_start + 1;

        newFWBlkSize = written / MATTER_OTA_SECTOR_SIZE;
        if (newFWBlkSize * MATTER_OTA_SECTOR_SIZE > slot_size) {
            /* Never erase past the region, even if the base is stale */
            newFWBlkSize = slot_size / MATTER_OTA_SECTOR_SIZE;
        }
    }

    RTK_LOGI(TAG, "Cleaning up aborted OTA\r\n");
    RTK_LOGI(TAG, "Erasing %d sectors\r\n", newFWBlkSize);

    if (matter_ota_new_firmware_addr_start != 0) {
        rtos_time_delay_ms(2); // to avoid undefined behaviour when it suddenly resets the ameba during flash erase
#if defined(CONFIG_AMEBASMART)
        if (!boot_from_nor) { // NAND
            /* NAND has no per-sector erase: the whole new OTA region goes at once */
            matter_ota_nand_flash_erase_new_ota_region();
            matter_ota_nand_free_bbt();
        } else
#endif
        {
            for (size_t i = 0; i < newFWBlkSize; i++) {
                flash_erase_sector(&matter_ota_flash, matter_ota_new_firmware_addr_start + (i * MATTER_OTA_SECTOR_SIZE));
            }
        }
    }
    matter_ota_first_sector_written = false;
    matter_ota_header_parsed = false;

    free(matter_ota_buffer);
    matter_ota_buffer = NULL;

    vTaskDelete(NULL);
}

void matter_ota_create_abort_task(void)
{
    if (xTaskCreate(matter_ota_abort_task, "matter_ota_abort", 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        RTK_LOGE(TAG, "[%s] Failed to create matter_ota_abort_task\n", __FUNCTION__);
    }
}
