#include <FreeRTOS.h>
#include <platform_stdlib.h>
#include <platform_opts.h>
#include <device_lock.h>
#include <flash_api.h>
#include <string>

#if defined(CONFIG_ENABLE_AMEBA_DLOG) && (CONFIG_ENABLE_AMEBA_DLOG == 1)
#include <lfs.h>
#include <matter_fs.h>
#include <diagnostic_logs/ameba_diagnosticlogs_provider_delegate_impl.h>
#include <diagnostic_logs/ameba_logging_insert_logs.h>
extern bool ClearLogStrategy(void* fp);
#ifdef __cplusplus
extern "C" {
#endif

static int insert_log_data(char* filename, lfs_file_t* fp, uint8_t* data, uint32_t data_len)
{
    int res;
    uint unused_writecount = 0;
    bool fsReady = chip::app::Clusters::DiagnosticLogs::AmebaDiagnosticLogsProvider::IsFSReady();

    if (!fsReady) {
        res = matter_fs_fopen(filename, fp, LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC);
        if (res < 0)
        {
            ChipLogError(DeviceLayer, "Open res: %d", res);
            return res;
        }
    }

    ChipLogProgress(DeviceLayer, "Copy log.....");

    res = matter_fs_fwrite(fp, data, data_len, &unused_writecount);
    if (res < 0) {
        if (res == LFS_ERR_NOSPC) {
            ChipLogProgress(DeviceLayer, "Insufficient space, try clearing out old data");
            if (ClearLogStrategy((void *)fp) == false) {
                ChipLogError(DeviceLayer, "Failed to execute log clear strategy");
                return res;
            }
            else {
                res = matter_fs_fwrite(fp, data, data_len, &unused_writecount);
                if (res == LFS_ERR_NOSPC) {
                    ChipLogProgress(DeviceLayer, "Still insufficient space, ignore, dropping data...");
                    return LFS_ERR_OK;
                }
            }
        }
        else {
            ChipLogError(DeviceLayer, "Error writing log to file: %d", res);
            return res;
        }
    }
    else {
        ChipLogProgress(DeviceLayer, "fs wrote %d bytes", unused_writecount);
    }

    // Close file if not already ready
    if (!fsReady) {
        matter_fs_fclose(fp);
    }

    return LFS_ERR_OK;
}

_WEAK void matter_insert_user_log(uint8_t* data, uint32_t data_len)
{
    lfs_file_t* fp = chip::app::Clusters::DiagnosticLogs::AmebaDiagnosticLogsProvider::GetFpUserLog();
    int res = insert_log_data(USER_LOG_FILENAME, fp, data, data_len);
    if (res != LFS_ERR_OK) {
        ChipLogError(DeviceLayer, "Failed to insert user log: %d", res);
    }
}

// Network log function
_WEAK void matter_insert_network_log(uint8_t* data, uint32_t data_len)
{
    lfs_file_t* fp = chip::app::Clusters::DiagnosticLogs::AmebaDiagnosticLogsProvider::GetFpNetdiagLog();
    int res = insert_log_data(NET_LOG_FILENAME, fp, data, data_len);
    if (res != LFS_ERR_OK) {
        ChipLogError(DeviceLayer, "Failed to insert network log: %d", res);
    }
}

// Crash log function
_WEAK void matter_insert_crash_log(uint8_t* data, uint32_t data_len)
{
    lfs_file_t* fp = chip::app::Clusters::DiagnosticLogs::AmebaDiagnosticLogsProvider::GetFpCrashLog();
    int res = insert_log_data(CRASH_LOG_FILENAME, fp, data, data_len);
    if (res != LFS_ERR_OK) {
        ChipLogError(DeviceLayer, "Failed to insert crash log: %d", res);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_ENABLE_AMEBA_DLOG */
