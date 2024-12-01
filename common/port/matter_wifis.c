#include <platform_opts.h>
#include <platform_stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <chip_porting.h>
#include <lwip/dhcp.h>
#include <osdep_service.h>
#include <wifi_conf.h>

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#define MAX_NUM_OF_AP_SCANNED 65
#elif defined(CONFIG_PLATFORM_8735B)
#define MAX_NUM_OF_AP_SCANNED 16
#endif

u32 apNum = 0; // no of total AP scanned
static rtw_scan_result_t matter_userdata[MAX_NUM_OF_AP_SCANNED] = {0};
static char *matter_ssid;

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
};
extern unsigned char *rltk_wlan_get_saved_bssid(void);
extern void (*p_wlan_autoreconnect_hdl)(rtw_security_t, char*, int, char*, int, int);
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

static void print_matter_scan_result(rtw_scan_result_t *record)
{
    RTW_API_INFO("%s\t ", ( record->bss_type == RTW_BSS_TYPE_ADHOC ) ? "Adhoc" : "Infra");
    RTW_API_INFO(MAC_FMT, MAC_ARG(record->BSSID.octet));
    RTW_API_INFO(" %d\t ", record->signal_strength);
    RTW_API_INFO(" %d\t  ", record->channel);
    RTW_API_INFO(" %d\t  ", record->wps_type);
    RTW_API_INFO("%s\t\t ", ( record->security == RTW_SECURITY_OPEN ) ? "Open" :
                            ( record->security == RTW_SECURITY_WEP_PSK ) ? "WEP" :
                            ( record->security == RTW_SECURITY_WPA2_AES_PSK ) ? "WPA2 AES" :
                            ( record->security == RTW_SECURITY_WPA2_TKIP_PSK ) ? "WPA2 TKIP" :
                            ( record->security == RTW_SECURITY_WPA2_MIXED_PSK ) ? "WPA2 Mixed" :
                            ( record->security == RTW_SECURITY_WPA_TKIP_PSK ) ? "WPA TKIP" :
                            ( record->security == RTW_SECURITY_WPA_AES_PSK ) ? "WPA AES" :
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
                            ( record->security == RTW_SECURITY_WPA_MIXED_PSK ) ? "WPA Mixed" :
                            ( record->security == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? "WPA/WPA2 TKIP" :
                            ( record->security == RTW_SECURITY_WPA_WPA2_AES_PSK ) ? "WPA/WPA2 AES" :
                            ( record->security == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? "WPA/WPA2 Mixed" :
                            ( record->security == RTW_SECURITY_WPA2_TKIP_ENTERPRISE ) ? "WPA2 TKIP Enterprise" :
                            ( record->security == RTW_SECURITY_WPA2_AES_ENTERPRISE ) ? "WPA2 AES Enterprise" :
                            ( record->security == RTW_SECURITY_WPA2_MIXED_ENTERPRISE ) ? "WPA2 Mixed Enterprise" :
                            ( record->security == RTW_SECURITY_WPA_TKIP_ENTERPRISE ) ? "WPA TKIP Enterprise" :
                            ( record->security == RTW_SECURITY_WPA_AES_ENTERPRISE ) ? "WPA AES Enterprise" :
                            ( record->security == RTW_SECURITY_WPA_MIXED_ENTERPRISE ) ? "WPA Mixed Enterprise" :
                            ( record->security == RTW_SECURITY_WPA_WPA2_TKIP_ENTERPRISE ) ? "WPA/WPA2 TKIP Enterprise" :
                            ( record->security == RTW_SECURITY_WPA_WPA2_AES_ENTERPRISE ) ? "WPA/WPA2 AES Enterprise" :
                            ( record->security == RTW_SECURITY_WPA_WPA2_MIXED_ENTERPRISE ) ? "WPA/WPA2 Mixed Enterprise" :
#elif defined(CONFIG_PLATFORM_8735B)
                            ( record->security == RTW_SECURITY_WPA_WPA2_MIXED) ? "WPA/WPA2 AES" :
                            ( record->security == RTW_SECURITY_WPA2_ENTERPRISE ) ? "WPA2 Enterprise" :
                            ( record->security == RTW_SECURITY_WPA_WPA2_ENTERPRISE ) ? "WPA/WPA2 Enterprise" :
#endif
#ifdef CONFIG_SAE_SUPPORT
                            ( record->security == RTW_SECURITY_WPA3_AES_PSK ) ? "WPA3-SAE AES" :
                            ( record->security == RTW_SECURITY_WPA2_WPA3_MIXED ) ? "WPA2/WPA3-SAE AES" :
#endif
                             "Unknown");

    RTW_API_INFO(" %s ", record->SSID.val);
    RTW_API_INFO("\n");
}

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
static rtw_result_t matter_scan_result_handler(rtw_scan_handler_result_t *malloced_scan_result)
{
    if (malloced_scan_result->scan_complete != RTW_TRUE)
    {
        if (malloced_scan_result->ap_details.SSID.len != 0)
        {
            rtw_scan_result_t *record = &malloced_scan_result->ap_details;
            record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */

            RTW_API_INFO("%d\t ", ++apNum);
            print_matter_scan_result(record);

            if (malloced_scan_result->user_data)
            {
                memcpy((void *)((char *)malloced_scan_result->user_data+(apNum-1)*sizeof(rtw_scan_result_t)), (char *)record, sizeof(rtw_scan_result_t));
            }
        }
    }
    else
    {
        if (chip_connmgr_callback_func && chip_connmgr_callback_data)
        {
            // inform matter
            chip_connmgr_callback_func(chip_connmgr_callback_data);
        }
        else
        {
            RTW_API_INFO("ERROR: chip_connmgr_callback_func is NULL\n");
            apNum = 0;
            return RTW_ERROR;
        }
    }

    return RTW_SUCCESS;
}

static rtw_result_t matter_scan_with_ssid_result_handler(rtw_scan_handler_result_t *malloced_scan_result)
{
    if (malloced_scan_result->scan_complete != RTW_TRUE)
    {
        rtw_scan_result_t *record = &malloced_scan_result->ap_details;
        record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */

        if ((malloced_scan_result->user_data) && (!strcmp(matter_ssid, (char *)record->SSID.val)))
        {
            RTW_API_INFO("%d\t ", ++apNum);
            memcpy((void *)((char *)malloced_scan_result->user_data+(apNum-1)*sizeof(rtw_scan_result_t)), (char *)record, sizeof(rtw_scan_result_t));
            print_matter_scan_result(record);
        }
    }
    else
    {
        if (chip_connmgr_callback_func && chip_connmgr_callback_data)
        {
            // inform matter
            chip_connmgr_callback_func(chip_connmgr_callback_data);
            vPortFree(matter_ssid);
        }
        else
        {
            RTW_API_INFO("ERROR: chip_connmgr_callback_func is NULL\n");
            apNum = 0;
            vPortFree(matter_ssid);
            return RTW_ERROR;
        }
    }

    return RT W_SUCCESS;
}
#elif defined(CONFIG_PLATFORM_8735B)
static rtw_result_t matter_scan_result_handler(unsigned int scanned_AP_num, void *user_data)
{
    /* To avoid gcc warnings */
    (void) user_data;

    rtw_scan_result_t *scanned_AP_info;
    char *scan_buf = NULL;
    unsigned int i = 0;

    scan_buf = (char *)rtw_zmalloc(scanned_AP_num * sizeof(rtw_scan_result_t));
    if (scan_buf == NULL)
    {
        return RTW_ERROR;
    }

    if (wifi_get_scan_records(&scanned_AP_num, scan_buf) < 0)
    {
        rtw_mfree((u8 *)scan_buf, 0);
        return RTW_ERROR;
    }

    for (i = 0; i < scanned_AP_num; i++)
    {
        RTW_API_INFO("%d\t ", (i + 1));
        ++apNum;
        scanned_AP_info = (rtw_scan_result_t *)(scan_buf + i * sizeof(rtw_scan_result_t));
        scanned_AP_info->SSID.val[scanned_AP_info->SSID.len] = 0; /* Ensure the SSID is null terminated */
        if(i < MAX_NUM_OF_AP_SCANNED)
            memcpy(&matter_userdata[i], scanned_AP_info, sizeof(rtw_scan_result_t));
        print_matter_scan_result(scanned_AP_info);
    }
    rtw_mfree((u8 *)scan_buf, 0);

    if (scanned_AP_num != 0) /* scanned no AP */
    {
        if (chip_connmgr_callback_func && chip_connmgr_callback_data)
        {
            // inform matter
            chip_connmgr_callback_func(chip_connmgr_callback_data);
        }
        else
        {
            printf("chip_connmgr_callback_func is NULL\r\n");
            apNum = 0;
            return RTW_ERROR;
        }
    }
    else
    {
        return RTW_ERROR;
    }

    return RTW_SUCCESS;
}

static rtw_result_t matter_scan_with_ssid_result_handler(unsigned int scanned_AP_num, void *user_data)
{
    /* To avoid gcc warnings */
    (void) user_data;

    rtw_scan_result_t *scanned_AP_info;
    char *scan_buf = NULL;
    unsigned int i = 0;

    scan_buf = (char *)rtw_zmalloc(scanned_AP_num * sizeof(rtw_scan_result_t));
    if (scan_buf == NULL)
    {
        return RTW_ERROR;
    }

    if (wifi_get_scan_records(&scanned_AP_num, scan_buf) < 0)
    {
        rtw_mfree((u8 *)scan_buf, 0);
        return RTW_ERROR;
    }

    for (i = 0; i < scanned_AP_num; i++) {
        RTW_API_INFO("%d\t ", (i + 1));
        scanned_AP_info = (rtw_scan_result_t *)(scan_buf + i * sizeof(rtw_scan_result_t));
        scanned_AP_info->SSID.val[scanned_AP_info->SSID.len] = 0; /* Ensure the SSID is null terminated */

        if (strcmp(matter_ssid, (char *)scanned_AP_info->SSID.val) == 0)
        {
            ++apNum;
            memcpy(&matter_userdata[apNum-1], scanned_AP_info, sizeof(rtw_scan_result_t));
            print_matter_scan_result(scanned_AP_info);
        }
    }
    rtw_mfree((u8 *)scan_buf, 0);

    if (scanned_AP_num != 0) /* scanned no AP */
    {
        if (chip_connmgr_callback_func && chip_connmgr_callback_data)
        {
            // inform matter
            chip_connmgr_callback_func(chip_connmgr_callback_data);
            if(matter_ssid != NULL)
                vPortFree(matter_ssid);
        }
        else
        {
            printf("chip_connmgr_callback_func is NULL\r\n");
            apNum = 0;
            if(matter_ssid != NULL)
            {
                vPortFree(matter_ssid);
            }
            return RTW_ERROR;
        }
    }
    else
    {
        if(matter_ssid != NULL)
        {
            vPortFree(matter_ssid);
        }
        return RTW_ERROR;
    }

    return RTW_SUCCESS;
}
#endif

void matter_scan_networks(void)
{
    volatile int ret = RTW_SUCCESS;
    apNum = 0; // reset counter at the start of scan
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    if ((ret = wifi_scan_networks(matter_scan_result_handler, matter_userdata)) != RTW_SUCCESS)
    {
        RTW_API_INFO("ERROR: wifi scan failed\n");
    }
#elif defined(CONFIG_PLATFORM_8735B)
    rtw_scan_param_t scan_param = {0};

    scan_param.scan_user_callback = matter_scan_result_handler;
    scan_param.max_ap_record_num = MAX_NUM_OF_AP_SCANNED;
    if ((ret = wifi_scan_networks(&scan_param, 0)) != RTW_SUCCESS)
    {
        RTW_API_INFO("ERROR: wifi scan failed\n\r");
    }
#endif
}

void matter_scan_networks_with_ssid(const unsigned char *ssid, size_t length)
{
    volatile int ret = RTW_SUCCESS;

    apNum = 0; // reset counter at the start of scan

    matter_ssid = (char*) pvPortMalloc(length+1);

    memset(matter_ssid, 0, length+1);
    memcpy(matter_ssid, ssid, length);
    matter_ssid[length] = '\0';
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    if ((ret = wifi_scan_networks(matter_scan_with_ssid_result_handler, matter_userdata)) != RTW_SUCCESS)
    {
        RTW_API_INFO("ERROR: wifi scan failed\n");
    }
#elif defined(CONFIG_PLATFORM_8735B)
    rtw_scan_param_t scan_param = {0};
    scan_param.ssid = (char *)matter_ssid;
    scan_param.scan_user_callback = matter_scan_with_ssid_result_handler;
    if ((ret = wifi_scan_networks(&scan_param, 0)) != RTW_SUCCESS)
    {
        RTW_API_INFO("ERROR: wifi scan failed\n\r");
    }
#endif
}

rtw_scan_result_t *matter_get_scan_results(void)
{
    return matter_userdata;
}

#if defined(CONFIG_AUTO_RECONNECT) && CONFIG_AUTO_RECONNECT
static void matter_wifi_autoreconnect_thread(void *param)
{
    struct matter_wifi_autoreconnect_param *reconnect_param = (struct matter_wifi_autoreconnect_param *) param;
    int ret = RTW_ERROR;

    RTW_API_INFO("\nmatter auto reconnect ...\n");

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
    rtw_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    char empty_bssid[6] = {0}, assoc_by_bssid = 0;
    unsigned char *saved_bssid = rltk_wlan_get_saved_bssid();

    if (memcmp(saved_bssid, empty_bssid, ETH_ALEN))
    {
        assoc_by_bssid = 1;
    }

#if defined(CONFIG_SAE_SUPPORT) && (CONFIG_ENABLE_WPS==1)
    unsigned char is_wpa3_disable=0;
    if ((strncmp(wps_profile_ssid, reconnect_param->ssid, reconnect_param->ssid_len) == 0) &&
        (strncmp(wps_profile_password, reconnect_param->password, reconnect_param->password_len) == 0) &&
        (wext_get_support_wpa3() == 1))
    {
        wext_set_support_wpa3(DISABLE);
        is_wpa3_disable=1;
    }
#endif /* defined(CONFIG_SAE_SUPPORT) && (CONFIG_ENABLE_WPS==1) */

    if (assoc_by_bssid)
    {
        ret = wifi_connect_bssid(saved_bssid, reconnect_param->ssid, reconnect_param->security_type,
                                    reconnect_param->password, ETH_ALEN, reconnect_param->ssid_len, reconnect_param->password_len, reconnect_param->key_id, NULL);
    }
    else
    {
        ret = wifi_connect(reconnect_param->ssid, reconnect_param->security_type, reconnect_param->password,
                            reconnect_param->ssid_len, reconnect_param->password_len, reconnect_param->key_id, NULL);
    }

#if defined(CONFIG_SAE_SUPPORT) && (CONFIG_ENABLE_WPS==1)
    if (is_wpa3_disable)
        wext_set_support_wpa3(ENABLE);
#endif /* defined(CONFIG_SAE_SUPPORT) && (CONFIG_ENABLE_WPS==1) */
#elif defined(CONFIG_PLATFORM_8735B)
    rtw_network_info_t connect_param = {0};

    if (reconnect_param->ssid_len) {
        rtw_memcpy(connect_param.ssid.val, reconnect_param->ssid, reconnect_param->ssid_len);
        connect_param.ssid.len = reconnect_param->ssid_len;
    }
    connect_param.password = (unsigned char *)reconnect_param->password;
    connect_param.password_len = reconnect_param->password_len;
    connect_param.security_type = reconnect_param->security_type;
    connect_param.key_id = reconnect_param->key_id;

    RTW_API_INFO("\n\rauto reconnect ...\n");
    ret = wifi_connect(&connect_param, 1);
#endif

    if (ret == RTW_SUCCESS)
    {
        RTW_API_INFO("wifi connected successfully\n");
    } 
    else
    {
        RTW_API_INFO("ERROR: wifi connection failed with error code %d\n", ret);
    }

    matter_param_indicator = NULL;
    rtw_delete_task(&matter_wifi_autoreconnect_task);
}

void matter_wifi_autoreconnect_hdl(rtw_security_t security_type,
                            char *ssid, int ssid_len,
                            char *password, int password_len,
                            int key_id)
{
    static struct matter_wifi_autoreconnect_param param;
    matter_param_indicator = &param;
    param.security_type = security_type;
    param.ssid = ssid;
    param.ssid_len = ssid_len;
    param.password = password;
    param.password_len = password_len;
    param.key_id = key_id;

    if (matter_wifi_autoreconnect_task.task != NULL)
    {
        dhcp_stop(&xnetif[0]);
        u32 start_tick = rtw_get_current_time();
        while (1)
        {
            rtw_msleep_os(2);
            u32 passing_tick = rtw_get_current_time() - start_tick;
            if (rtw_systime_to_sec(passing_tick) >= 2)
            {
                RTW_API_INFO("Create matter_wifi_autoreconnect_task timeout\n");
                return;
            }

            if (matter_wifi_autoreconnect_task.task == NULL)
            {
                break;
            }
        }
    }

    rtw_create_task(&matter_wifi_autoreconnect_task, (const char *)"matter_wifi_autoreconnect", 512, tskIDLE_PRIORITY + 1, matter_wifi_autoreconnect_thread, &param);
}
#endif /* CONFIG_AUTO_RECONNECT */

void matter_set_autoreconnect(uint8_t mode)
{
    size_t ssidLen = 0;
    unsigned char buf[32];
    const char kWiFiSSIDKeyName[] = "wifi-ssid";

    memset(buf, 0, sizeof(buf));

    //if wifi-ssid exist in NVS, it has been commissioned before, CHIP will do autoreconnection
    s32 ret = getPref_bin_new(kWiFiSSIDKeyName, kWiFiSSIDKeyName, buf, sizeof(buf), &ssidLen);
    if (ret != DCT_SUCCESS)
    {
        if(mode == RTW_AUTORECONNECT_DISABLE)
        {
            p_wlan_autoreconnect_hdl = NULL;
        }
        else
        {
            p_wlan_autoreconnect_hdl = matter_wifi_autoreconnect_hdl;
        }
#if defined(CONFIG_PLATFORM_8710C)
        rltk_wlan_set_autoreconnect(WLAN0_NAME, mode, AUTO_RECONNECT_COUNT, AUTO_RECONNECT_INTERVAL);
#elif defined(CONFIG_PLATFORM_8721D)
        wext_set_autoreconnect(WLAN0_NAME, mode, AUTO_RECONNECT_COUNT, AUTO_RECONNECT_INTERVAL);
#elif defined(CONFIG_PLATFORM_8735B)
        rtw_wx_set_autoreconnect(mode, AUTO_RECONNECT_COUNT, AUTO_RECONNECT_INTERVAL);
#endif
    }
    return;
}

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
static int matter_find_ap_from_scan_buf(char *buf, int buflen, char *target_ssid, void *user_data)
{
    rtw_wifi_setting_t *pwifi = (rtw_wifi_setting_t *)user_data;
    int plen = 0;

    while (plen < buflen)
    {
        u8 len, ssid_len, security_mode;
        char *ssid;

        // len offset = 0
        len = (int)*(buf + plen);
        // check end
        if (len == 0) break;
        // ssid offset = 14
        ssid_len = len - 14;
        ssid = buf + plen + 14 ;
        if ((ssid_len == strlen(target_ssid)) && (!memcmp(ssid, target_ssid, ssid_len)))
        {
            strncpy((char*)pwifi->ssid, target_ssid, 33);
            // channel offset = 13
            pwifi->channel = *(buf + plen + 13);
            // security_mode offset = 11
            security_mode = (u8)*(buf + plen + 11);
            if (security_mode == IW_ENCODE_ALG_NONE)
            {
                pwifi->security_type = RTW_SECURITY_OPEN;
            }
            else if (security_mode == IW_ENCODE_ALG_WEP)
            {
                pwifi->security_type = RTW_SECURITY_WEP_PSK;
            }
            else if (security_mode == IW_ENCODE_ALG_CCMP)
            {
                pwifi->security_type = RTW_SECURITY_WPA2_AES_PSK;
            }
            break;
        }
        plen += len;
    }

    return 0;
}
#elif defined(CONFIG_PLATFORM_8735B)
static int matter_find_ap_from_scan_buf(char *target_ssid, void *user_data, int ap_num)
{
    rtw_wifi_setting_t *pwifi = (rtw_wifi_setting_t *)user_data;
    rtw_scan_result_t *scanned_ap_info;
    int i = 0;
    char *scan_buf = NULL;

    scan_buf = (char *)rtw_zmalloc(ap_num * sizeof(rtw_scan_result_t));
    if (scan_buf == NULL)
    {
        return -1;
    }

    if (wifi_get_scan_records(&ap_num, scan_buf) < 0)
    {
        rtw_mfree((uint8_t *)scan_buf, 0);
        return -1;
    }

    for (i = 0; i < ap_num; i++)
    {
        scanned_ap_info = (rtw_scan_result_t *)(scan_buf + i * sizeof(rtw_scan_result_t));
        if ((scanned_ap_info->SSID.len == strlen(target_ssid)) && (!memcmp(scanned_ap_info->SSID.val, target_ssid, scanned_ap_info->SSID.len)))
        {
            strncpy((char *)pwifi->ssid, target_ssid, sizeof(pwifi->ssid));
            pwifi->channel = scanned_ap_info->channel;
            pwifi->security_type = scanned_ap_info->security;
        }
    }
    rtw_mfree((uint8_t *)scan_buf, 0);
    return 0;
}
#endif

static int matter_get_ap_security_mode(IN char *ssid, OUT rtw_security_t *security_mode, OUT u8 *channel)
{
    rtw_wifi_setting_t wifi;

    memset(&wifi, 0, sizeof(wifi));

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    u32 scan_buflen = 1000;

    if (wifi_scan_networks_with_ssid(matter_find_ap_from_scan_buf, (void*)&wifi, scan_buflen, ssid, strlen(ssid)) != RTW_SUCCESS)
    {
        RTW_API_INFO("ERROR: wifi scan failed!\n");
        return 0;
    }

    if (strcmp((char *)wifi.ssid, ssid) == 0)
    {
        *security_mode = wifi.security_type;
        *channel = wifi.channel;
        return 1;
    }
#elif defined(CONFIG_PLATFORM_8735B)
    rtw_scan_param_t scan_param;
    int scanned_ap_num = 0;

    rtw_memset(&scan_param, 0, sizeof(rtw_scan_param_t));
    scan_param.ssid = ssid;
    scanned_ap_num = wifi_scan_networks(&scan_param, 1);

    if (scanned_ap_num <= 0)
    {
        RTW_API_INFO("ERROR: wifi scan failed!\n");
        return 0;
    }
    else
    {
        matter_find_ap_from_scan_buf(ssid, (void *)&wifi, scanned_ap_num);

        if (strcmp((char*)wifi.ssid, ssid) == 0)
        {
            *security_mode = wifi.security_type;
            *channel = wifi.channel;
            return 1;
        }
    }
#endif
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

    if (strlen((const char *) password) != 0)
    {
        security_type = RTW_SECURITY_WPA_WPA2_MIXED;
    }
    else
    {
        security_type = RTW_SECURITY_OPEN;
    }

    if (security_type == RTW_SECURITY_WPA_WPA2_MIXED)
    {
        while (1)
        {
            if (matter_get_ap_security_mode((char *)ssid, &security_type, &connect_channel))
            {
                break;
            }
            security_retry_count++;
            if (security_retry_count >= 3)
            {
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

    matter_set_autoreconnect(1);

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    err = wifi_connect(ssid, security_type, password, strlen(ssid), strlen(password), key_id, NULL);
    if (err != RTW_SUCCESS)
    {
        RTW_API_INFO("ERROR: wifi Connect failed\n");
    }
#elif defined(CONFIG_PLATFORM_8735B)
    rtw_network_info_t wifi = {0};

    wifi.ssid.len = strlen((char const*)ssid);
    memcpy(wifi.ssid.val, ssid, wifi.ssid.len);
    wifi.password_len = strlen((char const*)password);
    wifi.password = (unsigned char *)password;
    wifi.security_type = security_type;
    wifi.key_id = key_id; // WEP key ID missed in BT Config, default WEP key ID 0
    err = wifi_connect(&wifi, 1);
    if (err != RTW_SUCCESS)
    {
        RTW_API_INFO("ERROR: wifi Connect failed\n");
    }
#endif
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
    if (wifi_mode == mode)
    {
        return 0;
    }

    return wifi_set_mode(mode);
}

int matter_wifi_is_connected_to_ap(void)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return wifi_is_connected_to_ap();
#elif defined(CONFIG_PLATFORM_8735B)
    if (wifi_get_join_status() == RTW_JOINSTATUS_ASSOCIATING ||
        wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) {
        return RTW_SUCCESS;
    } else {
        return wifi_is_connected_to_ap();
    }
#endif
 
}

int matter_wifi_is_open_security(void)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    if (wifi_get_sta_security_type() == RTW_SECURITY_OPEN)
    {
        return 1;
    }
#elif defined(CONFIG_PLATFORM_8735B)
    rtw_wifi_setting_t wifi_setting = {0};
    wifi_get_setting(WLAN0_IDX, &wifi_setting);

    if(wifi_setting.security_type == RTW_SECURITY_OPEN)
    {
        return 1;
    }
#endif
    return 0;
}

int matter_wifi_is_ready_to_transceive(rtw_interface_t interface)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return wifi_is_ready_to_transceive(interface);
#elif defined(CONFIG_PLATFORM_8735B)
    switch (interface)
    {
    case RTW_STA_INTERFACE:
    {
        uint8_t *ip = LwIP_GetIP(0);
        if (ip == NULL)
        {
            printf("IP address not assigned yet.\n");
            return RTW_ERROR;
        }

        if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0)
        {
            printf("IP address is 0.0.0.0\n");
            return RTW_ERROR;
        }
        else
        {
            printf("IP address is: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
            return RTW_SUCCESS;
        }
    }
    default:
        return RTW_ERROR;
    }
#endif
}

int matter_wifi_is_up(rtw_interface_t interface)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return wifi_is_up(interface);
#elif defined(CONFIG_PLATFORM_8735B)
    switch (interface)
    {
    case RTW_STA_INTERFACE:
        return rltk_wlan_running(WLAN0_IDX);
    default:
        return 0;
    }
#endif
}

int matter_wifi_get_ap_bssid(unsigned char *bssid)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return wifi_get_ap_bssid(bssid);
#elif defined(CONFIG_PLATFORM_8735B)
    return rtw_wx_get_wap(WLAN0_IDX, bssid);
#endif
}

