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

#include <algorithm>

#include <camera_driver.h>

#include <support/logging/CHIPLogging.h>

MatterCamera* MatterCamera::instance = nullptr;

// Public

void MatterCamera::Init(void)
{
    int status;
    rtos_task_t task;

    instance = this;

    mUsbhConfig   = (usbh_config_t *)  pvPortMalloc(sizeof(usbh_config_t));
    mUvcConfig    = (usbh_uvc_ctx_t *) pvPortMalloc(sizeof(usbh_uvc_ctx_t));
    mUvcCallBacks = (usbh_uvc_cb_t *)  pvPortMalloc(sizeof(usbh_uvc_cb_t));

    // Init USB Host Configuration
    mUsbhConfig->speed              = USB_SPEED_HIGH;
    mUsbhConfig->ext_intr_enable    = USBH_SOF_INTR;
    mUsbhConfig->isr_priority       = INT_PRI_MIDDLE;
    mUsbhConfig->main_task_priority = CONFIG_USBH_UVC_MAIN_THREAD_PRIORITY;
    mUsbhConfig->tick_source        = USBH_SOF_TICK;
#if defined (CONFIG_AMEBAGREEN2)
    /*FIFO total depth is 1024, reserve 12 for DMA addr*/
    mUsbhConfig->rx_fifo_depth      = 500;
    mUsbhConfig->nptx_fifo_depth    = 256;
    mUsbhConfig->ptx_fifo_depth     = 256;
#endif

    // Init UVC Context
#if USBH_UVC_USE_HW
    mUvcConfig->hw_isr_pri = CONFIG_USBH_UVC_HW_IRQ_PRIORITY,
#endif
   
    // Init UVC Callbacks
    mUvcCallBacks->init     = UvcInitWrapper;
    mUvcCallBacks->deinit   = UvcDeinitWrapper;
    mUvcCallBacks->attach   = UvcAttachWrapper;
    mUvcCallBacks->detach   = UvcDetachWrapper;
    mUvcCallBacks->setup    = UvcSetupWrapper;
    mUvcCallBacks->setparam = UvcSetparamWrapper;

    status = rtos_task_create(&task, "UsbhUvcMainThread", UsbhUvcMainThreadWrapper, NULL, 1024U, 1U);
    if (status != RTK_SUCCESS) {
        ChipLogError(DeviceLayer, "Create thread fail");
    }
}

void MatterCamera::deInit(void)
{
    vPortFree(mUsbhConfig);
    vPortFree(mUvcConfig);
    vPortFree(mUvcCallBacks);
}

void MatterCamera::EnableMatterVideoStream(uint16_t streamId)
{
    ChipLogProgress(DeviceLayer, "Enabling video stream for streamId(%u)", streamId);
    mCurrentVideoStreamId = streamId;
    mStreamEnabled        = true;
}

void MatterCamera::DisableMatterVideoStream(void)
{
    ChipLogProgress(DeviceLayer, "Disabling video stream for streamId(%u)", mCurrentVideoStreamId);
    mCurrentVideoStreamId = USBH_UVC_MATTER_INVALID_ID;
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
    mCurrentSessionId = USBH_UVC_MATTER_INVALID_ID;
    mWebrtcTransport  = nullptr;
}

MatterCamera *MatterCamera::GetInstance(void)
{
    return instance;
}

// Private

