#include <algorithm>

#include <bridge_driver.h>

#include <support/logging/CHIPLogging.h>

void MatterBridge::Init(const char *szDeviceName, const char *szLocation)
{
    memcpy(mName, szDeviceName, sizeof(mName));
    memcpy(mLocation, szLocation, sizeof(mLocation));
    ChipLogProgress(DeviceLayer, "Setting up Device[%s] at %s", mName, szLocation);

    mReachable  = false;
    mEndpointId = 0;
    mChanged_CB = nullptr;
}

bool MatterBridge::IsTurnedOn()
{
    return this->mState;
}

void MatterBridge::Set(bool state, int call_callback)
{
    bool changed;

    changed = (mState != state);
    mState = state;

    ChipLogProgress(DeviceLayer, "Device[%s]: %s", mName, state ? "ON" : "OFF");

    if ((changed) && (mChanged_CB && call_callback))
    {
        mChanged_CB(this, kChanged_State);
    }
}

bool MatterBridge::IsReachable()
{
    return this->mReachable;
}

void MatterBridge::SetReachable(bool aReachable)
{
    if (mReachable == aReachable)
    {
        return;
    }

    mReachable = aReachable;

    if (mReachable)
    {
        ChipLogProgress(DeviceLayer, "Device[%s]: ONLINE", mName);
    } 
    else
    {
        ChipLogProgress(DeviceLayer, "Device[%s]: OFFLINE", mName);
    }
}

void MatterBridge::SetName(const char *szDeviceName)
{
    bool changed = (strncmp(mName, szDeviceName, sizeof(mName)) != 0);

    ChipLogProgress(DeviceLayer, "Device[%s]: New Name=\"%s\"", mName, szDeviceName);

    memcpy(mName, szDeviceName, sizeof(mName));

    if (changed && mChanged_CB)
    {
        mChanged_CB(this, kChanged_Name);
    }
}
void MatterBridge::SetLocation(const char *szLocation)
{
    bool changed = (strncmp(mLocation, szLocation, sizeof(mLocation)) != 0);

    memcpy(mLocation, szLocation, sizeof(mLocation));

    ChipLogProgress(DeviceLayer, "Device[%s]: Location=\"%s\"", mName, mLocation);

    if (changed && mChanged_CB)
    {
        mChanged_CB(this, kChanged_Location);
    }
}

void MatterBridge::SetChangeCallback(DeviceCallback_fn aChanged_CB)
{
    mChanged_CB = aChanged_CB;
}
