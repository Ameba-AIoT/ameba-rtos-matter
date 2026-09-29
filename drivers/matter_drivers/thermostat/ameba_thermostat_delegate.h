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
#pragma once

#include <ameba_cluster_config.h>
#include <app/clusters/thermostat-server/CodegenIntegration.h>
#include <app/clusters/thermostat-server/ThermostatCluster.h>
#include <app/persistence/AttributePersistenceProvider.h>
#include <app/persistence/AttributePersistenceProviderInstance.h>

namespace chip {
namespace app {
namespace Clusters {
namespace Thermostat {

/*
 * A simple implementation of ThermostatDelegate.
 * It reports and persists the basic state of the thermostat.
 */
class ThermostatDelegate : public Delegate
{
public:
    ThermostatDelegate(EndpointId endpoint, AttributePersistenceProvider *provider = nullptr,
                       FabricTable *fabricTable = nullptr) :
        mEndpointId(endpoint),
        mProvider(provider), mFabricTable(fabricTable)
    {}

    FabricTable &GetFabricTable() const override;

    CHIP_ERROR Startup(ServerClusterContext &context) override;

    SystemModeEnum GetSystemMode() const override;
    Protocols::InteractionModel::Status SetSystemMode(SystemModeEnum systemMode, bool &changed) override;

    Protocols::InteractionModel::Status GetRunningMode(ThermostatRunningModeEnum &runningMode) const override;
    Protocols::InteractionModel::Status SetRunningMode(ThermostatRunningModeEnum runningMode, bool &changed) override;

    Protocols::InteractionModel::Status GetRunningState(BitMask<RelayStateBitmap> &runningState) const override;
    Protocols::InteractionModel::Status SetRunningState(BitMask<RelayStateBitmap> runningState, bool &changed) override;

    ControlSequenceOfOperationEnum GetControlSequenceOfOperation() const override;
    Protocols::InteractionModel::Status SetControlSequenceOfOperation(ControlSequenceOfOperationEnum seq, bool &changed) override;

    DataModel::Nullable<temperature> GetLocalTemperature() const override;
    Protocols::InteractionModel::Status SetLocalTemperature(DataModel::Nullable<temperature> temp, bool &changed) override;

    Protocols::InteractionModel::Status GetOutdoorTemperature(DataModel::Nullable<temperature> &outdoorTemp) const override;

    int8_t GetLocalTemperatureCalibration() const override;
    Protocols::InteractionModel::Status SetLocalTemperatureCalibration(int8_t temp, bool &changed) override;

    Protocols::InteractionModel::Status GetRemoteSensing(BitMask<RemoteSensingBitmap> &remoteSensing) const override;
    Protocols::InteractionModel::Status SetRemoteSensing(BitMask<RemoteSensingBitmap> sensing, bool &changed) override;

private:
    EndpointId mEndpointId;
    AttributePersistenceProvider *mProvider = nullptr;
    FabricTable *mFabricTable               = nullptr;

    ControlSequenceOfOperationEnum mControlSequenceOfOperation = ControlSequenceOfOperationEnum::kCoolingAndHeating;

    SystemModeEnum mSystemMode                         = SystemModeEnum::kOff;
    ThermostatRunningModeEnum mRunningMode             = ThermostatRunningModeEnum::kOff;
    BitMask<RelayStateBitmap> mRunningState            = BitMask<RelayStateBitmap>(0);
    DataModel::Nullable<temperature> mLocalTemperature = DataModel::Nullable<int16_t>();
    int8_t mLocalTemperatureCalibration                = 0;

