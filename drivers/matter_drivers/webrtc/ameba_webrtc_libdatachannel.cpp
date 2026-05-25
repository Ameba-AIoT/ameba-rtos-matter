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

#include <webrtc/ameba_webrtc_abstract.h>
#include <lib/support/logging/CHIPLogging.h>

namespace {

class LibDataChannelPeerConnection : public WebRTCPeerConnection
{
public:
    LibDataChannelPeerConnection(const std::vector<ICEServerInfo> & servers = {})
    {
        ChipLogProgress(DeviceLayer, "Please implement LibDataChannelPeerConnection properly!!!");
    }

    void SetCallbacks(OnLocalDescriptionCallback onLocalDescription, OnICECandidateCallback onICECandidate,
                              OnConnectionStateCallback onConnectionState, OnTrackCallback onTrack)
    {
        ChipLogProgress(DeviceLayer, "Please implement SetCallbacks properly!!!");
    }

    void Close()
    {
        ChipLogProgress(DeviceLayer, "Please implement Close properly!!!");
    }

    void CreateOffer()
    {
        ChipLogProgress(DeviceLayer, "Please implement CreateOffer properly!!!");
    }

    void CreateAnswer()
    {
        ChipLogProgress(DeviceLayer, "Please implement CreateAnswer properly!!!");
    }

    void SetRemoteDescription(const std::string & sdp, SDPType type)
    {
        ChipLogProgress(DeviceLayer, "Please implement SetRemoteDescription properly!!!");
    }

    void AddRemoteCandidate(const std::string & candidate, const std::string & mid)
    {
        ChipLogProgress(DeviceLayer, "Please implement AddRemoteCandidate properly!!!");
    }

    std::shared_ptr<WebRTCTrack> AddTrack(MediaType mediaType, const std::string & mid, int payloadType)
    {
        ChipLogProgress(DeviceLayer, "Please implement AddTrack properly!!!");
        return nullptr;
    }

    int GetPayloadType(const std::string & sdp, SDPType type, const std::string & codec)
    {
        ChipLogProgress(DeviceLayer, "Please implement GetPayloadType properly!!!");
        return -1;
    }

};

} // namespace

std::shared_ptr<WebRTCPeerConnection> CreateWebRTCPeerConnection(const std::vector<ICEServerInfo> & iceServers)
{
    return std::make_shared<LibDataChannelPeerConnection>(iceServers);
}
