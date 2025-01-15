/*
 *    Copyright (c) 2022 Project CHIP Authors
 *    Copyright (c) 2024 Realtek Semiconductor Corporation
 *    All rights reserved.
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
 *
 *    This file includes code originally developed by the Project CHIP Authors
 *    and has been modified by Realtek.
 */

#pragma once

#if defined(CONFIG_ENABLE_AMEBA_MDNS_FILTER) && (CONFIG_ENABLE_AMEBA_MDNS_FILTER == 1)

#include <atomic>
#include <inet/EndpointQueueFilter.h>
#include <inet/IPPacketInfo.h>
#include <lib/support/CodeUtils.h>
#include <system/SystemPacketBuffer.h>

namespace chip {
namespace Inet {

/**
 * @brief Basic filter that counts how many pending (not yet dequeued) packets
 *        are accumulated that match a predicate function, and drops those that
 *        would cause crossing of the threshold.
 */
class AmebaEndpointQueueFilter : public chip::Inet::EndpointQueueFilter
{
public:
    /**
     * @brief Implementation of filtering before queueing that applies the predicate.
     *
     * See base class for arguments
     */
    FilterOutcome FilterBeforeEnqueue(const void * endpoint, const chip::Inet::IPPacketInfo & pktInfo,
                                      const chip::System::PacketBufferHandle & pktPayload) override
    {
        // Drop all non-matter mDNS packets
        if(pktInfo.DestPort == 5353 && PayloadContainsComparison(pktPayload) == false)
        {
            ChipLogProgress(DeviceLayer, "Not Matter mDNS...");
            return FilterOutcome::kDropPacket;
        }

        // Drop all IPv4 mDNS packets
        if(pktInfo.DestAddress.IsIPv4() && pktInfo.DestPort == 5353)
        {
            ChipLogProgress(DeviceLayer, "IPv4 MDNS packet dropped...");
            return FilterOutcome::kDropPacket;
        }

        return FilterOutcome::kAllowPacket;
    }

    /**
     * @brief Implementation of filtering after dequeueing that applies the predicate.
     *
     * See base class for arguments
     */
    FilterOutcome FilterAfterDequeue(const void * endpoint, const chip::Inet::IPPacketInfo & pktInfo,
                                     const chip::System::PacketBufferHandle & pktPayload) override
    {
        return FilterOutcome::kAllowPacket;
    }

private:
    static bool PayloadContainsComparison(const chip::System::PacketBufferHandle & payload)
    {
        const uint8_t matterBytes[] = { 'm', 'a', 't', 't', 'e', 'r' };
        size_t matterLength = sizeof(matterBytes);

        for (size_t i = 0; i <= payload->TotalLength() - matterLength; i++)
        {
            if (memcmp(payload->Start() + i, matterBytes, matterLength) == 0) {
                return true;
            }
        }
        return false;
    }
};

} // namespace Inet
} // namespace chip

#endif /* CONFIG_ENABLE_AMEBA_MDNS_FILTER */
