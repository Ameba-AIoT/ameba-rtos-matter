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

#ifndef _RTK_MATTER_WIFIS_H_
#define _RTK_MATTER_WIFIS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <wifi_conf.h>
#include <lwip_netconf.h>
#if defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)
#include <wifi_ind.h>
#endif

/******************************************************
 *               WiFi Modes
 ******************************************************/
#define RTW_MODE_STA_AP RTW_MODE_STA

/******************************************************
 *               WiFi Security
 ******************************************************/
#define RTW_SECURITY_WPA_WPA2_MIXED    RTW_SECURITY_WPA_WPA2_MIXED_PSK

#define MATTER_WIFI_VERSION_11B       0x01
#define MATTER_WIFI_VERSION_11G       0x02
#define MATTER_WIFI_VERSION_11A       0x04
#define MATTER_WIFI_VERSION_11N       0x18  // 0x8: 2.4G, 0x10: 5Gs
#define MATTER_WIFI_VERSION_11AC      0x40
#define MATTER_WIFI_VERSION_11AX      0x80
#define MATTER_WIFI_VERSION_11AH      0x100

/******************************************************
 *               WiFi Connection Status
 ******************************************************/
#define JOIN_HANDSHAKE_DONE            (uint32_t)(1 << 7)

#define IW_ENCODE_ALG_NONE             0
#define IW_ENCODE_ALG_WEP              1
#define IW_ENCODE_ALG_TKIP             2
#define IW_ENCODE_ALG_CCMP             3
#define IW_ENCODE_ALG_PMK              4
#define IW_ENCODE_ALG_AES_CMAC         5

/******************************************************
 *         WiFi Network Mode (BGN)
 ******************************************************/
typedef int rtw_network_mode_t;

/******************************************************
 *               WiFi Config
 ******************************************************/
typedef struct rtw_wifi_config {
    unsigned int     boot_mode;
    unsigned char    ssid[32];
    unsigned char    ssid_len;
    unsigned char    security_type;
    unsigned char    password[RTW_MAX_PSK_LEN + 1];
    unsigned char    password_len;
    unsigned char    channel;
} rtw_wifi_config_t;

/******************************************************
 *               WiFi Connect Error
 ******************************************************/

enum rtw_connect_error_flag_t {
    RTW_NO_ERROR = 0,           /**< no error */
    RTW_NONE_NETWORK,           /**< none network */
    RTW_WRONG_PASSWORD,         /**< wrong password */
    RTW_4WAY_HANDSHAKE_TIMEOUT, /**< 4 way handshake timeout*/
    RTW_CONNECT_FAIL,           /**< connect fail*/
    RTW_DHCP_FAIL,              /**< dhcp fail*/
    RTW_UNKNOWN,                /**< unknown*/
};

typedef enum {
    MATTER_WIFI_EVENT_FOURWAY_HANDSHAKE_DONE = 0,
    MATTER_WIFI_EVENT_CONNECT                = 0,
    // both MATTER_WIFI_EVENT_FOURWAY_HANDSHAKE_DONE and MATTER_WIFI_EVENT_CONNECT
    // are registered to the same handler, so it does not need to report the event twice
    // both matter_wifi_event are declared because both are used in the connectedhomeip SDK
    MATTER_WIFI_EVENT_DISCONNECT,
    MATTER_WIFI_EVENT_DHCP6_DONE,
    MATTER_WIFI_EVENT_MAX,
} matter_wifi_event;

#if defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1)

/******************************************************
 *         WiFi Modes
 ******************************************************/
typedef uint8_t rtw_mode_t;

/******************************************************
 *         WiFi Interface
 ******************************************************/
#define WLAN0_IDX   STA_WLAN_INDEX
#define WLAN1_IDX   SOFTAP_WLAN_INDEX

/******************************************************
 *         WiFi Result
 ******************************************************/
typedef int rtw_result_t;

/******************************************************
 *         WiFi Scan Result
 ******************************************************/
