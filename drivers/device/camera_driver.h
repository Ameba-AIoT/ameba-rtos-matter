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

#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>
#include <camera/ameba_camera_device.h>
#include <webrtc/ameba_webrtc_provider_manager.h>

extern "C" {
#ifdef CONFIG_USB_HOST_EN
#include <usbh_uvc_intf.h>
#include <usbh.h>
#endif
#include <lwipconf.h>
#include <lwip_netconf.h>
#include <ringbuffer.h>
}

#ifdef CONFIG_USB_HOST_EN
/* Private defines -----------------------------------------------------------*/
/* Supported formats: USBH_UVC_FORMAT_MJPEG, USBH_UVC_FORMAT_YUV, USBH_UVC_FORMAT_H264
 * Note: Users must verify which formats their specific camera supports and
 * adjust the definition below accordingly. */
#define CONFIG_USBH_UVC_FORMAT_TYPE                USBH_UVC_FORMAT_H264

/* Target resolution and compression ratio.
 * If the specific camera device does not support
 * these values, the host stack will automatically select the closest match.
 * Always check the logs to confirm the actual parameters applied. */
#if ((CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG) || (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_H264))
#define CONFIG_USBH_UVC_WIDTH                      1280
#define CONFIG_USBH_UVC_HEIGHT                     720
#define CONFIG_USBH_UVC_FRAME_RATE                 30
#elif (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_YUV) // Uncompressed YUV format takes a lot of space for the settings above
#define CONFIG_USBH_UVC_WIDTH                      640
#define CONFIG_USBH_UVC_HEIGHT                     480
#define CONFIG_USBH_UVC_FRAME_RATE                 30
#endif

/* Frame buffer size in bytes
 * Size depends on format, resolution, and scene complexity.
 * Please increase this value if an oversize error occurs.
 * ┌────────┬─────────────────────────────┬─────────┬──────────┐
 * │ Format │           Formula           | 640×480 │ 1280×720 │
 * ├────────┼─────────────────────────────┼─────────┼──────────┤
 * │ YUYV   │ W × H × 2                   | 600 KB  │ 1.8 MB   │
 * ├────────┼─────────────────────────────┼─────────┼──────────┤
 * │ MJPEG  │ W × H × 2 x 0.05 (estimate) | ~30 KB  │ ~90 KB   │
 * ├────────┼─────────────────────────────┼─────────┼──────────┤
 * │ H.264  │ W × H × 2 x 0.05 (estimate) | ~30 KB  │ ~90 KB   │
 * └────────┴─────────────────────────────┴─────────┴──────────┘
 * MJPEG and H.264 size only takes 5% of YUV size
 */
#if ((CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG) || (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_H264))
#define CONFIG_USBH_UVC_FRAME_BUF_SIZE             (100 * 1024)
#elif (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_YUV) // Uncompressed YUV format takes a lot of space even after lowering the resolution
#define CONFIG_USBH_UVC_FRAME_BUF_SIZE             (610 * 1024)
#endif

/* Most cameras have a single video stream interface, so use default 0.
 * If the camera supports dual streams, set this to 1.
 * Note: Current protocol stack supports a maximum of 2 video stream interfaces. */
#define CONFIG_USBH_UVC_IF_NUM_0                   0

/* Hot plug / memory leak test */
#define CONFIG_USBH_UVC_HOT_PLUG                   1

/* Check image data validity (0: Disable, 1: Enable) */
#define CONFIG_USBH_UVC_CHECK_MJEPG_DATA           1

/* Number of frames to capture in the loop */
#define CONFIG_USBH_UVC_LOOP                       200

/* Maximum continuous error count before stopping */
#define CONFIG_USBH_UVC_MAX_FAIL_COUNT             5

/* Base Stack Size of the UVC threads */
#define CONFIG_USBH_UVC_BASE_STACK_SIZE              1024
/* Stack Size of the UVC test thread */
#define CONFIG_USBH_UVC_TEST_THREAD_STACK_SIZE       CONFIG_USBH_UVC_BASE_STACK_SIZE * 2
/* Stack Size of the USB hotplug detection thread */
#define CONFIG_USBH_UVC_HOTPLUG_THREAD_STACK_SIZE    CONFIG_USBH_UVC_BASE_STACK_SIZE * 2
/* Stack Size of the USB Matter thread */
#define CONFIG_USBH_UVC_MATTER_THREAD_STACK_SIZE     CONFIG_USBH_UVC_BASE_STACK_SIZE * 5

/* Base Priority of the UVC threads */
#define CONFIG_USBH_UVC_BASE_PRIORITY              tskIDLE_PRIORITY + 2
/* Priority of the main UVC thread */
#define CONFIG_USBH_UVC_MAIN_THREAD_PRIORITY       CONFIG_USBH_UVC_BASE_PRIORITY + 1
/* Priority of the UVC test thread */
#define CONFIG_USBH_UVC_TEST_THREAD_PRIORITY       CONFIG_USBH_UVC_BASE_PRIORITY + 2
/* Priority of the USB hotplug detection thread */
#define CONFIG_USBH_UVC_HOTPLUG_THREAD_PRIORITY    CONFIG_USBH_UVC_BASE_PRIORITY + 4
/* Stack Size of the USB Matter thread */
#define CONFIG_USBH_UVC_MATTER_THREAD_PRIORITY     CONFIG_USBH_UVC_BASE_PRIORITY

