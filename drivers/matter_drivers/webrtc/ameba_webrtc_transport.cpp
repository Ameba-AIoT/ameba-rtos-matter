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
#include <platform/Ameba/AmebaUtils.h>
#include <webrtc/ameba_webrtc_abstract.h>
#include <lib/support/logging/CHIPLogging.h>
#include <webrtc/ameba_webrtc_transport.h>

/* LwIP socket & IP helpers */
#include <lwip/sockets.h>
#include <lwip/inet.h>

/* OS related includes */
#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>

/* Math for sine wave generation */
#include <math.h>

/* Ameba WiFi IP helper */
#include <matter_lwip.h>

/* Static buffers for streaming task (to avoid stack overflow) */
uint8_t WebrtcTransport::sVideoBuffer[1400];
uint8_t WebrtcTransport::sAudioBuffer[256];

/* Ameba WebRTC C library */
extern "C" {
#include <webrtc/library/webrtc/ameba_webrtc.h>
#include <webrtc/library/ice/ameba_ice.h>
}

WebrtcTransport::WebrtcTransport()
{
    ChipLogProgress(Camera, "WebrtcTransport created");
    rtos_mutex_create(&mTrackStatusLock);
    mRequestArgs = {}; // Default initialize request arguments
}

WebrtcTransport::~WebrtcTransport()
{
    StopICETimer(); // AI generated
    DestroyUdpSocket(); // AI generated
    ClosePeerConnection();
    ChipLogProgress(Camera, "WebrtcTransport destroyed for sessionID: [%u]", mRequestArgs.sessionId);
}

void WebrtcTransport::SetCallbacks(OnTransportLocalDescriptionCallback onLocalDescription,
                                   OnTransportConnectionStateCallback onConnectionState)
{
    mOnLocalDescription = onLocalDescription;
    mOnConnectionState  = onConnectionState;
}

void WebrtcTransport::SetRequestArgs(const RequestArgs &args)
{
    mRequestArgs = args;
}

void WebrtcTransport::SetICEServers(const std::vector<ICEServerInfo> &servers)
{
    mICEServers = servers;
}

WebrtcTransport::RequestArgs &WebrtcTransport::GetRequestArgs()
{
    return mRequestArgs;
}

void WebrtcTransport::SendVideo(const chip::ByteSpan &data, int64_t timestamp, uint16_t videoStreamID)
{
    rtos_mutex_take(mTrackStatusLock, RTOS_MAX_TIMEOUT);
    if (mLocalVideoTrack) {
        // TODO: Implement frame encryption HERE (per-transport, during RTP packetization)
        // Current state: data contains raw H.264 encoded frames
        //
        // frame encryption should happen here because:
        // 1. Each transport may have different frameConfig (different keys, cipher suites)
        // 2. Multiple transports can share the same video stream
        // 3. Encryption must be per-client, not per-stream
        //
        // Implementation steps:
        // if (frameConfig.HasValue())
        // {
        //     auto& config = frameConfig.Value();
        //     // 1. Encrypt H.264 payload using config.baseKey and config.cipherSuite:
        //     //    - 0x0001: AES-128-GCM-SHA256 (16 byte key)
        //     //    - 0x0002: AES-256-GCM-SHA512 (32 byte key)
        //     // 2. Build frame header with config.kid and frame counter
        //     // 3. Prepend frame header to encrypted payload
        //     // 4. Pass encrypted data to RTP packetization
        //     //    Result: [RTP Header | frame Header | Encrypted(H.264)]
        // }
        // else
        // {
        //     // No encryption - pass raw H.264 to RTP packetization
        // }

        mLocalVideoTrack->SendFrame(data, timestamp);
    }
    rtos_mutex_give(mTrackStatusLock);
}

