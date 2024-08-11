/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

#include <matter_events.h>
#include <lwip/ip_addr.h>

#include <platform/CHIPDeviceLayer.h>

struct bridge_table {
    in_addr_t bridged_device_addr;
    int bridge_endpoint;
    int sock_conn;
};

/**
 * @brief  Initialize the bridge light driver.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise (if necessary).
 */
CHIP_ERROR matter_driver_bridge_light_init(void);

/**
 * @brief  Initialize the bridge light driver.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise (if necessary).
 */
void matter_driver_bridge_endpoint_assign(void);

/**
 * @brief  Initialize a TCP server for bridged device to connect to.
 *         This is ONLY an example to reach out to bridged device. 
 *         User should implement according to their application
 */
void matter_driver_bridge_setup_server(void);

/**
 * @brief  Initialize a bridge receiving thread to listen for incoming commands from server.
 */
void matter_driver_bridge_recv_server(void);

/**
 * @brief  Update uplink handler when receiving commands from Matter Controller.
 * @param[in]  event: Pointer to the AppEvent structure containing event details.
 */
void matter_driver_uplink_update_handler(AppEvent *aEvent);

/**
 * @brief  Update downlink handler when receiving commands from external (e.g., GPIO, PWM).
 * @param[in]  event: Pointer to the AppEvent structure containing event details.
 */
void matter_driver_downlink_update_handler(AppEvent *event);
