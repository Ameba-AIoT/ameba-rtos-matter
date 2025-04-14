#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#include <platform_opts.h>
#include <platform/platform_stdlib.h>
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include <platform_stdlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <flash_api.h>
#include <sys_api.h>
#include <device_lock.h>
#include <chip_porting.h>
#if defined(CONFIG_PLATFORM_8710C)
#include <ota_8710c.h>
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include <FreeRTOS.h>
#include <task.h>
#include <ameba_ota.h>
#endif

#define MATTER_OTA_SECTOR_SIZE 4096

#if defined(CONFIG_PLATFORM_8710C)
#define MATTER_OTA_HEADER_SIZE 32
#define MATTER_OTA_FIRMWARE_LENGTH   0x1AC000
static flash_t matter_ota_flash;
#elif defined(CONFIG_PLATFORM_8721D)
#define MATTER_OTA_SIGNATURE_SIZE 8
#define MATTER_OTA_HEADER_SIZE (32 + MATTER_OTA_SIGNATURE_SIZE)
update_ota_target_hdr targetHeader;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#define MATTER_OTA_HEADER_SIZE 32
#define MATTER_OTA_FIRMWARE_LENGTH   0x1EC000
flash_t matter_ota_flash;
ota_context matterCtx = {0};
update_ota_ctrl_info matterOtaCtrl = {0};
update_ota_target_hdr matterOtaTargetHdr = {0};
#endif

bool matter_ota_first_sector_written = false;
uint32_t matter_ota_flash_sector_base;
uint32_t matter_ota_new_firmware_addr;

uint8_t matter_ota_header[MATTER_OTA_HEADER_SIZE];
uint8_t matter_ota_header_size = 0; // variable to track size of ota header
uint8_t matter_ota_buffer[MATTER_OTA_SECTOR_SIZE]; // 4KB buffer to be written to one sector
uint16_t matter_ota_buffer_size = 0; // variable to track size of buffer

uint8_t matter_ota_get_total_header_size(void)
{
    return MATTER_OTA_HEADER_SIZE;
}

uint8_t matter_ota_get_current_header_size(void)
{
    return matter_ota_header_size;
}

void matter_ota_prepare_partition(void)
{
    matter_ota_first_sector_written = false;
    memset(matter_ota_buffer, 0, sizeof(matter_ota_buffer));
    memset(matter_ota_header, 0, sizeof(matter_ota_header));
    matter_ota_header_size = 0;
    matter_ota_buffer_size = 0;

#if defined(CONFIG_PLATFORM_8710C)
    matter_ota_new_firmware_addr = sys_update_ota_prepare_addr();
#elif defined(CONFIG_PLATFORM_8721D)
    memset(&targetHeader, 0, sizeof(targetHeader));
    if (ota_get_cur_index() == OTA_INDEX_1) {
        matter_ota_new_firmware_addr = LS_IMG2_OTA2_ADDR - SPI_FLASH_BASE;
    } else if (ota_get_cur_index() == OTA_INDEX_2) {
        matter_ota_new_firmware_addr = LS_IMG2_OTA1_ADDR - SPI_FLASH_BASE;
    }
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    memset(&matterCtx, 0, sizeof(matterCtx));
    memset(&matterOtaCtrl, 0, sizeof(matterOtaCtrl));
    memset(&matterOtaTargetHdr, 0, sizeof(matterOtaTargetHdr));

    matterCtx.otactrl = &matterOtaCtrl;
    matterCtx.otaTargetHdr = &matterOtaTargetHdr;

    matterCtx.otactrl->ImgId = OTA_IMGID_APP;
    matterCtx.otactrl->ImageLen = MATTER_OTA_FIRMWARE_LENGTH;

    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_1].ImgID = OTA_IMGID_APP;
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_2].ImgID = OTA_IMGID_APP;
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_1].ImgLen = MATTER_OTA_FIRMWARE_LENGTH;
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_2].ImgLen = MATTER_OTA_FIRMWARE_LENGTH;
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_1].Offset = 0x1000; //Offset of Manifest in firmware
    matterCtx.otaTargetHdr->FileImgHdr[OTA_INDEX_2].Offset = 0x1000; //Offset of Manifest in firmware

    if (ota_get_cur_index(matterCtx.otactrl->ImgId) == OTA_INDEX_1) {
        matterCtx.otactrl->index = OTA_INDEX_1;
        matterCtx.otactrl->targetIdx = OTA_INDEX_2;
        flash_get_layout_info(IMG_APP_OTA2, &matter_ota_new_firmware_addr, NULL);
    } else {
        matterCtx.otactrl->index = OTA_INDEX_2;
        matterCtx.otactrl->targetIdx = OTA_INDEX_1;
        flash_get_layout_info(IMG_APP_OTA1, &matter_ota_new_firmware_addr, NULL);
    }
    matter_ota_new_firmware_addr = matter_ota_new_firmware_addr - SPI_FLASH_BASE;
    matterCtx.otactrl->FlashAddr = matter_ota_new_firmware_addr;