#if USBH_UVC_USE_HW
/* HW UVC IRQ PRIORITY*/
#define CONFIG_USBH_UVC_HW_IRQ_PRIORITY            INT_PRI_LOWEST
#endif

#define USBH_UVC_MATTER_WRITE_SIZE        (4 * 1024)
#define USBH_UVC_MATTER_VIDEO_SIZE        (2 * 1024 * 1024)
#define USBH_UVC_MATTER_TAG               "MATTER"
#endif

#define MATTER_INVALID_SESSION_ID        0xFFFF

/* Buffer size for the generated dummy H.264 1280x720 keyframe.
 * The frame is ~3.6 KB (SPS + PPS + one 80x45-macroblock I-slice). Keep this
 * tight: the buffer is allocated permanently in Init(), and oversizing it
 * starves the heap that WebRTC/CASE negotiation needs (malloc fails otherwise). */
#define DUMMY_H264_BUF_SIZE              (5 * 1024)

/* Stack size (bytes) for the dummy streaming task. It performs the same deep
 * WebRTC SendVideo() calls (plus CHIP logging) as the USB UVC Matter thread, so
 * it needs a comparably large stack. 1 KB overflows on the first log call. */
#define DUMMY_STREAMING_THREAD_STACK_SIZE (5 * 1024)

using namespace chip::app::Clusters::WebRTCTransportProvider;

class MatterCamera
{
public:
    /* USBH UVC */
    void Init(void);
    void deInit(void);
    void EnableMatterVideoStream(uint16_t streamId);
    void DisableMatterVideoStream(void);
    void RegisterWebRtcTransport(WebRTCProviderManager *mWebRTCProviderManager, uint16_t sessionId);
    void DeregisterWebRtcTransport(void);
    static MatterCamera *GetInstance(void);

private:
#ifdef CONFIG_USB_HOST_EN
    /* USBH UVC */
    void UsbhUvcMainThread(void *param);
#if CONFIG_USBH_UVC_HOT_PLUG
    void UvcHotplugThread(void *param);
#endif
    void UvcTestThread(void *param);
    void UvcMatterThread(void *param);
    int UvcMatterStart(void);
    void UsbhUvcImgPrepare(usbh_uvc_frame_t *frame);
    void UvcCalculateTp(u32 loop);

    int UvcInit(void);
    int UvcDeinit(void);
    int UvcAttach(void);
    int UvcDetach(void);
    int UvcSetup(void);
    int UvcSetparam(void);

    static void UsbhUvcMainThreadWrapper(void *param);
#if CONFIG_USBH_UVC_HOT_PLUG
    static void UvcHotplugThreadWrapper(void *param);
#endif
    static void UvcTestThreadWrapper(void *param);
    static void UvcMatterThreadWrapper(void *param);

    static int UvcInitWrapper(void);
    static int UvcDeinitWrapper(void);
    static int UvcAttachWrapper(void);
    static int UvcDetachWrapper(void);
    static int UvcSetupWrapper(void);
    static int UvcSetparamWrapper(void);

    rtos_sema_t mUvcAttachSema;
    rtos_sema_t mUvcDetachSema;
    rtos_sema_t mUvcStartSema;
    rtos_sema_t mUvcSetparamSema;
    rtos_mutex_t mUvcBufMutex = NULL;
    usbh_uvc_s_ctx_t mUvcSCtx;
    rtos_task_t mUvcTask;

    uint32_t mRxTotalH;
    uint32_t mRxTotalL;
    uint32_t mRxStart;

    rtos_sema_t mUvcMatterSaveImgSema = NULL;
    int mUvcMatterIsInit = 0;
    int mUvcBufSize = 0;
    RingBuffer *mUvcRb;

    u8 *mUvcBuf = nullptr;

    usbh_config_t  *mUsbhConfig   = NULL;
    usbh_uvc_ctx_t *mUvcConfig    = NULL;
    usbh_uvc_cb_t  *mUvcCallBacks = NULL;
#else
    void StartDummyStreaming(void *param);
    static void StartDummyStreamingWrapper(void *param);
    void DummyStreaming(void *param);
    void GenerateDummyH264Frame(void);

    rtos_mutex_t mDummyBufMutex = NULL;
    rtos_task_t mDummyTask = NULL;
    int mDummyBufSize = 0;
    u8 *mDummyBuf = nullptr;
#endif
    static MatterCamera *instance;

    bool mStreamEnabled = false;
    uint16_t mCurrentSessionId        = MATTER_INVALID_SESSION_ID;
    uint16_t mCurrentVideoStreamId    = MATTER_INVALID_SESSION_ID;
    WebrtcTransport *mWebrtcTransport = nullptr;

};