#pragma pack(1) /*scan related structs are 1 byte alignment for some issues long long ago*/
struct matter_scan_result {
    struct rtw_ssid    SSID;             /**< SSID of the AP. */
    struct rtw_mac     BSSID;            /**< BSSID (MAC address) of the AP. */
    s16                signal_strength;  /**< Receive Signal Strength Indication (RSSI) in dBm: <-90 Very poor, >-30 Excellent. */
    u8                 bss_type;         /**< BSS type. Common value: @ref RTW_BSS_TYPE_INFRASTRUCTURE.*/
    u32                security;         /**< Security type of the AP: @ref RTW_SECURITY_OPEN, @ref RTW_SECURITY_WEP_PSK, etc. */
    u8                 wps_type;         /**< WPS types supported by the AP: @ref RTW_WPS_TYPE_DEFAULT, @ref RTW_WPS_TYPE_USER_SPECIFIED, etc. */
    u32                channel;          /**< Radio channel where the AP beacon was detected.*/
    u8                 band;             /**< Frequency band used by the AP: @ref RTW_BAND_ON_5G, @ref RTW_BAND_ON_24G. */

    /** The wireless spectrum management regulations followed by the AP. Coded according to ISO 3166 standard. \n
     *  Reference: ameba_wifi_country_code_table_usrcfg.c for specific values. \n
     *  Example: For China, country_code[0] = 'C', country_code[1] = 'N'. */
    u8                 country_code[2];
    u8                 wireless_mode;    /**< Wireless mode: @ref RTW_80211_B, @ref RTW_80211_A, etc.*/
    u8                 rom_rsvd[3];
};
typedef struct matter_scan_result rtw_scan_result_t;
#pragma pack()

/******************************************************
 *         WiFi Security
 ******************************************************/
typedef uint32_t rtw_security_t;

/******************************************************
 *         WiFi Settings
 ******************************************************/
typedef struct rtw_wifi_setting rtw_wifi_setting_t;

/******************************************************
 *         WiFi Event Handler
 ******************************************************/
typedef void (*rtw_event_handler_t)(char *buf, int buf_len, int flags, void *handler_user_data);

/******************************************************
 *         WiFi Mac Address
 ******************************************************/
typedef struct rtw_mac rtw_mac_t;

/******************************************************
 *         WiFi Network Info
 ******************************************************/
typedef struct rtw_network_info rtw_network_info_t;

/******************************************************
 *         WiFi Scan Parameter
 ******************************************************/
typedef struct rtw_scan_param rtw_scan_param_t;

/******************************************************
 *         WiFi Auto Reconnect
 ******************************************************/
#define wifi_config_autoreconnect wifi_set_autoreconnect

/******************************************************
 *         WiFi is connected to AP
 ******************************************************/
#define wifi_is_connected_to_ap matter_wifi_is_connected_to_ap

#endif // CONFIG_AMEBARTOS_XXX

/******************************************************
 *               WiFi Interface
 ******************************************************/

typedef u8 rtw_interface_t;
#define RTW_STA_INTERFACE WLAN0_IDX
#define RTW_AP_INTERFACE WLAN1_IDX

/******************************************************
 *               WiFi Structure
 ******************************************************/
#if defined(CONFIG_AUTO_RECONNECT) && CONFIG_AUTO_RECONNECT
struct matter_wifi_autoreconnect_param {
    rtw_security_t security_type;
    char *ssid;
    int ssid_len;
    char *password;
    int password_len;
    int key_id;
};
#endif /* CONFIG_AUTO_RECONNECT */

/******************************************************
 *               Other Variables
 ******************************************************/
extern u32 apNum;
extern uint32_t rtw_join_status;
extern rtw_mode_t wifi_mode;

/******************************************************
 * Undefine WiFi Macro to avoid conflict when compiling Matter Project
 ******************************************************/
#if defined(CHIP_PROJECT) && CHIP_PROJECT
#ifdef GetPriority
#undef GetPriority
#endif
#endif // CHIP_PROJECT

/******************************************************
 *               Matter WiFi Functions
 ******************************************************/

/**
 * @brief  Sets a callback function and data for handling WiFi manager events in a Matter device.
 * @param[in]  p:     Pointer to the callback function that handles WiFi manager events.
 * @param[in]  data:  Pointer to the data structure or context information needed by the callback function.
 */
typedef int (*chip_connmgr_callback)(void *object);
void chip_connmgr_set_callback_func(chip_connmgr_callback p, void *data);