#endif
    matter_ota_flash_sector_base = matter_ota_new_firmware_addr; // Note that the new fw address must be multiples of 4KB
}

int8_t matter_ota_store_header(uint8_t *data, uint32_t size)
{
    if (size + matter_ota_header_size > MATTER_OTA_HEADER_SIZE) {
        return OTA_ERROR;
    }

    memcpy(&(matter_ota_header[matter_ota_header_size]), data, size);
    matter_ota_header_size += size;

    return OTA_SUCCESS;
}

int8_t matter_ota_flash_burst_write(uint8_t *data, uint32_t size)
{
    if (size == 0) {
        return OTA_SUCCESS;
    }

    bool overflow = false;
    uint32_t sectorBase = matter_ota_flash_sector_base;
    uint32_t writeLength = MATTER_OTA_SECTOR_SIZE;
    int16_t bufferRemainSize = (int16_t)(MATTER_OTA_SECTOR_SIZE - matter_ota_buffer_size);

    if (!matter_ota_first_sector_written) {
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
        sectorBase += matter_ota_header_size; // leave first 32-bytes for header
        writeLength -= matter_ota_header_size;
        bufferRemainSize -= matter_ota_header_size;
#elif defined(CONFIG_PLATFORM_8721D)
        sectorBase += MATTER_OTA_SIGNATURE_SIZE; // leave first 8-bytes for signature
        writeLength -= MATTER_OTA_SIGNATURE_SIZE;
        bufferRemainSize -= MATTER_OTA_SIGNATURE_SIZE;
#endif
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
        // buffer is full, time to erase sector and write buffer data to flash
#if defined(CONFIG_PLATFORM_8710C)
        device_mutex_lock(RT_DEV_LOCK_FLASH);
        flash_erase_sector(&matter_ota_flash, matter_ota_flash_sector_base);
        flash_burst_write(&matter_ota_flash, sectorBase, writeLength, matter_ota_buffer);
        device_mutex_unlock(RT_DEV_LOCK_FLASH);
#elif defined(CONFIG_PLATFORM_8721D)
        erase_ota_target_flash(matter_ota_flash_sector_base + SPI_FLASH_BASE, MATTER_OTA_SECTOR_SIZE);
        ota_writestream_user(sectorBase, writeLength, matter_ota_buffer);
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
        flash_erase_sector(&matter_ota_flash, matter_ota_flash_sector_base);
        flash_burst_write(&matter_ota_flash, sectorBase, writeLength, matter_ota_buffer);
#endif
        if (!matter_ota_first_sector_written) {
            matter_ota_first_sector_written = true;
        }

        matter_ota_flash_sector_base += MATTER_OTA_SECTOR_SIZE; // point to next sector
        memset(matter_ota_buffer, 0, sizeof(matter_ota_buffer)); // clear buffer after writing
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
#if defined(CONFIG_PLATFORM_8710C)
    if (matter_ota_buffer_size > 0) {
        device_mutex_lock(RT_DEV_LOCK_FLASH);
        flash_erase_sector(&matter_ota_flash, matter_ota_flash_sector_base);
        flash_burst_write(&matter_ota_flash, matter_ota_flash_sector_base, matter_ota_buffer_size, matter_ota_buffer);
        device_mutex_unlock(RT_DEV_LOCK_FLASH);

        matter_ota_flash_sector_base += MATTER_OTA_SECTOR_SIZE; // point to next sector
        memset(matter_ota_buffer, 0, sizeof(matter_ota_buffer)); // clear buffer after writing
        matter_ota_buffer_size = 0;
    }
#elif defined(CONFIG_PLATFORM_8721D)
    // Populate the targetHeader which will be used during verify_ota_checksum()
    memcpy(&(targetHeader.FileHdr), matter_ota_header, 8);  // FwVer, HdrNum
    memcpy(&(targetHeader.FileImgHdr->ImgId), matter_ota_header + 8, 4);    // ImgId
    memcpy(&(targetHeader.FileImgHdr->ImgHdrLen), matter_ota_header + 12, 16);  // ImgHdrLen, Checksum, ImgLen, Offset
    targetHeader.FileImgHdr->FlashAddr = matter_ota_new_firmware_addr + SPI_FLASH_BASE;
    targetHeader.ValidImgCnt = 1;   // 1 image only in ota file
    memcpy(&(targetHeader.Sign), matter_ota_header + (MATTER_OTA_HEADER_SIZE - MATTER_OTA_SIGNATURE_SIZE), MATTER_OTA_SIGNATURE_SIZE);   // Signature

    if (matter_ota_buffer_size > 0) {
        erase_ota_target_flash(matter_ota_flash_sector_base + SPI_FLASH_BASE, MATTER_OTA_SECTOR_SIZE);
        ota_writestream_user(matter_ota_flash_sector_base, matter_ota_buffer_size, matter_ota_buffer);

        matter_ota_flash_sector_base += MATTER_OTA_SECTOR_SIZE; // point to next sector
        memset(matter_ota_buffer, 0, sizeof(matter_ota_buffer)); // clear buffer after writing
        matter_ota_buffer_size = 0;
    }

    if (strncmp("OTA", (const char *) & (targetHeader.FileImgHdr->ImgId), 3) != 0) {
        return OTA_ERROR;
    }

    if (verify_ota_checksum(&targetHeader) != 1) {
        return OTA_ERROR;
    }

#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    if (matter_ota_buffer_size > 0) {
        flash_erase_sector(&matter_ota_flash, matter_ota_flash_sector_base);
        flash_burst_write(&matter_ota_flash, matter_ota_flash_sector_base, matter_ota_buffer_size, matter_ota_buffer);

        matter_ota_flash_sector_base += MATTER_OTA_SECTOR_SIZE; // point to next sector
        memset(matter_ota_buffer, 0, sizeof(matter_ota_buffer)); // clear buffer after writing
        matter_ota_buffer_size = 0;
    }
#endif

    return OTA_SUCCESS;
}

