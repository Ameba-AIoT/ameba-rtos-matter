#include <platform_opts.h>
#include <platform/platform_stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <chip_porting.h>
#include <lwip/dhcp.h>
#include <osdep_service.h>
#include <wifi_conf.h>

#define SCAN_RECORD_SECURITY_OFFSET 11
#define SCAN_RECORD_CHANNEL_OFFSET  13
#define SCAN_RECORD_SSID_OFFSET     14
#define MAX_SSID_LEN                32

u32 apNum = 0;
static rtw_scan_result_t matter_wifi_userdata[65] = {0};
static char *matter_ssid;

#if defined(CONFIG_AUTO_RECONNECT) && CONFIG_AUTO_RECONNECT
static void *matter_param_indicator;
struct task_struct matter_wifi_autoreconnect_task;
extern unsigned char *rltk_wlan_get_saved_bssid(void);
extern void (*p_wlan_autoreconnect_hdl)(rtw_security_t, char *, int, char *, int, int);
#endif /* CONFIG_AUTO_RECONNECT */

chip_connmgr_callback chip_connmgr_callback_func = NULL;
void *chip_connmgr_callback_data = NULL;
void chip_connmgr_set_callback_func(chip_connmgr_callback p, void *data)
{
    chip_connmgr_callback_func = p;
    chip_connmgr_callback_data = data;
}

void matter_wifi_reg_event_handler(matter_wifi_event event_cmds, rtw_event_handler_t handler_func, void *handler_user_data)
{
    wifi_reg_event_handler(event_cmds, handler_func, handler_user_data);
}

static const char *matter_wifi_security_to_str(rtw_security_t sec)
{
    switch (sec) {
    case RTW_SECURITY_OPEN:                        return "Open";
    case RTW_SECURITY_WEP_PSK:                     return "WEP";
    case RTW_SECURITY_WPA_TKIP_PSK:                return "WPA TKIP";
    case RTW_SECURITY_WPA_AES_PSK:                 return "WPA AES";
    case RTW_SECURITY_WPA_MIXED_PSK:               return "WPA Mixed";
    case RTW_SECURITY_WPA2_AES_PSK:                return "WPA2 AES";
    case RTW_SECURITY_WPA2_TKIP_PSK:               return "WPA2 TKIP";
    case RTW_SECURITY_WPA2_MIXED_PSK:              return "WPA2 Mixed";
    case RTW_SECURITY_WPA_WPA2_TKIP_PSK:           return "WPA/WPA2 TKIP";
    case RTW_SECURITY_WPA_WPA2_AES_PSK:            return "WPA/WPA2 AES";
    case RTW_SECURITY_WPA_WPA2_MIXED_PSK:          return "WPA/WPA2 Mixed";
    case RTW_SECURITY_WPA_TKIP_ENTERPRISE:         return "WPA TKIP Enterprise";
    case RTW_SECURITY_WPA_AES_ENTERPRISE:          return "WPA AES Enterprise";
    case RTW_SECURITY_WPA_MIXED_ENTERPRISE:        return "WPA Mixed Enterprise";
    case RTW_SECURITY_WPA2_TKIP_ENTERPRISE:        return "WPA2 TKIP Enterprise";
    case RTW_SECURITY_WPA2_AES_ENTERPRISE:         return "WPA2 AES Enterprise";
    case RTW_SECURITY_WPA2_MIXED_ENTERPRISE:       return "WPA2 Mixed Enterprise";
    case RTW_SECURITY_WPA_WPA2_TKIP_ENTERPRISE:    return "WPA/WPA2 TKIP Enterprise";
    case RTW_SECURITY_WPA_WPA2_AES_ENTERPRISE:     return "WPA/WPA2 AES Enterprise";
    case RTW_SECURITY_WPA_WPA2_MIXED_ENTERPRISE:   return "WPA/WPA2 Mixed Enterprise";
#ifdef CONFIG_SAE_SUPPORT
    case  RTW_SECURITY_WPA3_AES_PSK:               return "WPA3-SAE AES";
    case  RTW_SECURITY_WPA2_WPA3_MIXED:            return "WPA2/WPA3-SAE AES";
#endif
    default:                                       return "Unknown";
    }
}

