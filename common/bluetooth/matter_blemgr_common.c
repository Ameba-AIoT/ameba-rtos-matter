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
#include <platform_stdlib.h>
#include <platform_autoconf.h>

#if defined(CONFIG_BLE_MATTER_ADAPTER) && CONFIG_BLE_MATTER_ADAPTER
#include <string.h>
#include <matter_blemgr_common.h>
#include <ble_matter_adapter_peripheral_main.h>
#include <chip_porting.h>

/*============================================================================*
 *                              Constants
 *============================================================================*/
#define MAX_ADV_NUMBER 2
#define BLE_MATTER_DEVICE_NAME_LEN	39

uint16_t matter_adv_interval = 0;
uint16_t matter_adv_int_min = 0x20;
uint16_t matter_adv_int_max = 0x20;
uint8_t matter_adv_data_length = 0;
uint8_t matter_adv_data[31] = {0};
matter_blemgr_callback matter_blemgr_callback_func = NULL;
void *matter_blemgr_callback_data = NULL;

extern int ble_matter_adapter_peripheral_main(uint8_t enable);
extern int ble_matter_adapter_start_adv(void);
extern int ble_matter_adapter_stop_adv(void);
extern int ble_matter_adapter_config_adv(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t *padv_data, uint8_t padv_data_length);
extern uint16_t ble_matter_adapter_get_mtu(uint16_t conn_handle);
extern int ble_matter_adapter_get_device_name(char *device_name);
extern int ble_matter_adapter_disconnect(uint16_t conn_handle);
extern int ble_matter_adapter_send_indication(uint16_t conn_handle, uint8_t *data, uint16_t data_length);

/*============================================================================*
 *                              Functions
 *============================================================================*/

int matter_blemgr_init(void)
{
    ble_matter_adapter_peripheral_main(1);
    return 0;
}

int matter_blemgr_deinit(void)
{
    ble_matter_adapter_peripheral_main(0);
    return 0;
}

void matter_blemgr_set_callback_func(matter_blemgr_callback p, void *data)
{
    matter_blemgr_callback_func = p;
    matter_blemgr_callback_data = data;
}

int matter_blemgr_start_adv(void)
{
    ble_matter_adapter_start_adv();

    return 0;
}

int matter_blemgr_stop_adv(void)
{
    ble_matter_adapter_stop_adv();

    return 0;
}

int matter_blemgr_config_adv(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t *adv_data, uint8_t adv_data_length)
{
    matter_adv_data_length = adv_data_length;
    memcpy(matter_adv_data, adv_data, adv_data_length);
    ble_matter_adapter_config_adv(adv_int_min, adv_int_max, matter_adv_data, matter_adv_data_length);

    return 0;
}

uint16_t matter_blemgr_get_mtu(uint16_t conn_handle)
{
    int ret;
    uint16_t mtu_size;

    mtu_size = ble_matter_adapter_get_mtu(conn_handle);
    if (mtu_size != 0xFFFF) {
        RTK_LOGI(NOTAG, "printing MTU size\n");
        return mtu_size;
    } else {
        return 0;
    }
}

int matter_blemgr_set_device_name(char *device_name, uint8_t device_name_length)
{
    if (device_name == NULL || device_name_length > BLE_MATTER_DEVICE_NAME_LEN) {
        RTK_LOGE(NOTAG, "[%s]:invalid name or len:name 0x%x,len %d\n", __func__, device_name, device_name_length);
        return 1;
    }

    return ble_matter_adapter_set_device_name(device_name);
}

int matter_blemgr_disconnect(uint16_t conn_handle)
{
    ble_matter_adapter_disconnect(conn_handle);

    return 0;
}

int matter_blemgr_send_indication(uint16_t conn_handle, uint8_t *data, uint16_t data_length)
{
    ble_matter_adapter_send_indication(conn_handle, data, data_length);

    return 0;
}
#endif /* CONFIG_BLE_MATTER_ADAPTER */