// Implementation of SendAudio method
void WebrtcTransport::SendAudio(const chip::ByteSpan &data, int64_t timestamp, uint16_t audioStreamID)
{
    rtos_mutex_take(mTrackStatusLock, RTOS_MAX_TIMEOUT);
    if (mLocalAudioTrack) {
        // TODO: Implement frame encryption HERE (per-transport, during RTP packetization)
        // Current state: data contains raw Opus encoded frames
        //
        // frame encryption should happen here because:
        // 1. Each transport may have different frameConfig (different keys, cipher suites)
        // 2. Multiple transports can share the same audio stream
        // 3. Encryption must be per-client, not per-stream
        //
        // Implementation steps:
        // if (frameConfig.HasValue())
        // {
        //     auto& config = frameConfig.Value();
        //     // 1. Encrypt Opus payload using config.baseKey and config.cipherSuite:
        //     //    - 0x0001: AES-128-GCM-SHA256 (16 byte key)
        //     //    - 0x0002: AES-256-GCM-SHA512 (32 byte key)
        //     // 2. Build frame header with config.kid and frame counter
        //     // 3. Prepend frame header to encrypted payload
        //     // 4. Pass encrypted data to RTP packetization
        //     //    Result: [RTP Header | frame Header | Encrypted(Opus)]
        // }
        // else
        // {
        //     // No encryption - pass raw Opus to RTP packetization
        // }

        mLocalAudioTrack->SendFrame(data, timestamp);
    }
    rtos_mutex_give(mTrackStatusLock);
}

// Implementation of SendAudioVideo method
void WebrtcTransport::SendAudioVideo(const chip::ByteSpan &data, uint16_t videoStreamID, uint16_t audioStreamID)
{
    // Placeholder for actual WebRTC implementation to send synchronized audio/video data
}

// Implementation of CanSendVideo method
bool WebrtcTransport::CanSendVideo()
{
    return mLocalVideoTrack != nullptr;
}

// Implementation of CanSendAudio method
bool WebrtcTransport::CanSendAudio()
{
    return mLocalAudioTrack != nullptr;
}

const char *WebrtcTransport::GetStateStr() const
{
    switch (mState) {
    case State::Idle:
        return "Idle";

    case State::SendingOffer:
        return "SendingOffer";

    case State::SendingAnswer:
        return "SendingAnswer";

    case State::SendingICECandidates:
        return "SendingICECandidates";

    case State::SendingEnd:
        return "SendingEnd";
    }
    return "N/A";
}

void WebrtcTransport::MoveToState(const State targetState)
{
    mState = targetState;
    ChipLogProgress(Camera, "WebrtcTransport moving to [ %s ]", GetStateStr());
}

void WebrtcTransport::SetCommandType(const CommandType commandtype)
{
    mCommandType = commandtype;
}

void WebrtcTransport::Start()
{
    if (mPeerConnection.get()) {
        ChipLogProgress(Camera, "Start, mPeerConnection is already created");
        return;
    }

    mPeerConnection = CreateWebRTCPeerConnection(mICEServers);

    mPeerConnection->SetCallbacks([this](const std::string & sdp, SDPType type) {
        this->OnLocalDescription(sdp, type);
    },
    [this](const ICECandidateInfo & candidateInfo) {
        this->OnICECandidate(candidateInfo);
    },
    [this](bool connected) {
        this->OnConnectionStateChanged(connected);
    },
    [this](std::shared_ptr<WebRTCTrack> track) {
        this->OnTrack(track);
    });

    /* Create UDP socket and register local ICE candidate */
    InitUdpSocket(); // AI generated
}

void WebrtcTransport::Stop()
{
    rtos_mutex_take(mTrackStatusLock, RTOS_MAX_TIMEOUT);
    if (mPeerConnection != nullptr) {
        mPeerConnection->Close();
    }

    mLocalVideoTrack = nullptr;
    mLocalAudioTrack = nullptr;
    rtos_mutex_give(mTrackStatusLock);

    // AI generated starts
    /* Stop periodic ICE tick timer */
    StopICETimer();

    /* Close UDP socket */
    DestroyUdpSocket();

    mIceStarted = false;
    // AI generated ends
}

void WebrtcTransport::AddVideoTrack(const std::string &videoMid, int payloadType)
{
    if (mPeerConnection != nullptr) {
        // Adding local tracks to send video data to remote peer
        mLocalVideoTrack = mPeerConnection->AddTrack(MediaType::Video, videoMid, payloadType);
    }
}

void WebrtcTransport::AddAudioTrack(const std::string &audioMid, int payloadType)
{
    if (mPeerConnection != nullptr) {
        // Adding local tracks to send audio data to remote peer
        mLocalAudioTrack = mPeerConnection->AddTrack(MediaType::Audio, audioMid, payloadType);
    }
}

void WebrtcTransport::AddRemoteCandidate(const std::string &candidate, const std::string &mid)
{
    ChipLogProgress(Camera, "Adding remote candidate for sessionID: %u", mRequestArgs.sessionId);
    mPeerConnection->AddRemoteCandidate(candidate, mid);

    // AI generated starts
    /* Start ICE connectivity checks after we have at least one remote candidate */
    MaybeStartIce();
    // AI generated ends
}

