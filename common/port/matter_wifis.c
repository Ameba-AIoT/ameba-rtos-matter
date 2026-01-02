/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
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

#define SCAN_RECORD_SECURITY_OFFSET 11
#define SCAN_RECORD_CHANNEL_OFFSET  13
#define SCAN_RECORD_SSID_OFFSET     14
#define MAX_SSID_LEN                32

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
extern void (*p_wlan_autoreconnect_hdl)(rtw_security_t, char *, int, char *, int, int);
#endif /* CONFIG_AUTO_RECONNECT */

struct event_list_elem_t {
    void (*handler)(u8 *buf, s32 len, s32 flags, void *user_data);
    void  *handler_user_data;
};
#define MATTER_WIFI_EVENT_MAX_ROW 2
static struct event_list_elem_t matter_event_callback_list[MATTER_WIFI_EVENT_MAX][MATTER_WIFI_EVENT_MAX_ROW] = {0};

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
    DiagPrintf("%s\t ", (record->bss_type == RTW_BSS_TYPE_INFRASTRUCTURE) ? "Infra" : "Adhoc");
    DiagPrintf(MAC_FMT, MAC_ARG(record->BSSID.octet));
    DiagPrintf(" %d\t ", record->signal_strength);
    DiagPrintf(" %d\t  ", record->channel);
    DiagPrintf("%s\t\t ", (record->security == RTW_SECURITY_OPEN) ? "Open" :
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

    DiagPrintf(" %s ", record->SSID.val);
    DiagPrintf("\n");
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

            DiagPrintf("%d\t ", ++apNum);
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

void matter_wifi_scan_networks(void)
{
    volatile int ret = RTW_SUCCESS;
    apNum = 0; // reset counter at the start of scan
    rtw_scan_param_t scan_param;
    memset(&scan_param, 0, sizeof(scan_param));
    scan_param.scan_user_callback = matter_scan_result_handler;

    ret = wifi_scan_networks(&scan_param, 0);
    if (ret != RTW_SUCCESS)
    {
        RTK_LOGE(TAG, "ERROR: wifi scan failed\n");
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

    rtw_scan_param_t scan_param;
    memset(&scan_param, 0, sizeof(scan_param));
    scan_param.scan_user_callback = matter_scan_result_handler;
    scan_param.ssid = matter_ssid;

    ret = wifi_scan_networks(&scan_param, 0);
    if (ret != RTW_SUCCESS)
    {
        RTK_LOGE(TAG, "ERROR: wifi scan failed\n");
    }
}

rtw_scan_result_t *matter_get_scan_results(void)
{
    return matter_userdata;
}

#if defined(CONFIG_AUTO_RECONNECT) && CONFIG_AUTO_RECONNECT
void matter_wifi_reconn_task_hdl(void *param)
{
    (void) param;
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
    rtos_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif

    matter_initiate_wifi_and_connect(&rtw_reconn.conn_param);

    // matter_wifi_autoreconnect_task.task = 0;
    rtos_task_delete(NULL);
}

void matter_reconn_timer_hdl(rtos_timer_t timer_hdl)
{
    (void) timer_hdl;

    rtw_reconn.b_waiting = 0;
    /*Creat a task to do wifi reconnect because call WIFI API in WIFI event is not safe*/
    if (rtos_task_create(NULL, ((const char *)"matter_wifi_reconn_task_hdl"), matter_wifi_reconn_task_hdl, NULL, 1024, 6) != RTW_SUCCESS) {
        RTK_LOGE(TAG, "Create reconn task failed\n");
    } else {
        RTK_LOGI(TAG, "auto reconn %d\n", rtw_reconn.cnt);
    }
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
    if (ret == DCT_SUCCESS) {
        if ((mode == 0) && rtw_reconn.b_enable) {
            rtos_timer_stop(rtw_reconn.timer, 1000);
            rtos_timer_delete(rtw_reconn.timer, 1000);
            rtw_reconn.timer = NULL;
            rtw_reconn.b_waiting = 0;
            rtw_reconn.b_enable = 0;
        } else if ((mode != 0) && (rtw_reconn.b_enable == 0)) {
            if (rtos_timer_create(&(rtw_reconn.timer), "matter_reconn_timer", NULL, wifi_user_config.auto_reconnect_interval * 1000, FALSE,
                                matter_reconn_timer_hdl) != RTW_SUCCESS) {
                RTK_LOGI(TAG, "matter_reconn_timer create fail\n");
                return RTW_ERROR;
            }
            rtw_reconn.b_enable = 1;
            rtw_reconn.cnt = 0;
        }

        rtw_reconn.b_infinite = (wifi_user_config.auto_reconnect_count == 0xff) ? 1 : 0;
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

        len = (int) * (buf + plen);
        if (len == 0) {
            break;
        }
        ssid_len = len - SCAN_RECORD_SSID_OFFSET;
        ssid = buf + plen + SCAN_RECORD_SSID_OFFSET;

        if ((ssid_len == strlen(target_ssid)) && (!memcmp(ssid, target_ssid, ssid_len))) {
            strncpy((char *)pwifi->ssid, target_ssid, MAX_SSID_LEN + 1);
            pwifi->channel = *(buf + plen + SCAN_RECORD_CHANNEL_OFFSET);

            security_mode = (u8) * (buf + plen + SCAN_RECORD_SECURITY_OFFSET);
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

static int matter_get_ap_security_mode(char *ssid, rtw_security_t *security_mode, u8 *channel)
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
    matter_wifi_set_autoreconnect(1);

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
        RTK_LOGE(TAG, "ERROR: wifi Connect failed\n");
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
#if defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)
    return wifi_is_connected_to_ap();
#elif defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1)
    u8 join_status = RTW_JOINSTATUS_UNKNOWN;
    if ((wifi_get_join_status(&join_status) == RTK_SUCCESS) && (join_status == RTW_JOINSTATUS_SUCCESS))
        return RTW_SUCCESS;
    else
        return RTW_ERROR;
#endif // (CONFIG_AMEBARTOS_XXX)
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

int matter_wifi_sta_get_network_mode(rtw_network_mode_t *pmode)
{
    *pmode = wifi_get_network_mode();
    return 0;
}

int matter_wifi_get_setting(unsigned char wlan_idx, rtw_wifi_setting_t *psetting)
{
    int ret = RTW_SUCCESS;

    ret = wifi_get_setting(wlan_idx, psetting);

    return ret;
}

int matter_get_sta_wifi_info(rtw_wifi_setting_t *pSetting)
{
    return wifi_get_setting(WLAN0_IDX, pSetting);
}

int matter_wifi_indication(u32 event_cmd, u8 *buf, s32 buf_len, s32 flags)
{
    void (*handle)(u8 * buf, s32 len, s32 flags, void *user_data) = NULL;
    int i = 0;

    /*user callback handle*/
    if (event_cmd < MATTER_WIFI_EVENT_MAX) {
        for (i = 0; i < MATTER_WIFI_EVENT_MAX_ROW; i++) {
            handle = matter_event_callback_list[event_cmd][i].handler;
            if (handle == NULL) {
                continue;
            }
            handle(buf, buf_len, flags, matter_event_callback_list[event_cmd][i].handler_user_data);
        }
    }
    return RTW_SUCCESS;
}

void matter_wifi_reg_event_handler(matter_wifi_event event_cmds, rtw_event_handler_t handler_func, void *handler_user_data)
{
    int i = 0, j = 0;
    if (event_cmds < MATTER_WIFI_EVENT_MAX) {
        for (i = 0; i < MATTER_WIFI_EVENT_MAX_ROW; i++) {
            if (matter_event_callback_list[event_cmds][i].handler == NULL) { //there exists an empty position for new handler
                for (j = 0; j < MATTER_WIFI_EVENT_MAX_ROW; j++) {
                    if (matter_event_callback_list[event_cmds][j].handler == handler_func) { //the new handler already exists in the table
                        return;
                    }
                }
                matter_event_callback_list[event_cmds][i].handler = handler_func;
                matter_event_callback_list[event_cmds][i].handler_user_data = handler_user_data;
                return;
            }
        }
        //there is no empty position for new handler
        RTK_LOGE(TAG, "%s fail: %d %d \n", __func__, event_cmds, MATTER_WIFI_EVENT_MAX_ROW);
    }
}

#if defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)
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
            matter_wifi_indication(MATTER_WIFI_EVENT_CONNECT, NULL, 0, flags);
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
            matter_wifi_indication(MATTER_WIFI_EVENT_DISCONNECT, NULL, 0, flags);
            break;
        case RTW_JOINSTATUS_DISCONNECT: // Connected --> Disconnected
            error_flag = RTW_CONNECT_FAIL;
            RTK_LOGI(TAG, "Disconnected, try to reconnect...\n");
            matter_wifi_indication(MATTER_WIFI_EVENT_DISCONNECT, NULL, 0, flags);
            break;
        default:
            break;
    }
}
#elif defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1)
static void matter_wifi_join_status_event_hdl(u8 *buf, s32 buf_len, s32 flags, void *userdata)
{
    UNUSED(buf_len);
    UNUSED(userdata);
    u8 join_status = (u8)flags;
    struct rtw_event_info_joinstatus_joinfail *fail_info = (struct rtw_event_info_joinstatus_joinfail *)buf;
    struct rtw_event_info_joinstatus_disconn *disconn_info = (struct rtw_event_info_joinstatus_disconn *)buf;

    switch (join_status) {
        case RTW_JOINSTATUS_SUCCESS: // Connecting --> Connected Succesfully
            error_flag = RTW_NO_ERROR;
            RTK_LOGI(TAG, "Join success!\n");
#if CONFIG_ENABLE_AMEBA_SNTP
            matter_sntp_init();
#endif
            matter_wifi_indication(MATTER_WIFI_EVENT_CONNECT, NULL, 0, flags);
            break;
        case RTW_JOINSTATUS_FAIL: // Connecting --> Failed to Connect
            RTK_LOGI(TAG, "Join fail, error_flag = ");
            switch (fail_info->fail_reason) {
                case -RTK_ERR_WIFI_CONN_SCAN_FAIL:
                    error_flag = RTW_NONE_NETWORK;
                    RTK_LOGI(NOTAG, "%d (Can not found target AP)\n", error_flag);
                    break;
                case -RTK_ERR_WIFI_CONN_AUTH_FAIL:
                case -RTK_ERR_WIFI_CONN_ASSOC_FAIL:
                case -RTK_ERR_WIFI_CONN_4WAY_HANDSHAKE_FAIL:
                    error_flag = RTW_CONNECT_FAIL;
                    RTK_LOGI(NOTAG, "%d (Auth/Assoc/Handshake failed)\n", error_flag);
                    break;
                case -RTK_ERR_WIFI_CONN_AUTH_PASSWORD_WRONG:
                case -RTK_ERR_WIFI_CONN_4WAY_PASSWORD_WRONG:
                    error_flag = RTW_WRONG_PASSWORD;
                    RTK_LOGI(NOTAG, "%d (Wrong Password)\n", error_flag);
                    break;
                default:
                    error_flag = RTW_UNKNOWN;
                    RTK_LOGI(NOTAG, "%d (Unknown Error)\n", error_flag);
                    break;
            }
            matter_wifi_indication(MATTER_WIFI_EVENT_DISCONNECT, NULL, 0, flags);
            break;
        case RTW_JOINSTATUS_DISCONNECT: // Connected --> Disconnected
            error_flag = RTW_CONNECT_FAIL;
            RTK_LOGI(TAG, "Disconnected, try to reconnect...\n");
            matter_wifi_indication(MATTER_WIFI_EVENT_DISCONNECT, NULL, 0, flags);
            break;
        default:
            break;
    }
}
#endif // (CONFIG_AMEBARTOS_XXX)

void matter_wifi_init(void)
{
    matter_wifi_on(RTW_MODE_STA);
#if CONFIG_AUTO_RECONNECT
    //setup reconnection flag
    matter_wifi_set_autoreconnect(1);
#endif
#if defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)
    wifi_reg_event_handler(WIFI_EVENT_JOIN_STATUS, matter_wifi_join_status_event_hdl, NULL);
#elif defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1)
    wifi_reg_event_handler(RTW_EVENT_JOIN_STATUS, matter_wifi_join_status_event_hdl, NULL);
#endif
}

void matter_wifi_wait(void)
{
    do // Wait first to avoid hang issue for ameba-smart cmake
    {
        vTaskDelay(10);
    }
    while (!(wifi_is_running(WLAN0_IDX) || wifi_is_running(WLAN1_IDX)));

    matter_wifi_init();
}

/* Support for WiFi Network Diagnostics. */

int matter_wifi_sta_get_ap_bssid(unsigned char *bssid)
{
    if ((int) RTW_SUCCESS == matter_wifi_is_ready_to_transceive(RTW_STA_INTERFACE)) {
        memcpy(bssid, ap_bssid.octet, ETH_ALEN);
        return RTW_SUCCESS;
    }
    return RTW_ERROR;
}

int matter_wifi_sta_get_security_type(uint32_t *wifi_security)
{
    int ret = RTW_SUCCESS;

    rtw_wifi_setting_t setting = {0};
    if (wifi_get_setting(WLAN0_IDX, &setting) < 0) {
        ret = RTW_ERROR;
    } else {
        *wifi_security = setting.security_type;
    }

    return ret;
}

int matter_wifi_sta_get_channel_number(uint8_t *ch)
{
    int ret = RTW_SUCCESS;
#if defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)
    if (wifi_get_channel(WLAN0_IDX, ch) < 0) {
        ret = RTW_ERROR;
    }
#elif defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1)
    rtw_wifi_setting_t setting = {0};
    if (wifi_get_setting(WLAN0_IDX, &setting) < 0) {
        ret = RTW_ERROR;
    } else {
        *ch = setting.channel;
    }
#endif // (CONFIG_AMEBARTOS_XXX)
    return ret;
}

int matter_wifi_sta_get_rssi(int *prssi)
{
    int ret;
#if defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)
    rtw_phy_statistics_t phy_statistics;
    ret = wifi_fetch_phy_statistic(&phy_statistics);
    if (ret >= 0) {
        *prssi = phy_statistics.rssi;
    }
#elif defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1)
    union rtw_phy_stats phy_stats;
    ret = wifi_get_phy_stats(STA_WLAN_INDEX, NULL, &phy_stats);
    if (ret >= 0) {
        *prssi = phy_stats.sta.rssi;
    }
#endif // (CONFIG_AMEBARTOS_XXX)
    return ret;
}

int matter_wifi_sta_get_wifi_version(uint8_t *mode)
{
    int ret;
#if defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)
    ret = wifi_get_network_mode();
    if (ret >= 0) {
        switch (ret) {
        case WLAN_MD_11B:
            *mode = MATTER_WIFI_VERSION_11B;
            break;
        case WLAN_MD_11G:
            *mode = MATTER_WIFI_VERSION_11G;
            break;
        case WLAN_MD_11A:
            *mode = MATTER_WIFI_VERSION_11A;
            break;
        case WLAN_MD_11N:
            *mode = MATTER_WIFI_VERSION_11N;
            break;
        case WLAN_MD_11AC:
            *mode = MATTER_WIFI_VERSION_11AC;
            break;
        case WLAN_MD_11AX:
            *mode = MATTER_WIFI_VERSION_11AX;
            break;
        default:
            *mode = MATTER_WIFI_VERSION_11N;
            break;
        }
        ret = RTW_SUCCESS;
    } else {
        *mode = MATTER_WIFI_VERSION_11N;
        ret = RTW_ERROR;

    }
#elif defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1)
    ret = wifi_get_wireless_mode(mode);
    if (ret >= 0) {
        switch (*mode) {
        case RTW_80211_B:
            *mode = MATTER_WIFI_VERSION_11B;
            break;
        case RTW_80211_G:
            *mode = MATTER_WIFI_VERSION_11G;
            break;
        case RTW_80211_A:
            *mode = MATTER_WIFI_VERSION_11A;
            break;
        case RTW_80211_N:
            *mode = MATTER_WIFI_VERSION_11N;
            break;
        case RTW_80211_AC:
            *mode = MATTER_WIFI_VERSION_11AC;
            break;
        case RTW_80211_AX:
            *mode = MATTER_WIFI_VERSION_11AX;
            break;
        default:
            *mode = MATTER_WIFI_VERSION_11N;
            break;
        }
    } else {
        *mode = MATTER_WIFI_VERSION_11N;
    }
#endif // (CONFIG_AMEBARTOS_XXX)
    return ret;
}

#ifdef __cplusplus
}
#endif