static void print_scan_result(rtw_scan_result_t *record)
{
    RTW_API_INFO("%s\t ", (record->bss_type == RTW_BSS_TYPE_ADHOC) ? "Adhoc" : "Infra");
    RTW_API_INFO(MAC_FMT, MAC_ARG(record->BSSID.octet));
    RTW_API_INFO(" %d\t ", record->signal_strength);
    RTW_API_INFO(" %d\t ", record->channel);
    RTW_API_INFO(" %d\t ", record->wps_type);
    RTW_API_INFO(" %s\t\t ", matter_wifi_security_to_str(record->security));
    RTW_API_INFO(" %s\n", record->SSID.val);
}

static rtw_result_t scan_result_handler(rtw_scan_handler_result_t *malloced_scan_result)
{
    if (malloced_scan_result->scan_complete != RTW_TRUE) {
        if (malloced_scan_result->ap_details.SSID.len != 0) {
            rtw_scan_result_t *record = &malloced_scan_result->ap_details;
            record->SSID.val[record->SSID.len] = 0; // Ensure the SSID is null terminated

            RTW_API_INFO("%d\t ", ++apNum);
            print_scan_result(record);

            if (malloced_scan_result->user_data) {
                memcpy((void *)((char *)malloced_scan_result->user_data + (apNum - 1)*sizeof(rtw_scan_result_t)), (char *)record, sizeof(rtw_scan_result_t));
            }
        }
    } else {
        if (chip_connmgr_callback_func && chip_connmgr_callback_data) {
            chip_connmgr_callback_func(chip_connmgr_callback_data); // inform matter
        } else {
            RTW_API_INFO("ERROR: chip_connmgr_callback_func is NULL\n");
            apNum = 0;
            return RTW_ERROR;
        }
    }

    return RTW_SUCCESS;
}

static rtw_result_t matter_scan_with_ssid_result_handler(rtw_scan_handler_result_t *malloced_scan_result)
{
    if (malloced_scan_result->scan_complete != RTW_TRUE) {
        rtw_scan_result_t *record = &malloced_scan_result->ap_details;
        record->SSID.val[record->SSID.len] = 0; // Ensure the SSID is null terminated

        if ((malloced_scan_result->user_data) && (!strcmp(matter_ssid, (char *)record->SSID.val))) {
            RTW_API_INFO("%d\t ", ++apNum);
            memcpy((void *)((char *)malloced_scan_result->user_data + (apNum - 1)*sizeof(rtw_scan_result_t)), (char *)record, sizeof(rtw_scan_result_t));
            print_scan_result(record);
        }
    } else {
        if (chip_connmgr_callback_func && chip_connmgr_callback_data) {
            chip_connmgr_callback_func(chip_connmgr_callback_data); // inform matter
            vPortFree(matter_ssid);
        } else {
            RTW_API_INFO("ERROR: chip_connmgr_callback_func is NULL\n");
            apNum = 0;
            vPortFree(matter_ssid);
            return RTW_ERROR;
        }
    }

    return RTW_SUCCESS;
}

void matter_wifi_scan_networks(void)
{
    volatile int ret = RTW_SUCCESS;

    apNum = 0; // reset counter at the start of scan

    ret = wifi_scan_networks(scan_result_handler, matter_wifi_userdata);

    if (ret != RTW_SUCCESS) {
        RTW_API_INFO("ERROR: wifi scan failed\n");
    }
}

void matter_wifi_scan_networks_with_ssid(const unsigned char *ssid, size_t length)
{
    volatile int ret = RTW_SUCCESS;

    apNum = 0; // reset counter at the start of scan

    matter_ssid = (char *) pvPortMalloc(length + 1);

    memset(matter_ssid, 0, length + 1);
    memcpy(matter_ssid, ssid, length);
    matter_ssid[length] = '\0';

    ret = wifi_scan_networks(matter_scan_with_ssid_result_handler, matter_wifi_userdata);

    if (ret != RTW_SUCCESS) {
        RTW_API_INFO("ERROR: wifi scan failed\n");
    }
}

