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
