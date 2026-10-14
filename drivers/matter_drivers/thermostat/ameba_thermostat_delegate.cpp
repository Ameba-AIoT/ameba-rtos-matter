/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
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
#include <thermostat/ameba_thermostat_delegate.h>

#include "app/data-model/Nullable.h"
#include "app/server-cluster/ServerClusterContext.h"

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/persistence/AttributePersistence.h>
#include <app/reporting/reporting.h>
#include <app/server/Server.h>
#include <lib/support/Span.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/internal/CHIPDeviceLayerInternal.h>

#include <app/clusters/thermostat-server/Temperature.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Thermostat;
using namespace chip::app::Clusters::Thermostat::Attributes;
using namespace chip::app::Clusters::Thermostat::Structs;
using namespace Protocols::InteractionModel;
using namespace System::Clock;

namespace {
constexpr EndpointId gThermostatEndpoint(1);

ThermostatDelegate gThermostatDelegate(gThermostatEndpoint);
ThermostatSetpointsDelegate gSetpointsDelegate(gThermostatEndpoint);
} // namespace

CHIP_ERROR Thermostat::AmebaThermostatDelegateInit(EndpointId endpoint)
{
    Clusters::Thermostat::ServerInit(endpoint, gThermostatDelegate, gSetpointsDelegate);

    return CHIP_NO_ERROR;
}

/* -------------------------------------------------------------------------- */
/* ThermostatDelegate                                                          */
/* -------------------------------------------------------------------------- */

FabricTable &ThermostatDelegate::GetFabricTable() const
{
    return mFabricTable != nullptr ? *mFabricTable : Server::GetInstance().GetFabricTable();
}

CHIP_ERROR ThermostatDelegate::Startup(ServerClusterContext &context)
{
    AttributePersistenceProvider *provider = mProvider != nullptr ? mProvider : GetAttributePersistenceProvider();
    VerifyOrReturnError(provider != nullptr, CHIP_ERROR_PERSISTED_STORAGE_FAILED);
    AttributePersistence persistence(*provider);

    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, SystemMode::Id }, mSystemMode, mSystemMode);
    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, ControlSequenceOfOperation::Id }, mControlSequenceOfOperation,
                                      mControlSequenceOfOperation);

    uint8_t remoteSensing = 0;
    if (persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, RemoteSensing::Id }, remoteSensing, remoteSensing)) {
        mRemoteSensing = BitMask<RemoteSensingBitmap>(remoteSensing);
    }

    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, LocalTemperatureCalibration::Id },
                                      mLocalTemperatureCalibration, mLocalTemperatureCalibration);

    return CHIP_NO_ERROR;
}

SystemModeEnum ThermostatDelegate::GetSystemMode() const
{
    return mSystemMode;
}

