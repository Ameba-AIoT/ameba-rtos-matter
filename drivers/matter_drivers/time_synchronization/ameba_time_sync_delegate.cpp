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
#include <cstring>

#include <app/clusters/time-synchronization-server/CodegenIntegration.h>
#include <app/clusters/time-synchronization-server/time-synchronization-delegate.h>
#include <inet/IPAddress.h>
#include <lib/core/CHIPCallback.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceLayer.h>
#include <system/SystemClock.h>

#include <time_synchronization/ameba_time_sync_delegate.h>

#include <chip_porting.h>
#include <sntp/sntp_api.h>

using namespace ::chip;
using namespace ::chip::app::Clusters::TimeSynchronization;

bool AmebaTimeSyncDelegate::IsNTPAddressValid(CharSpan ntp)
{
    Inet::IPAddress addr;
    return Inet::IPAddress::FromString(ntp.data(), ntp.size(), addr) && addr.IsIPv6();
}

bool AmebaTimeSyncDelegate::IsNTPAddressDomain(CharSpan ntp)
{
    return !IsNTPAddressValid(ntp) && (memchr(ntp.data(), '.', ntp.size()) != nullptr);
}

CHIP_ERROR AmebaTimeSyncDelegate::UpdateTimeFromPlatformSource(Callback::Callback<OnTimeSyncCompletion> *callback)
{
    if (mSimulatePlatformSourceUnavailable) {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }

    System::Clock::Microseconds64 utcTime;
    if (System::SystemClock().GetClock_RealTime(utcTime) != CHIP_NO_ERROR) {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }
    callback->mCall(callback->mContext, TimeSourceEnum::kMixedNTP, GranularityEnum::kMillisecondsGranularity);
    return CHIP_NO_ERROR;
}

CHIP_ERROR AmebaTimeSyncDelegate::UpdateTimeUsingNTPFallback(const CharSpan &fallbackNTP,
        Callback::Callback<OnFallbackNTPCompletion> *callback)
{
#if defined(CONFIG_ENABLE_AMEBA_SNTP) && (CONFIG_ENABLE_AMEBA_SNTP == 1)
    if (fallbackNTP.empty() || fallbackNTP.size() >= sizeof(mServerNameBuffer)) {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    if (!IsNTPAddressValid(fallbackNTP) && !IsNTPAddressDomain(fallbackNTP)) {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    DeviceLayer::SystemLayer().CancelTimer(PollSntpResult, this);

    memset(mServerNameBuffer, 0, kMaxServerNameSize);
    memcpy(mServerNameBuffer, fallbackNTP.data(), fallbackNTP.size());
    mServerNameBuffer[fallbackNTP.size()] = '\0';
    mFallbackCallback                     = callback;
    mPollAttemptsRemaining                = kMaxPollAttempts;

    matter_sntp_init_with_server(mServerNameBuffer);

    ChipLogProgress(DeviceLayer, "TimeSync: attempting NTP fallback sync against %s", mServerNameBuffer);
    DeviceLayer::SystemLayer().StartTimer(System::Clock::Milliseconds32(kPollIntervalMs), PollSntpResult, this);
    return CHIP_NO_ERROR;
#else
    (void) fallbackNTP;
    (void) callback;
    return CHIP_ERROR_NOT_IMPLEMENTED;
#endif
}

void AmebaTimeSyncDelegate::TryPeriodicResync()
{
#if defined(CONFIG_ENABLE_AMEBA_SNTP) && (CONFIG_ENABLE_AMEBA_SNTP == 1)
    if (mFallbackCallback != nullptr || mPollAttemptsRemaining > 0) {
        ChipLogDetail(DeviceLayer, "TimeSync: resync skipped, poll in progress (callback=%p attempts=%u)", mFallbackCallback,
                      static_cast<unsigned>(mPollAttemptsRemaining));
        return;
    }

    uint64_t nowMs = System::SystemClock().GetMonotonicMilliseconds64().count();
    uint64_t deltaMs = nowMs - mLastResyncAttemptMs;
    uint64_t chosenResyncIntervalMs = 0;
    if (mTimeResyncQuickTest) {
        chosenResyncIntervalMs = kQuickTestResyncIntervalMs;
    } else {
        chosenResyncIntervalMs = kResyncIntervalMs;
    }

    ChipLogDetail(DeviceLayer, "TimeSync: resync check now=%llu last=%llu delta=%llu target=%llu", nowMs,
                  mLastResyncAttemptMs, deltaMs, chosenResyncIntervalMs);
    // Skip if still below chosenResyncIntervalMs
    if (deltaMs < chosenResyncIntervalMs) {
        return;
    }

    mLastResyncAttemptMs   = nowMs;
    mPollAttemptsRemaining = kMaxPollAttempts;

    if (mServerNameBuffer[0] == '\0') { // mServerNameBuffer was not initialized by UpdateTimeUsingNTPFallback, set it to pool.ntp.org
        strncpy(mServerNameBuffer, "pool.ntp.org", kMaxServerNameSize - 1);
        mServerNameBuffer[kMaxServerNameSize - 1] = '\0';
    }

    matter_sntp_init_with_server(mServerNameBuffer);

    ChipLogProgress(DeviceLayer, "TimeSync: starting periodic resync");
    DeviceLayer::SystemLayer().StartTimer(System::Clock::Milliseconds32(kPollIntervalMs), PollSntpResult, this);
#endif
}

#if defined(CONFIG_ENABLE_AMEBA_SNTP) && (CONFIG_ENABLE_AMEBA_SNTP == 1)
void AmebaTimeSyncDelegate::PollSntpResult(System::Layer *layer, void *appState)
{
    static_cast<AmebaTimeSyncDelegate *>(appState)->OnPollTick();
}

void AmebaTimeSyncDelegate::OnPollTick()
{
    time_t seconds = 0, uSeconds = 0;
    uint32_t sec = 0, uSec = 0;
    SNTP_GET_SYSTEM_TIME(sec, uSec);
    seconds  = (time_t)sec;
    uSeconds = (time_t)uSec;

    if ((seconds >= CHIP_SYSTEM_CONFIG_VALID_REAL_TIME_THRESHOLD) && (uSeconds >= 0)) {
        System::SystemClock().SetClock_RealTime(
                        System::Clock::Microseconds64((static_cast<uint64_t>(seconds) * UINT64_C(1000000)) + static_cast<uint64_t>(uSeconds)));
        ChipLogProgress(DeviceLayer, "TimeSync: NTP sync succeeded against %s", mServerNameBuffer);
        CompleteFallback(true);
        return;
    }

    if (mPollAttemptsRemaining == 0 || --mPollAttemptsRemaining == 0) {
        ChipLogError(DeviceLayer, "TimeSync: NTP sync against %s timed out", mServerNameBuffer);
        CompleteFallback(false);
        return;
    }

    DeviceLayer::SystemLayer().StartTimer(System::Clock::Milliseconds32(kPollIntervalMs), PollSntpResult, this);
}

void AmebaTimeSyncDelegate::CompleteFallback(bool success)
{
    mPollAttemptsRemaining = 0;
    auto *callback   = mFallbackCallback;
    mFallbackCallback = nullptr;
    if (callback != nullptr) {
        callback->mCall(callback->mContext, success);
    }
}

#endif
