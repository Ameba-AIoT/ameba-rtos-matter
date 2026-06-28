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
#include <cJSON.h>

#include <bridge_dm_driver.h>

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <support/logging/CHIPLogging.h>

void MatterBridge::Init(Node &mNode)
{
    // start polling task to poll for messages from bridged device
    node = &mNode;
}

chip::EndpointId MatterBridge::addBridgedEndpoint(EndpointConfig bridgedConfig, Span<const EmberAfDeviceType> bridgedDeviceType)
{
    chip::EndpointId assignedEndpoint = node->addEndpoint(bridgedConfig, bridgedDeviceType);
    node->enableAllEndpoints();
    return assignedEndpoint;
}

void MatterBridge::removeBridgedEndpoint(chip::EndpointId endpointID)
{
    Endpoint *ep = node->getEndpoint(endpointID);
    ep->disableEndpoint();
    node->removeEndpoint(endpointID);
}

MatterBridgeDevice::MatterBridgeDevice(const char *szDeviceName, const char *szLocation)
{
    memcpy(mName, szDeviceName, sizeof(mName));
    memcpy(mLocation, szLocation, sizeof(mLocation));
    ChipLogProgress(DeviceLayer, "Setting up Device[%s] at %s", mName, mLocation);

    mReachable  = false;
    mEndpointId = 0;
}

bool MatterBridgeDevice::IsReachable(void)
{
    return this->mReachable;
}

void MatterBridgeDevice::SetReachable(bool aReachable)
{
    bool changed = (mReachable != aReachable);

    if (mReachable == aReachable) {
        return;
    }

    mReachable = aReachable;

    if (mReachable) {
        ChipLogProgress(DeviceLayer, "Device[%s]: ONLINE", mName);
    } else {
        ChipLogProgress(DeviceLayer, "Device[%s]: OFFLINE", mName);
    }

    if (changed) {
        HandleDeviceChange(this, kChanged_Reachable);
    }
}

void MatterBridgeDevice::SetName(const char *szDeviceName)
{
    bool changed = (strncmp(mName, szDeviceName, sizeof(mName)) != 0);

    ChipLogProgress(DeviceLayer, "Device[%s]: New Name=\"%s\"", mName, szDeviceName);

    memcpy(mName, szDeviceName, sizeof(mName));

    if (changed) {
        HandleDeviceChange(this, kChanged_Name);
    }
}

void MatterBridgeDevice::SetLocation(const char *szLocation)
{
    bool changed = (strncmp(mLocation, szLocation, sizeof(mLocation)) != 0);

    memcpy(mLocation, szLocation, sizeof(mLocation));

    ChipLogProgress(DeviceLayer, "Device[%s]: Location=\"%s\"", mName, mLocation);

    if (changed) {
        HandleDeviceChange(this, kChanged_Location);
    }
}

/***** Device Type: ONOFF *****/

MatterBridgedDeviceOnOff::MatterBridgedDeviceOnOff(const char *szDeviceName, const char *szLocation) : MatterBridgeDevice(szDeviceName, szLocation)
{
    mState = false;
}

bool MatterBridgedDeviceOnOff::IsTurnedOn(void)
{
    return this->mState;
}

void MatterBridgedDeviceOnOff::Set(bool state, int call_callback)
{
    bool changed;

    changed = (mState != state);
    mState = state;

    ChipLogProgress(DeviceLayer, "Device[%s]: %s", mName, state ? "ON" : "OFF");

    if ((changed) && (mChanged_CB && call_callback)) {
        mChanged_CB(this, kChanged_OnOff);
    }
}

void MatterBridgedDeviceOnOff::SetChangeCallback(DeviceCallback_fn aChanged_CB)
{
    mChanged_CB = aChanged_CB;
}

void MatterBridgedDeviceOnOff::HandleDeviceChange(MatterBridgeDevice *device, MatterBridgeDevice::Changed_t changeMask)
{
    if (mChanged_CB) {
        mChanged_CB(this, (MatterBridgedDeviceOnOff::Changed_t) changeMask);
    }
}