Protocols::InteractionModel::Status ThermostatDelegate::SetSystemMode(SystemModeEnum systemMode, bool &changed)
{
    changed = false;
    if (mSystemMode == systemMode) {
        return Status::Success;
    }
    AttributePersistenceProvider *provider = mProvider != nullptr ? mProvider : GetAttributePersistenceProvider();
    VerifyOrReturnError(provider != nullptr, Status::InvalidInState);
    AttributePersistence persistence(*provider);
    CHIP_ERROR result = persistence.StoreNativeEndianValue({ mEndpointId, Thermostat::Id, SystemMode::Id }, systemMode);
    if (result != CHIP_NO_ERROR) {
        ChipLogError(Zcl, "Failed to store SystemMode attribute");
        return Status::Failure;
    }
    mSystemMode = systemMode;
    changed     = true;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatDelegate::GetRunningMode(ThermostatRunningModeEnum &runningMode) const
{
    runningMode = mRunningMode;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatDelegate::SetRunningMode(ThermostatRunningModeEnum runningMode, bool &changed)
{
    changed = false;
    if (mRunningMode == runningMode) {
        return Status::Success;
    }
    mRunningMode = runningMode;
    changed      = true;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatDelegate::GetRunningState(BitMask<RelayStateBitmap> &runningState) const
{
    runningState = mRunningState;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatDelegate::SetRunningState(BitMask<RelayStateBitmap> runningState, bool &changed)
{
    changed = false;
    if (mRunningState == runningState) {
        return Status::Success;
    }
    mRunningState = runningState;
    changed       = true;
    return Status::Success;
}

ControlSequenceOfOperationEnum ThermostatDelegate::GetControlSequenceOfOperation() const
{
    return mControlSequenceOfOperation;
}

Protocols::InteractionModel::Status ThermostatDelegate::SetControlSequenceOfOperation(ControlSequenceOfOperationEnum seq,
        bool &changed)
{
    changed = false;
    if (mControlSequenceOfOperation == seq) {
        return Status::Success;
    }
    AttributePersistenceProvider *provider = mProvider != nullptr ? mProvider : GetAttributePersistenceProvider();
    VerifyOrReturnError(provider != nullptr, Status::InvalidInState);
    AttributePersistence persistence(*provider);
    CHIP_ERROR result = persistence.StoreNativeEndianValue({ mEndpointId, Thermostat::Id, ControlSequenceOfOperation::Id }, seq);
    if (result != CHIP_NO_ERROR) {
        ChipLogError(Zcl, "Failed to store ControlSequenceOfOperation attribute");
        return Status::Failure;
    }
    mControlSequenceOfOperation = seq;
    changed                     = true;
    return Status::Success;
}

DataModel::Nullable<temperature> ThermostatDelegate::GetLocalTemperature() const
{
    return mLocalTemperature;
}

Protocols::InteractionModel::Status ThermostatDelegate::SetLocalTemperature(DataModel::Nullable<temperature> temp, bool &changed)
{
    changed = false;
    if (mLocalTemperature == temp) {
        return Status::Success;
    }
    mLocalTemperature = temp;
    changed           = true;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatDelegate::GetOutdoorTemperature(DataModel::Nullable<temperature> &outdoorTemp) const
{
    outdoorTemp = DataModel::NullNullable;
    return Status::Success;
}

int8_t ThermostatDelegate::GetLocalTemperatureCalibration() const
{
    return mLocalTemperatureCalibration;
}

Protocols::InteractionModel::Status ThermostatDelegate::SetLocalTemperatureCalibration(int8_t temp, bool &changed)
{
    changed = false;
    if (mLocalTemperatureCalibration == temp) {
        return Status::Success;
    }
    AttributePersistenceProvider *provider = mProvider != nullptr ? mProvider : GetAttributePersistenceProvider();
    VerifyOrReturnError(provider != nullptr, Status::InvalidInState);
    AttributePersistence persistence(*provider);
    CHIP_ERROR result = persistence.StoreNativeEndianValue({ mEndpointId, Thermostat::Id, LocalTemperatureCalibration::Id }, temp);
    if (result != CHIP_NO_ERROR) {
        ChipLogError(Zcl, "Failed to store LocalTemperatureCalibration attribute");
        return Status::Failure;
    }
    mLocalTemperatureCalibration = temp;
    changed                      = true;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatDelegate::GetRemoteSensing(BitMask<RemoteSensingBitmap> &remoteSensing) const
{
    remoteSensing = mRemoteSensing;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatDelegate::SetRemoteSensing(BitMask<RemoteSensingBitmap> sensing, bool &changed)
{
    changed = false;
    if (mRemoteSensing == sensing) {
        return Status::Success;
    }
    AttributePersistenceProvider *provider = mProvider != nullptr ? mProvider : GetAttributePersistenceProvider();
    VerifyOrReturnError(provider != nullptr, Status::InvalidInState);
    AttributePersistence persistence(*provider);
    CHIP_ERROR result = persistence.StoreNativeEndianValue({ mEndpointId, Thermostat::Id, RemoteSensing::Id }, sensing.Raw());
    if (result != CHIP_NO_ERROR) {
        ChipLogError(Zcl, "Failed to store RemoteSensing attribute");
        return Status::Failure;
    }
    mRemoteSensing = sensing;
    changed        = true;
    return Status::Success;
}

/* -------------------------------------------------------------------------- */
/* ThermostatSetpointsDelegate                                                 */
/* -------------------------------------------------------------------------- */

CHIP_ERROR ThermostatSetpointsDelegate::Startup(ServerClusterContext &context)
{
    if (mStarted) {
        return CHIP_NO_ERROR;
    }
    AttributePersistenceProvider *provider = mProvider != nullptr ? mProvider : GetAttributePersistenceProvider();
    VerifyOrReturnError(provider != nullptr, CHIP_ERROR_PERSISTED_STORAGE_FAILED);
    AttributePersistence persistence(*provider);

#if CONFIG_TSTAT_FEAT_F00
    AbsMinHeatSetpointLimit::GetDefaultOr(mEndpointId, mAbsMinHeatSetpointLimit, kDefaultAbsMinHeatSetpointLimit);
    AbsMaxHeatSetpointLimit::GetDefaultOr(mEndpointId, mAbsMaxHeatSetpointLimit, kDefaultAbsMaxHeatSetpointLimit);

    temperature defaultMinHeatLimit;
    MinHeatSetpointLimit::GetDefaultOr(mEndpointId, defaultMinHeatLimit, mAbsMinHeatSetpointLimit);
    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, MinHeatSetpointLimit::Id }, mMinHeatSetpointLimit,
                                      defaultMinHeatLimit);

    temperature defaultMaxHeatLimit;
    MaxHeatSetpointLimit::GetDefaultOr(mEndpointId, defaultMaxHeatLimit, mAbsMaxHeatSetpointLimit);
    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, MaxHeatSetpointLimit::Id }, mMaxHeatSetpointLimit,
                                      defaultMaxHeatLimit);

    OccupiedHeatingSetpoint::GetDefaultOr(mEndpointId, mOccupiedHeatingSetpoint, kDefaultHeatingSetpoint);
    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, OccupiedHeatingSetpoint::Id }, mOccupiedHeatingSetpoint,
                                      mOccupiedHeatingSetpoint);
#endif

#if CONFIG_TSTAT_FEAT_F01
    AbsMinCoolSetpointLimit::GetDefaultOr(mEndpointId, mAbsMinCoolSetpointLimit, kDefaultAbsMinCoolSetpointLimit);
    AbsMaxCoolSetpointLimit::GetDefaultOr(mEndpointId, mAbsMaxCoolSetpointLimit, kDefaultAbsMaxCoolSetpointLimit);

    temperature defaultMinCoolLimit;
    MinCoolSetpointLimit::GetDefaultOr(mEndpointId, defaultMinCoolLimit, mAbsMinCoolSetpointLimit);
    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, MinCoolSetpointLimit::Id }, mMinCoolSetpointLimit,
                                      defaultMinCoolLimit);

    temperature defaultMaxCoolLimit;
    MaxCoolSetpointLimit::GetDefaultOr(mEndpointId, defaultMaxCoolLimit, mAbsMaxCoolSetpointLimit);
    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, MaxCoolSetpointLimit::Id }, mMaxCoolSetpointLimit,
                                      defaultMaxCoolLimit);

    OccupiedCoolingSetpoint::GetDefaultOr(mEndpointId, mOccupiedCoolingSetpoint, kDefaultCoolingSetpoint);
    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, OccupiedCoolingSetpoint::Id }, mOccupiedCoolingSetpoint,
                                      mOccupiedCoolingSetpoint);
