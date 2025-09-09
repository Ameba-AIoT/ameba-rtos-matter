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

exit:
    if (path) {
        rtos_mem_free(path);
    }
    return res;
}