rtw_scan_result_t *matter_get_scan_results(void)
{
    return matter_wifi_userdata;
}

#if defined(CONFIG_AUTO_RECONNECT) && CONFIG_AUTO_RECONNECT
static void matter_wifi_autoreconnect_thread(void *param)
{
    struct matter_wifi_autoreconnect_param *reconnect_param = (struct matter_wifi_autoreconnect_param *) param;

    int ret = RTW_ERROR;
    char empty_bssid[ETH_ALEN] = {0};
    bool assoc_by_bssid = false;

    unsigned char *saved_bssid = rltk_wlan_get_saved_bssid();

    RTW_API_INFO("Matter auto reconnect...\n");

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
    rtw_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif

    if (memcmp(saved_bssid, empty_bssid, ETH_ALEN) != 0) {
        assoc_by_bssid = true;
    }

    if (assoc_by_bssid) {
        ret = wifi_connect_bssid(saved_bssid, reconnect_param->ssid, reconnect_param->security_type,
                                 reconnect_param->password, ETH_ALEN, reconnect_param->ssid_len,
                                 reconnect_param->password_len, reconnect_param->key_id, NULL);
    } else {
        ret = wifi_connect(reconnect_param->ssid, reconnect_param->security_type, reconnect_param->password,
                           reconnect_param->ssid_len, reconnect_param->password_len, reconnect_param->key_id, NULL);
    }

    if (ret == RTW_SUCCESS) {
        RTW_API_INFO("Wi-Fi connected successfully\n");
    } else {
        RTW_API_INFO("ERROR: Wi-Fi connection failed (code %d)\n", ret);
    }

    matter_param_indicator = NULL;
    rtw_delete_task(&matter_wifi_autoreconnect_task);
}

void matter_wifi_autoreconnect_hdl(rtw_security_t security_type, char *ssid, int ssid_len,
                                   char *password, int password_len, int key_id)
{
    static struct matter_wifi_autoreconnect_param param;

    matter_param_indicator = &param;
    param.security_type    = security_type;
    param.ssid             = ssid;
    param.ssid_len         = ssid_len;
    param.password         = password;
    param.password_len     = password_len;
    param.key_id           = key_id;

    if (matter_wifi_autoreconnect_task.task != NULL) {
        dhcp_stop(&xnetif[0]);

        u32 start_tick = rtw_get_current_time();
        while (matter_wifi_autoreconnect_task.task != NULL) {
            rtw_msleep_os(2);

            u32 passing_tick = rtw_get_current_time() - start_tick;
            if (rtw_systime_to_sec(passing_tick) >= 2) {
                RTW_API_INFO("Timeout waiting for matter_wifi_autoreconnect_task\n");
                return;
            }
        }
    }

    rtw_create_task(&matter_wifi_autoreconnect_task, "matter_wifi_autoreconnect", 512, tskIDLE_PRIORITY + 1, matter_wifi_autoreconnect_thread, &param);
}

#endif /* CONFIG_AUTO_RECONNECT */

