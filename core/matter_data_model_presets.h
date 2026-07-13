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

namespace Presets {
namespace Clusters {

/******************************************************
 *               Preset Clusters
 ******************************************************/
void matter_cluster_descriptor_server(ClusterConfig *clusterConfig);
void matter_cluster_acl_server(ClusterConfig *clusterConfig);
void matter_cluster_basic_information_server(ClusterConfig *clusterConfig);
void matter_cluster_ota_requestor_server(ClusterConfig *clusterConfig);
void matter_cluster_general_commissioning_server(ClusterConfig *clusterConfig);
void matter_cluster_network_commissioning_server(ClusterConfig *clusterConfig);
void matter_cluster_general_diagnostics_server(ClusterConfig *clusterConfig);
void matter_cluster_software_diagnostics_server(ClusterConfig *clusterConfig);
void matter_cluster_wifi_diagnostics_server(ClusterConfig *clusterConfig);
void matter_cluster_administrator_commissioning_server(ClusterConfig *clusterConfig);
void matter_cluster_operational_credentials_server(ClusterConfig *clusterConfig);
void matter_cluster_group_key_management_server(ClusterConfig *clusterConfig);
void matter_cluster_identify_server(ClusterConfig *clusterConfig);
void matter_cluster_groups_server(ClusterConfig *clusterConfig);
void matter_cluster_scenes_server(ClusterConfig *clusterConfig);
void matter_cluster_onoff_server(ClusterConfig *clusterConfig);
void matter_cluster_level_control_server(ClusterConfig *clusterConfig);

} // Clusters

namespace Endpoints {

/******************************************************
 *               Preset Device types (Endpoint)
 ******************************************************/
void matter_root_node_preset(EndpointConfig *rootNodeEndpointConfig);
void matter_dimmable_light_preset(EndpointConfig *dimmableLightEndpointConfig);
void matter_aggregator_preset(EndpointConfig *aggregatorEndpointConfig);

} // Endpoints
} // Presets
