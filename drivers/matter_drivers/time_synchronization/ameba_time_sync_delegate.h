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

#include <app/clusters/time-synchronization-server/time-synchronization-delegate.h>

namespace chip::app::Clusters::TimeSynchronization {

class AmebaTimeSyncDelegate : public Delegate
{

public:
    AmebaTimeSyncDelegate() : Delegate() {};

    /**
     * @brief Check whether an NTP address string represents a valid IPv6 literal.
     * @param ntp  NTP server address as a CharSpan.
     * @return true if the string parses as a valid IPv6 address, false otherwise.
     */
    bool IsNTPAddressValid(CharSpan ntp) override;

    /**
     * @brief Check whether an NTP address string is a domain name (not a numeric literal).
     * @param ntp  NTP server address as a CharSpan.
     * @return true if the string contains a '.' and is not a valid IP literal, false otherwise.
     */
    bool IsNTPAddressDomain(CharSpan ntp) override;

    /**
     * @brief Attempt to obtain the current time from the platform's SNTP/RTC source (priority 1).
     *        Reads the system real-time clock via GetClock_RealTime(). If
     *        mSimulatePlatformSourceUnavailable is set, reports unavailable unconditionally so
     *        that priority 2/3 paths can be tested without disabling SNTP at the platform level.
     * @param callback  Invoked with (TimeSource, Granularity) on success; not called on failure.
     * @return CHIP_NO_ERROR on success; CHIP_ERROR_NOT_IMPLEMENTED when no valid time is available.
     */
    CHIP_ERROR UpdateTimeFromPlatformSource(chip::Callback::Callback<OnTimeSyncCompletion> *callback) override;

    /**
     * @brief Attempt to sync time from the cluster-provided DefaultNTP server (priority 3).
     *        Restarts the SNTP client using the cluster-provided fallbackNTP server, then polls
     *        SNTP_GET_SYSTEM_TIME every kPollIntervalMs ms for up to kMaxPollAttempts attempts.
     *        On completion (success or timeout) the cluster callback is invoked. The SNTP client
     *        is left pointed at fallbackNTP (the configured DefaultNTP server); it is not reverted
     *        to the built-in default. Any poll loop already in progress (including a background
     *        periodic resync) is cancelled first so only one poll loop runs at a time.
     * @param fallbackNTP  NTP server domain or IPv6 address from the DefaultNTP cluster attribute.
     * @param callback     Invoked with (bool success) when the sync attempt finishes.
     * @return CHIP_NO_ERROR if the async poll was started; CHIP_ERROR_INVALID_ARGUMENT if
     *         fallbackNTP is empty, too long, or neither a valid IP nor a domain name;
     *         CHIP_ERROR_NOT_IMPLEMENTED when CONFIG_ENABLE_AMEBA_SNTP is disabled.
     */
    CHIP_ERROR UpdateTimeUsingNTPFallback(const CharSpan &fallbackNTP,
                                          Callback::Callback<OnFallbackNTPCompletion> *callback) override;

    /**
     * @brief Test hook: force UpdateTimeFromPlatformSource to report unavailable, allowing
     *        priority 2 (trusted node) and priority 3 (NTP fallback) paths to be exercised
     *        without disabling CONFIG_ENABLE_AMEBA_SNTP at build time.
     * @param simulate  true to suppress priority-1, false to restore normal behaviour.
     */
    void SetSimulatePlatformSourceUnavailable(bool simulate)
    {
        mSimulatePlatformSourceUnavailable = simulate;
    }

    /**
     * @brief Test hook: force time resnyc interval to 10 minutes for quick testing
     * @param quickTest  true to set resync interval to 10 minutes, false to restore normal behaviour.
     */
    void SetTimeResyncQuickTest(bool quickTest)
    {
        mTimeResyncQuickTest = quickTest;
    }

    /**
     * @brief Periodically refresh the system clock via SNTP while the ICD is in active mode.
     *        Rate-limited by kResyncIntervalMs; safe to call on every OnEnterActiveMode() event.
     *        Reuses the existing PollSntpResult / OnPollTick / CompleteFallback poll loop with
     *        mFallbackCallback left as nullptr, so the cluster is not notified on completion.
     *        No-op when CONFIG_ENABLE_AMEBA_SNTP is disabled or any poll loop is already running.
     * @param none
     * @return void
     */
    void TryPeriodicResync();

private:
    static constexpr size_t kMaxServerNameSize           = 128;
    static constexpr uint16_t kPollIntervalMs            = 500;
    static constexpr uint8_t kMaxPollAttempts            = 20;           // ~10s total
    static constexpr uint64_t kResyncIntervalMs          = 6ULL * 3600ULL * 1000ULL; // 6 hours
    static constexpr uint64_t kQuickTestResyncIntervalMs = 10ULL * 60ULL * 1000ULL; // 10 minutes, for quick test

    bool mSimulatePlatformSourceUnavailable = false;
    bool mTimeResyncQuickTest               = false;
    uint64_t mLastResyncAttemptMs           = 0;

#if defined(CONFIG_ENABLE_AMEBA_SNTP) && (CONFIG_ENABLE_AMEBA_SNTP == 1)
    // --- NTP fallback (UpdateTimeUsingNTPFallback) state ---
    char mServerNameBuffer[kMaxServerNameSize]                      = { 0 };
    Callback::Callback<OnFallbackNTPCompletion> *mFallbackCallback = nullptr;
    uint8_t mPollAttemptsRemaining                                  = 0;

    static void PollSntpResult(System::Layer *layer, void *appState);
    void OnPollTick();
    void CompleteFallback(bool success);

#endif
};

} // namespace chip::app::Clusters::TimeSynchronization