void matter_wifi_set_autoreconnect(uint8_t mode)
{
#if defined(CONFIG_AUTO_RECONNECT) && CONFIG_AUTO_RECONNECT
    size_t ssidLen = 0;
    unsigned char buf[32];
    const char kWiFiSSIDKeyName[] = "wifi-ssid";

    memset(buf, 0, sizeof(buf));

    //if wifi-ssid exist in NVS, it has been commissioned before, CHIP will do autoreconnection
    s32 ret = getPref_bin_new(kWiFiSSIDKeyName, kWiFiSSIDKeyName, buf, sizeof(buf), &ssidLen);
    if (ret != DCT_SUCCESS) {
        if (mode == RTW_AUTORECONNECT_DISABLE) {
            p_wlan_autoreconnect_hdl = NULL;
        } else {
            p_wlan_autoreconnect_hdl = matter_wifi_autoreconnect_hdl;
        }

#if defined(CONFIG_PLATFORM_8710C)
        rltk_wlan_set_autoreconnect(WLAN0_NAME, mode, AUTO_RECONNECT_COUNT, AUTO_RECONNECT_INTERVAL);
#elif defined(CONFIG_PLATFORM_8721D)
        wext_set_autoreconnect(WLAN0_NAME, mode, AUTO_RECONNECT_COUNT, AUTO_RECONNECT_INTERVAL);
#endif
    }
#endif /* CONFIG_AUTO_RECONNECT */
    return;
}

static int matter_find_ap_from_scan_buf(char *buf, int buflen, char *target_ssid, void *user_data)
{
    rtw_wifi_setting_t *pwifi = (rtw_wifi_setting_t *) user_data;
    int plen = 0;

    while (plen < buflen)
    {
        u8 len = (u8) *(buf + plen);
        if (len == 0) {
            break; 
        }

        u8 ssid_len = len - SCAN_RECORD_SSID_OFFSET;
        char *ssid  = buf + plen + SCAN_RECORD_SSID_OFFSET;

        if (ssid_len == strlen(target_ssid) && memcmp(ssid, target_ssid, ssid_len) == 0) {
            strncpy((char *) pwifi->ssid, target_ssid, MAX_SSID_LEN + 1);
            pwifi->channel = *(buf + plen + SCAN_RECORD_CHANNEL_OFFSET);

            u8 security_mode = (u8) *(buf + plen + SCAN_RECORD_SECURITY_OFFSET);
            if (security_mode == IW_ENCODE_ALG_NONE) {
                pwifi->security_type = RTW_SECURITY_OPEN;
            } else if (security_mode == IW_ENCODE_ALG_WEP) {
                pwifi->security_type = RTW_SECURITY_WEP_PSK;
            } else if (security_mode == IW_ENCODE_ALG_CCMP) {
                pwifi->security_type = RTW_SECURITY_WPA2_AES_PSK;
            }
            break;
        }
        plen += len;
    }

    return 0;
}

static int matter_get_ap_security_mode(IN char *ssid, OUT rtw_security_t *security_mode, OUT u8 *channel)
{
    rtw_wifi_setting_t wifi;
    u32 scan_buflen = 1000;

    memset(&wifi, 0, sizeof(wifi));

    if (wifi_scan_networks_with_ssid(matter_find_ap_from_scan_buf, (void *)&wifi, scan_buflen, ssid, strlen(ssid)) != RTW_SUCCESS) {
        RTW_API_INFO("ERROR: wifi scan failed!\n");
        return 0;
    }

    if (strcmp((char *)wifi.ssid, ssid) == 0) {
        *security_mode = wifi.security_type;
        *channel = wifi.channel;
        return 1;
    }

    return 0;
}

int matter_wifi_connect(char *ssid, rtw_security_t security_type, char *password,
                        int ssid_len, int password_len, int key_id, void *semaphore)
{
    uint8_t connect_channel;
    int security_retry_count = 0;
    int err = 0;

    if (strlen((const char *) password) != 0) {
        security_type = RTW_SECURITY_WPA_WPA2_MIXED_PSK;
    } else {
        security_type = RTW_SECURITY_OPEN;
    }

    if (security_type == RTW_SECURITY_WPA_WPA2_MIXED) {
        while (1) {
            if (matter_get_ap_security_mode((char *)ssid, &security_type, &connect_channel)) {
                break;
            }
            security_retry_count++;
            if (security_retry_count >= 3) {
                // Can't get AP security mode and channel. Use RTW_SECURITY_WPA_WPA2_MIXED as default
                security_type = RTW_SECURITY_WPA_WPA2_MIXED;
                break;
            }
        }
        /* Don't set WEP Key ID, default use key_id = 0 for connection
         * If connection fails, use onnetwork (connect to AP with AT Command) instead of ble-wifi
         * This behavior matches other devices behavior as phone and laptop is unable to connect with key_id > 0
         * */
    }

    matter_wifi_set_autoreconnect(1);

    err = wifi_connect(ssid, security_type, password, strlen(ssid), strlen(password), key_id, NULL);
    if (err != RTW_SUCCESS) {
        RTW_API_INFO("ERROR: wifi Connect failed\n");
    }

    return err;
}

