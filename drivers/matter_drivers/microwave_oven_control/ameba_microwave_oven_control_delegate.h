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

#include <app-common/zap-generated/cluster-objects.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/clusters/microwave-oven-control-server/microwave-oven-control-server.h>
#include <protocols/interaction_model/StatusCode.h>

namespace chip {
namespace app {
namespace Clusters {
namespace MicrowaveOvenControl {

class AmebaMicrowaveOvenControlDelegate : public MicrowaveOvenControl::Delegate
{
public:
    Protocols::InteractionModel::Status HandleSetCookingParametersCallback(uint8_t cookMode, uint32_t cookTimeSec,
                                                                           bool startAfterSetting,
                                                                           Optional<uint8_t> powerSettingNum,
                                                                           Optional<uint8_t> wattSettingIndex) override;
    Protocols::InteractionModel::Status HandleModifyCookTimeSecondsCallback(uint32_t finalcookTimeSec) override;


    CHIP_ERROR GetWattSettingByIndex(uint8_t index, uint16_t & wattSetting) override;
    uint8_t GetPowerSettingNum() const override { return mPowerSettingNum; }
    uint8_t GetMinPowerNum() const override { return kMinPowerNum; }
    uint8_t GetMaxPowerNum() const override { return kMaxPowerNum; }
    uint8_t GetPowerStepNum() const override { return kPowerStepNum; }
    uint32_t GetMaxCookTimeSec() const override { return kMaxCookTimeSec; }
    uint8_t GetCurrentWattIndex() const override { return mSelectedWattIndex; };
    uint16_t GetWattRating() const override { return mWattRating; };

private:
    static constexpr uint8_t kMinPowerNum            = 20u;
    static constexpr uint8_t kMaxPowerNum            = 90u;
    static constexpr uint8_t kPowerStepNum           = 10u;
    static constexpr uint32_t kMaxCookTimeSec        = 86400u;
    static constexpr uint8_t kDefaultPowerSettingNum = kMaxPowerNum;

    static constexpr uint8_t kModeNormal  = 0u;
    static constexpr uint8_t kModeDefrost = 1u;

    static constexpr uint16_t kExampleWatt1 = 100u;
    static constexpr uint16_t kExampleWatt2 = 300u;
    static constexpr uint16_t kExampleWatt3 = 500u;
    static constexpr uint16_t kExampleWatt4 = 800u;
    static constexpr uint16_t kExampleWatt5 = 1000u;

    uint8_t mPowerSettingNum   = kDefaultPowerSettingNum;
    uint8_t mSelectedWattIndex = 0;
    uint16_t mWattRating       = 0;

    const uint16_t mWattSettingList[5] = { kExampleWatt1, kExampleWatt2, kExampleWatt3, kExampleWatt4, kExampleWatt5 };
};

AmebaMicrowaveOvenControlDelegate * GetAmebaMicrowaveOvenControlDelegate(void);
CHIP_ERROR AmebaMicrowaveOvenControlDelegateInit(EndpointId endpoint);
void AmebaMicrowaveOvenControlDelegateShutdown(void);

} // namespace MicrowaveOvenControl
} // namespace Clusters
} // namespace app
} // namespace chip
