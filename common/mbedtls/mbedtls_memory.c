#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void* app_mbedtls_calloc_func(size_t nelements, size_t elementSize)
{
    size_t size;
    void *ptr = NULL;

    size = nelements * elementSize;
    ptr = pvPortMalloc(size);

    if(ptr) {
        memset(ptr, 0, size);
    }

    return ptr;
}

#ifdef __cplusplus
}
#endif
