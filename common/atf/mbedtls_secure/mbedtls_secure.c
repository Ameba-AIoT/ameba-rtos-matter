/*
 * Copyright (c) 2015-2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
