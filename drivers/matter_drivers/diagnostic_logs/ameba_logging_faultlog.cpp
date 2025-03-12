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

#ifdef __cplusplus
extern "C" {
#endif

char bt_message[1024];
char fault_message[6 * 1024];

#define MIN_BDX_TRANSFER_LENGTH 1024
int requires_bdx = 0;
int restricted_len = 0;

void matter_fault_log(char *msg, int len)
{
    flash_t fault_flash;

    if (requires_bdx)
    {
        if (len > MIN_BDX_TRANSFER_LENGTH)
        {
            restricted_len = MIN_BDX_TRANSFER_LENGTH;
        }
    }
    else
    {
        restricted_len = len;
    }

    matter_insert_crash_log((uint8_t *)msg, (uint32_t)restricted_len);
}

void matter_bt_log(char *msg, int len)
{
    flash_t fault_flash;

    if (requires_bdx)
    {
        if (restricted_len + len > MIN_BDX_TRANSFER_LENGTH)
        {
            restricted_len = 0;
        }
        else
        {
            restricted_len = MIN_BDX_TRANSFER_LENGTH - restricted_len;
        }
    }
    else
    {
        restricted_len = len;
    }

    matter_insert_crash_log((uint8_t *)msg, (uint32_t)restricted_len);
}

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_ENABLE_AMEBA_DLOG */