    BitMask<RemoteSensingBitmap> mRemoteSensing = BitMask<RemoteSensingBitmap>(0);
};

/*
 * A simple implementation of ThermostatCoolingSetpoints::Delegate,
 * ThermostatHeatingSetpoints::Delegate, and ThermostatAutoSetpoints::Delegate.
 * It reports and persists the state of the thermostat's setpoints attributes.
 *
 * It also demonstrates that a single class can implement multiple related thermostat delegates.
 */
class ThermostatSetpointsDelegate
#if CONFIG_TSTAT_FEAT_F00
    : public ThermostatHeatingSetpoints::Delegate
#endif
#if CONFIG_TSTAT_FEAT_F01
    , public ThermostatCoolingSetpoints::Delegate
#endif
#if CONFIG_TSTAT_FEAT_F05
    , public ThermostatAutoSetpoints::Delegate
#endif
{
public:
    ThermostatSetpointsDelegate(EndpointId endpoint, AttributePersistenceProvider *provider = nullptr) :
        mEndpointId(endpoint), mProvider(provider)
    {}

    CHIP_ERROR Startup(ServerClusterContext &context) override;
    void Shutdown(ClusterShutdownType type) override;

#if CONFIG_TSTAT_FEAT_F00
    Protocols::InteractionModel::Status GetOccupiedHeatingSetpoint(temperature &occupiedHeatingSetpoint) const override;
    Protocols::InteractionModel::Status SetOccupiedHeatingSetpoint(temperature occupiedHeatingSetpoint, bool &changed) override;

    Protocols::InteractionModel::Status GetAbsMinHeatSetpointLimit(temperature &absMinHeatSetpointLimit) const override;
    Protocols::InteractionModel::Status GetAbsMaxHeatSetpointLimit(temperature &absMaxHeatSetpointLimit) const override;

    Protocols::InteractionModel::Status GetMinHeatSetpointLimit(temperature &minHeatSetpointLimit) const override;
    Protocols::InteractionModel::Status SetMinHeatSetpointLimit(temperature minHeatSetpointLimit, bool &changed) override;

    Protocols::InteractionModel::Status GetMaxHeatSetpointLimit(temperature &maxHeatSetpointLimit) const override;
    Protocols::InteractionModel::Status SetMaxHeatSetpointLimit(temperature maxHeatSetpointLimit, bool &changed) override;
#endif
#if CONFIG_TSTAT_FEAT_F01
    Protocols::InteractionModel::Status GetOccupiedCoolingSetpoint(temperature &occupiedCoolingSetpoint) const override;
    Protocols::InteractionModel::Status SetOccupiedCoolingSetpoint(temperature occupiedCoolingSetpoint, bool &changed) override;

    Protocols::InteractionModel::Status GetAbsMinCoolSetpointLimit(temperature &absMinCoolSetpointLimit) const override;
    Protocols::InteractionModel::Status GetAbsMaxCoolSetpointLimit(temperature &absMaxCoolSetpointLimit) const override;

    Protocols::InteractionModel::Status GetMinCoolSetpointLimit(temperature &minCoolSetpointLimit) const override;
    Protocols::InteractionModel::Status SetMinCoolSetpointLimit(temperature minCoolSetpointLimit, bool &changed) override;

    Protocols::InteractionModel::Status GetMaxCoolSetpointLimit(temperature &maxCoolSetpointLimit) const override;
    Protocols::InteractionModel::Status SetMaxCoolSetpointLimit(temperature maxCoolSetpointLimit, bool &changed) override;
#endif
#if CONFIG_TSTAT_FEAT_F05
    Protocols::InteractionModel::Status GetMinDeadband(temperature &minDeadband) const override;
#endif
#if CONFIG_TSTAT_FEAT_F00 && CONFIG_TSTAT_FEAT_F02
    Protocols::InteractionModel::Status GetUnoccupiedHeatingSetpoint(temperature &unoccupiedHeatingSetpoint) const override;
    Protocols::InteractionModel::Status SetUnoccupiedHeatingSetpoint(temperature unoccupiedHeatingSetpoint,
            bool &changed) override;
#endif
#if CONFIG_TSTAT_FEAT_F01 && CONFIG_TSTAT_FEAT_F02
    Protocols::InteractionModel::Status GetUnoccupiedCoolingSetpoint(temperature &unoccupiedCoolingSetpoint) const override;
    Protocols::InteractionModel::Status SetUnoccupiedCoolingSetpoint(temperature unoccupiedCoolingSetpoint,
            bool &changed) override;
#endif
#if CONFIG_TSTAT_ATTR_PROTECTION
    Protocols::InteractionModel::Status GetCriticalFreezeProtection(bool &enabled) const override;
    Protocols::InteractionModel::Status GetCriticalOverheatProtection(bool &enabled) const override;
#endif

private:
    EndpointId mEndpointId;
    AttributePersistenceProvider *mProvider = nullptr;
    bool mStarted                            = false;

#if CONFIG_TSTAT_FEAT_F00
    temperature mOccupiedHeatingSetpoint;
    temperature mAbsMinHeatSetpointLimit;
    temperature mAbsMaxHeatSetpointLimit;
    temperature mMinHeatSetpointLimit;
    temperature mMaxHeatSetpointLimit;
#endif
#if CONFIG_TSTAT_FEAT_F01
    temperature mOccupiedCoolingSetpoint;
    temperature mAbsMinCoolSetpointLimit;
    temperature mAbsMaxCoolSetpointLimit;
    temperature mMinCoolSetpointLimit;
    temperature mMaxCoolSetpointLimit;
#endif
#if CONFIG_TSTAT_FEAT_F00 && CONFIG_TSTAT_FEAT_F02
    temperature mUnoccupiedHeatingSetpoint;
#endif
#if CONFIG_TSTAT_FEAT_F01 && CONFIG_TSTAT_FEAT_F02
    temperature mUnoccupiedCoolingSetpoint;
#endif
#if CONFIG_TSTAT_ATTR_PROTECTION
    bool mCriticalFreezeProtection   = false;
    bool mCriticalOverheatProtection = false;
#endif
    Protocols::InteractionModel::Status SaveSetpoint(AttributeId attributeId, temperature value);
};

#if CONFIG_TSTAT_FEAT_F02
/*
 * A simple implementation of ThermostatOccupancy::Delegate.
 * It reports and persists the basic state of the thermostat's occupancy attributes.
 */
class ThermostatOccupancyDelegate : public ThermostatOccupancy::Delegate
{
public:
    ThermostatOccupancyDelegate() = default;

    BitMask<OccupancyBitmap> GetOccupancy() const override;
    Protocols::InteractionModel::Status SetOccupancy(BitMask<OccupancyBitmap> occupancy, bool &changed) override;

private:
    BitMask<OccupancyBitmap> mOccupancy{ OccupancyBitmap::kOccupied };
};
#endif

CHIP_ERROR AmebaThermostatDelegateInit(EndpointId endpoint);

} // namespace Thermostat
} // namespace Clusters
} // namespace app
} // namespace chip
