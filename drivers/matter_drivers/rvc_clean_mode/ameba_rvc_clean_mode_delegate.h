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
#include <app/clusters/mode-base-server/mode-base-server.h>

namespace chip {
namespace app {
namespace Clusters {
namespace RvcCleanMode {

const uint8_t ModeVacuum    = 0;
const uint8_t ModeWash      = 1;
const uint8_t ModeDeepClean = 2;

class AmebaRvcCleanModeDelegate : public ModeBase::Delegate
{
private:
    using ModeTagStructType            = detail::Structs::ModeTagStruct::Type;
    ModeTagStructType modeTagsVac[1]   = { { .value = to_underlying(ModeTag::kVacuum) } };
    ModeTagStructType modeTagsMop[1]   = { { .value = to_underlying(ModeTag::kMop) } };
    ModeTagStructType modeTagsBoost[2] = { { .value = to_underlying(ModeBase::ModeTag::kMax) },
                                           { .value = to_underlying(ModeTag::kDeepClean) } };

    const detail::Structs::ModeOptionStruct::Type kModeOptions[3] = {
        detail::Structs::ModeOptionStruct::Type{ .label    = CharSpan::fromCharString("Vacuum"),
                                                 .mode     = ModeVacuum,
                                                 .modeTags = DataModel::List<const ModeTagStructType>(modeTagsVac) },
        detail::Structs::ModeOptionStruct::Type{ .label    = CharSpan::fromCharString("Wash"),
                                                 .mode     = ModeWash,
                                                 .modeTags = DataModel::List<const ModeTagStructType>(modeTagsMop) },
        detail::Structs::ModeOptionStruct::Type{ .label    = CharSpan::fromCharString("Deep clean"),
                                                 .mode     = ModeDeepClean,
                                                 .modeTags = DataModel::List<const ModeTagStructType>(modeTagsBoost) },
    };

    CHIP_ERROR Init() override;
    void HandleChangeToMode(uint8_t mode, ModeBase::Commands::ChangeToModeResponse::Type & response) override;

    CHIP_ERROR GetModeLabelByIndex(uint8_t modeIndex, MutableCharSpan & label) override;
    CHIP_ERROR GetModeValueByIndex(uint8_t modeIndex, uint8_t & value) override;
    CHIP_ERROR GetModeTagsByIndex(uint8_t modeIndex, DataModel::List<ModeTagStructType> & tags) override;

public:
    ~AmebaRvcCleanModeDelegate() override = default;
};

AmebaRvcCleanModeDelegate * GetAmebaRvcCleanModeDelegate(void);
CHIP_ERROR AmebaRvcCleanModeDelegateInit(EndpointId endpoint);
void AmebaRvcCleanModeDelegateShutdown(void);

} // namespace RvcCleanMode
} // namespace Clusters
} // namespace app
} // namespace chip