// AI generated starts
void WebrtcTransport::MaybeStartIce()
{
    /* Start ICE connectivity checks once, after the remote transport address is
     * known. Remote candidates arrive either via a ProvideICECandidates command
     * (AddRemoteCandidate) or embedded in the remote SDP handled by
     * SetRemoteDescription (e.g. TC-WEBRTC-1.3); both paths funnel here so the
     * ICE tick timer is armed regardless of how the candidates were delivered. */
    if (mIceStarted || mUdpSocket < 0) {
        return;
    }

    struct ameba_webrtc_session *session = mPeerConnection ? mPeerConnection->GetSession() : nullptr;
    if (session == nullptr) {
        return;
    }

    int rc = ameba_webrtc_session_start_ice(session);
    if (rc == 0) {
        mIceStarted = true;
        ChipLogProgress(Camera, "ICE connectivity checks started for sessionID: %u", mRequestArgs.sessionId);

        /* Start periodic tick timer to drive ICE timeouts */
        StartICETimer();
    } else {
        ChipLogError(Camera, "Failed to start ICE for sessionID: %u (rc=%d)", mRequestArgs.sessionId, rc);
    }
}
// AI generated ends

// WebRTC Callbacks
void WebrtcTransport::OnLocalDescription(const std::string &sdp, SDPType type)
{
    ChipLogProgress(Camera, "Local description received for sessionID: %u", mRequestArgs.sessionId);
    mLocalSdp     = sdp;
    mLocalSdpType = type;
    if (mOnLocalDescription) {
        mOnLocalDescription(sdp, type, mRequestArgs.sessionId);
    }
}

bool WebrtcTransport::ClosePeerConnection()
{
    rtos_mutex_take(mTrackStatusLock, RTOS_MAX_TIMEOUT);
    if (mPeerConnection == nullptr) {
        rtos_mutex_give(mTrackStatusLock);
        return false;
    }
    mPeerConnection->Close();
    mPeerConnection.reset();

    rtos_mutex_give(mTrackStatusLock);
    return true;
}

void WebrtcTransport::OnICECandidate(const ICECandidateInfo &candidateInfo)
{
    ChipLogProgress(Camera, "ICE Candidate received for sessionID: %u", mRequestArgs.sessionId);
    mLocalCandidates.push_back(candidateInfo);
    ChipLogProgress(Camera, "Local Candidate:");
    ChipLogProgress(Camera, "%s", candidateInfo.candidate.c_str());
    ChipLogProgress(Camera, "  mid: %s, mlineIndex: %d", candidateInfo.mid.c_str(), candidateInfo.mlineIndex);
}

void WebrtcTransport::OnConnectionStateChanged(bool connected)
{
    ChipLogProgress(Camera, "Connection state changed for sessionID: %u", mRequestArgs.sessionId);
    if (mOnConnectionState) {
        mOnConnectionState(connected, mRequestArgs.sessionId);
    }
}

void WebrtcTransport::OnTrack(std::shared_ptr<WebRTCTrack> track)
{
    ChipLogProgress(Camera, "Track received for sessionID: %u, type: %s", mRequestArgs.sessionId, track->GetType().c_str());
}

// AI generated starts
// ─── UDP Socket Management ─────────────────────────────────────────