/**
 * @brief  Initialize a WiFi scan to search for all 802.11 networks.
 */
void matter_wifi_scan_networks(void);

/**
 * @brief  Initialize a WiFi scan to search for specific 802.11 networks using SSID.
 * @param[in]  ssid:   The targeted SSID to scan.
 * @param[in]  length: The length of the SSID.
 */
void matter_wifi_scan_networks_with_ssid(const unsigned char *ssid, size_t length);

/**
 * @brief  The results of WiFi scan.
 * @return  A pointer to the scan results, containing information about the found networks.
 */
rtw_scan_result_t *matter_get_scan_results(void);

/**
 * @brief  Handle WiFi auto-reconnect.
 * @param[in]  param  Dummy parameter.
 */
void matter_wifi_reconn_task_hdl(void *param);

/**
 * @brief  Set the auto-reconnect mode for WiFi.
 * @param[in]  mode  The auto-reconnect mode.
                    0: Disable autoreconnect.
                    1: Set finite times for autoreconnect.
                    2: Set infinite times for autoreconnect.
 */
void matter_wifi_set_autoreconnect(uint8_t mode);

/**
 * @brief  Connect to a WiFi network.
 * @param[in]  ssid            The SSID of the WiFi network to connect to.
 * @param[in]  security_type   The security type of the WiFi network.
 * @param[in]  password        The password for the WiFi network.
 * @param[in]  ssid_len        The length of the SSID.
 * @param[in]  password_len    The length of the password.
 * @param[in]  key_id          The key ID used for the WiFi network.
 * @param[in]  semaphore       A pointer to a semaphore for synchronization.
 * @return  RTW_SUCCESS on success, RTW_ERROR otherwise.
 */
int matter_wifi_connect(
    char              *ssid,
    rtw_security_t    security_type,
    char              *password,
    int               ssid_len,
    int               password_len,
    int               key_id,
    void              *semaphore);
/**
 * @brief  Disconnect from the currently connected WiFi network.
 * @return  RTW_SUCCESS on success, RTW_ERROR otherwise.
 */
int matter_wifi_disconnect(void);

/**
 * @brief  Turn on the WiFi module and set the operating mode.
 * @param[in]  mode  The operating mode for the WiFi (e.g., RTW_MODE_STA, RTW_MODE_AP).
 * @return  RTW_SUCCESS on success, RTW_ERROR otherwise.
 */
int matter_wifi_on(rtw_mode_t mode);

/**
 * @brief  Set the operating mode of the WiFi module.
 * @param[in]  mode  The operating mode for the WiFi (e.g., RTW_MODE_STA, RTW_MODE_AP).
 * @return  0 on success, or an error code on failure.
 */
int matter_wifi_set_mode(rtw_mode_t mode);

/**
 * @brief  Check if the device is connected to an access point.
 * @return  RTW_SUCCESS if connected, RTW_ERROR if not connected.
 */
int matter_wifi_is_connected_to_ap(void);

/**
 * @brief  Check if the WiFi security is open.
 * @return  1 if the security is open, 0 otherwise.
 */
int matter_wifi_is_open_security(void);

/**
 * @brief  Check if the WiFi interface is ready to transceive data.
 * @param[in]  interface: The WiFi interface to check.
 * @return  RTW_SUCCESS if ready to transceive, RTW_ERROR otherwise.
 */
int matter_wifi_is_ready_to_transceive(rtw_interface_t interface);

/**
 * @brief  Check if the WiFi interface is up.
 * @param[in]  interface: The WiFi interface to check.
 * @return  RTW_SUCCESS if the interface is up, RTW_ERROR otherwise.
 */
int matter_wifi_is_up(rtw_interface_t interface);

/**
 * @brief  Check if the wifi mode is station mode.
 * @return  RTW_SUCCESS if wifi_mode is station, RTW_ERROR otherwise.
 */
int matter_wifi_is_station_mode(void);

/**
 * @brief  Get the last WiFi error.
 * @return The last WiFi error code.
 *         - RTW_NO_ERROR
 *         - RTW_NONE_NETWORK
 *         - RTW_CONNECT_FAIL
 *         - RTW_WRONG_PASSWORD
 *         - RTW_4WAY_HANDSHAKE_TIMEOUT
 *         - RTW_DHCP_FAIL
 *         - RTW_AUTH_FAIL
 *         - RTW_ASSOC_FAIL
 *         - RTW_DEAUTH_DEASSOC
 *         - RTW_UNKNOWN
 */
