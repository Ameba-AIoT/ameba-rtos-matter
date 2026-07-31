/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
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
#include <assert.h>
#include <stddef.h>

/* mbed TLS headers */
#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#else
#include <mbedtls/config.h>
#include <drivers/auth/mbedtls/mbedtls_common.h>
#include <drivers/auth/mbedtls/mbedtls_config.h>
#endif
#include <mbedtls/memory_buffer_alloc.h>
#ifdef MBEDTLS_PLATFORM_SNPRINTF_ALT
#include <mbedtls/platform.h>
#endif

#include <common/debug.h>
#include <plat/common/platform.h>

uint32_t RandSeedTZ = 0x12345;

static void cleanup(void)
{
    ERROR("EXIT from BL2\n");
    panic();
}

/*
 * mbed TLS initialization function
 */
void matter_secure_mbedtls_init(uint32_t random_seed)
{
    static int ready = 0;
    void *heap_addr;
    size_t heap_size = 0;
    int err;

    if (!ready) {
        if (atexit(cleanup)) {
            panic();
        }

        err = get_mbedtls_heap_helper(&heap_addr, &heap_size);

        /* Ensure heap setup is proper */
        if (err < 0) {
            ERROR("Mbed TLS failed to get a heap\n");
            panic();
        }
        assert(heap_size >= MATTER_MBEDTLS_SECURE_HEAP_SIZE);

        /* Initialize the mbed TLS heap */
        mbedtls_memory_buffer_alloc_init(heap_addr, heap_size);

#ifdef MBEDTLS_PLATFORM_SNPRINTF_ALT
        mbedtls_platform_set_snprintf(snprintf);
#endif
        RandSeedTZ = random_seed;
        ready = 1;
    }
}

/*
 * The following helper function simply returns the default allocated heap.
 * It can be used by platforms for their plat_get_mbedtls_heap() implementation.
 */
int get_mbedtls_heap_helper(void **heap_addr, size_t *heap_size)
{
    static unsigned char heap[MATTER_MBEDTLS_SECURE_HEAP_SIZE];

    assert(heap_addr != NULL);
    assert(heap_size != NULL);

    *heap_addr = heap;
    *heap_size = sizeof(heap);
    return 0;
}

void matter_secure_mbedtls_heap_status(void)
{
#if defined(MBEDTLS_MEMORY_DEBUG)
    INFO("MATTER_MBEDTLS_SECURE_HEAP_SIZE = %d\n", MATTER_MBEDTLS_SECURE_HEAP_SIZE);
    mbedtls_memory_buffer_alloc_status();
#else
    INFO("Enable MBEDTLS_MEMORY_DEBUG to check the secure mbedtls heap status!\n");
#endif
}
