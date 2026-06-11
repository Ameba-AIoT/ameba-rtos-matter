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
#include <webrtc/ameba_webrtc_abstract.h>
#include <lwip/inet.h>
#include <lib/support/logging/CHIPLogging.h>
#include <lib/support/CHIPMem.h>

/* Ameba WebRTC C library headers */
extern "C" {
#include <webrtc/library/webrtc/ameba_webrtc.h>
#include <webrtc/library/webrtc/ameba_sdp.h>
#include <webrtc/library/webrtc/ameba_rtp.h>
#include <webrtc/library/ice/ameba_ice.h>
#include <webrtc/library/ice/ameba_stun.h>
#include <webrtc/library/libdatachannel/ameba_dtls.h>
#include <webrtc/library/libdatachannel/ameba_datachannel.h>
}

namespace {

// Constants
constexpr int kVideoH264PayloadType    = 96;
constexpr int kVideoBitRate            = 3000;
constexpr int kMaxFragmentSize         = 1188;
constexpr int kAudioBitRate            = 64000;
constexpr int kOpusPayloadType         = 111;

class LibDataChannelTrack : public WebRTCTrack
{
public:
    LibDataChannelTrack(ameba_webrtc_session_t *session, int trackType, int payloadType)
        : mSession(session), mTrackType(trackType), mPayloadType(payloadType)
    {
        mTimestampBase = 0;
    }

    ~LibDataChannelTrack() = default;

    void SendData(const chip::ByteSpan &data) override
    {
        if (mSession == NULL) {
            return;
        }

        uint32_t timestamp = mTimestampBase++;
        if (mTrackType == 0) { /* video */
            ameba_webrtc_session_send_video(mSession, data.data(), data.size(), timestamp * 3000);
        } else { /* audio */
            ameba_webrtc_session_send_audio(mSession, data.data(), data.size(), timestamp * 960);
        }
    }

    void SendFrame(const chip::ByteSpan &data, int64_t timestamp) override
    {
        if (mSession == NULL) {
            return;
        }

        if (mTrackType == 0) { /* video */
            ameba_webrtc_session_send_video(mSession, data.data(), data.size(), (uint32_t)timestamp);
        } else { /* audio */
            ameba_webrtc_session_send_audio(mSession, data.data(), data.size(), (uint32_t)timestamp);
        }
    }

    bool IsReady() override
    {
        return mSession != NULL && ameba_webrtc_session_get_state(mSession) >= AMEBA_WEBRTC_STATE_CONNECTED;
    }

    std::string GetType() override
    {
        return mTrackType == 0 ? "video" : "audio";
    }

private:
    ameba_webrtc_session_t *mSession;
    int mTrackType;
    int mPayloadType;
    uint32_t mTimestampBase;
};

/* Forward declarations for bridge functions (defined after class) */
static void OnLocalDescriptionBridge(void *ctx, const char *sdp, int sdp_type);
static void OnICECandidateBridge(void *ctx, const char *candidate, const char *mid);
static void OnConnectionStateBridge(void *ctx, int state);
static void OnSendUdpBridge(void *ctx, const uint8_t *data, size_t len, const struct sockaddr *dst_addr);

class LibDataChannelPeerConnection : public WebRTCPeerConnection
{
public:
    LibDataChannelPeerConnection(const std::vector<ICEServerInfo> &servers = {})
    {
        ameba_webrtc_ice_server_t iceServers[4];
        int numServers = 0;

        for (const auto &server : servers) {
            for (const auto &url : server.urls) {
                if (numServers >= 4) {
                    break;
                }
                strncpy(iceServers[numServers].url, url.c_str(), sizeof(iceServers[numServers].url) - 1);
                strncpy(iceServers[numServers].username, server.username.c_str(), sizeof(iceServers[numServers].username) - 1);
                strncpy(iceServers[numServers].credential, server.credential.c_str(), sizeof(iceServers[numServers].credential) - 1);
                numServers++;
            }
        }

        mSession = ameba_webrtc_session_create(iceServers, numServers);
        if (mSession == NULL) {
            ChipLogError(Camera, "Failed to create Ameba WebRTC session");
            return;
        }

        mOnLocalDescriptionCB = nullptr;
        mOnICECandidateCB     = nullptr;
        mOnConnectionStateCB  = nullptr;
        mOnTrackCB            = nullptr;

        ameba_webrtc_session_set_callbacks(mSession,
                                           OnLocalDescriptionBridge,
                                           OnICECandidateBridge,
                                           OnConnectionStateBridge,
                                           nullptr, OnSendUdpBridge, this);
    }