int matter_wifi_disconnect(void)
{
    return wifi_disconnect();
}

int matter_wifi_on(rtw_mode_t mode)
{
    return wifi_on(mode);
}

int matter_wifi_set_mode(rtw_mode_t mode)
{
    if (wifi_mode == mode) {
        return 0;
    }

    return wifi_set_mode(mode);
}

int matter_wifi_is_connected_to_ap(void)
{
    return wifi_is_connected_to_ap();
}

int matter_wifi_is_open_security(void)
{
    if (wifi_get_sta_security_type() == RTW_SECURITY_OPEN) {
        return 1;
    }

    return 0;
}

int matter_wifi_is_ready_to_transceive(rtw_interface_t interface)
{
    return wifi_is_ready_to_transceive(interface);
}

int matter_wifi_is_up(rtw_interface_t interface)
{
    return wifi_is_up(interface);
}

int matter_wifi_is_station_mode(void)
{
    if (wifi_mode == RTW_MODE_STA || wifi_mode == RTW_MODE_STA_AP) {
        return RTW_SUCCESS;
    } else {
        return RTW_ERROR;
    }

}

int matter_wifi_get_last_error(void)
{
    return wifi_get_last_error();
}

int matter_wifi_get_mac_address(char *mac)
{
    return wifi_get_mac_address(mac);
}

int matter_wifi_sta_get_network_mode(rtw_network_mode_t *pmode)
{
    return wifi_get_network_mode(pmode);
}

int matter_wifi_get_setting(unsigned char wlan_idx, rtw_wifi_setting_t *psetting)
{
    int ret = RTW_SUCCESS;

    switch (wlan_idx) {
    case WLAN0_IDX:
        ret = wifi_get_setting((const char *)WLAN0_NAME, psetting);
        break;
    case WLAN1_IDX:
        ret = wifi_get_setting((const char *)WLAN1_NAME, psetting);
        break;
    default:
        RTW_API_INFO("ERROR: Invalid index\n");
        ret = RTW_ERROR;
        break;
    }

    return ret;
}

int matter_get_sta_wifi_info(rtw_wifi_setting_t *pSetting)
{
    return wifi_get_setting((const char *)WLAN0_NAME, pSetting);
}

/* Support for WiFi Network Diagnostics. */

int matter_wifi_sta_get_ap_bssid(unsigned char *bssid)
{
    return wifi_get_ap_bssid(bssid);
}

int matter_wifi_sta_get_security_type(uint32_t *wifi_security)
{
    rtw_wifi_setting_t setting;

    int ret = wifi_get_setting(WLAN0_NAME, &setting);
    if (ret == RTW_SUCCESS) {
        *wifi_security = setting.security_type;
    }
    return ret;
}

int matter_wifi_sta_get_channel_number(uint8_t *ch)
{
    int ret = wext_get_channel(WLAN0_NAME, ch);
    if (ret < 0) {
        RTW_API_INFO("ERROR: Wi-Fi get chl failed\n");
    }

    return ret;
}

int matter_wifi_sta_get_rssi(int *prssi)
{
    return wifi_get_rssi(prssi);
}

int matter_wifi_sta_get_wifi_version(uint8_t *mode)
{
    int cur_mode = wifi_get_cur_network_mode();
    if (cur_mode == RTW_ERROR) {
        *mode = MATTER_WIFI_VERSION_11N;
        return RTW_ERROR;
    } else {
        *mode = cur_mode;
    }

    return RTW_SUCCESS;
}

#ifdef __cplusplus
}
#endif
