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
#include <algorithm>

#include <camera_driver.h>
#include <matter_timers.h>

#include <support/logging/CHIPLogging.h>

MatterCamera *MatterCamera::instance = nullptr;

// Public

void MatterCamera::Init(void)
{
    int status;
    rtos_task_t task;

    instance = this;

#ifdef CONFIG_USB_HOST_EN
    mUsbhConfig   = (usbh_config_t *)  rtos_mem_zmalloc(sizeof(usbh_config_t));
    mUvcConfig    = (usbh_uvc_ctx_t *) rtos_mem_zmalloc(sizeof(usbh_uvc_ctx_t));
    mUvcCallBacks = (usbh_uvc_cb_t *)  rtos_mem_zmalloc(sizeof(usbh_uvc_cb_t));

    // Init USB Host Configuration
    mUsbhConfig->speed                = USB_SPEED_HIGH;
    mUsbhConfig->ext_intr_enable      = USBH_SOF_INTR;
    mUsbhConfig->isr_priority         = INT_PRI_MIDDLE;
    mUsbhConfig->main_task_stack_size = CONFIG_USBH_UVC_MAIN_TASK_STACK_SIZE;
    mUsbhConfig->main_task_priority   = CONFIG_USBH_UVC_MAIN_THREAD_PRIORITY;
    mUsbhConfig->tick_source          = USBH_SOF_TICK;
#if defined (CONFIG_AMEBAGREEN2)
    /*FIFO total depth is 1024, reserve 12 for DMA addr*/
    mUsbhConfig->rx_fifo_depth      = 500;
    mUsbhConfig->nptx_fifo_depth    = 256;
    mUsbhConfig->ptx_fifo_depth     = 256;
#endif

    // Init UVC Context
    mUvcConfig->frame_buf_size = CONFIG_USBH_UVC_FRAME_BUF_SIZE;
#if USBH_UVC_USE_HW
    mUvcConfig->hw_isr_pri     = CONFIG_USBH_UVC_HW_IRQ_PRIORITY;
#endif

    // Init UVC Callbacks
    mUvcCallBacks->init      = UvcInitWrapper;
    mUvcCallBacks->deinit    = UvcDeinitWrapper;
    mUvcCallBacks->attach    = UvcAttachWrapper;
    mUvcCallBacks->detach    = UvcDetachWrapper;
    mUvcCallBacks->setup     = UvcSetupWrapper;
    mUvcCallBacks->set_param = UvcSetparamWrapper;

    status = rtos_task_create(&task, "UsbhUvcMainThread", UsbhUvcMainThreadWrapper, NULL, 2048U, 1U);
    if (status != RTK_SUCCESS) {
        ChipLogError(DeviceLayer, "Create thread fail");
    }
#else
    //TODO: If camera is not using USBH, please initial it by other means
    rtos_mutex_create(&mDummyBufMutex);
    UNUSED(status);
    UNUSED(task);
#endif
}

void MatterCamera::deInit(void)
{
#ifdef CONFIG_USB_HOST_EN
    vPortFree(mUsbhConfig);
    vPortFree(mUvcConfig);
    vPortFree(mUvcCallBacks);
#else
    if (mDummyTask != NULL) {
        rtos_task_delete(mDummyTask);
        mDummyTask = NULL;
    }
    if (mDummyBuf != nullptr) {
        rtos_mem_free(mDummyBuf);
        mDummyBuf = nullptr;
    }
    rtos_mutex_delete(mDummyBufMutex);
#endif
}

void MatterCamera::EnableMatterVideoStream(uint16_t streamId)
{
    ChipLogProgress(DeviceLayer, "Enabling video stream for streamId(%u)", streamId);
    mCurrentVideoStreamId = streamId;
    mStreamEnabled        = true;
#ifndef CONFIG_USB_HOST_EN
    if (mDummyTask == NULL) {
        int status = rtos_task_create(&mDummyTask, "DummyStreaming", StartDummyStreamingWrapper, NULL,
                                      DUMMY_STREAMING_THREAD_STACK_SIZE, 1U);
        if (status != RTK_SUCCESS) {
            ChipLogError(DeviceLayer, "Create DummyStreaming thread fail");
            mDummyTask = NULL;
        }
    }
#endif
}

void MatterCamera::DisableMatterVideoStream(void)
{
    ChipLogProgress(DeviceLayer, "Disabling video stream for streamId(%u)", mCurrentVideoStreamId);
    mCurrentVideoStreamId = MATTER_INVALID_SESSION_ID;
    mStreamEnabled        = false;
}

void MatterCamera::RegisterWebRtcTransport(WebRTCProviderManager *mWebRTCProviderManager, uint16_t sessionId)
{
    ChipLogProgress(DeviceLayer, "Registering WebRTC transport for sessionId(%u)", sessionId);
    mCurrentSessionId = sessionId;
    mWebrtcTransport  = mWebRTCProviderManager->GetTransport(mCurrentSessionId);
}

void MatterCamera::DeregisterWebRtcTransport(void)
{
    ChipLogProgress(DeviceLayer, "Deregistering WebRTC transport for sessionId(%u)", mCurrentSessionId);
    mCurrentSessionId = MATTER_INVALID_SESSION_ID;
    mWebrtcTransport  = nullptr;
    mStreamEnabled    = false;
}

MatterCamera *MatterCamera::GetInstance(void)
{
    return instance;
}

// Private