#endif

#if CONFIG_TSTAT_FEAT_F00 && CONFIG_TSTAT_FEAT_F02
    UnoccupiedHeatingSetpoint::GetDefaultOr(mEndpointId, mUnoccupiedHeatingSetpoint, kDefaultHeatingSetpoint);
    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, UnoccupiedHeatingSetpoint::Id }, mUnoccupiedHeatingSetpoint,
                                      mUnoccupiedHeatingSetpoint);
#endif

#if CONFIG_TSTAT_FEAT_F01 && CONFIG_TSTAT_FEAT_F02
    UnoccupiedCoolingSetpoint::GetDefaultOr(mEndpointId, mUnoccupiedCoolingSetpoint, kDefaultCoolingSetpoint);
    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, UnoccupiedCoolingSetpoint::Id }, mUnoccupiedCoolingSetpoint,
                                      mUnoccupiedCoolingSetpoint);
#endif
    mStarted = true;
    return CHIP_NO_ERROR;
}

void ThermostatSetpointsDelegate::Shutdown(ClusterShutdownType type)
{
    mStarted = false;
}

#if CONFIG_TSTAT_FEAT_F05
Protocols::InteractionModel::Status ThermostatSetpointsDelegate::GetMinDeadband(temperature &minDeadband) const
{
    AttributePersistenceProvider *provider = mProvider != nullptr ? mProvider : GetAttributePersistenceProvider();
    VerifyOrReturnError(provider != nullptr, Status::Failure);
    AttributePersistence persistence(*provider);

    // The deadband is stored as a SignedTemperature, which is an int8_t in tenths of a degree
    int8_t deadBand;
    MinSetpointDeadBand::GetDefaultOr(mEndpointId, deadBand, static_cast<int8_t>(kDefaultDeadBand / 10));
    persistence.LoadNativeEndianValue({ mEndpointId, Thermostat::Id, MinSetpointDeadBand::Id }, deadBand, deadBand);
    minDeadband = static_cast<int16_t>(deadBand * 10);
    return Status::Success;
}
#endif

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::SaveSetpoint(AttributeId attributeId, temperature value)
{
    AttributePersistenceProvider *provider = mProvider != nullptr ? mProvider : GetAttributePersistenceProvider();
    VerifyOrReturnError(provider != nullptr, Status::Failure);
    AttributePersistence persistence(*provider);
    if (auto status = persistence.StoreNativeEndianValue({ mEndpointId, Thermostat::Id, attributeId }, value);
        status != CHIP_NO_ERROR) {
        return ClusterStatusCode(status).GetStatus();
    }
    return Status::Success;
}

