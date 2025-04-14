/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#include "kv.h"
#include "vfs.h"
#include "ff.h"
#include "littlefs_adapter.h"
#include <ameba_flashcfg.h>
#include <flash_api.h>

extern FILE *fopen(const char *filename, const char *mode);
extern int fclose(FILE *stream);
extern size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
extern size_t fread(void *ptr, size_t size, size_t count, FILE *stream);


static char *kv_matter_prefix;

int rt_kv_deinit(void)
{
    int res = -1;
    char *path = NULL;
    DIR *directory;
    struct dirent *dir;

    if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 1)) == NULL) {
        VFS_DBG(VFS_ERROR, "KV deinit fail");
        goto exit;
    }

    if (lfs_mount_fail) {
        VFS_DBG(VFS_ERROR, "KV deinit fail");
        goto exit;
    }

    kv_matter_prefix = find_vfs_tag(VFS_REGION_1);
    if (kv_matter_prefix == NULL) {
        goto exit;
    }

    DiagSnPrintf(path, MAX_KEY_LENGTH + 1, "%s:KV", kv_matter_prefix);

    directory = opendir(path);
    if (directory) {
        while ((dir = readdir(directory)) != NULL) {
            if (dir->d_type == DT_REG) { // If the entry is a regular file then delete it
                DiagSnPrintf(path, MAX_KEY_LENGTH + 1, "%s:KV/%s", kv_matter_prefix, dir->d_name);
                vTaskDelay(10);
                if (remove(path) == 0) {
                    DiagPrintf("rt_kv_deinit: succesfully deleted %s\n", path);
                }
            }
            rtos_mem_free(dir);
        }
        res = closedir(directory);
    } else {
        goto exit;
    }
    if (res < 0) {
        VFS_DBG(VFS_ERROR, "closedir failed,err is %d!!!", res);
    }

exit:
    if (path) {
        rtos_mem_free(path);
    }
    return res;
}

int32_t rt_kv_get_length(const char *key)
{
    vfs_file *finfo;
    int res = -1;
    char *path = NULL;

    if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
        VFS_DBG(VFS_ERROR, "KV init fail");
        goto exit;
    }

    if (lfs_mount_fail) {
        VFS_DBG(VFS_ERROR, "KV init fail");
        goto exit;
    }

    kv_matter_prefix = find_vfs_tag(VFS_REGION_1);
    if (kv_matter_prefix == NULL) {
        goto exit;
    }

    if (strlen(key) > MAX_KEY_LENGTH - 3) {
        VFS_DBG(VFS_ERROR, "key len limite exceed, max len is %d", MAX_KEY_LENGTH - 3);
        goto exit;
    }

    DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:KV/%s", kv_matter_prefix, key);
    finfo = (vfs_file *)fopen(path, "r");
    if (finfo == NULL) {
        VFS_DBG(VFS_ERROR, "fopen failed, res = %d", res);
        goto exit;
    }

    fseek((FILE *)finfo, 0, SEEK_END);
    res = ftell((FILE *)finfo);
    if (res <= 0) {
        VFS_DBG(VFS_ERROR, "ftell failed,err is %d!!!", res);
        fclose((FILE *)finfo);
        goto exit;
    }
    fclose((FILE *)finfo);

exit:
    if (path) {
        rtos_mem_free(path);
    }
    return res;
}
