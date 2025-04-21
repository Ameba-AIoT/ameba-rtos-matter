#include <platform_stdlib.h>
#include <platform_autoconf.h>

#if defined(CONFIG_BLE_MATTER_ADAPTER) && CONFIG_BLE_MATTER_ADAPTER
#include <string.h>
#include <matter_blemgr_common.h>

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
extern int ble_matter_adapter_config_adv(uint8_t *adv_data, uint8_t adv_data_length);
extern uint16_t ble_matter_adapter_get_mtu(uint8_t conn_id);
extern int ble_matter_adapter_disconnect(uint8_t connect_id);
extern int ble_matter_adapter_send_indication(uint8_t connect_id, uint8_t *data, uint16_t data_length);

#if CONFIG_BLE_MATTER_MULTI_ADV_ON
extern uint8_t customer_adv_id;
#endif

/*============================================================================*
 *                              Functions
 *============================================================================*/

int matter_blemgr_init(void)
{
    ble_matter_adapter_peripheral_main(1);

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
    ble_matter_adapter_config_adv(matter_adv_data, matter_adv_data_length);

    return 0;
}

uint16_t matter_blemgr_get_mtu(uint8_t connect_id)
{
    int ret;
    uint16_t mtu_size;

    mtu_size = ble_matter_adapter_get_mtu(connect_id);
    if (mtu_size != 0xFFFF)
    {
        printf("printing MTU size\n");
        return mtu_size;
    } else {
        return 0;
    }
}

int matter_blemgr_set_device_name(char *device_name, uint8_t device_name_length)
{
    if (device_name == NULL || device_name_length > BLE_MATTER_DEVICE_NAME_LEN)
    {
        printf("[%s]:invalid name or len:name 0x%x,len %d\n", __func__, device_name, device_name_length);
        return 1;
    }

    int ret = rtk_bt_le_gap_set_device_name((const uint8_t *)device_name);
    if (ret) {
        printf("[%s]:set device name failed! err: 0x%x", __func__, ret);
        return -1;
    }

    printf("[%s] set device name success", __func__);

    return 0;
}

int matter_blemgr_disconnect(uint8_t connect_id)
{
    ble_matter_adapter_disconnect(connect_id);

    return 0;
}

int matter_blemgr_send_indication(uint8_t connect_id, uint8_t *data, uint16_t data_length)
{
    ble_matter_adapter_send_indication(connect_id, data, data_length);

    return 0;
}
#endif /* CONFIG_BLE_MATTER_ADAPTER */
