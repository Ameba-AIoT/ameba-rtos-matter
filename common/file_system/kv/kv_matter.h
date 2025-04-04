/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef _KV_MATTER_H_
#define _KV_MATTER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
  *                                Functions
  *============================================================================*/
int rt_kv_deinit(void);
int32_t rt_kv_get_length(const char *key);

#ifdef __cplusplus
}
#endif

#endif // _KV_MATTER_H_