#ifdef CONFIG_USB_HOST_EN
void MatterCamera::UsbhUvcMainThread(void *param)
{
    ChipLogProgress(DeviceLayer, "USBH UVC task start");
#if CONFIG_USBH_UVC_HOT_PLUG
    rtos_task_t hotplug_task;
#endif
    int ret = 0;

    UNUSED(param);

    mUvcBuf = (uint8_t *) rtos_mem_zmalloc(CONFIG_USBH_UVC_FRAME_BUF_SIZE);
    mSnapshotBuf = (uint8_t *) rtos_mem_zmalloc(CONFIG_USBH_UVC_SNAPSHOT_FRAME_BUF_SIZE);
    // video stream related
    rtos_sema_create(&mUvcAttachSema, 0U, 1U);
    rtos_sema_create(&mUvcDetachSema, 0U, 1U);
    rtos_sema_create(&mUvcStartSema, 0U, 1U);
    rtos_sema_create(&mUvcSetparamSema, 0U, 1U);
    rtos_mutex_create(&mUvcBufMutex);
    // snapshot related
    rtos_sema_create(&mSnapshotDoneSema, 0U, 1U);
    rtos_mutex_create(&mSnapshotMutex);

    ret = usbh_init(mUsbhConfig, NULL);
    if (ret != HAL_OK) {
        goto free_sema_exit;
    }

    ret = usbh_uvc_init(mUvcConfig, mUvcCallBacks);
    if (ret != HAL_OK) {
        usbh_deinit();
        goto usb_deinit_exit;
    }

#if CONFIG_USBH_UVC_HOT_PLUG
    ret = rtos_task_create(&hotplug_task, "UvcHotplugThread", UvcHotplugThreadWrapper, NULL, CONFIG_USBH_UVC_HOTPLUG_THREAD_STACK_SIZE,
                           CONFIG_USBH_UVC_HOTPLUG_THREAD_PRIORITY);
    if (ret != RTK_SUCCESS) {
        goto usbh_uvc_deinit_exit;
    }
#endif

    while (1) {
        if (rtos_sema_take(mUvcAttachSema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
            if (mUvcTask == NULL) {
                ret = rtos_task_create(&mUvcTask, "UvcTestThread", UvcTestThreadWrapper, NULL,
                                       CONFIG_USBH_UVC_TEST_THREAD_STACK_SIZE, CONFIG_USBH_UVC_TEST_THREAD_PRIORITY);
                if (ret != RTK_SUCCESS) {
                    goto delete_hotplug_task_exit;
                }
            }
        }
    }

    goto example_exit;

delete_hotplug_task_exit:
#if CONFIG_USBH_UVC_HOT_PLUG
    rtos_task_delete(hotplug_task);
#endif

#if CONFIG_USBH_UVC_HOT_PLUG
usbh_uvc_deinit_exit:
#endif
    usbh_uvc_deinit();

usb_deinit_exit:
    usbh_deinit();

free_sema_exit:
    // video stream related
    rtos_mutex_delete(mUvcBufMutex);
    rtos_sema_delete(mUvcAttachSema);
    rtos_sema_delete(mUvcDetachSema);
    rtos_sema_delete(mUvcStartSema);
    rtos_sema_delete(mUvcSetparamSema);
    // snapshots related
    rtos_mutex_delete(mSnapshotMutex);
    rtos_sema_delete(mSnapshotDoneSema);
example_exit:
    rtos_mem_free(mUvcBuf);
    rtos_mem_free(mSnapshotBuf);
    ChipLogProgress(DeviceLayer, "USBH UVC task ends");
    rtos_task_delete(NULL);
}

void MatterCamera::UvcTestThread(void *param)
{
    usbh_uvc_frame_t *buf;
    const char *fmt_name = NULL;
    int ret = 0;
    int img_cnt = 0;
    int fail_cnt = 0;
    u32 len;

    UNUSED(param);

    while (1) {
        if (rtos_sema_take(mUvcStartSema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
            continue;
        }

        mUvcSCtx.fmt_type = CONFIG_USBH_UVC_FORMAT_TYPE;
        mUvcSCtx.width = CONFIG_USBH_UVC_WIDTH;
        mUvcSCtx.height = CONFIG_USBH_UVC_HEIGHT;
        mUvcSCtx.frame_rate = CONFIG_USBH_UVC_FRAME_RATE;
        mUvcSCtx.frame_buf_size = CONFIG_USBH_UVC_FRAME_BUF_SIZE;

        if (mUvcSCtx.fmt_type == USBH_UVC_FORMAT_H264) {
            fmt_name = "H264";
        } else {
            ChipLogError(DeviceLayer, "Unsupport type %d", mUvcSCtx.fmt_type);
            goto exit;
        }

        /* Trigger the UVC parameter setting process */
        ret = usbh_uvc_set_param(&mUvcSCtx, CONFIG_USBH_UVC_IF_NUM_0);
        /* Check if the request itself failed immediately */
        if (ret != RTK_SUCCESS) {
            ChipLogError(DeviceLayer, "Set param req: %d", ret);
            goto exit;
        }

        /* Wait for the semaphore indicating the setting is actually completed */
        if (rtos_sema_take(mUvcSetparamSema, 5000) == RTK_SUCCESS) {
            if (mUvcSetparamStatus != HAL_OK) {
                ChipLogProgress(DeviceLayer, "Set paras err: %s %d*%d@%dfps status=%d\n",
                                fmt_name, mUvcSCtx.width, mUvcSCtx.height, mUvcSCtx.frame_rate, mUvcSetparamStatus);
                goto exit;
            }
            ChipLogProgress(DeviceLayer, "Set paras ok: %s %d*%d@%dfps",
                            fmt_name, mUvcSCtx.width, mUvcSCtx.height, mUvcSCtx.frame_rate);
        } else {
            ChipLogError(DeviceLayer, "Set paras fail: %s %d*%d@%dfps",
                         fmt_name, mUvcSCtx.width, mUvcSCtx.height, mUvcSCtx.frame_rate);
            goto exit;
        }

        ChipLogProgress(DeviceLayer, "Start %s client", USBH_UVC_MATTER_TAG);
        ret = UvcMatterStart();
        if (ret != 0) {
            ChipLogError(DeviceLayer, "Start UVC Matter err");
            goto exit;
        }

        /* H.264 negotiated: ready to serve snapshots even before a live stream. */
        mUvcReady = true;

        /* Park until a live stream starts. H.264 is negotiated but not yet
         * transferring here, so an idle snapshot must not stop/restart it. */
        while (mUvcMatterIsInit == 0) {
            if (mSnapshotRequested) {
                ServiceSnapshotRequest(false /* h264Streaming */);
            } else {
                rtos_time_delay_ms(100);
            }
        }

        img_cnt = 0;
        fail_cnt = 0;

        ChipLogProgress(DeviceLayer, "Stream on");
        ret = usbh_uvc_start(CONFIG_USBH_UVC_IF_NUM_0);

        if (ret) {
            ChipLogProgress(DeviceLayer, "Stream on err");
            goto exit;
        }
        while (mUvcMatterIsInit) {
            /* Serve snapshots here (this thread owns get_frame/stop/start).
             * H.264 is streaming, so it is stopped first and restarted after. */
            if (mSnapshotRequested) {
                ServiceSnapshotRequest(true /* h264Streaming */);
                continue;
            }

            buf = usbh_uvc_get_frame(CONFIG_USBH_UVC_IF_NUM_0);

            if (buf == NULL) {
                fail_cnt++;
                if (fail_cnt >= CONFIG_USBH_UVC_MAX_FAIL_COUNT) {
                    ChipLogError(DeviceLayer, "Stop (fail:%d)", fail_cnt);
                    break;
                }

                rtos_time_delay_ms(1);
                continue;
            }

            len = buf->byteused;

            if (len > CONFIG_USBH_UVC_FRAME_BUF_SIZE) {
                usbh_uvc_put_frame(buf, CONFIG_USBH_UVC_IF_NUM_0);
                ChipLogError(DeviceLayer, "Frame %d overflow %lu > %d", img_cnt, len, CONFIG_USBH_UVC_FRAME_BUF_SIZE);
                return;
            }

            if (len > 0) {
                ChipLogDetail(DeviceLayer, "Captured frame %d, len=%lu", img_cnt, len);
                UsbhUvcImgPrepare(buf);
            }

            usbh_uvc_put_frame(buf, CONFIG_USBH_UVC_IF_NUM_0);

            img_cnt ++;
        }
        UvcCalculateTp(img_cnt);
        usbh_uvc_stop(CONFIG_USBH_UVC_IF_NUM_0);
        ChipLogProgress(DeviceLayer, "Stream off");
    }

exit:
    mUvcReady = false;
    rtos_task_delete(NULL);
    mUvcTask = NULL;
}

/* Negotiate the interface to the given format/resolution, then wait for the
 * completion callback. Returns true on success. */
bool MatterCamera::UvcSetFormat(uint8_t fmtType, uint16_t width, uint16_t height, uint16_t frameRate, uint32_t frameBufSize)
{
    mUvcSCtx.fmt_type       = fmtType;
    mUvcSCtx.width          = width;
    mUvcSCtx.height         = height;
    mUvcSCtx.frame_rate     = frameRate;
    mUvcSCtx.frame_buf_size = frameBufSize;

    /* Drain any stale signal so we observe this request's status. */
    while (rtos_sema_take(mUvcSetparamSema, 0) == RTK_SUCCESS) { /* drain */ }

    if (usbh_uvc_set_param(&mUvcSCtx, CONFIG_USBH_UVC_IF_NUM_0) != RTK_SUCCESS) {
        ChipLogError(DeviceLayer, "Snapshot: set_param req fail (fmt %u)", fmtType);
        return false;
    }
    if (rtos_sema_take(mUvcSetparamSema, 5000) != RTK_SUCCESS || mUvcSetparamStatus != HAL_OK) {
        ChipLogError(DeviceLayer, "Snapshot: set_param fail (fmt %u %ux%u)", fmtType, width, height);
        return false;
    }
    return true;
}

namespace {
/* Strip trailing zero padding some cameras add, then require both the SOI
 * (0xFFD8) and EOI (0xFFD9) markers (matching example_usbh_uvc.c). Returns the
 * trimmed length, or 0 if the frame is not a complete JPEG. */
u32 ValidateAndTrimJpeg(const uint8_t *buf, u32 len)
{
    if (len < 4) {
        return 0;
    }
    while (len > 2 && buf[len - 1] == 0x00) {
        len--;
    }
    if (len < 4 || buf[0] != 0xFF || buf[1] != 0xD8 || buf[len - 2] != 0xFF || buf[len - 1] != 0xD9) {
        return 0;
    }
    return len;
}
} // namespace

void MatterCamera::ServiceSnapshotRequest(bool h264Streaming)
{
    usbh_uvc_frame_t *buf;
    int attempts = 0;

    mSnapshotOk  = false;
    mSnapshotLen = 0;
    memset(mSnapshotBuf, 0, CONFIG_USBH_UVC_SNAPSHOT_FRAME_BUF_SIZE);

    ChipLogProgress(DeviceLayer, "Snapshot: switching to MJPEG %ux%u (h264Streaming=%d)", mSnapshotWidth, mSnapshotHeight,
                    h264Streaming);

    /* Stop H.264 before re-negotiating (nothing to stop when idle). */
    if (h264Streaming) {
        usbh_uvc_stop(CONFIG_USBH_UVC_IF_NUM_0);
    }

    if (!UvcSetFormat(CONFIG_USBH_UVC_SNAPSHOT_FORMAT_TYPE, mSnapshotWidth, mSnapshotHeight, CONFIG_USBH_UVC_FRAME_RATE, CONFIG_USBH_UVC_SNAPSHOT_FRAME_BUF_SIZE)) {
        goto restore;
    }

    if (usbh_uvc_start(CONFIG_USBH_UVC_IF_NUM_0) != RTK_SUCCESS) {
        ChipLogError(DeviceLayer, "Snapshot: MJPEG stream on fail");
        goto restore;
    }

    /* Grab frames until one is a complete JPEG; the first after a switch may be partial. */
    for (attempts = 0; attempts < CONFIG_USBH_UVC_SNAPSHOT_MAX_RETRY_FRAMES; attempts++) {
        buf = usbh_uvc_get_frame(CONFIG_USBH_UVC_IF_NUM_0);
        if (buf == NULL) {
            rtos_time_delay_ms(10);
            continue;
        }

        u32 validLen = ValidateAndTrimJpeg(buf->buf, buf->byteused);
        if (validLen > 0) {
            memcpy(mSnapshotBuf, buf->buf, validLen);
            mSnapshotLen = validLen;
            mSnapshotOk  = true;
            usbh_uvc_put_frame(buf, CONFIG_USBH_UVC_IF_NUM_0);
            ChipLogProgress(DeviceLayer, "Snapshot: captured JPEG %lu bytes", (unsigned long) validLen);
            break;
        }

        usbh_uvc_put_frame(buf, CONFIG_USBH_UVC_IF_NUM_0);
    }

    if (!mSnapshotOk) {
        ChipLogError(DeviceLayer, "Snapshot: no valid JPEG after %d frames", attempts);
    }

    usbh_uvc_stop(CONFIG_USBH_UVC_IF_NUM_0);

restore:
    /* Always restore H.264 negotiation (even on failure) so we never stay stuck
     * in MJPEG; only restart the transfer if it was streaming before. */
    if (UvcSetFormat(CONFIG_USBH_UVC_FORMAT_TYPE, CONFIG_USBH_UVC_WIDTH, CONFIG_USBH_UVC_HEIGHT, CONFIG_USBH_UVC_FRAME_RATE, CONFIG_USBH_UVC_FRAME_BUF_SIZE)) {
        if (h264Streaming && usbh_uvc_start(CONFIG_USBH_UVC_IF_NUM_0) != RTK_SUCCESS) {
            ChipLogError(DeviceLayer, "Snapshot: failed to restart H.264 stream");
        }
    } else {
        ChipLogError(DeviceLayer, "Snapshot: failed to restore H.264 format");
    }

    mSnapshotRequested = false;
    rtos_sema_give(mSnapshotDoneSema);
}

uint32_t MatterCamera::CaptureJpegSnapshot(uint16_t width, uint16_t height, uint8_t **jpegBuf)
{
    *jpegBuf = nullptr;

    /* Serialize concurrent callers. */
    rtos_mutex_take(mSnapshotMutex, RTOS_MAX_TIMEOUT);

    /* Needs a ready (H.264-negotiated) camera, but not an active WebRTC stream:
     * snapshots are served both while idle and while streaming. */
    if (instance == nullptr || !mUvcReady) {
        ChipLogError(DeviceLayer, "Snapshot: camera not ready, cannot capture");
        rtos_mutex_give(mSnapshotMutex);
        return 0;
    }

    /* Requested resolution is not honored: only the fixed
     * CONFIG_USBH_UVC_SNAPSHOT_WIDTH/HEIGHT fits in one CaptureSnapshotResponse
     * message (see camera_driver.h). width/height are logged for diagnostics only. */
    if (width != CONFIG_USBH_UVC_SNAPSHOT_WIDTH || height != CONFIG_USBH_UVC_SNAPSHOT_HEIGHT) {
        ChipLogProgress(DeviceLayer, "Snapshot: requested %ux%u, capturing at fixed %ux%u instead", width, height,
                        CONFIG_USBH_UVC_SNAPSHOT_WIDTH, CONFIG_USBH_UVC_SNAPSHOT_HEIGHT);
    }
    mSnapshotWidth  = CONFIG_USBH_UVC_SNAPSHOT_WIDTH;
    mSnapshotHeight = CONFIG_USBH_UVC_SNAPSHOT_HEIGHT;

    /* Drain any stale signal, then arm the request. */
    while (rtos_sema_take(mSnapshotDoneSema, 0) == RTK_SUCCESS) { /* drain */ }
    mSnapshotRequested = true;

    if (rtos_sema_take(mSnapshotDoneSema, CONFIG_USBH_UVC_SNAPSHOT_TIMEOUT_MS) == RTK_SUCCESS && mSnapshotOk) {
        *jpegBuf = mSnapshotBuf;
    } else {
        ChipLogError(DeviceLayer, "Snapshot: capture timed out or failed");
        mSnapshotRequested = false;
    }

    rtos_mutex_give(mSnapshotMutex);
    return mSnapshotLen;
}

#if CONFIG_USBH_UVC_HOT_PLUG
/**
 * Hotplug handling:
 * 1) usbh_uvc_stop(): stop internal UVC data stream and prepare for resource free.
 * 2) After this, UVC data consumers must stop getting frames
 *   (e.g. exit loop or delete UvcTestThread task).
 * 3) On next attach, consumers can resume or be re-created after UVC re-init.
 * 4) Thread priority: The hotplug handling thread MUST have higher priority than the UVC
 *   get-frame thread to ensure stream stop and cleanup run first on detach.
*/
void MatterCamera::UvcHotplugThread(void *param)
{
    int ret = 0;

    UNUSED(param);

    for (;;) {
        if (rtos_sema_take(mUvcDetachSema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {

            usbh_uvc_stop(CONFIG_USBH_UVC_IF_NUM_0);
            if (mUvcTask) {
                ChipLogProgress(DeviceLayer, "Hotplug: delete UvcTestThread task");
                rtos_task_delete(mUvcTask);
                mUvcTask = NULL;
            }

            usbh_uvc_deinit();
            usbh_deinit();
            rtos_time_delay_ms(10);
            ChipLogProgress(DeviceLayer, "Free heap: 0x%lx", rtos_mem_get_free_heap_size());

            ret = usbh_init(mUsbhConfig, NULL);
            if (ret != HAL_OK) {
                ChipLogError(DeviceLayer, "Init USBH fail");
                break;
            }

            ret = usbh_uvc_init(mUvcConfig, mUvcCallBacks);
            if (ret < 0) {
                ChipLogError(DeviceLayer, "Init UVC fail");
                usbh_deinit();
                break;
            }
        }
    }

    ChipLogError(DeviceLayer, "Hotplug thread fail");
    rtos_task_delete(NULL);
}
#endif

void MatterCamera::UvcMatterThread(void *param)
{
    UNUSED(param);
    time_t sec, usec;
    uint64_t current_time_usec;
    u8 *buffer_h264 = (u8 *) rtos_mem_malloc(CONFIG_USBH_UVC_FRAME_BUF_SIZE);
    if (buffer_h264 == nullptr) {
        ChipLogError(DeviceLayer, "Failed to allocate H.264 send buffer (%d bytes)", CONFIG_USBH_UVC_FRAME_BUF_SIZE);
        mUvcMatterIsInit = 0;
        rtos_task_delete(NULL);
        return;
    }

    while ((mStreamEnabled == false) || (mWebrtcTransport == nullptr)) {
        rtos_time_delay_ms(100);
    }

    ChipLogProgress(DeviceLayer, "Start UVC Matter");
    rtos_time_delay_ms(2000);
    mUvcMatterIsInit = 1;

    while (mStreamEnabled == true) {
        if (rtos_sema_take(mUvcFrameSema, 100) != RTK_SUCCESS) {
            continue;
        }

        rtos_mutex_take(mUvcBufMutex, RTOS_MAX_TIMEOUT);
        uint32_t frameLen = mUvcFrameLen[mUvcFrameQTail];
        mUvcFrameQTail    = (mUvcFrameQTail + 1) % kUvcFrameQueueDepth;
        RingBuffer_Read(mUvcRb, buffer_h264, frameLen);
        rtos_mutex_give(mUvcBufMutex);

#if CONFIG_ENABLE_AMEBA_SNTP
        matter_sntp_get_current_time(&sec, &usec);
        current_time_usec = (uint64_t)sec * UINT64_C(1000000) + (uint64_t)usec;
#else
#error "Matter Camera Example requires CONFIG_ENABLE_AMEBA_SNTP to be activated!"
#endif

        if (mWebrtcTransport == nullptr) {
            ChipLogDetail(DeviceLayer, "WebRTC transport deregistered, stopping stream");
            break;
        } else if (mWebrtcTransport->CanSendVideo() == true) {
            chip::ByteSpan videoData(buffer_h264, frameLen);
            mWebrtcTransport->SendVideo(videoData, (int64_t) current_time_usec, mCurrentVideoStreamId);
            if ((++mUvcFramesSent % (mUvcSCtx.frame_rate > 0 ? (uint32_t) mUvcSCtx.frame_rate : 30u)) == 0) {
                ChipLogProgress(DeviceLayer, "Video streaming: %llu frames sent", mUvcFramesSent);
            }
        } else {
            ChipLogError(DeviceLayer, "WebRTC Transport is not ready to send Video");
        }
    }

    rtos_mem_free(buffer_h264);
    RingBuffer_Destroy(mUvcRb);
    mUvcMatterIsInit = 0;
    rtos_task_delete(NULL);
}

int MatterCamera::UvcMatterStart(void)
{
    int ret = 0;
    rtos_task_t task;

    // Delay to check successful WiFi connection and obtain of an IP address
    while (lwip_check_connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
        rtos_time_delay_ms(1000);
    }

    mUvcRb = RingBuffer_Create(mUvcBuf, CONFIG_USBH_UVC_FRAME_BUF_SIZE, LOCAL_RINGBUFF, 0);

    rtos_sema_create(&mUvcMatterSaveImgSema, 0, 1);

    mUvcFrameQHead  = 0;
    mUvcFrameQTail  = 0;
    mUvcFramesSent  = 0;
    if (mUvcFrameSema == NULL) {
        rtos_sema_create(&mUvcFrameSema, 0, kUvcFrameQueueDepth);
    } else {
        while (rtos_sema_take(mUvcFrameSema, 0) == RTK_SUCCESS) { /* drain */ }
    }

    ret = rtos_task_create(&task, "UvcMatterThread", UvcMatterThreadWrapper, NULL, CONFIG_USBH_UVC_MATTER_THREAD_STACK_SIZE,
                           CONFIG_USBH_UVC_MATTER_THREAD_PRIORITY);
    if (ret != RTK_SUCCESS) {
        ChipLogError(DeviceLayer, "Create %s client thread fail", USBH_UVC_MATTER_TAG);
        rtos_sema_delete(&mUvcMatterSaveImgSema);
    }

    return ret;
}

void MatterCamera::UsbhUvcImgPrepare(usbh_uvc_frame_t *frame)
{
    u32 len = frame->byteused;

    if (rtos_mutex_take(mUvcBufMutex, 1000 / mUvcSCtx.frame_rate / 2) == RTK_SUCCESS) {
        if (mUvcSCtx.fmt_type == USBH_UVC_FORMAT_H264) {
            uint32_t next = (mUvcFrameQHead + 1) % kUvcFrameQueueDepth;
            bool queued   = false;
            if (next != mUvcFrameQTail && (u32) RingBuffer_Space(mUvcRb) >= frame->byteused) {
                RingBuffer_Write(mUvcRb, frame->buf, frame->byteused);
                mUvcFrameLen[mUvcFrameQHead] = frame->byteused;
                mUvcFrameQHead               = next;
                queued                       = true;
            }
            rtos_mutex_give(mUvcBufMutex);
            if (queued && mUvcFrameSema != NULL) {
                rtos_sema_give(mUvcFrameSema);
            }

        } else {
            memcpy(mUvcBuf, (void *)(frame->buf), len);
            mUvcBufSize = len;
            rtos_mutex_give(mUvcBufMutex);
            rtos_sema_give(mUvcMatterSaveImgSema);
        }
    }

}

void MatterCamera::UvcCalculateTp(u32 loop)
{
    u32 rx_elapse;
    u32 rx_perf;
    u32 rx_fps;
    u32 rx_perf_total;

    rx_elapse = SYSTIMER_GetPassTime(mRxStart);

    rx_fps = loop * 1000 / rx_elapse;
    rx_perf = mRxTotalL / 1024 * 1000 / rx_elapse;//KB/S

    ChipLogDetail(DeviceLayer, "TP %lu KB/s @ %lu ms, fps %lu/s", rx_perf, rx_elapse, rx_fps);

    rx_perf = rx_perf * 10 / 1024;
    rx_perf_total = rx_perf + ((mRxTotalH * 10000 << 12) / rx_elapse);
    ChipLogDetail(DeviceLayer, "TP %lu.%lu MB/s-%lu (%lu_%lu/%lu)", rx_perf_total / 10, rx_perf_total % 10, rx_perf, mRxTotalH, mRxTotalL, loop);

    (void) rx_fps;
    (void) rx_perf_total;

    mRxTotalL = 0;
    mRxTotalH = 0;
}

int MatterCamera::UvcInit(void)
{
    ChipLogProgress(DeviceLayer, "MatterCamera: UvcInit");
    return HAL_OK;
}

int MatterCamera::UvcDeinit(void)
{
    ChipLogProgress(DeviceLayer, "MatterCamera: UvcDeinit");
    return HAL_OK;
}

int MatterCamera::UvcAttach(void)
{
    ChipLogProgress(DeviceLayer, "MatterCamera: UvcAttach");
    rtos_sema_give(mUvcAttachSema);
    return HAL_OK;
}

int MatterCamera::UvcDetach(void)
{
    ChipLogProgress(DeviceLayer, "MatterCamera: UvcDetach");
    /* Interface is going away; block snapshot capture until it is re-negotiated. */
    mUvcReady = false;
#if CONFIG_USBH_UVC_HOT_PLUG
    rtos_sema_give(mUvcDetachSema);
#endif
    return HAL_OK;
}

int MatterCamera::UvcSetup(void)
{
    ChipLogProgress(DeviceLayer, "MatterCamera: UvcSetup");
    rtos_sema_give(mUvcStartSema);
    return HAL_OK;
}

int MatterCamera::UvcSetparam(int status)
{
    ChipLogProgress(DeviceLayer, "MatterCamera: UvcSetparam");
    mUvcSetparamStatus = status;
    rtos_sema_give(mUvcSetparamSema);
    return HAL_OK;
}

void MatterCamera::UsbhUvcMainThreadWrapper(void *param)
{
    instance->UsbhUvcMainThread(param);
}

#if CONFIG_USBH_UVC_HOT_PLUG
void MatterCamera::UvcHotplugThreadWrapper(void *param)
{
    instance->UvcHotplugThread(param);
}
#endif

void MatterCamera::UvcTestThreadWrapper(void *param)
{
    instance->UvcTestThread(param);
}

void MatterCamera::UvcMatterThreadWrapper(void *param)
{
    instance->UvcMatterThread(param);
}

int MatterCamera::UvcInitWrapper()
{
    return instance->UvcInit();
}

int MatterCamera::UvcDeinitWrapper()
{
    return instance->UvcDeinit();
}

int MatterCamera::UvcAttachWrapper()
{
    return instance->UvcAttach();
}

int MatterCamera::UvcDetachWrapper()
{
    return instance->UvcDetach();
}

int MatterCamera::UvcSetupWrapper()
{
    return instance->UvcSetup();
}

int MatterCamera::UvcSetparamWrapper(int status)
{
    return instance->UvcSetparam(status);
}
#else

namespace {

/*
 * Minimal MSB-first bitstream writer used to assemble the dummy H.264 frame.
 * It supports the few Exp-Golomb / fixed-length syntax elements needed to
 * build an SPS, PPS and a single I-slice. Writes are clamped to the buffer
 * capacity so a miscalculation can never overflow mDummyBuf.
 */
class BitWriter
{
public:
    BitWriter(uint8_t *buf, size_t cap) : mBuf(buf), mCap(cap) {}

    void PutBit(uint32_t bit)
    {
        mCurrent = (uint8_t)((mCurrent << 1) | (bit & 1U));
        if (++mBitsInByte == 8) {
            Flush();
        }
    }

    void PutBits(uint32_t value, int numBits)
    {
        for (int i = numBits - 1; i >= 0; i--) {
            PutBit((value >> i) & 1U);
        }
    }

    /* ue(v): unsigned Exp-Golomb */
    void PutUE(uint32_t codeNum)
    {
        uint32_t v = codeNum + 1;
        int leadingZeros = 0;
        for (uint32_t t = v >> 1; t != 0; t >>= 1) {
            leadingZeros++;
        }
        for (int i = 0; i < leadingZeros; i++) {
            PutBit(0);
        }
        PutBits(v, leadingZeros + 1);
    }

    /* se(v): signed Exp-Golomb */
    void PutSE(int32_t value)
    {
        uint32_t codeNum = (value <= 0) ? (uint32_t)(-2 * value) : (uint32_t)(2 * value - 1);
        PutUE(codeNum);
    }

    /* rbsp_trailing_bits(): stop-one bit followed by zero padding to byte align */
    void Trailing()
    {
        PutBit(1);
        while (mBitsInByte != 0) {
            PutBit(0);
        }
    }

    size_t Bytes() const
    {
        return mLen;
    }

private:
    void Flush()
    {
        if (mLen < mCap) {
            mBuf[mLen] = mCurrent;
        }
        mLen++;
        mCurrent    = 0;
        mBitsInByte = 0;
    }

    uint8_t *mBuf;
    size_t   mCap;
    size_t   mLen        = 0;
    uint8_t  mCurrent    = 0;
    int      mBitsInByte = 0;
};

} // namespace

/*
 * Build a single, fully decodable H.264 Annex-B keyframe (SPS + PPS + IDR
 * I-slice) for a 1280x720 picture and store it in mDummyBuf.
 *
 * Every macroblock is encoded as I_16x16 with DC prediction and no residual
 * coefficients, so the frame decodes to a flat gray image (luma/chroma = 128).
 * This is intentionally a placeholder ("dummy") frame whose only purpose is to
 * feed valid, decodable data into mWebrtcTransport->SendVideo() for testing.
 *
 * 1280x720 => 80 x 45 macroblocks = 3600 macroblocks. With the chosen syntax
 * each macroblock costs exactly 8 bits, so the whole stream is ~3.6 KB.
 */
void MatterCamera::GenerateDummyH264Frame(void)
{
    const int kMbWidth  = 1280 / 16;            // 80
    const int kMbHeight = 720 / 16;             // 45
    const int kNumMbs   = kMbWidth * kMbHeight; // 3600

    BitWriter bw(mDummyBuf, DUMMY_H264_BUF_SIZE);

    /* ---- Sequence Parameter Set (nal_ref_idc=3, nal_unit_type=7) ---- */
    bw.PutBits(0x00000001, 32);   // Annex-B start code
    bw.PutBits(0x67, 8);          // NAL unit header
    bw.PutBits(66, 8);            // profile_idc = Baseline
    bw.PutBits(0, 8);             // constraint_set flags + reserved_zero_2bits
    bw.PutBits(31, 8);            // level_idc = 3.1 (supports 1280x720@30)
    bw.PutUE(0);                  // seq_parameter_set_id
    bw.PutUE(0);                  // log2_max_frame_num_minus4
    bw.PutUE(0);                  // pic_order_cnt_type
    bw.PutUE(0);                  // log2_max_pic_order_cnt_lsb_minus4
    bw.PutUE(1);                  // max_num_ref_frames
    bw.PutBits(0, 1);             // gaps_in_frame_num_value_allowed_flag
    bw.PutUE(kMbWidth - 1);       // pic_width_in_mbs_minus1
    bw.PutUE(kMbHeight - 1);      // pic_height_in_map_units_minus1
    bw.PutBits(1, 1);             // frame_mbs_only_flag
    bw.PutBits(1, 1);             // direct_8x8_inference_flag
    bw.PutBits(0, 1);             // frame_cropping_flag
    bw.PutBits(0, 1);             // vui_parameters_present_flag
    bw.Trailing();

    /* ---- Picture Parameter Set (nal_ref_idc=3, nal_unit_type=8) ---- */
    bw.PutBits(0x00000001, 32);
    bw.PutBits(0x68, 8);
    bw.PutUE(0);                  // pic_parameter_set_id
    bw.PutUE(0);                  // seq_parameter_set_id
    bw.PutBits(0, 1);             // entropy_coding_mode_flag (0 = CAVLC)
    bw.PutBits(0, 1);             // bottom_field_pic_order_in_frame_present_flag
    bw.PutUE(0);                  // num_slice_groups_minus1
    bw.PutUE(0);                  // num_ref_idx_l0_default_active_minus1
    bw.PutUE(0);                  // num_ref_idx_l1_default_active_minus1
    bw.PutBits(0, 1);             // weighted_pred_flag
    bw.PutBits(0, 2);             // weighted_bipred_idc
    bw.PutSE(0);                  // pic_init_qp_minus26 (QP = 26)
    bw.PutSE(0);                  // pic_init_qs_minus26
    bw.PutSE(0);                  // chroma_qp_index_offset
    bw.PutBits(0, 1);             // deblocking_filter_control_present_flag
    bw.PutBits(0, 1);             // constrained_intra_pred_flag
    bw.PutBits(0, 1);             // redundant_pic_cnt_present_flag
    bw.Trailing();

    /* ---- IDR slice (nal_ref_idc=3, nal_unit_type=5) ---- */
    bw.PutBits(0x00000001, 32);
    bw.PutBits(0x65, 8);
    /* slice_header() */
    bw.PutUE(0);                  // first_mb_in_slice
    bw.PutUE(7);                  // slice_type = 7 (I, all slices in pic are I)
    bw.PutUE(0);                  // pic_parameter_set_id
    bw.PutBits(0, 4);             // frame_num (log2_max_frame_num = 4 bits)
    bw.PutUE(0);                  // idr_pic_id
    bw.PutBits(0, 4);             // pic_order_cnt_lsb (4 bits)
    bw.PutBits(0, 1);             // no_output_of_prior_pics_flag
    bw.PutBits(0, 1);             // long_term_reference_flag
    bw.PutSE(0);                  // slice_qp_delta

    /* slice_data(): every macroblock is I_16x16, DC pred, no residual */
    for (int i = 0; i < kNumMbs; i++) {
        bw.PutUE(3);              // mb_type = 3 => I_16x16, DC pred, CBP luma/chroma = 0
        bw.PutUE(0);              // intra_chroma_pred_mode = DC
        bw.PutSE(0);              // mb_qp_delta = 0
        bw.PutBits(1, 1);         // luma DC coeff_token (TotalCoeff=0, TrailingOnes=0, nC<2)
    }
    bw.Trailing();

    mDummyBufSize = (int) bw.Bytes();
    ChipLogProgress(DeviceLayer, "Generated dummy H.264 1280x720 keyframe: %d bytes", mDummyBufSize);
}

void MatterCamera::StartDummyStreaming(void *param)
{
    if (mDummyBuf == nullptr) {

        mDummyBuf = (uint8_t *) rtos_mem_zmalloc(DUMMY_H264_BUF_SIZE);
        if (mDummyBuf == nullptr) {
            ChipLogError(DeviceLayer, "Failed to allocate dummy H.264 buffer (%d bytes)", DUMMY_H264_BUF_SIZE);
            mDummyTask = NULL;
            rtos_task_delete(NULL);
            return;
        }
        GenerateDummyH264Frame();
    }

    DummyStreaming(param);
    rtos_task_delete(NULL);
}

void MatterCamera::StartDummyStreamingWrapper(void *param)
{
    instance->StartDummyStreaming(param);
}

void MatterCamera::DummyStreaming(void *param)
{
    UNUSED(param);
    time_t sec, usec;
    uint64_t current_time_usec;

    while (1) {
        while ((mStreamEnabled == false) || (mWebrtcTransport == nullptr)) {
            rtos_time_delay_ms(100);
        }

        ChipLogProgress(DeviceLayer, "Start Matter Dummy Streaming");
        rtos_time_delay_ms(2000);

        while (mStreamEnabled == true) {
            rtos_mutex_take(mDummyBufMutex, RTOS_MAX_TIMEOUT);
            chip::ByteSpan videoData(mDummyBuf, static_cast<size_t>(mDummyBufSize));
            rtos_mutex_give(mDummyBufMutex);

            if (mWebrtcTransport == nullptr) {
                ChipLogDetail(DeviceLayer, "WebRTC transport deregistered, stopping stream");
                break;
            } else {
                if (mWebrtcTransport->CanSendVideo() == true) {
#if CONFIG_ENABLE_AMEBA_SNTP
                    matter_sntp_get_current_time(&sec, &usec);
                    current_time_usec = (uint64_t)sec * UINT64_C(1000000) + (uint64_t)usec;
#else
#error "Matter Camera Example requires CONFIG_ENABLE_AMEBA_SNTP to be activated!"
#endif
                    mWebrtcTransport->SendVideo(videoData, current_time_usec, mCurrentVideoStreamId);
                    ChipLogProgress(DeviceLayer, "Video sent");
                } else {
                    ChipLogError(DeviceLayer, "WebRTC Transport is not ready to send Video");
                }
            }

            rtos_time_delay_ms(100);
        }
    }
}

#endif
