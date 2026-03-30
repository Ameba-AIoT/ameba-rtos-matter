/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

namespace Presets
{
namespace Clusters
{

/******************************************************
 *               Preset Clusters
 ******************************************************/
void cluster_server_descriptor(ClusterConfig *clusterConfig);
void cluster_server_identify(ClusterConfig *clusterConfig);
void cluster_server_groups(ClusterConfig *clusterConfig);
void cluster_server_onoff(ClusterConfig *clusterConfig);
void cluster_server_level_control(ClusterConfig *clusterConfig);
void cluster_server_oven_cavity_operational_state(ClusterConfig *clusterConfig);
void cluster_server_oven_mode(ClusterConfig *clusterConfig);
void cluster_server_laundry_dryer_controls(ClusterConfig *clusterConfig);
void cluster_server_mode_select(ClusterConfig *clusterConfig);
void cluster_server_laundry_washer_mode(ClusterConfig *clusterConfig);
void cluster_server_refrigerator_mode(ClusterConfig *clusterConfig);
void cluster_server_laundry_washer_controls(ClusterConfig *clusterConfig);
void cluster_server_rvc_run_mode(ClusterConfig *clusterConfig);
void cluster_server_rvc_clean_mode_operational_state(ClusterConfig *clusterConfig);
void cluster_server_temperature_control(ClusterConfig *clusterConfig);
void cluster_server_refrigerator_alarm(ClusterConfig *clusterConfig);
void cluster_server_dishwasher_mode(ClusterConfig *clusterConfig);
void cluster_server_air_quality(ClusterConfig *clusterConfig);
void cluster_server_dishwasher_alarm(ClusterConfig *clusterConfig);
void cluster_server_microwave_oven_control(ClusterConfig *clusterConfig);
void cluster_server_operational_state(ClusterConfig *clusterConfig);
void cluster_server_rvc_operational_state(ClusterConfig *clusterConfig);
void cluster_server_scenes_management(ClusterConfig *clusterConfig);
void cluster_server_hepa_filter_monitoring(ClusterConfig *clusterConfig);
void cluster_server_activated_carbon_filter_monitoring(ClusterConfig *clusterConfig);
void cluster_server_valve_configuration_and_control(ClusterConfig *clusterConfig);
void cluster_server_thermostat(ClusterConfig *clusterConfig);
void cluster_server_fan_control(ClusterConfig *clusterConfig);
void cluster_server_thermostat_user_interface_configuration(ClusterConfig *clusterConfig);
void cluster_server_color_control(ClusterConfig *clusterConfig);
void cluster_server_illuminance_measurement(ClusterConfig *clusterConfig);
void cluster_server_temperature_measurement(ClusterConfig *clusterConfig);
void cluster_server_pressure_measurement(ClusterConfig *clusterConfig);
void cluster_server_flow_measurement(ClusterConfig *clusterConfig);
void cluster_server_relative_humidity_measurement(ClusterConfig *clusterConfig);
void cluster_server_occupancy_sensing(ClusterConfig *clusterConfig);
void cluster_server_carbon_monoxide_concentration_measurement(ClusterConfig *clusterConfig);
void cluster_server_carbon_dioxide_concentration_measurement(ClusterConfig *clusterConfig);
void cluster_server_nitrogen_dioxide_concentration_measurement(ClusterConfig *clusterConfig);
void cluster_server_ozone_concentration_measurement(ClusterConfig *clusterConfig);
void cluster_server_pm25_concentration_measurement(ClusterConfig *clusterConfig);
void cluster_server_formaldehyde_concentration_measurement(ClusterConfig *clusterConfig);
void cluster_server_pm1_concentration_measurement(ClusterConfig *clusterConfig);
void cluster_server_pm10_concentration_measurement(ClusterConfig *clusterConfig);
void cluster_server_total_volatile_concentration_measurement(ClusterConfig *clusterConfig);
void cluster_server_radon_concentration_measurement(ClusterConfig *clusterConfig);
void cluster_server_soil_measurement(ClusterConfig *clusterConfig);
} // Clusters

namespace Endpoints
{

/******************************************************
 *               Preset Device types (Endpoint)
 ******************************************************/
void matter_air_purifier_preset(EndpointConfig *mEndpointConfig);
void matter_room_air_con_preset(EndpointConfig *mEndpointConfig);
void matter_onoff_light_preset(EndpointConfig *mEndpointConfig);
void matter_dimmable_light_preset(EndpointConfig *mEndpointConfig);
void matter_aggregator_preset(EndpointConfig *mEndpointConfig);
void matter_color_temperature_light_preset(EndpointConfig *mEndpointConfig);
} // Endpoints
} // Presets
