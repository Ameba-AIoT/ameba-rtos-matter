/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/

#pragma once

#include <platform_stdlib.h>
#include <functional>
#include <lib/core/DataModelTypes.h>

class MatterBridge
{
public:
    static const int kDeviceNameSize     = 32;
    static const int kDeviceLocationSize     = 32;

    enum Changed_t
    {
        kChanged_Reachable = 0x01,
        kChanged_State     = 0x02,
        kChanged_Location  = 0x04,
        kChanged_Name      = 0x08,
    } Changed;

    void Init(const char *szDeviceName, const char *szLocation);
    bool IsTurnedOn(void);
    void Set(bool state, int call_callback);
    bool IsReachable(void);
    void SetReachable(bool aReachable);
    void SetName(const char *szDeviceName);
    void SetLocation(const char *szLocation);
    inline void SetEndpointId(chip::EndpointId id) { mEndpointId = id; };
    inline chip::EndpointId GetEndpointId() { return mEndpointId; };
    inline char *GetName() { return mName; };
    inline char *GetLocation() { return mLocation; };

    using DeviceCallback_fn = std::function<void(MatterBridge *, Changed_t)>;
    void SetChangeCallback(DeviceCallback_fn aChanged_CB);

private:
    bool mState;
    bool mReachable;
    char mName[kDeviceNameSize];
    char mLocation[kDeviceLocationSize];
    chip::EndpointId mEndpointId;
    DeviceCallback_fn mChanged_CB;
};