    ~LibDataChannelPeerConnection()
    {
        if (mSession) {
            ameba_webrtc_session_close(mSession);
            ameba_webrtc_session_destroy(mSession);
            mSession = NULL;
        }
    }

    void SetCallbacks(OnLocalDescriptionCallback onLocalDescription,
                      OnICECandidateCallback onICECandidate,
                      OnConnectionStateCallback onConnectionState,
                      OnTrackCallback onTrack) override
    {
        mOnLocalDescriptionCB = onLocalDescription;
        mOnICECandidateCB     = onICECandidate;
        mOnConnectionStateCB  = onConnectionState;
        mOnTrackCB            = onTrack;
        ChipLogProgress(Camera, "LibDataChannelPeerConnection callbacks registered");
    }

    void Close() override
    {
        if (mSession) {
            ameba_webrtc_session_close(mSession);
        }
    }

    void CreateOffer() override
    {
        ChipLogProgress(Camera, "Creating SDP offer via Ameba WebRTC");
        if (mSession == NULL) {
            ChipLogError(Camera, "WebRTC session not created");
            return;
        }
        if (ameba_webrtc_session_create_offer(mSession) != 0) {
            ChipLogError(Camera, "Failed to create SDP offer");
        }
    }

    void CreateAnswer() override
    {
        ChipLogProgress(Camera, "Creating SDP answer via Ameba WebRTC");
        if (mSession == NULL) {
            ChipLogError(Camera, "WebRTC session not created");
            return;
        }
        if (ameba_webrtc_session_create_answer(mSession) != 0) {
            ChipLogError(Camera, "Failed to create SDP answer");
        }
    }

    void SetRemoteDescription(const std::string &sdp, SDPType type) override
    {
        ChipLogProgress(Camera, "Setting remote SDP description");
        if (mSession == NULL) {
            return;
        }

        int sdp_type = 0;
        switch (type) {
        case SDPType::Offer:
            sdp_type = 0;
            break;
        case SDPType::Answer:
            sdp_type = 1;
            break;
        case SDPType::Pranswer:
            sdp_type = 2;
            break;
        case SDPType::Rollback:
            sdp_type = 3;
            break;
        }

        if (ameba_webrtc_session_set_remote_description(mSession, sdp.c_str(), sdp_type) != 0) {
            ChipLogError(Camera, "Failed to set remote description");
        }
    }

    void AddRemoteCandidate(const std::string &candidate, const std::string &mid) override
    {
        ChipLogProgress(Camera, "Adding remote ICE candidate: mid=%s", mid.c_str());
        if (mSession == NULL) {
            return;
        }
        ameba_webrtc_session_add_remote_candidate(mSession, candidate.c_str(), mid.c_str());
    }

    std::shared_ptr<WebRTCTrack> AddTrack(MediaType mediaType, const std::string &mid, int payloadType) override
    {
        ChipLogProgress(Camera, "Adding track: %s, mid=%s, pt=%d",
                        mediaType == MediaType::Video ? "video" : "audio",
                        mid.c_str(), payloadType);

        if (mSession == NULL) {
            return nullptr;
        }

        int trackType = (mediaType == MediaType::Video) ? 0 : 1;
        if (payloadType <= 0) {
            payloadType = (mediaType == MediaType::Video) ? kVideoH264PayloadType : kOpusPayloadType;
        }

        int trackId;
        if (mediaType == MediaType::Video) {
            trackId = ameba_webrtc_session_add_video_track(mSession, mid.c_str(), payloadType);
        } else {
            trackId = ameba_webrtc_session_add_audio_track(mSession, mid.c_str(), payloadType);
        }

        if (trackId < 0) {
            ChipLogError(Camera, "Failed to add track");
            return nullptr;
        }

        auto track = std::make_shared<LibDataChannelTrack>(mSession, trackType, payloadType);
        mTracks.push_back(track);

        if (mOnTrackCB && track) {
            mOnTrackCB(track);
        }

        return track;
    }

