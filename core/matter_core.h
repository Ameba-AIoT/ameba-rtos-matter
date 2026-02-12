/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

#include <platform/CHIPDeviceLayer.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Matter device event callback prototype
 *
 * @param event       Event ID
 * @param context     User registered context pointer
 */
typedef void (*matter_app_device_callback_t)(uint16_t event, void * context);

/**
 * @brief Register Matter device callback
 *        This API allows the application to register a callback function to receive
 *        Matter device events.
 * @param callback    User callback function
 * @param context     User context pointer, returned when callback is invoked
 *
 * @reference for "event":
 *      Please check the enumerate in
 *          - connectedhomeip/src/include/platform/CHIPDeviceEvent.h
 *          - sdk/component/common/application/matter/core/matter_events.h
 * @code
 * static void example_matter_app_device_callback(uint16_t event, void * context)
 * {
 *     printf("Device Event = %d\n", event);
 * }
 *
 * // Register callback
 * matter_reg_app_device_callback(example_matter_app_device_callback, NULL);
 * @endcode
 */
void matter_reg_app_device_callback(matter_app_device_callback_t callback, void * context);

/**
 * @brief  Start the Matter Core task in the porting layer implementation.
 *         This function initializes and starts the Matter Core task.
 * @return CHIP_NO_ERROR if start successfully.
 */
CHIP_ERROR matter_core_start(void);

#ifdef __cplusplus
}
#endif