int8_t matter_ota_update_signature(void)
{
#if defined(CONFIG_PLATFORM_8710C)
    return (update_ota_signature(matter_ota_header, matter_ota_new_firmware_addr) == 0);
#elif defined(CONFIG_PLATFORM_8721D)
    uint32_t targetIndex;
    if (matter_ota_new_firmware_addr == LS_IMG2_OTA2_ADDR - SPI_FLASH_BASE) {
        targetIndex = OTA_INDEX_2;
    } else {
        targetIndex = OTA_INDEX_1;
    }

    if (change_ota_signature(&targetHeader, targetIndex) != 1) {
        return OTA_ERROR;
    }

    return OTA_SUCCESS;
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    memcpy(&(matterCtx.otaTargetHdr->Manifest[matterCtx.otactrl->index]), matter_ota_header, sizeof(Manifest_TypeDef));
    if (!ota_update_manifest(matterCtx.otaTargetHdr, matterCtx.otactrl->targetIdx, matterCtx.otactrl->index)) {
        return OTA_ERROR;
    }
    return OTA_SUCCESS;
#endif
}

void matter_ota_platform_reset(void)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    ota_platform_reset();
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    rtos_time_delay_ms(100);
    sys_reset();
#endif
}

static void matter_ota_abort_task(void *pvParameters)
{
#if defined(CONFIG_PLATFORM_8710C)
    uint32_t newFWBlkSize = (MATTER_OTA_FIRMWARE_LENGTH - 1) / MATTER_OTA_SECTOR_SIZE + 1;
    printf("Cleaning up aborted OTA\n");
    printf("Erasing %d sectors\n", newFWBlkSize);

    if (matter_ota_new_firmware_addr != 0) {
        device_mutex_lock(RT_DEV_LOCK_FLASH);
        for (size_t i = 0; i < newFWBlkSize; i++) {
            flash_erase_sector(&matter_ota_flash, matter_ota_new_firmware_addr + (i * MATTER_OTA_SECTOR_SIZE));
        }
        device_mutex_unlock(RT_DEV_LOCK_FLASH);
    }
#elif defined(CONFIG_PLATFORM_8721D)
    uint32_t newFWBlkSize = (MATTER_OTA_HEADER_SIZE + targetHeader.FileImgHdr->ImgLen - 1) / MATTER_OTA_SECTOR_SIZE + 1;
    printf("Cleaning up aborted OTA\n");
    printf("Erasing %d sectors\n", newFWBlkSize);

    if (matter_ota_new_firmware_addr != 0) {
        for (size_t i = 0; i < newFWBlkSize; i++) {
            erase_ota_target_flash(matter_ota_new_firmware_addr + SPI_FLASH_BASE + (i * MATTER_OTA_SECTOR_SIZE), MATTER_OTA_SECTOR_SIZE);
        }
    }
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    uint32_t newFWBlkSize = (MATTER_OTA_FIRMWARE_LENGTH - 1) / MATTER_OTA_SECTOR_SIZE + 1;
    DiagPrintf("Cleaning up aborted OTA\r\n");
    DiagPrintf("Erasing %d sectors\r\n", newFWBlkSize);

    if (matter_ota_new_firmware_addr != 0) {
        for (size_t i = 0; i < newFWBlkSize; i++) {
            rtos_time_delay_ms(2); // to avoid undefined behaviour when it suddenly resets the ameba during flash erase
            flash_erase_sector(&matter_ota_flash, matter_ota_new_firmware_addr + (i * MATTER_OTA_SECTOR_SIZE));
        }
    }
#endif
    matter_ota_first_sector_written = false;
    vTaskDelete(NULL);
}

void matter_ota_create_abort_task(void)
{
    if (xTaskCreate(matter_ota_abort_task, "matter_ota_abort", 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        printf("[%s] Failed to create matter_ota_abort_task\n", __FUNCTION__);
    }
}

#ifdef __cplusplus
}
#endif