int matter_wifi_get_last_error(void)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return wifi_get_last_error();
#elif defined(CONFIG_PLATFORM_8735B)
    int reason_code = 0;
    wifi_get_disconn_reason_code((unsigned short *)&reason_code);
    return reason_code;
#endif
}

int matter_wifi_get_mac_address(char *mac)
{
    return wifi_get_mac_address(mac);
}

int matter_wifi_get_network_mode(rtw_network_mode_t *pmode)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return wifi_get_network_mode(pmode);
#elif defined(CONFIG_PLATFORM_8735B)
    *pmode = RTW_NETWORK_BGN;
    return RTW_SUCCESS;
#endif
}

int matter_wifi_get_rssi(int *prssi)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return wifi_get_rssi(prssi);
#elif defined(CONFIG_PLATFORM_8735B)
    rtw_phy_statistics_t phy_statistics;
    int ret = -1;
    ret = wifi_fetch_phy_statistic(&phy_statistics);
    if (ret != -1)
    {
        *prssi = phy_statistics.rssi;
    }
    return ret;
#endif
}

int matter_wifi_get_security_type(uint8_t wlan_idx, uint16_t *alg, uint8_t *key_idx, uint8_t *passphrase)
{
    int ret = RTW_SUCCESS;

    switch (wlan_idx)
    {
    case WLAN0_IDX:
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
        if (wext_get_enc_ext(WLAN0_NAME, alg, key_idx, passphrase) < 0)
#elif defined(CONFIG_PLATFORM_8735B)
        if (rtw_wx_get_enc_ext(WLAN0_IDX, alg, key_idx, passphrase) < 0)
#endif
        {
            ret = RTW_ERROR;
        }
        break;
    case WLAN1_IDX:
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
        if (wext_get_enc_ext(WLAN1_NAME, alg, key_idx, passphrase) < 0)
#elif defined(CONFIG_PLATFORM_8735B)
        if (rtw_wx_get_enc_ext(WLAN1_IDX, alg, key_idx, passphrase) < 0)
#endif
        {
            ret = RTW_ERROR;
        }
        break;
    default:
        RTW_API_INFO("ERROR: Invalid index\n");
        ret = RTW_ERROR;
        break;
    }

    return ret;
}

