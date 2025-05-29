#include <platform_stdlib.h>
#include <FreeRTOS.h>
#include <task.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <chip_porting.h>
#include <lwip/dhcp.h>
#include <wifi_auto_reconnect.h>
#include <wifi_conf.h>

static const char *TAG = "matter-wifis";
u32 apNum = 0; // no of total AP scanned
static u8 matter_wifi_trigger = 0;
static rtw_scan_result_t matter_userdata[65] = {0};
static char *matter_ssid;
rtw_mac_t ap_bssid;
rtw_security_t sta_security_type;
int volatile error_flag = RTW_UNKNOWN;
rtw_mode_t wifi_mode = RTW_MODE_NONE;

#if defined(CONFIG_AUTO_RECONNECT) && CONFIG_AUTO_RECONNECT
static void *matter_param_indicator;
struct task_struct matter_wifi_autoreconnect_task;
struct matter_wifi_autoreconnect_param {
    rtw_security_t security_type;
    char *ssid;
    int ssid_len;
    char *password;
    int password_len;
    int key_id;
    char *bssid;
    char is_wps_trigger;
};
extern void (*p_wlan_autoreconnect_hdl)(rtw_security_t, char *, int, char *, int, int);
#endif /* CONFIG_AUTO_RECONNECT */

#if CONFIG_ENABLE_WPS
extern char wps_profile_ssid[33];
extern char wps_profile_password[65];
#endif /* CONFIG_ENABLE_WPS */

chip_connmgr_callback chip_connmgr_callback_func = NULL;
void *chip_connmgr_callback_data = NULL;
void chip_connmgr_set_callback_func(chip_connmgr_callback p, void *data)
{
    chip_connmgr_callback_func = p;
    chip_connmgr_callback_data = data;
}

int matter_initiate_wifi_and_connect(rtw_network_info_t *connect_param)
{
    int ret = RTW_UNKNOWN;
    sta_security_type = -1;
    memset(&ap_bssid, 0, ETH_ALEN);
    ret = wifi_connect(connect_param, 1);

    if (ret == RTW_SUCCESS) {
        sta_security_type = connect_param->security_type;
        memcpy(&ap_bssid, connect_param->bssid.octet, ETH_ALEN);
    } else if (rtw_join_status != RTW_JOINSTATUS_SUCCESS) {
        ret = error_flag;
    }

    return ret;
}