    int GetPayloadType(const std::string &sdp, SDPType type, const std::string &codec) override
    {
        return ameba_sdp_get_payload_type(sdp.c_str(), codec.c_str());
    }

    /**
     * Get the underlying C WebRTC session (ameba_webrtc_session_t).
     * Used by the transport layer to access ICE/DTLS directly.
     * Returns NULL if not available.
     */
    struct ameba_webrtc_session *GetSession()
    {
        return mSession;
    }

    /* Public to allow bridge function access */
    ameba_webrtc_session_t *mSession = NULL;
    OnLocalDescriptionCallback mOnLocalDescriptionCB = nullptr;
    OnICECandidateCallback mOnICECandidateCB         = nullptr;
    OnConnectionStateCallback mOnConnectionStateCB   = nullptr;
    OnTrackCallback mOnTrackCB                       = nullptr;
    std::vector<std::shared_ptr<WebRTCTrack>> mTracks;
};

/*---------------------------------------------------------------------------
 * Static bridge functions (defined after class so the type is complete)
 *---------------------------------------------------------------------------*/
static void OnLocalDescriptionBridge(void *ctx, const char *sdp, int sdp_type)
{
    auto *self = static_cast<LibDataChannelPeerConnection *>(ctx);
    if (self && self->mOnLocalDescriptionCB) {
        SDPType type = SDPType::Offer;
        if (sdp_type == 1) {
            type = SDPType::Answer;
        } else if (sdp_type == 2) {
            type = SDPType::Pranswer;
        } else if (sdp_type == 3) {
            type = SDPType::Rollback;
        }
        self->mOnLocalDescriptionCB(std::string(sdp), type);
    }
}

static void OnICECandidateBridge(void *ctx, const char *candidate, const char *mid)
{
    auto *self = static_cast<LibDataChannelPeerConnection *>(ctx);
    if (self && self->mOnICECandidateCB) {
        ICECandidateInfo info;
        info.candidate  = std::string(candidate);
        info.mid        = std::string(mid);
        info.mlineIndex = -1;
        self->mOnICECandidateCB(info);
    }
}

static void OnConnectionStateBridge(void *ctx, int state)
{
    auto *self = static_cast<LibDataChannelPeerConnection *>(ctx);
    if (self && self->mOnConnectionStateCB) {
        /* Report connected for CONNECTING and CONNECTED — both are non-terminal.
         * Only report disconnected on terminal states (FAILED, CLOSED).
         * CONNECTING means ICE succeeded and DTLS handshake is in progress;
         * reporting it as disconnected would cause the transport layer to
         * tear down the session before DTLS completes. */
        bool is_connected = (state != AMEBA_WEBRTC_STATE_FAILED && state != AMEBA_WEBRTC_STATE_CLOSED);
        self->mOnConnectionStateCB(is_connected);
    }
}

static void OnSendUdpBridge(void *ctx, const uint8_t *data, size_t len, const struct sockaddr *dst_addr)
{
    auto *self = static_cast<LibDataChannelPeerConnection *>(ctx);
    if (self && self->mSession) {
        int udp_fd = ameba_webrtc_session_get_udp_socket(self->mSession);
        if (udp_fd >= 0) {
            int sent = sendto(udp_fd, data, len, 0, dst_addr, sizeof(struct sockaddr_in));
            if (sent < 0) {
                ChipLogError(Camera, "OnSendUdpBridge: sendto failed");
            }
        } else {
            ChipLogError(Camera, "OnSendUdpBridge: udp_fd not set");
        }
    }
}

} // namespace

std::shared_ptr<WebRTCPeerConnection> CreateWebRTCPeerConnection(const std::vector<ICEServerInfo> &iceServers)
{
    return std::make_shared<LibDataChannelPeerConnection>(iceServers);
}