int matter_wifi_get_setting(unsigned char wlan_idx, rtw_wifi_setting_t *psetting)
{
    int ret = RTW_SUCCESS;

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    switch (wlan_idx)
    {
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
#else
    return wifi_get_setting(wlan_idx, psetting);
#endif
    return ret;
}

int matter_wifi_get_wifi_channel_number(uint8_t wlan_idx, uint8_t *ch)
{
    int ret = RTW_SUCCESS;

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    switch (wlan_idx)
    {
    case WLAN0_IDX:
        if (wext_get_channel(WLAN0_NAME, ch) < 0)
        {
            ret = RTW_ERROR;
        }
        break;
    case WLAN1_IDX:
        if (wext_get_channel(WLAN1_NAME, ch) < 0)
        {
            ret = RTW_ERROR;
        }
        break;
    default:
        RTW_API_INFO("ERROR: Invalid index\n");
        ret = RTW_ERROR;
        break;
    }
#elif defined(CONFIG_PLATFORM_8735B)
    if (rtw_wx_get_freq(wlan_idx, ch) < 0)

    {
        ret = RTW_ERROR;
    }
#endif
    return ret;
}

int matter_get_sta_wifi_info(rtw_wifi_setting_t *pSetting)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return wifi_get_setting((const char *)WLAN0_NAME, pSetting);
#elif defined(CONFIG_PLATFORM_8735B)
    return wifi_get_setting(WLAN0_IDX, pSetting);
#endif
}

void matter_wifi_reg_event_handler(matter_wifi_event event_cmds, rtw_event_handler_t handler_func, void *handler_user_data)
{
    wifi_reg_event_handler(event_cmds, handler_func, handler_user_data);
}

#ifdef __cplusplus
}
#endif
