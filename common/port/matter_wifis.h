/********************************************************************************
 * @file    matter_wifi.h
 * @author
 * @version
 * @brief   WiFi API to support Matter protocol.
 ********************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
********************************************************************************/

#ifndef _RTK_MATTER_WIFIS_H_
#define _RTK_MATTER_WIFIS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <wifi_conf.h>
#include <lwip_netconf.h>

/******************************************************
 *               Other Variables
 ******************************************************/
extern u32 apNum;
extern uint32_t rtw_join_status;
extern rtw_mode_t wifi_mode;

/******************************************************
 *               WiFi Security
 ******************************************************/
#define RTW_SECURITY_WPA_WPA2_MIXED    RTW_SECURITY_WPA_WPA2_MIXED_PSK

/******************************************************
 *               WiFi Connection Status
 ******************************************************/
#define JOIN_HANDSHAKE_DONE            (uint32_t)(1 << 7)

#if defined(CONFIG_PLATFORM_8721D)
#define IW_ENCODE_ALG_NONE             0
#define IW_ENCODE_ALG_WEP              1
#define IW_ENCODE_ALG_TKIP             2
#define IW_ENCODE_ALG_CCMP             3
#define IW_ENCODE_ALG_PMK              4
#define IW_ENCODE_ALG_AES_CMAC         5
#endif

/******************************************************
 *               Matter WiFi Event
 ******************************************************/
typedef enum{
    MATTER_WIFI_EVENT_CONNECT                = WIFI_EVENT_CONNECT,
    MATTER_WIFI_EVENT_FOURWAY_HANDSHAKE_DONE = WIFI_EVENT_FOURWAY_HANDSHAKE_DONE,
    MATTER_WIFI_EVENT_DISCONNECT             = WIFI_EVENT_DISCONNECT,
    MATTER_WIFI_EVENT_DHCP6_DONE             = WIFI_EVENT_DHCP6_DONE,
} matter_wifi_event;

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
void matter_scan_networks(void);

/**
 * @brief  Initialize a WiFi scan to search for specific 802.11 networks using SSID.
 * @param[in]  ssid:   The targeted SSID to scan.
 * @param[in]  length: The length of the SSID.
 */
void matter_scan_networks_with_ssid(const unsigned char *ssid, size_t length);

/**
 * @brief  The results of WiFi scan.
 * @return  A pointer to the scan results, containing information about the found networks.
 */
rtw_scan_result_t *matter_get_scan_results(void);

/**
 * @brief  Handle WiFi auto-reconnect.
 * @param[in]  security_type  The security type of the WiFi network.
 * @param[in]  ssid           The SSID of the WiFi network.
 * @param[in]  ssid_len       The length of the SSID.
 * @param[in]  password       The password for the WiFi network.
 * @param[in]  password_len   The length of the password.
 * @param[in]  key_id         The key ID used for the WiFi network.
 */
void matter_wifi_autoreconnect_hdl(
    rtw_security_t security_type,
    char *ssid, int ssid_len,
    char *password, int password_len,
    int key_id);

/**
 * @brief  Set the auto-reconnect mode for WiFi.
 * @param[in]  mode  The auto-reconnect mode.
                    0: Disable autoreconnect.
                    1: Set finite times for autoreconnect.
                    2: Set infinite times for autoreconnect.
 */
void matter_set_autoreconnect(uint8_t mode);

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
 * @brief  Get the BSSID of the connected access point.
 * @param[out]  bssid: Pointer to store the BSSID.
 * @return  RTW_SUCCESS on success, RTW_ERROR otherwise.
 */
int matter_wifi_get_ap_bssid(unsigned char *bssid);

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
int matter_wifi_get_network_mode(rtw_network_mode_t *pmode);

/**
 * @brief  Get the RSSI (signal strength) of the connected access point.
 * @param[out]  prssi: Pointer to store the RSSI value.
 * @return  RTW_SUCCESS on success, RTW_ERROR otherwise.
 */
int matter_wifi_get_rssi(int *prssi);

/**
 * @brief  Get the security type of the specified WLAN index.
 * @param[in]  wlan_idx: The WLAN index.
 * @param[out]  wifi_security: Pointer to store the wifi security algorithm.
 * @return  Non-zero on success, zero on failure.
 */
int matter_wifi_get_security_type(uint8_t wlan_idx, uint32_t *wifi_security);

/**
 * @brief  Get the current WiFi settings of the specified WLAN index.
 * @param[in]  wlan_idx: The WLAN index.
 * @param[out]  psetting: Pointer to store the WiFi settings.
 * @return  Non-zero on success, zero on failure.
 */
int matter_wifi_get_setting(unsigned char wlan_idx, rtw_wifi_setting_t *psetting);

/**
 * @brief  Get the WiFi channel number of the specified WLAN index.
 * @param[in]  wlan_idx: The WLAN index.
 * @param[out]  ch: Pointer to store the channel number.
 * @return  Non-zero on success, zero on failure.
 */
int matter_wifi_get_wifi_channel_number(uint8_t wlan_idx, uint8_t *ch);

/**
 * @brief  Get the STA (station) WiFi information.
 * @param[out]  pSetting: Pointer to store the STA WiFi settings.
 * @return  Non-zero on success, zero on failure.
 */
int matter_get_sta_wifi_info(rtw_wifi_setting_t *pSetting);

/**
 * @brief  Register an event handler for a specific WiFi event.
 * @param[in]  event_cmds: The specific WiFi event to register the handler for.
 * @param[in]  handler_func: The function to be called when the event occurs.
 * @param[in]  handler_user_data: User data to be passed to the handler function (optional).
 */
void matter_wifi_reg_event_handler(matter_wifi_event event_cmds, rtw_event_handler_t handler_func, void *handler_user_data);

#ifdef __cplusplus
}
#endif

#endif //_RTK_MATTER_WIFIS_H_