int matter_wifi_get_last_error(void);

/**
 * @brief  Get the MAC address of the device.
 * @param[out]  mac: Pointer to store the MAC address.
 * @return  RTW_SUCCESS on success, RTW_ERROR otherwise.
 */
int matter_wifi_get_mac_address(char *mac);

/**
 * @brief  Get the current network mode.
 * @param[out]  pmode: Pointer to store the network mode.
 * @return  RTW_SUCCESS on success, RTW_ERROR otherwise.
 */
int matter_wifi_sta_get_network_mode(rtw_network_mode_t *pmode);

/**
 * @brief  Get the current WiFi settings of the specified WLAN index.
 * @param[in]  wlan_idx: The WLAN index.
 * @param[out]  psetting: Pointer to store the WiFi settings.
 * @return  Non-zero on success, zero on failure.
 */
int matter_wifi_get_setting(unsigned char wlan_idx, rtw_wifi_setting_t *psetting);

/**
 * @brief  Get the STA (station) WiFi information.
 * @param[out]  pSetting: Pointer to store the STA WiFi settings.
 * @return  Non-zero on success, zero on failure.
 */
int matter_get_sta_wifi_info(rtw_wifi_setting_t *pSetting);

/**
 * @brief  Register a Matter WiFi event handler for a specific Matter WiFi event.
 * @param[in]  event_cmds: The specific Matter WiFi event to register the handler for.
 * @param[in]  handler_func: The function to be called when the event occurs.
 * @param[in]  handler_user_data: User data to be passed to the handler function (optional).
 */
void matter_wifi_reg_event_handler(matter_wifi_event event_cmds, rtw_event_handler_t handler_func, void *handler_user_data);

/**
 * @brief  Indicate a Matter WiFi event to trigger the corresponding handler.
 * @param[in]  event_cmd: The specific WiFi event to be triggered.
 * @param[in]  buf: Buffer to be passed to the handler function (optional).
 * @param[in]  buf_len: Lengrh of buffer to be passed to the handler function (optional).
 * @param[in]  flags: WiFi flags to be passed to the handler function (optional).
 */
int matter_wifi_indication(u32 event_cmd, u8 *buf, s32 buf_len, s32 flags);

/**
 * @brief  Initialize matter WiFis module
 */
void matter_wifi_init(void);

/**
 * @brief  Wait until WiFi module are ready
 */
void matter_wifi_wait(void);

/******************************************************
 * Matter Feature: Wi-Fi Network Diagnostics
 ******************************************************/

/**
 * @brief  Get the BSSID of the connected access point.
 * @param[out]  bssid: Pointer to store the BSSID.
 * @return  RTW_SUCCESS on success, RTW_ERROR otherwise.
 */
int matter_wifi_sta_get_ap_bssid(unsigned char *bssid);

/**
 * @brief  Get the security type of the specified WLAN index.
 * @param[out] wifi_security: Pointer to store the wifi security algorithm.
 * @return  Non-zero on success, zero on failure.
 */
int matter_wifi_sta_get_security_type(uint32_t *wifi_security);

/**
 * @brief  Get the WiFi channel number of the specified WLAN index.
 * @param[out]  ch: Pointer to store the channel number.
 * @return  Non-zero on success, zero on failure.
 */
int matter_wifi_sta_get_channel_number(uint8_t *ch);

/**
 * @brief  Get the RSSI (signal strength) of the connected access point.
 * @param[out]  prssi: Pointer to store the RSSI value.
 * @return  RTW_SUCCESS on success, RTW_ERROR otherwise.
 */
int matter_wifi_sta_get_rssi(int *prssi);

/**
 * @brief  Get the WiFi Version of the connected access point.
 * @param[out]  mode: Pointer to store the WiFi Version value.
 * @return  RTW_SUCCESS on success, RTW_ERROR otherwise.
 */
int matter_wifi_sta_get_wifi_version(uint8_t *mode);

#ifdef __cplusplus
}
#endif

#endif //_RTK_MATTER_WIFIS_H_