void MatterCamera::UsbhUvcMainThread(void *param)
{
    ChipLogProgress(DeviceLayer, "USBH UVC task start");
#if CONFIG_USBH_UVC_HOT_PLUG
    rtos_task_t hotplug_task;
#endif
    int ret = 0;

    UNUSED(param);

    mUvcBuf = (uint8_t*) rtos_mem_zmalloc(CONFIG_USBH_UVC_FRAME_BUF_SIZE);
    rtos_sema_create(&mUvcAttachSema, 0U, 1U);
    rtos_sema_create(&mUvcDetachSema, 0U, 1U);
    rtos_sema_create(&mUvcStartSema, 0U, 1U);
    rtos_sema_create(&mUvcSetparamSema, 0U, 1U);
    rtos_mutex_create(&mUvcBufMutex);

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
    ret = rtos_task_create(&hotplug_task, "UvcHotplugThread", UvcHotplugThreadWrapper, NULL, CONFIG_USBH_UVC_HOTPLUG_THREAD_STACK_SIZE, CONFIG_USBH_UVC_HOTPLUG_THREAD_PRIORITY);
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
    rtos_mutex_delete(mUvcBufMutex);
    rtos_sema_delete(mUvcAttachSema);
    rtos_sema_delete(mUvcDetachSema);
    rtos_sema_delete(mUvcStartSema);
    rtos_sema_delete(mUvcSetparamSema);
example_exit:
    rtos_mem_free(mUvcBuf);
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

        if (mUvcSCtx.fmt_type == USBH_UVC_FORMAT_MJPEG) {
            fmt_name = "MJPEG";
        } else if (mUvcSCtx.fmt_type == USBH_UVC_FORMAT_H264) {
            fmt_name = "H264";
        } else if (mUvcSCtx.fmt_type == USBH_UVC_FORMAT_YUV) {
            fmt_name = "YUV";
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
        if (rtos_sema_take(mUvcSetparamSema, 1000) == RTK_SUCCESS) {
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

        while (mUvcMatterIsInit == 0) {
            rtos_time_delay_ms(500);
        }

        img_cnt = 0;
        fail_cnt = 0;

        ChipLogProgress(DeviceLayer, "Stream on");
        ret = usbh_uvc_stream_on(&mUvcSCtx, CONFIG_USBH_UVC_IF_NUM_0);

        if (ret) {
            ChipLogProgress(DeviceLayer, "Stream on err");
            goto exit;
        }
        while (mUvcMatterIsInit) {
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
        usbh_uvc_stream_off(CONFIG_USBH_UVC_IF_NUM_0);
        ChipLogProgress(DeviceLayer, "Stream off");
    }

exit:
    rtos_task_delete(NULL);
    mUvcTask = NULL;
}

#if CONFIG_USBH_UVC_HOT_PLUG
/**
 * Hotplug handling:
 * 1) usbh_uvc_stream_off(): stop internal UVC data stream and prepare for resource free.
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

            usbh_uvc_stream_off(CONFIG_USBH_UVC_IF_NUM_0);
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
#if (CONFIG_USBH_UVC_FORMAT_TYPE != USBH_UVC_FORMAT_MJPEG)
    u8 *buffer_h264 = (u8*) rtos_mem_malloc(USBH_UVC_MATTER_WRITE_SIZE);
#endif
    UNUSED(param);


    while ((mStreamEnabled == false) || (mWebrtcTransport == nullptr)) {
        ChipLogProgress(DeviceLayer, "Waiting for liveview start request from controller...");
        rtos_time_delay_ms(1000);
    }

    ChipLogProgress(DeviceLayer, "Start UVC Matter");
    rtos_time_delay_ms(2000);
    mUvcMatterIsInit = 1;

    while (mStreamEnabled == true) {
// Among the 3 formats, Matter only supports H.264 encoded video, if video is not in H.264 format, it will be dropped.
#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)
        rtos_mutex_take(mUvcBufMutex, RTOS_MAX_TIMEOUT);
        chip::ByteSpan videoData(mUvcBuf, static_cast<size_t>(mUvcBufSize));
        rtos_mutex_give(mUvcBufMutex);

        if (mWebrtcTransport == nullptr) {
            ChipLogError(DeviceLayer, "Error, WebRTC transport is null!");
        } else {
            if(mWebrtcTransport->CanSendVideo() == true){
                mWebrtcTransport->SendVideo(videoData, rtos_time_get_current_system_time_ms_64bit(), mCurrentVideoStreamId);
                ChipLogProgress(DeviceLayer, "Video sent");
            } else {
                ChipLogError(DeviceLayer, "WebRTC Transport is not ready to send Video");
            }
        }

        rtos_time_delay_ms(1000);
#else // USBH_UVC_FORMAT_YUV or USBH_UVC_FORMAT_H264
        if (RingBuffer_Available(mUvcRb) >= USBH_UVC_MATTER_WRITE_SIZE) {
            rtos_mutex_take(mUvcBufMutex, RTOS_MAX_TIMEOUT);
            RingBuffer_Read(mUvcRb, buffer_h264, USBH_UVC_MATTER_WRITE_SIZE);
            chip::ByteSpan videoData(buffer_h264, USBH_UVC_MATTER_WRITE_SIZE);
            rtos_mutex_give(mUvcBufMutex);
            if (mWebrtcTransport == nullptr) {
                ChipLogError(DeviceLayer, "Error, WebRTC transport is null!");
            } else {
                if(mWebrtcTransport->CanSendVideo() == true){
                    mWebrtcTransport->SendVideo(videoData, rtos_time_get_current_system_time_ms_64bit(), mCurrentVideoStreamId);
                    ChipLogProgress(DeviceLayer, "Video sent");
                } else {
                    ChipLogError(DeviceLayer, "WebRTC Transport is not ready to send Video");
                }
            }
            rtos_time_delay_ms(1000);
        } else {
            rtos_time_delay_ms(2);
        }
#endif
    }

#if (CONFIG_USBH_UVC_FORMAT_TYPE != USBH_UVC_FORMAT_MJPEG)
    rtos_mem_free(buffer_h264);
#endif
    RingBuffer_Destroy(mUvcRb);
    mUvcMatterIsInit = 0;
    rtos_task_delete(NULL);
}

int MatterCamera::UvcMatterStart(void)
{
    int ret = 0;
    rtos_task_t task;

    // Delay to check successful WiFi connection and obtain of an IP address
    while (LwIP_Check_Connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
        rtos_time_delay_ms(2000);
    }

    mUvcRb = RingBuffer_Create(mUvcBuf, CONFIG_USBH_UVC_FRAME_BUF_SIZE, LOCAL_RINGBUFF, 0);

    rtos_sema_create(&mUvcMatterSaveImgSema, 0, 1);

    ret = rtos_task_create(&task, "UvcMatterThread", UvcMatterThreadWrapper, NULL, CONFIG_USBH_UVC_MATTER_THREAD_STACK_SIZE, CONFIG_USBH_UVC_MATTER_THREAD_PRIORITY);
    if (ret != RTK_SUCCESS) {
        ChipLogError(DeviceLayer, "Create %s client thread fail", USBH_UVC_MATTER_TAG);
        rtos_sema_delete(&mUvcMatterSaveImgSema);
    }

    return ret;
}

void MatterCamera::UsbhUvcImgPrepare(usbh_uvc_frame_t *frame)
{
    u32 len = frame->byteused;

#if CONFIG_USBH_UVC_CHECK_MJEPG_DATA
    //some camera may pad 0 to the end of image
    while (1) {
        if (frame->buf[len - 1] == 0) {
            len--;
        } else {
            break;
        }
    }

    /* UVC Host only passes data through. */
    /* Invalid data from camera should be handled by application and must not stopping fetching the next frame. */
    if (frame->buf[0] != 0xff || frame->buf[1] != 0xd8 || frame->buf[len - 2] != 0xff || frame->buf[len - 1] != 0xd9) {
        ChipLogDetail(DeviceLayer, "[mjpeg] image error: %x %x %x %x", frame->buf[0], frame->buf[1], frame->buf[2], frame->buf[3]);
        ChipLogDetail(DeviceLayer, "[mjpeg] image error: %x %x %x %x", frame->buf[len - 4], frame->buf[len - 3], frame->buf[len - 2], frame->buf[len - 1]);
        /* should not return */
        /* The application can adopt a drop mechanism here, discarding frames that do not comply with the specification without storing them */
    }
#endif

    if (rtos_mutex_take(mUvcBufMutex, 1000 / mUvcSCtx.frame_rate / 2) == RTK_SUCCESS) {
        if (mUvcSCtx.fmt_type == USBH_UVC_FORMAT_H264) {
            if ((u32)RingBuffer_Space(mUvcRb) > frame->byteused) {
                RingBuffer_Write(mUvcRb, frame->buf, frame->byteused);
            }
            rtos_mutex_give(mUvcBufMutex);

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

int MatterCamera::UvcSetparam(void)
{
    ChipLogProgress(DeviceLayer, "MatterCamera: UvcSetparam");
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

int MatterCamera::UvcSetparamWrapper()
{
    return instance->UvcSetparam();
}