#if CONFIG_TSTAT_FEAT_F00
Protocols::InteractionModel::Status
ThermostatSetpointsDelegate::GetAbsMinHeatSetpointLimit(temperature &absMinHeatSetpointLimit) const
{
    absMinHeatSetpointLimit = mAbsMinHeatSetpointLimit;
    return Status::Success;
}

Protocols::InteractionModel::Status
ThermostatSetpointsDelegate::GetAbsMaxHeatSetpointLimit(temperature &absMaxHeatSetpointLimit) const
{
    absMaxHeatSetpointLimit = mAbsMaxHeatSetpointLimit;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::GetMinHeatSetpointLimit(temperature &minHeatSetpointLimit) const
{
    minHeatSetpointLimit = mMinHeatSetpointLimit;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::SetMinHeatSetpointLimit(temperature minHeatSetpointLimit,
        bool &changed)
{
    changed = false;
    if (mMinHeatSetpointLimit == minHeatSetpointLimit) {
        return Status::Success;
    }
    if (auto status = SaveSetpoint(MinHeatSetpointLimit::Id, minHeatSetpointLimit); status != Status::Success) {
        return status;
    }
    mMinHeatSetpointLimit = minHeatSetpointLimit;
    changed               = true;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::GetMaxHeatSetpointLimit(temperature &maxHeatSetpointLimit) const
{
    maxHeatSetpointLimit = mMaxHeatSetpointLimit;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::SetMaxHeatSetpointLimit(temperature maxHeatSetpointLimit,
        bool &changed)
{
    changed = false;
    if (mMaxHeatSetpointLimit == maxHeatSetpointLimit) {
        return Status::Success;
    }
    if (auto status = SaveSetpoint(MaxHeatSetpointLimit::Id, maxHeatSetpointLimit); status != Status::Success) {
        return status;
    }
    mMaxHeatSetpointLimit = maxHeatSetpointLimit;
    changed               = true;
    return Status::Success;
}

Protocols::InteractionModel::Status
ThermostatSetpointsDelegate::GetOccupiedHeatingSetpoint(temperature &occupiedHeatingSetpoint) const
{
    occupiedHeatingSetpoint = mOccupiedHeatingSetpoint;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::SetOccupiedHeatingSetpoint(temperature occupiedHeatingSetpoint,
        bool &changed)
{
    changed = false;
    if (mOccupiedHeatingSetpoint == occupiedHeatingSetpoint) {
        return Status::Success;
    }
    if (auto status = SaveSetpoint(OccupiedHeatingSetpoint::Id, occupiedHeatingSetpoint); status != Status::Success) {
        return status;
    }
    mOccupiedHeatingSetpoint = occupiedHeatingSetpoint;
    changed                  = true;
    return Status::Success;
}
#endif

#if CONFIG_TSTAT_FEAT_F01
Protocols::InteractionModel::Status
ThermostatSetpointsDelegate::GetAbsMinCoolSetpointLimit(temperature &absMinCoolSetpointLimit) const
{
    absMinCoolSetpointLimit = mAbsMinCoolSetpointLimit;
    return Status::Success;
}

Protocols::InteractionModel::Status
ThermostatSetpointsDelegate::GetAbsMaxCoolSetpointLimit(temperature &absMaxCoolSetpointLimit) const
{
    absMaxCoolSetpointLimit = mAbsMaxCoolSetpointLimit;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::GetMinCoolSetpointLimit(temperature &minCoolSetpointLimit) const
{
    minCoolSetpointLimit = mMinCoolSetpointLimit;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::SetMinCoolSetpointLimit(temperature minCoolSetpointLimit,
        bool &changed)
{
    changed = false;
    if (mMinCoolSetpointLimit == minCoolSetpointLimit) {
        return Status::Success;
    }
    if (auto status = SaveSetpoint(MinCoolSetpointLimit::Id, minCoolSetpointLimit); status != Status::Success) {
        return status;
    }
    mMinCoolSetpointLimit = minCoolSetpointLimit;
    changed               = true;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::GetMaxCoolSetpointLimit(temperature &maxCoolSetpointLimit) const
{
    maxCoolSetpointLimit = mMaxCoolSetpointLimit;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::SetMaxCoolSetpointLimit(temperature maxCoolSetpointLimit,
        bool &changed)
{
    changed = false;
    if (mMaxCoolSetpointLimit == maxCoolSetpointLimit) {
        return Status::Success;
    }
    if (auto status = SaveSetpoint(MaxCoolSetpointLimit::Id, maxCoolSetpointLimit); status != Status::Success) {
        return status;
    }
    mMaxCoolSetpointLimit = maxCoolSetpointLimit;
    changed               = true;
    return Status::Success;
}

Protocols::InteractionModel::Status
ThermostatSetpointsDelegate::GetOccupiedCoolingSetpoint(temperature &occupiedCoolingSetpoint) const
{
    occupiedCoolingSetpoint = mOccupiedCoolingSetpoint;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::SetOccupiedCoolingSetpoint(temperature occupiedCoolingSetpoint,
        bool &changed)
{
    changed = false;
    if (mOccupiedCoolingSetpoint == occupiedCoolingSetpoint) {
        return Status::Success;
    }
    if (auto status = SaveSetpoint(OccupiedCoolingSetpoint::Id, occupiedCoolingSetpoint); status != Status::Success) {
        return status;
    }
    mOccupiedCoolingSetpoint = occupiedCoolingSetpoint;
    changed                  = true;
    return Status::Success;
}
#endif

#if CONFIG_TSTAT_FEAT_F00 && CONFIG_TSTAT_FEAT_F02
Protocols::InteractionModel::Status
ThermostatSetpointsDelegate::GetUnoccupiedHeatingSetpoint(temperature &unoccupiedHeatingSetpoint) const
{
    unoccupiedHeatingSetpoint = mUnoccupiedHeatingSetpoint;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::SetUnoccupiedHeatingSetpoint(temperature unoccupiedHeatingSetpoint,
        bool &changed)
{
    changed = false;
    if (mUnoccupiedHeatingSetpoint == unoccupiedHeatingSetpoint) {
        return Status::Success;
    }
    if (auto status = SaveSetpoint(UnoccupiedHeatingSetpoint::Id, unoccupiedHeatingSetpoint); status != Status::Success) {
        return status;
    }
    mUnoccupiedHeatingSetpoint = unoccupiedHeatingSetpoint;
    changed                    = true;
    return Status::Success;
}
#endif

#if CONFIG_TSTAT_FEAT_F01 && CONFIG_TSTAT_FEAT_F02
Protocols::InteractionModel::Status
ThermostatSetpointsDelegate::GetUnoccupiedCoolingSetpoint(temperature &unoccupiedCoolingSetpoint) const
{
    unoccupiedCoolingSetpoint = mUnoccupiedCoolingSetpoint;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::SetUnoccupiedCoolingSetpoint(temperature unoccupiedCoolingSetpoint,
        bool &changed)
{
    changed = false;
    if (mUnoccupiedCoolingSetpoint == unoccupiedCoolingSetpoint) {
        return Status::Success;
    }
    if (auto status = SaveSetpoint(UnoccupiedCoolingSetpoint::Id, unoccupiedCoolingSetpoint); status != Status::Success) {
        return status;
    }
    mUnoccupiedCoolingSetpoint = unoccupiedCoolingSetpoint;
    changed                    = true;
    return Status::Success;
}
#endif

#if CONFIG_TSTAT_ATTR_PROTECTION
Protocols::InteractionModel::Status ThermostatSetpointsDelegate::GetCriticalFreezeProtection(bool &enabled) const
{
    enabled = mCriticalFreezeProtection;
    return Status::Success;
}

Protocols::InteractionModel::Status ThermostatSetpointsDelegate::GetCriticalOverheatProtection(bool &enabled) const
{
    enabled = mCriticalOverheatProtection;
    return Status::Success;
}
#endif

/* -------------------------------------------------------------------------- */
/* ThermostatOccupancyDelegate                                                 */
/* -------------------------------------------------------------------------- */
#if CONFIG_TSTAT_FEAT_F02
BitMask<OccupancyBitmap> ThermostatOccupancyDelegate::GetOccupancy() const
{
    return mOccupancy;
}

Status ThermostatOccupancyDelegate::SetOccupancy(BitMask<OccupancyBitmap> occupancy, bool &changed)
{
    if (mOccupancy == occupancy) {
        changed = false;
        return Status::Success;
    }
    mOccupancy = occupancy;
    changed    = true;
    return Status::Success;
}
#endif

void MatterThermostatClusterInitCallback(chip::EndpointId endpointId)
{
    Clusters::Thermostat::AmebaThermostatDelegateInit(endpointId);
}