static void print_matter_scan_result(rtw_scan_result_t *record)
{
    RTW_API_INFO("%s\t ", (record->bss_type == RTW_BSS_TYPE_ADHOC) ? "Adhoc" : "Infra");
    RTW_API_INFO(MAC_FMT, MAC_ARG(record->BSSID.octet));
    RTW_API_INFO(" %d\t ", record->signal_strength);
    RTW_API_INFO(" %d\t  ", record->channel);
    RTW_API_INFO("%s\t\t ", (record->security == RTW_SECURITY_OPEN) ? "Open" :
                 (record->security == RTW_SECURITY_WEP_PSK) ? "WEP" :
                 (record->security == RTW_SECURITY_WPA_TKIP_PSK) ? "WPA TKIP" :
                 (record->security == RTW_SECURITY_WPA_AES_PSK) ? "WPA AES" :
                 (record->security == RTW_SECURITY_WPA_MIXED_PSK) ? "WPA Mixed" :
                 (record->security == RTW_SECURITY_WPA2_AES_PSK) ? "WPA2 AES" :
                 (record->security == RTW_SECURITY_WPA2_TKIP_PSK) ? "WPA2 TKIP" :
                 (record->security == RTW_SECURITY_WPA2_MIXED_PSK) ? "WPA2 Mixed" :
                 (record->security == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? "WPA/WPA2 TKIP" :
                 (record->security == RTW_SECURITY_WPA_WPA2_AES_PSK) ? "WPA/WPA2 AES" :
                 (record->security == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? "WPA/WPA2 Mixed" :
                 "Unknown");

    RTW_API_INFO(" %s ", record->SSID.val);
    RTW_API_INFO("\n");
}

static rtw_result_t matter_scan_result_handler(unsigned int scanned_AP_num, void *user_data)
{
    static int total_ap_num = 0;
    rtw_scan_result_t *scanned_AP_info;
    char *scan_buf = NULL;
    int ret = RTW_ERROR;

    if (scanned_AP_num > 0) {
        scan_buf = (char *)rtos_mem_zmalloc(scanned_AP_num * sizeof(rtw_scan_result_t));
        if (scan_buf == NULL) {
            RTK_LOGE(TAG, "malloc scan buf fail for scan mcc\n");
            ret = RTW_ERROR;
            goto exit;
        }
        if (wifi_get_scan_records(&scanned_AP_num, scan_buf) < 0) {
            rtos_mem_free(scan_buf);
            ret = RTW_ERROR;
            goto exit;
        }
        //print each scan result on one channel
        for (int i = 0; i < scanned_AP_num; i++) {
            scanned_AP_info = (rtw_scan_result_t *)(scan_buf + i * sizeof(rtw_scan_result_t));

            RTW_API_INFO("%d\t ", ++apNum);
            memcpy(&matter_userdata[i], scanned_AP_info, sizeof(rtw_scan_result_t));
            print_matter_scan_result(&matter_userdata[i]);
        }
        rtos_mem_free(scan_buf);
        ret = RTW_SUCCESS;
    }

    if (chip_connmgr_callback_func && chip_connmgr_callback_data) {
        // inform matter
        chip_connmgr_callback_func(chip_connmgr_callback_data);
        ret = RTW_SUCCESS;
    } else {
        RTK_LOGE(TAG, "chip_connmgr_callback_func is NULL\r\n");
        apNum = 0;
        ret = RTW_ERROR;
    }

exit:
    if (matter_ssid) {
        vPortFree(matter_ssid);
    }
    return ret;
}

void matter_scan_networks(void)
{
    volatile int ret = RTW_SUCCESS;
    apNum = 0; // reset counter at the start of scan
    rtw_scan_param_t scan_param;
    memset(&scan_param, 0, sizeof(scan_param));
    scan_param.scan_user_callback = matter_scan_result_handler;

    ret = wifi_scan_networks(&scan_param, 0);
    if (ret != RTW_SUCCESS)
    {
        RTW_API_INFO("ERROR: wifi scan failed\n");
    }
}

void matter_scan_networks_with_ssid(const unsigned char *ssid, size_t length)
{
    volatile int ret = RTW_SUCCESS;

    apNum = 0; // reset counter at the start of scan

    matter_ssid = (char *) pvPortMalloc(length + 1);

    memset(matter_ssid, 0, length + 1);
    memcpy(matter_ssid, ssid, length);
    matter_ssid[length] = '\0';

    rtw_scan_param_t scan_param;
    memset(&scan_param, 0, sizeof(scan_param));
    scan_param.scan_user_callback = matter_scan_result_handler;
    scan_param.ssid = matter_ssid;

    ret = wifi_scan_networks(&scan_param, 0);
    if (ret != RTW_SUCCESS)
    {
        RTW_API_INFO("ERROR: wifi scan failed\n");
    }
}

rtw_scan_result_t *matter_get_scan_results(void)
{
    return matter_userdata;
}

#if defined(CONFIG_AUTO_RECONNECT) && CONFIG_AUTO_RECONNECT
void matter_reconn_task_hdl(void *param)
{
    (void) param;
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
    rtos_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif

    matter_initiate_wifi_and_connect(&rtw_reconn.conn_param);

    // matter_wifi_autoreconnect_task.task = 0;
    rtos_task_delete(NULL);
}
#endif /* CONFIG_AUTO_RECONNECT */

void matter_set_autoreconnect(uint8_t mode)
{
#if defined(CONFIG_AUTO_RECONNECT) && CONFIG_AUTO_RECONNECT
    size_t ssidLen = 0;
    unsigned char buf[32];
    const char kWiFiSSIDKeyName[] = "wifi-ssid";

    memset(buf, 0, sizeof(buf));

    //if wifi-ssid exist in NVS, it has been commissioned before, CHIP will do autoreconnection
    s32 ret = getPref_bin_new(kWiFiSSIDKeyName, kWiFiSSIDKeyName, buf, sizeof(buf), &ssidLen);
    if (ret == DCT_SUCCESS) {
        wifi_config_autoreconnect(mode);
    }
#endif /* CONFIG_AUTO_RECONNECT */
    return;
}

static int matter_find_ap_from_scan_buf(char *buf, int buflen, char *target_ssid, void *user_data)
{
    rtw_wifi_setting_t *pwifi = (rtw_wifi_setting_t *)user_data;
    int plen = 0;

    while (plen < buflen) {
        u8 len, ssid_len, security_mode;
        char *ssid;

        // len offset = 0
        len = (int) * (buf + plen);
        // check end
        if (len == 0) {
            break;
        }
        // ssid offset = 14
        ssid_len = len - 14;
        ssid = buf + plen + 14 ;
        if ((ssid_len == strlen(target_ssid)) && (!memcmp(ssid, target_ssid, ssid_len))) {
            strncpy((char *)pwifi->ssid, target_ssid, 33);
            // channel offset = 13
            pwifi->channel = *(buf + plen + 13);
            // security_mode offset = 11
            security_mode = (u8) * (buf + plen + 11);
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
    volatile int ret = RTW_SUCCESS;
    rtw_scan_param_t scan_param;

    memset(&scan_param, 0, sizeof(scan_param));
    scan_param.ssid = ssid;
    scan_param.scan_user_callback = matter_scan_result_handler;
    // scan_param.scan_user_data = NULL;
    apNum = 0; // reset counter at the start of scan
    memset(matter_userdata, 0, sizeof(matter_userdata));
    ret = wifi_scan_networks(&scan_param, 0);
    if (ret != RTW_SUCCESS) {
        RTK_LOGE(TAG, "ERROR: wifi scan failed\n\r");
        return 0;
    } else {
        int wait_wifi_scan_count = 0;
        RTK_LOGI(TAG, "Getting AP Security Mode\n\r");
        while (!*matter_userdata[0].SSID.val) {
            vTaskDelay(1000); //Wait until the scanning is finished
            if (++wait_wifi_scan_count >= 12) { // 12 seconds for AmebaD+
                break;
            }
        }
    }
    for (int i = 0; i < (sizeof(matter_userdata) / sizeof(matter_userdata[0])); i++) {
        if (strcmp((char *)matter_userdata[i].SSID.val, ssid) == 0) {
            *security_mode = matter_userdata[i].security;
            *channel = matter_userdata[i].channel;
            return 1;
        }
    }
    return 0;
}

int matter_wifi_connect(
    char              *ssid,
    rtw_security_t    security_type,
    char              *password,
    int               ssid_len,
    int               password_len,
    int               key_id,
    void              *semaphore)
{
    uint8_t connect_channel;
    int security_retry_count = 0;
    int err = 0;
    rtw_network_info_t connect_param = {0};

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

    matter_wifi_trigger = 1;
    matter_set_autoreconnect(1);

    strncpy((char *)connect_param.ssid.val, ssid, sizeof(connect_param.ssid.val) - 1);
    connect_param.ssid.len = ssid_len;
    memset(connect_param.bssid.octet, 0, ETH_ALEN);
    connect_param.password = (unsigned char *)password;
    connect_param.password_len = password_len;
    connect_param.security_type = security_type;
    connect_param.key_id = key_id;
    connect_param.channel = connect_channel;
    connect_param.pscan_option = 0;

    err = matter_initiate_wifi_and_connect(&connect_param);
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
    int ret = -1;

    struct netif *pnetif = NULL;
    pnetif = &xnetif[0]; //Station will always be netif[0]

    if (wifi_mode == mode) {
        ret = RTW_SUCCESS;
    } else {
        ret = wifi_on(mode);
    }

    if (ret == RTW_SUCCESS) {
        wifi_mode = mode;
#if LWIP_VERSION_MAJOR >= 2 && LWIP_VERSION_MINOR >= 1
#if LWIP_IPV6
        if (mode == RTW_MODE_STA) {
            if (!netif_is_up(pnetif)) {
                netifapi_netif_set_up(pnetif);
            }
            netif_create_ip6_linklocal_address(pnetif, 1);
        }
#endif
#endif
    }
    return ret;
}

int matter_wifi_set_mode(rtw_mode_t mode)
{
    return matter_wifi_on(mode);
}

int matter_wifi_is_connected_to_ap(void)
{
    return wifi_is_connected_to_ap();
}

int matter_wifi_is_open_security(void)
{
    if (sta_security_type == RTW_SECURITY_OPEN)
    {
        return 1;
    }

    return 0;
}

int matter_wifi_is_ready_to_transceive(rtw_interface_t interface)
{
    switch (interface) {
    case RTW_AP_INTERFACE:
        return wifi_is_running(SOFTAP_WLAN_INDEX);
    case RTW_STA_INTERFACE:
        switch (error_flag) {
        case RTW_NO_ERROR:
            return RTW_SUCCESS;
        default:
            return RTW_ERROR;
        }
    default:
        return RTW_ERROR;
    }
}

int matter_wifi_is_up(rtw_interface_t interface)
{
    switch (interface) {
    case RTW_STA_INTERFACE:
        return wifi_is_running(STA_WLAN_INDEX);
    default:
        return wifi_is_running(SOFTAP_WLAN_INDEX);
    }
}

int matter_wifi_is_station_mode(void)
{
    if (wifi_mode == RTW_MODE_STA || wifi_mode == RTW_MODE_STA_AP) {
        return RTW_SUCCESS;
    } else {
        return RTW_ERROR;
    }
}

int matter_wifi_get_ap_bssid(unsigned char *bssid)
{
    if ((int) RTW_SUCCESS == matter_wifi_is_ready_to_transceive(RTW_STA_INTERFACE)) {
        memcpy(bssid, ap_bssid.octet, ETH_ALEN);
        return RTW_SUCCESS;
    }
    return RTW_ERROR;
}

int matter_wifi_get_last_error(void)
{
    return error_flag;
}

int matter_wifi_get_mac_address(char *mac)
{
    rtw_mac_t mac_struct = {0};
    int ret = wifi_get_mac_address(WLAN0_IDX, &mac_struct, 0);
    DiagSnPrintf(mac, 32, "%02X:%02X:%02X:%02X:%02X:%02X", mac_struct.octet[0], mac_struct.octet[1],
                 mac_struct.octet[2], mac_struct.octet[3], mac_struct.octet[4], mac_struct.octet[5]);
    return ret;
}

int matter_wifi_get_network_mode(rtw_network_mode_t *pmode)
{
    *pmode = wifi_get_network_mode();
    return 0;
}

int matter_wifi_get_rssi(int *prssi)
{
    int ret;
    rtw_phy_statistics_t phy_statistics;
    ret = wifi_fetch_phy_statistic(&phy_statistics);
    if (ret >= 0) {
        *prssi = phy_statistics.rssi;
    }
    return ret;
}

int matter_wifi_get_security_type(uint8_t wlan_idx, uint32_t *wifi_security)
{
    int ret = RTW_SUCCESS;

    rtw_wifi_setting_t setting = {0};
    if (wifi_get_setting(wlan_idx, &setting) < 0) {
        ret = RTW_ERROR;
    } else {
        *wifi_security = setting.security_type;
    }

    return ret;
}

int matter_wifi_get_setting(unsigned char wlan_idx, rtw_wifi_setting_t *psetting)
{
    int ret = RTW_SUCCESS;

    ret = wifi_get_setting(wlan_idx, psetting);

    return ret;
}

int matter_wifi_get_wifi_channel_number(uint8_t wlan_idx, uint8_t *ch)
{
    int ret = RTW_SUCCESS;

    if (wifi_get_channel(wlan_idx, ch) < 0) {
        ret = RTW_ERROR;
    }

    return ret;
}

int matter_get_sta_wifi_info(rtw_wifi_setting_t *pSetting)
{
    return wifi_get_setting(WLAN0_IDX, pSetting);
}

void matter_wifi_reg_event_handler(matter_wifi_event event_cmds, rtw_event_handler_t handler_func, void *handler_user_data)
{
    wifi_reg_event_handler(event_cmds, handler_func, handler_user_data);
}

static void matter_wifi_join_status_event_hdl(char *buf, int buf_len, int flags, void *userdata)
{
    UNUSED(buf_len);
    UNUSED(userdata);

    enum rtw_join_status_type join_status = (enum rtw_join_status_type)flags;
    struct rtw_event_join_fail_info_t *fail_info = (struct rtw_event_join_fail_info_t *)buf;

    switch (join_status) {
        case RTW_JOINSTATUS_SUCCESS: // Connecting --> Connected Succesfully
            error_flag = RTW_NO_ERROR;
            RTK_LOGI(TAG, "Join success!\n");
            wifi_indication(WIFI_EVENT_MATTER_STA_CONN, NULL, 0, 0);
            break;
        case RTW_JOINSTATUS_FAIL: // Connecting --> Failed to Connect
            RTK_LOGI(TAG, "Join fail, error_flag = ");
            switch (fail_info->fail_reason) {
                case RTW_CONNECT_SCAN_FAIL:
                    error_flag = RTW_NONE_NETWORK;
                    RTK_LOGI(NOTAG, "%d (Can not found target AP)\n", error_flag);
                    break;
                case RTW_CONNECT_AUTH_FAIL:
                case RTW_CONNECT_ASSOC_FAIL:
                case RTW_CONNECT_4WAY_HANDSHAKE_FAIL:
                    error_flag = RTW_CONNECT_FAIL;
                    RTK_LOGI(NOTAG, "%d (Auth/Assoc/Handshake failed)\n", error_flag);
                    break;
                case RTW_CONNECT_AUTH_PASSWORD_WRONG:
                case RTW_CONNECT_4WAY_PASSWORD_WRONG:
                    error_flag = RTW_WRONG_PASSWORD;
                    RTK_LOGI(NOTAG, "%d (Wrong Password)\n", error_flag);
                    break;
                default:
                    error_flag = RTW_UNKNOWN;
                    RTK_LOGI(NOTAG, "%d (Unknown Error)\n", error_flag);
                    break;
            }
            wifi_indication(WIFI_EVENT_MATTER_STA_DISCONN, NULL, 0, 0);
            break;
        case RTW_JOINSTATUS_DISCONNECT: // Connected --> Disconnected
            error_flag = RTW_CONNECT_FAIL;
            RTK_LOGI(TAG, "Disconnected, try to reconnect...\n");
            wifi_indication(WIFI_EVENT_MATTER_STA_DISCONN, NULL, 0, 0);
            break;
        default:
            break;
    }
}

void matter_wifi_reg_join_status_handler(void)
{
    wifi_reg_event_handler(WIFI_EVENT_JOIN_STATUS, matter_wifi_join_status_event_hdl, NULL);
}

void matter_wifi_init(void)
{
    matter_wifi_on(RTW_MODE_STA);
    matter_wifi_reg_join_status_handler();
#if CONFIG_AUTO_RECONNECT
    //setup reconnection flag
    matter_set_autoreconnect(1);
#endif
}

#ifdef __cplusplus
}
#endif
