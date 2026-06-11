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
#pragma once

#include <FreeRTOS.h>
#include <platform_stdlib.h>
#include <os_wrapper.h>

#include <camera/ameba_transport.h>
#include <webrtc/ameba_webrtc_abstract.h>
#include <lib/core/DataModelTypes.h>
#include <lib/core/ScopedNodeId.h>
#include <system/SystemLayer.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>

#include <string>
#include <vector>

using OnTransportLocalDescriptionCallback = std::function<void(const std::string &sdp, SDPType type, const int16_t sessionId)>;
using OnTransportConnectionStateCallback  = std::function<void(bool connected, const int16_t sessionId)>;

// Derived class for WebRTC transport
class WebrtcTransport : public Transport
{
public:
    enum class CommandType : uint8_t {
        kUndefined     = 0,
        kOffer         = 1,
        kAnswer        = 2,
        kICECandidates = 3,
        kEnd           = 4,
    };

    enum class State : uint8_t {
        Idle,                 ///< Default state, no communication initiated yet
        SendingOffer,         ///< Sending Offer command from camera
        SendingAnswer,        ///< Sending Answer command from camera
        SendingICECandidates, ///< Sending ICECandidates command from camera
        SendingEnd,           ///< Sending End command from camera
    };

    struct RequestArgs {
        uint16_t sessionId;
        std::vector<uint16_t> videoStreams;
        std::vector<uint16_t> audioStreams;
        chip::NodeId peerNodeId;
        chip::FabricIndex fabricIndex;
        chip::EndpointId originatingEndpointId;
        chip::ScopedNodeId peerId;
    };

    WebrtcTransport();

    ~WebrtcTransport();

    void SetCallbacks(OnTransportLocalDescriptionCallback onLocalDescription, OnTransportConnectionStateCallback onConnectionState);

    void MoveToState(const State targetState);
    const char *GetStateStr() const;

    State GetState()
    {
        return mState;
    }

    // Send video data for a given stream ID
    void SendVideo(const chip::ByteSpan &data, int64_t timestamp, uint16_t videoStreamID) override;

    // Send audio data for a given stream ID
    void SendAudio(const chip::ByteSpan &data, int64_t timestamp, uint16_t audioStreamID) override;

    // Send synchronized audio/video data for given audio and video stream IDs
    void SendAudioVideo(const chip::ByteSpan &data, uint16_t videoStreamID, uint16_t audioStreamID) override;

    // Indicates that the transport is ready to send video data
    bool CanSendVideo() override;

    // Indicates that the transport is ready to send audio data
    bool CanSendAudio() override;

    // Takes care of creation WebRTC peer connection and registering the necessary callbacks
    void Start();

    // Stops WebRTC peer connection and cleanup
    void Stop();

    // Adds video track to the peerconnection with H264 codec with default payload type as 96
    void AddVideoTrack(const std::string &videoMid = "video", int payloadType = 96);

    // Adds audio track to the peerconnection with opus codec with default payload type as 111
    void AddAudioTrack(const std::string &audioMid = "audio", int payloadType = 111);

    // // Set ICE servers to use when creating the underlying PeerConnection. Call this before Start().
    void SetICEServers(const std::vector<ICEServerInfo> &servers);

    std::shared_ptr<WebRTCPeerConnection> GetPeerConnection()
    {
        return mPeerConnection;
    }

    std::string GetLocalDescription()
    {
        return mLocalSdp;
    }

    void SetSdpAnswer(std::string localSdp)
    {
        mLocalSdp = localSdp;
    }

    const std::vector<ICECandidateInfo> &GetCandidates()
    {
        return mLocalCandidates;
    }

    void SetCandidates(std::vector<ICECandidateInfo> candidates)
    {
        mLocalCandidates = candidates;
    }

    void AddRemoteCandidate(const std::string &candidate, const std::string &mid);

    // AI generated starts
    /* Kick off ICE connectivity checks (idempotent). Safe to call from either
     * the ProvideICECandidates path or after SetRemoteDescription parses
     * candidates embedded in the remote SDP. */
    void MaybeStartIce();
    // AI generated ends

    bool ClosePeerConnection();

    void SetCommandType(const CommandType commandtype);

    CommandType GetCommandType()
    {
        return mCommandType;
    }

    // WebRTC Callbacks
    void OnLocalDescription(const std::string &sdp, SDPType type);
    void OnICECandidate(const ICECandidateInfo &candidateInfo);
    void OnConnectionStateChanged(bool connected);
    void OnTrack(std::shared_ptr<WebRTCTrack> track);

    void SetRequestArgs(const RequestArgs &args);
    RequestArgs &GetRequestArgs();

    // AI generated starts
    // Start/Stop dummy media streaming (for testing)
    void StartStreaming();
    void StopStreaming();

    // Check if streaming is active
    bool IsStreaming() const
    {
        return mStreamingActive;
    }
    // AI generated ends

private:

    // AI generated starts
    // ── Dummy Frame Generation Task ────────────────────────────────
    static void StreamingTaskCallback(void *context);
    void StreamingTask();

    // Generate dummy H.264 video frame (simple colored pattern as IDR)
    int GenerateDummyH264Frame(uint8_t *buffer, size_t bufferSize, uint32_t timestamp);

    // Generate dummy Opus audio frame (simple tone)
    int GenerateDummyOpusFrame(uint8_t *buffer, size_t bufferSize, uint32_t timestamp);

    // Streaming control
    bool mStreamingActive = false;
    rtos_task_t mStreamingTaskHandle = NULL;

    // Static buffers for streaming task (to avoid stack overflow)
    static uint8_t sVideoBuffer[1400];
    static uint8_t sAudioBuffer[256];

    // ── ICE/UDP Transport ─────────────────────────────────────────
    void InitUdpSocket();
    void DestroyUdpSocket();

    // ── Periodic ICE Tick Timer ────────────────────────────────────
    static void OnICETickTimerCallback(chip::System::Layer *systemLayer, void *appState);
    void OnICETick();
    void StartICETimer();
    void StopICETimer();

    // ── Member variables ──────────────────────────────────────────
    int mUdpSocket           = -1;
    bool mIceStarted         = false;
    bool mICETimerRunning    = false;
    // AI generated ends

    CommandType mCommandType = CommandType::kUndefined;
    State mState             = State::Idle;

    std::shared_ptr<WebRTCPeerConnection> mPeerConnection;

    std::shared_ptr<WebRTCTrack> mLocalVideoTrack;
    std::shared_ptr<WebRTCTrack> mLocalAudioTrack;

    std::string mLocalSdp;
    SDPType mLocalSdpType;
    std::vector<ICECandidateInfo> mLocalCandidates;

    RequestArgs mRequestArgs;
    OnTransportLocalDescriptionCallback mOnLocalDescription = nullptr;
    OnTransportConnectionStateCallback mOnConnectionState   = nullptr;
    std::vector<ICEServerInfo> mICEServers;

    rtos_mutex_t mTrackStatusLock = NULL;
};
