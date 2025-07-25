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

#include <app/util/af-types.h>
#include <lib/core/CHIPCore.h>
#include <lib/core/CHIPError.h>

namespace chip {
namespace app {
namespace Clusters {

class AmebaDeviceManager
{
public:
    AmebaDeviceManager(EndpointId endpointId) : mEndpointId(endpointId) {}

    static void InitInstance(EndpointId aEndpointId)
    {
        if (mInstance == nullptr)
        {
            mInstance = new AmebaDeviceManager(aEndpointId);
        }
    };

    static AmebaDeviceManager *GetInstance()
    {
        return mInstance;
    };

    void AmebaPostAttributeChangeCallback(EndpointId endpoint, ClusterId clusterId, AttributeId attributeId,
                                          uint8_t type, uint16_t size, uint8_t *value);

private:
    inline static AmebaDeviceManager *mInstance;
    EndpointId mEndpointId;
};

} // namespace Clusters
} // namespace app
} // namespace chip