void WebrtcTransport::InitUdpSocket()
{
    if (mUdpSocket >= 0) {
        return; // Already initialized
    }

    // Create UDP socket
    mUdpSocket = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (mUdpSocket < 0) {
        ChipLogError(Camera, "Failed to create UDP socket for sessionID: %u", mRequestArgs.sessionId);
        return;
    }

    // Get WiFi IP address
    unsigned char *ip_bytes = matter_LwIP_GetIP(0);
    if (ip_bytes == NULL || (ip_bytes[0] == 0 && ip_bytes[1] == 0 && ip_bytes[2] == 0 && ip_bytes[3] == 0)) {
        ChipLogError(Camera, "WiFi IP not available for sessionID: %u", mRequestArgs.sessionId);
        lwip_close(mUdpSocket);
        mUdpSocket = -1;
        return;
    }

    char ip_str[16];
    snprintf(ip_str, sizeof(ip_str), "%u.%u.%u.%u",
             ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3]);

    // Bind to WiFi IP on random port
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family      = AF_INET;
    local_addr.sin_port        = 0; // Random port
    local_addr.sin_addr.s_addr = htonl((ip_bytes[0] << 24) | (ip_bytes[1] << 16) | (ip_bytes[2] << 8) | ip_bytes[3]);

    if (lwip_bind(mUdpSocket, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        ChipLogError(Camera, "Failed to bind UDP socket to %s for sessionID: %u", ip_str, mRequestArgs.sessionId);
        lwip_close(mUdpSocket);
        mUdpSocket = -1;
        return;
    }

    // Get actual port after bind
    struct sockaddr_in bound_addr;
    socklen_t addr_len = sizeof(bound_addr);
    if (lwip_getsockname(mUdpSocket, (struct sockaddr *)&bound_addr, &addr_len) == 0) {
        uint16_t local_port = ntohs(bound_addr.sin_port);
        ChipLogProgress(Camera, "UDP socket bound: %s:%u for sessionID: %u", ip_str, local_port, mRequestArgs.sessionId);

        // Register local ICE candidate and set UDP socket on the session
        struct ameba_webrtc_session *session = mPeerConnection ? mPeerConnection->GetSession() : nullptr;
        if (session) {
            ameba_webrtc_session_add_local_candidate(session, ip_str, local_port);
            ameba_webrtc_session_set_udp_socket(session, mUdpSocket);
            ChipLogProgress(Camera, "Local ICE candidate added: %s:%u for sessionID: %u", ip_str, local_port, mRequestArgs.sessionId);
        }
    }
}

void WebrtcTransport::DestroyUdpSocket()
{
    if (mUdpSocket >= 0) {
        lwip_close(mUdpSocket);
        mUdpSocket = -1;
        ChipLogProgress(Camera, "UDP socket closed for sessionID: %u", mRequestArgs.sessionId);
    }
}

// ─── Periodic ICE Tick Timer ──────────────────────────────────────

void WebrtcTransport::OnICETickTimerCallback(chip::System::Layer *systemLayer, void *appState)
{
    auto *self = static_cast<WebrtcTransport *>(appState);
    if (self) {
        self->OnICETick();
    }
}

void WebrtcTransport::OnICETick()
{
    if (mUdpSocket < 0 || mPeerConnection == nullptr) {
        return;
    }

    /* Clear the timer-running flag so StartICETimer() can re-arm */
    mICETimerRunning = false;

    struct ameba_webrtc_session *session = mPeerConnection->GetSession();
    if (session == nullptr) {
        return;
    }

    // Read all available UDP datagrams (non-blocking)
    uint8_t recv_buf[2048];
    struct sockaddr_in src_addr;
    socklen_t src_len = sizeof(src_addr);

    while (true) {
        ssize_t n = lwip_recvfrom(mUdpSocket, recv_buf, sizeof(recv_buf),
                                  MSG_DONTWAIT,
                                  (struct sockaddr *)&src_addr, &src_len);
        if (n <= 0) {
            break; // No more data
        }

        // Process incoming UDP data (ICE STUN, DTLS, RTP, RTCP)
        ameba_webrtc_session_process_udp(session, recv_buf, (size_t)n,
                                         (const struct sockaddr *)&src_addr);
    }

    // Periodic tick for ICE timeouts and DTLS retransmissions
    ameba_webrtc_session_tick(session);

    // Check if session reached a terminal state
    int state = ameba_webrtc_session_get_state(session);

    // Keep the timer running during DTLS handshake (after ICE connects).
    // Only stop when the session reaches a terminal state or socket is closed.
    // DTLS retransmissions need periodic ticks via ameba_dtls_tick().
    if (mUdpSocket >= 0 &&
        state != AMEBA_WEBRTC_STATE_FAILED &&
        state != AMEBA_WEBRTC_STATE_CLOSED) {
        StartICETimer();
    }
}

void WebrtcTransport::StartICETimer()
{
    if (mICETimerRunning) {
        return; // Already running
    }

    CHIP_ERROR err = chip::DeviceLayer::SystemLayer().StartTimer(
                                     chip::System::Clock::Milliseconds32(50),
                                     OnICETickTimerCallback,
                                     this);

    if (err == CHIP_NO_ERROR) {
        mICETimerRunning = true;
    } else {
        ChipLogError(Camera, "Failed to start ICE tick timer for sessionID: %u", mRequestArgs.sessionId);
    }
}

void WebrtcTransport::StopICETimer()
{
    if (!mICETimerRunning) {
        return;
    }

    chip::DeviceLayer::SystemLayer().CancelTimer(OnICETickTimerCallback, this);
    mICETimerRunning = false;
}

// AI generated ends
