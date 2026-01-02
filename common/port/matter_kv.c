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

#include "kv.h"
#include "vfs.h"
#ifdef CONFIG_VFS_FATFS_INCLUDED
#include "ff.h"
#endif
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
#if defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)
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
#elif defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1)
    dirent *info;
    DIR *dir;
    char *path = NULL;

    kv_matter_prefix = find_vfs_tag(VFS_REGION_1);
    if (kv_matter_prefix == NULL) {
        goto exit;
    }

    if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
        VFS_DBG(VFS_ERROR, "KV malloc fail");
        goto exit;
    }

    DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:KV", kv_matter_prefix);

    dir = (DIR *)opendir(path);
    if (dir == NULL) {
        VFS_DBG(VFS_ERROR, "opendir failed");
        goto exit;
    }

    while (1) {
        info = readdir((void **)dir);
        if (info == NULL) {
            break;
        } else if (strcmp(info->d_name, ".") != 0 && strcmp(info->d_name, "..") != 0) {
            res = rt_kv_delete(info->d_name);
            if (res < 0)
            {
                DiagPrintf("rt_kv_deinit: failed to delete %s\n", info->d_name);
                goto exit;
            }
            else
            {
                DiagPrintf("rt_kv_deinit: succesfully deleted %s\n", info->d_name);
            }
        }
    }

    res = closedir((void **)dir);
#endif // CONFIG_AMEBARTOS_XXX

exit:
    if (path) {
        rtos_mem_free(path);
    }
    return res;
}

#if defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)
int32_t rt_kv_size(const char *key)
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
#endif // CONFIG_AMEBARTOS_XXX
