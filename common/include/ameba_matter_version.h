/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/
#ifndef AMEBA_MATTER_VERSION_H
#define AMEBA_MATTER_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#define MATTER_VERSION_MAJOR 1
#define MATTER_VERSION_MINOR 6
#define MATTER_VERSION_PATCH 0

/* Helper macros */
#define MATTER_VERSION_STR_HELPER(x) #x
#define MATTER_VERSION_STR(x) MATTER_VERSION_STR_HELPER(x)

#define MATTER_VERSION_STRING \
    MATTER_VERSION_STR(MATTER_VERSION_MAJOR) "." \
    MATTER_VERSION_STR(MATTER_VERSION_MINOR) "." \
    MATTER_VERSION_STR(MATTER_VERSION_PATCH)

/* Packed 32-bit form, useful for Matter's Basic Information cluster
 * SoftwareVersion attribute (uint32) */
#define MATTER_VERSION_U32 \
    ((MATTER_VERSION_MAJOR << 16) | (MATTER_VERSION_MINOR << 8) | MATTER_VERSION_PATCH)

void app_print_matter_version(void)
{
    printf("\r\nMatter v%s\r\n", MATTER_VERSION_STRING);
}

#ifdef __cplusplus
}
#endif

#endif /* AMEBA_MATTER_VERSION_H */