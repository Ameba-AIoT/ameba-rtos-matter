#include <matter_data_model.h>

#include <app/PluginApplicationCallbacks.h>
#include <app/util/endpoint-config-defines.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app-common/zap-generated/callback.h>

using namespace chip::app::Clusters;

namespace Presets
{
namespace Clusters
{

/*************************************************************************
 * Attribute default values that are non trivial
 *************************************************************************/

EmberAfAttributeMinMaxValue onoffValue                 = { (uint16_t)0xFF, (uint16_t)0x0, (uint16_t)0x2 };   // StartUpOnOff
EmberAfAttributeMinMaxValue laundrydryercontrolsValue  = { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x3 };    // SelectedDrynessLevel
EmberAfAttributeMinMaxValue laundrywashercontrolsValue = { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x1F };   // SpinSpeedCurrent
EmberAfAttributeMinMaxValue windowcoveringValue        = { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0xF };    // Mode
EmberAfAttributeMinMaxValue pumpconfigandcontrolValue  = { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x3 };    // OperationMode
EmberAfAttributeMinMaxValue colorcontrolValue          = { (uint16_t)0x1, (uint16_t)0x0, (uint16_t)0xFEFF }; // StartUpColorTemperatureMireds
EmberAfAttributeMinMaxValue levelcontrolValue[] = {
    { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x3 },          // Options
    { (uint16_t)0xFF, (uint16_t)0x1, (uint16_t)0xFE },        // OnLevel
    { (uint16_t)0x32, (uint16_t)0x1, (uint16_t)0xFE }         // DefaultMoveRate
};
EmberAfAttributeMinMaxValue thermostatValue[] = {
    { (uint16_t)0xA28, (uint16_t) -0x6AB3, (uint16_t)0x7FFF }, // OccupiedCoolingSetpoint
    { (uint16_t)0x7D0, (uint16_t) -0x6AB3, (uint16_t)0x7FFF }, // OccupiedHeatingSetpoint
    { (uint16_t)0x2BC, (uint16_t) -0x6AB3, (uint16_t)0x7FFF }, // MinHeatSetpointLimit
    { (uint16_t)0xBB8, (uint16_t) -0x6AB3, (uint16_t)0x7FFF }, // MaxHeatSetpointLimit
    { (uint16_t)0x640, (uint16_t) -0x6AB3, (uint16_t)0x7FFF }, // MinCoolSetpointLimit
    { (uint16_t)0xC80, (uint16_t) -0x6AB3, (uint16_t)0x7FFF }, // MaxCoolSetpointLimit
    { (uint16_t)0x4, (uint16_t)0x0, (uint16_t)0x5 },          // ControlSequenceOfOperation
    { (uint16_t)0x1, (uint16_t)0x0, (uint16_t)0x7 }           // SystemMode
};
EmberAfAttributeMinMaxValue fancontrolValue[] = {
    { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x6 },          // FanMode
    { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x64 },         // PercentSetting
    { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x64 },         // SpeedSetting
    { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x7 },          // RockSetting
    { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x3 },          // WindSetting
    { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x1 }           // AirflowDirection
};
EmberAfAttributeMinMaxValue thermostatUIValue[] = {
    { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x1 },          // TemperatureDisplayMode
    { (uint16_t)0x0, (uint16_t)0x0, (uint16_t)0x5 }           // KeypadLockout
};

const uint8_t defaultActiveLocale[] = { 5, 'e', 'n', '-', 'U', 'S' };
const uint8_t defaultDescription[] = { 6, 'C', 'o', 'f', 'f', 'e', 'e' };
const uint8_t defaultAutoCloseTime[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/*************************************************************************
 * Type Macros for Attributes
 *************************************************************************/

#define ATTR_RO         ZAP_ATTRIBUTE_MASK(EXTERNAL_STORAGE)
#define ATTR_RW         ATTR_RO | ZAP_ATTRIBUTE_MASK(WRITABLE)
#define ATTR_NULLABLE   ZAP_ATTRIBUTE_MASK(NULLABLE)
#define ATTR_TOKENIZE   ZAP_ATTRIBUTE_MASK(TOKENIZE)
#define ATTR_MIN_MAX    ZAP_ATTRIBUTE_MASK(MIN_MAX)

/*************************************************************************
 * Cluster Initialization Functions
 *************************************************************************/

void cluster_server_descriptor(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x0000001D;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},  // DeviceTypeList
        {0x00000001, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},  // ServerList
        {0x00000002, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},  // ClientList
        {0x00000003, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},  // PartsList
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0), 4, ATTR_RO},  // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),   2, ATTR_RO}   // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
}

void cluster_server_identify(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000003;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(INT16U),  ZAP_EMPTY_DEFAULT(), 2, ATTR_RW},  // IdentifyTime
        {0x00000001, ZAP_TYPE(ENUM8),   ZAP_EMPTY_DEFAULT(), 1, ATTR_RO},  // IdentifyType
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(), 4, ATTR_RO}, // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),  ZAP_EMPTY_DEFAULT(), 2, ATTR_RO}   // ClusterRevision
    };

    static const CommandConfig commands[] = {
        {0x00000000, COMMAND_MASK_ACCEPTED},  // Identify
        {0x00000040, COMMAND_MASK_ACCEPTED},  // TriggerEffect
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED}
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_groups(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000004;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(INIT_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(BITMAP8),  ZAP_EMPTY_DEFAULT(),   1, ATTR_RO},  // NameSupport
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0), 4, ATTR_RO},  // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(4), 2, ATTR_RO}   // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // AddGroup
        {0x00000001, COMMAND_MASK_ACCEPTED},  // ViewGroup
        {0x00000002, COMMAND_MASK_ACCEPTED},  // GetGroupMembership
        {0x00000003, COMMAND_MASK_ACCEPTED},  // RemoveGroup
        {0x00000004, COMMAND_MASK_ACCEPTED},  // RemoveAllGroups
        {0x00000005, COMMAND_MASK_ACCEPTED},  // AddGroupIfIdentifying
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},

        // Generated commands
        {0x00000000, COMMAND_MASK_GENERATED}, // AddGroupResponse
        {0x00000001, COMMAND_MASK_GENERATED}, // ViewGroupResponse
        {0x00000002, COMMAND_MASK_GENERATED}, // GetGroupMembershipResponse
        {0x00000003, COMMAND_MASK_GENERATED}, // RemoveGroupResponse
        {chip::kInvalidCommandId, COMMAND_MASK_GENERATED}
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) emberAfGroupsClusterServerInitCallback);
}

void cluster_server_onoff(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000006;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(INIT_FUNCTION) | ZAP_CLUSTER_MASK(SHUTDOWN_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(BOOLEAN),  ZAP_SIMPLE_DEFAULT(0x00),   1, ATTR_RO | ATTR_TOKENIZE},                                 // OnOff
        {0x00004000, ZAP_TYPE(BOOLEAN),  ZAP_SIMPLE_DEFAULT(0x01),   1, ATTR_RO},                                                 // GlobalSceneControl
        {0x00004001, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x0000), 2, ATTR_RW},                                                 // OnTime
        {0x00004002, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x0000), 2, ATTR_RW},                                                 // OffWaitTime
        {0x00004003, ZAP_TYPE(ENUM8),    &onoffValue,                1, ATTR_RW | ATTR_TOKENIZE | ATTR_NULLABLE | ATTR_MIN_MAX},  // StartUpOnOff
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(1),      4, ATTR_RO},                                                 // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(6),      2, ATTR_RO}                                                  // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // Off
        {0x00000001, COMMAND_MASK_ACCEPTED},  // On
        {0x00000002, COMMAND_MASK_ACCEPTED},  // Toggle
        {0x00000040, COMMAND_MASK_ACCEPTED},  // OffWithEffect
        {0x00000041, COMMAND_MASK_ACCEPTED},  // OnWithRecallGlobalScene
        {0x00000042, COMMAND_MASK_ACCEPTED},  // OnWithTimedOff
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED}
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) emberAfOnOffClusterServerInitCallback);
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterOnOffClusterServerShutdownCallback);
}


void cluster_server_level_control(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000008;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(INIT_FUNCTION) | ZAP_CLUSTER_MASK(SHUTDOWN_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(INT8U),    ZAP_SIMPLE_DEFAULT(0xFE),          1, ATTR_RO | ATTR_TOKENIZE | ATTR_NULLABLE}, // CurrentLevel
        {0x00000001, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x0000),        2, ATTR_RO},                                 // RemainingTime
        {0x00000002, ZAP_TYPE(INT8U),    ZAP_SIMPLE_DEFAULT(0x01),          1, ATTR_RO},                                 // MinLevel
        {0x00000003, ZAP_TYPE(INT8U),    ZAP_SIMPLE_DEFAULT(0xFE),          1, ATTR_RO},                                 // MaxLevel
        {0x0000000F, ZAP_TYPE(BITMAP8),  &levelcontrolValue[0],             1, ATTR_RW | ATTR_MIN_MAX},                  // Options
        {0x00000010, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x0000),        2, ATTR_RW},                                 // OnOffTransitionTime
        {0x00000011, ZAP_TYPE(INT8U),    &levelcontrolValue[1],             1, ATTR_RW | ATTR_MIN_MAX | ATTR_NULLABLE},  // OnLevel
        {0x00000012, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),               2, ATTR_RW | ATTR_NULLABLE},                 // OnTransitionTime
        {0x00000013, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),               2, ATTR_RW | ATTR_NULLABLE},                 // OffTransitionTime
        {0x00000014, ZAP_TYPE(INT8U),    &levelcontrolValue[2],             1, ATTR_RW | ATTR_MIN_MAX | ATTR_NULLABLE},  // DefaultMoveRate
        {0x00004000, ZAP_TYPE(INT8U),    ZAP_SIMPLE_DEFAULT(255),           1, ATTR_RW | ATTR_TOKENIZE | ATTR_NULLABLE}, // StartUpCurrentLevel
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(3),             4, ATTR_RO},                                 // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(6),             2, ATTR_RO}                                  // ClusterRevision
    };

    static const CommandConfig commands[] = {
        {0x00000000, COMMAND_MASK_ACCEPTED}, // MoveToLevel
        {0x00000001, COMMAND_MASK_ACCEPTED}, // Move
        {0x00000002, COMMAND_MASK_ACCEPTED}, // Step
        {0x00000003, COMMAND_MASK_ACCEPTED}, // Stop
        {0x00000004, COMMAND_MASK_ACCEPTED}, // MoveToLevelWithOnOff
        {0x00000005, COMMAND_MASK_ACCEPTED}, // MoveWithOnOff
        {0x00000006, COMMAND_MASK_ACCEPTED}, // StepWithOnOff
        {0x00000007, COMMAND_MASK_ACCEPTED}, // StopWithOnOff
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED}
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) emberAfLevelControlClusterServerInitCallback);
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterLevelControlClusterServerShutdownCallback);
}

void cluster_server_oven_cavity_operational_state(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000048;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO | ATTR_NULLABLE}, // PhaseList
        {0x00000001, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),   1, ATTR_RO | ATTR_NULLABLE}, // CurrentPhase
        {0x00000003, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},                 // OperationalStateList
        {0x00000004, ZAP_TYPE(ENUM8),    ZAP_EMPTY_DEFAULT(),   1, ATTR_RO},                 // OperationalState
        {0x00000005, ZAP_TYPE(STRUCT),   ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},                 // OperationalError
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0), 4, ATTR_RO},                 // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),   2, ATTR_RO}                  // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000002, COMMAND_MASK_ACCEPTED},  // Start
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},

        // Generated commands
        {0x00000004, COMMAND_MASK_GENERATED}, // OperationalCommandResponse
        {chip::kInvalidCommandId, COMMAND_MASK_GENERATED}
    };

    static const EventConfig events[] = {
        {0x00000000},  // OperationalError
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->eventConfigs.assign(std::begin(events), std::end(events));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_oven_mode(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000049;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO}, // SupportedModes
        {0x00000001, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),   1, ATTR_RO}, // CurrentMode
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0), 4, ATTR_RO}, // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(2), 2, ATTR_RO}  // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // ChangeToMode
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},

        // Generated commands
        {0x00000001, COMMAND_MASK_GENERATED}, // ChangeToModeResponse
        {chip::kInvalidCommandId, COMMAND_MASK_GENERATED}
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_laundry_dryer_controls(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000004A;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(PRE_ATTRIBUTE_CHANGED_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),         0, ATTR_RO},                                // SupportedDrynessLevels
        {0x00000001, ZAP_TYPE(ENUM8),    &laundrydryercontrolsValue, 1, ATTR_RW | ATTR_NULLABLE | ATTR_MIN_MAX},  // SelectedDrynessLevel
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0),       4, ATTR_RO},                                // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(1),       2, ATTR_RO}                                 // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterLaundryDryerControlsClusterServerPreAttributeChangedCallback);
}

void cluster_server_mode_select(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000050;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(INIT_FUNCTION) | ZAP_CLUSTER_MASK(PRE_ATTRIBUTE_CHANGED_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(CHAR_STRING), defaultDescription,          65, ATTR_RO},                 // Description
        {0x00000001, ZAP_TYPE(ENUM16),      ZAP_SIMPLE_DEFAULT(0),       2, ATTR_RO | ATTR_NULLABLE}, // StandardNamespace
        {0x00000002, ZAP_TYPE(ARRAY),       ZAP_EMPTY_DEFAULT(),         0, ATTR_RO},                 // SupportedModes
        {0x00000003, ZAP_TYPE(INT8U),       ZAP_SIMPLE_DEFAULT(0),       1, ATTR_RO},                 // CurrentMode
        {0x00000004, ZAP_TYPE(INT8U),       ZAP_SIMPLE_DEFAULT(0),       1, ATTR_RW | ATTR_NULLABLE}, // StartUpMode
        {0x0000FFFC, ZAP_TYPE(BITMAP32),    ZAP_SIMPLE_DEFAULT(0),       4, ATTR_RO},                 // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),      ZAP_SIMPLE_DEFAULT(2),       2, ATTR_RO}                  // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // ChangeToMode
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) emberAfModeSelectClusterServerInitCallback);
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterModeSelectClusterServerPreAttributeChangedCallback);
}

void cluster_server_laundry_washer_mode(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000051;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},  // SupportedModes
        {0x00000001, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),   1, ATTR_RO},  // CurrentMode
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(),   4, ATTR_RO},  // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(3), 2, ATTR_RO}   // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // ChangeToMode
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},

        // Generated commands
        {0x00000001, COMMAND_MASK_GENERATED}, // ChangeToModeResponse
        {chip::kInvalidCommandId, COMMAND_MASK_GENERATED}
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterLaundryWasherControlsClusterServerPreAttributeChangedCallback);
}

void cluster_server_refrigerator_mode(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000052;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},  // SupportedModes
        {0x00000001, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),   1, ATTR_RO},  // CurrentMode
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(),   4, ATTR_RO},  // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(3), 2, ATTR_RO}   // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // ChangeToMode
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},

        // Generated commands
        {0x00000001, COMMAND_MASK_GENERATED}, // ChangeToModeResponse
        {chip::kInvalidCommandId, COMMAND_MASK_GENERATED}
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_laundry_washer_controls(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000053;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(PRE_ATTRIBUTE_CHANGED_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),         0, ATTR_RO},                                // SpinSpeeds
        {0x00000001, ZAP_TYPE(INT8U),    &laundrywashercontrolsValue, 1, ATTR_RW | ATTR_NULLABLE | ATTR_MIN_MAX}, // SpinSpeedCurrent
        {0x00000002, ZAP_TYPE(ENUM8),    ZAP_EMPTY_DEFAULT(),         1, ATTR_RW},                                // NumberOfRinses
        {0x00000003, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),         0, ATTR_RO},                                // SupportedRinses
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(3),       4, ATTR_RO},                                // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(2),       2, ATTR_RO}                                 // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
}

void cluster_server_rvc_run_mode(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000054;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},  // SupportedModes
        {0x00000001, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),   1, ATTR_RO},  // CurrentMode
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(),   4, ATTR_RO},  // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(3), 2, ATTR_RO}   // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // ChangeToMode
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},

        // Generated commands
        {0x00000001, COMMAND_MASK_GENERATED}, // ChangeToModeResponse
        {chip::kInvalidCommandId, COMMAND_MASK_GENERATED}
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_rvc_clean_mode(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000055;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},  // SupportedModes
        {0x00000001, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),   1, ATTR_RO},  // CurrentMode
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(),   4, ATTR_RO},  // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(4), 2, ATTR_RO}   // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // ChangeToMode
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},

        // Generated commands
        {0x00000001, COMMAND_MASK_GENERATED}, // ChangeToModeResponse
        {chip::kInvalidCommandId, COMMAND_MASK_GENERATED}
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_temperature_control(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000056;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(TEMPERATURE), ZAP_EMPTY_DEFAULT(),   2, ATTR_RO}, // TemperatureSetpoint
        {0x00000001, ZAP_TYPE(TEMPERATURE), ZAP_SIMPLE_DEFAULT(0), 2, ATTR_RO}, // MinTemperature
        {0x00000002, ZAP_TYPE(TEMPERATURE), ZAP_SIMPLE_DEFAULT(5), 2, ATTR_RO}, // MaxTemperature
        {0x00000003, ZAP_TYPE(TEMPERATURE), ZAP_SIMPLE_DEFAULT(5), 2, ATTR_RO}, // Step
        {0x0000FFFC, ZAP_TYPE(BITMAP32),    ZAP_SIMPLE_DEFAULT(5), 4, ATTR_RO}, // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),      ZAP_SIMPLE_DEFAULT(1), 2, ATTR_RO}  // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
}

void cluster_server_refrigerator_alarm(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000057;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(1), 4, ATTR_RO}, // Mask
        {0x00000002, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0), 4, ATTR_RO}, // State
        {0x00000003, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(1), 4, ATTR_RO}, // Supported
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0), 4, ATTR_RO}, // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(1), 2, ATTR_RO}  // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // SetTemperature
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},
    };

    static const EventConfig events[] = {
        {0x00000000},  // Notify
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->eventConfigs.assign(std::begin(events), std::end(events));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_dishwasher_mode(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000059;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},    // SupportedModes
        {0x00000001, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),   1, ATTR_RO},    // CurrentMode
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(),   4, ATTR_RO},    // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(3), 2, ATTR_RO}     // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // ChangeToMode
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},

        // Generated commands
        {0x00000001, COMMAND_MASK_GENERATED}, // ChangeToModeResponse
        {chip::kInvalidCommandId, COMMAND_MASK_GENERATED}
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_air_quality(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000005B;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ENUM8),    ZAP_EMPTY_DEFAULT(),   1, ATTR_RO}, // AirQuality
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(),   4, ATTR_RO}, // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(1), 2, ATTR_RO}  // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
}

void cluster_server_dishwasher_alarm(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000005D;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(1),  4, ATTR_RO}, // Mask
        {0x00000001, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(1),  4, ATTR_RO}, // Latch
        {0x00000002, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0),  4, ATTR_RO}, // State
        {0x00000003, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(15), 4, ATTR_RO}, // Supported
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(1),  4, ATTR_RO}, // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(1),  2, ATTR_RO}  // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // Reset
        {0x00000001, COMMAND_MASK_ACCEPTED},  // ModifyEnabledAlarms
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},
    };

    static const EventConfig events[] = {
        {0x00000000},  // Notify
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->eventConfigs.assign(std::begin(events), std::end(events));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_microwave_oven_mode(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x0000005E;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),    0, ATTR_RO}, // SupportedModes
        {0x00000001, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),    1, ATTR_RO}, // CurrentMode
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(),    4, ATTR_RO}, // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(2),  2, ATTR_RO}  // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
}

void cluster_server_microwave_oven_control(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000005F;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ELAPSED_S), ZAP_EMPTY_DEFAULT(),    4, ATTR_RO}, // CookTime
        {0x00000001, ZAP_TYPE(ELAPSED_S), ZAP_EMPTY_DEFAULT(),    4, ATTR_RO}, // MaxCookTime
        {0x00000002, ZAP_TYPE(INT8U),     ZAP_EMPTY_DEFAULT(),    1, ATTR_RO}, // PowerSetting
        {0x0000FFFC, ZAP_TYPE(BITMAP32),  ZAP_EMPTY_DEFAULT(),    4, ATTR_RO}, // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),    ZAP_SIMPLE_DEFAULT(1),  2, ATTR_RO}  // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // SetCookingParameters
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

static const AttributeConfig kOperationalStateAttributes[] = {
    {0x00000000, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO | ATTR_NULLABLE}, // PhaseList
    {0x00000001, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),   1, ATTR_RO | ATTR_NULLABLE}, // CurrentPhase
    {0x00000003, ZAP_TYPE(ARRAY),    ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},                 // OperationalStateList
    {0x00000004, ZAP_TYPE(ENUM8),    ZAP_EMPTY_DEFAULT(),   1, ATTR_RO},                 // OperationalState
    {0x00000005, ZAP_TYPE(STRUCT),   ZAP_EMPTY_DEFAULT(),   0, ATTR_RO},                 // OperationalError
    {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0), 4, ATTR_RO},                 // FeatureMap
    {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),   2, ATTR_RO}                  // ClusterRevision
};

void cluster_server_operational_state(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000060;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // Pause
        {0x00000001, COMMAND_MASK_ACCEPTED},  // Stop
        {0x00000002, COMMAND_MASK_ACCEPTED},  // Start
        {0x00000003, COMMAND_MASK_ACCEPTED},  // Resume
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},

        // Generated commands
        {0x00000004, COMMAND_MASK_GENERATED}, // OperationalCommandResponse
        {chip::kInvalidCommandId, COMMAND_MASK_GENERATED}
    };

    static const EventConfig events[] = {
        {0x00000000},  // OperationalError
    };

    clusterConfig->attributeConfigs.assign(std::begin(kOperationalStateAttributes), std::end(kOperationalStateAttributes));
    clusterConfig->eventConfigs.assign(std::begin(events), std::end(events));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_rvc_operational_state(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x000000061;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // Pause
        {0x00000003, COMMAND_MASK_ACCEPTED},  // Resume
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},

        // Generated commands
        {0x00000004, COMMAND_MASK_GENERATED}, // OperationalCommandResponse
        {chip::kInvalidCommandId, COMMAND_MASK_GENERATED}
    };

    static const EventConfig events[] = {
        {0x00000000},  // OperationalError
    };

    clusterConfig->attributeConfigs.assign(std::begin(kOperationalStateAttributes), std::end(kOperationalStateAttributes));
    clusterConfig->eventConfigs.assign(std::begin(events), std::end(events));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_scenes_management(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000062;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(INIT_FUNCTION) | ZAP_CLUSTER_MASK(SHUTDOWN_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),        1, ATTR_RO},                  // SceneCount
        {0x00000001, ZAP_TYPE(INT8U),    ZAP_SIMPLE_DEFAULT(0x00),   1, ATTR_RO},                  // CurrentScene
        {0x00000002, ZAP_TYPE(GROUP_ID), ZAP_SIMPLE_DEFAULT(0x0000), 2, ATTR_RO},                  // CurrentGroup
        {0x00000003, ZAP_TYPE(BOOLEAN),  ZAP_SIMPLE_DEFAULT(0x00),   1, ATTR_RO},                  // SceneValid
        {0x00000004, ZAP_TYPE(BITMAP8),  ZAP_EMPTY_DEFAULT(),        1, ATTR_RO},                  // NameSupport
        {0x00000005, ZAP_TYPE(NODE_ID),  uint32_t(0),                8, ATTR_RO | ATTR_NULLABLE},  // LastConfiguredBy
        {0x00000006, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),        2, ATTR_RO},                  // SceneTableSize
        {0x00000007, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),        1, ATTR_RO},                  // RemainingCapacity
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0),      4, ATTR_RO},                  // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(5),      2, ATTR_RO}                   // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // AddScene
        {0x00000001, COMMAND_MASK_ACCEPTED},  // ViewScene
        {0x00000002, COMMAND_MASK_ACCEPTED},  // RemoveScene
        {0x00000003, COMMAND_MASK_ACCEPTED},  // RemoveAllScenes
        {0x00000004, COMMAND_MASK_ACCEPTED},  // StoreScene
        {0x00000005, COMMAND_MASK_ACCEPTED},  // RecallScene
        {0x00000006, COMMAND_MASK_ACCEPTED},  // GetSceneMembership
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},

        // Generated commands
        {0x00000000, COMMAND_MASK_GENERATED}, // AddSceneResponse
        {0x00000001, COMMAND_MASK_GENERATED}, // ViewSceneResponse
        {0x00000002, COMMAND_MASK_GENERATED}, // RemoveSceneResponse
        {0x00000003, COMMAND_MASK_GENERATED}, // RemoveAllScenesResponse
        {0x00000004, COMMAND_MASK_GENERATED}, // StoreSceneResponse
        {0x00000006, COMMAND_MASK_GENERATED}, // GetSceneMembershipResponse
        {chip::kInvalidCommandId, COMMAND_MASK_GENERATED}
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) emberAfScenesManagementClusterServerInitCallback);
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterScenesManagementClusterServerShutdownCallback);
}

void cluster_server_hepa_filter_monitoring(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000071;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(PERCENT),  ZAP_EMPTY_DEFAULT(),    1, ATTR_RO},                   // Condition
        {0x00000001, ZAP_TYPE(ENUM8),    ZAP_EMPTY_DEFAULT(),    1, ATTR_RO},                   // DegradationDirection
        {0x00000002, ZAP_TYPE(ENUM8),    ZAP_EMPTY_DEFAULT(),    1, ATTR_RO},                   // ChangeIndication
        {0x00000003, ZAP_TYPE(BOOLEAN),  ZAP_EMPTY_DEFAULT(),    1, ATTR_RO},                   // InPlaceIndicator
        {0x00000004, ZAP_TYPE(EPOCH_S),  ZAP_EMPTY_DEFAULT(),    4, ATTR_RW | ATTR_NULLABLE},   // LastChangedTime
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(),    4, ATTR_RO},                   // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(1),  2, ATTR_RO}                    // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // ResetCondition
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_activated_carbon_filter_monitoring(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000072;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(PERCENT),  ZAP_EMPTY_DEFAULT(),    1, ATTR_RO},                   // Condition
        {0x00000001, ZAP_TYPE(ENUM8),    ZAP_EMPTY_DEFAULT(),    1, ATTR_RO},                   // DegradationDirection
        {0x00000002, ZAP_TYPE(ENUM8),    ZAP_EMPTY_DEFAULT(),    1, ATTR_RO},                   // ChangeIndication
        {0x00000003, ZAP_TYPE(BOOLEAN),  ZAP_EMPTY_DEFAULT(),    1, ATTR_RO},                   // InPlaceIndicator
        {0x00000004, ZAP_TYPE(EPOCH_S),  ZAP_EMPTY_DEFAULT(),    4, ATTR_RW | ATTR_NULLABLE},   // LastChangedTime
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(),    4, ATTR_RO},                   // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(1),  2, ATTR_RO}                    // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // ResetCondition
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_valve_configuration_and_control(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000081;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ELAPSED_S), ZAP_SIMPLE_DEFAULT(0xFFFFFFFF), 4, ATTR_RO | ATTR_NULLABLE},  // OpenDuration
        {0x00000001, ZAP_TYPE(ELAPSED_S), ZAP_SIMPLE_DEFAULT(20),         4, ATTR_RW | ATTR_NULLABLE},  // DefaultOpenDuration
        {0x00000002, ZAP_TYPE(EPOCH_US),  defaultAutoCloseTime,           8, ATTR_RO | ATTR_NULLABLE},  // AutoCloseTime
        {0x00000003, ZAP_TYPE(ELAPSED_S), ZAP_EMPTY_DEFAULT(),            4, ATTR_RO | ATTR_NULLABLE},  // RemainingDuration
        {0x00000004, ZAP_TYPE(ENUM8),     ZAP_SIMPLE_DEFAULT(0xFF),       1, ATTR_RO | ATTR_NULLABLE},  // CurrentState
        {0x00000005, ZAP_TYPE(ENUM8),     ZAP_SIMPLE_DEFAULT(0xFF),       1, ATTR_RO | ATTR_NULLABLE},  // TargetState
        {0x0000FFFC, ZAP_TYPE(BITMAP32),  ZAP_SIMPLE_DEFAULT(0x1),        4, ATTR_RO},                  // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),    ZAP_SIMPLE_DEFAULT(1),          2, ATTR_RO}                   // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // Open
        {0x00000001, COMMAND_MASK_ACCEPTED},  // Close
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
}

void cluster_server_window_covering(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000102;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(ATTRIBUTE_CHANGED_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ENUM8),         ZAP_SIMPLE_DEFAULT(0x08),      1, ATTR_RO},                 // Type
        {0x00000005, ZAP_TYPE(INT16U),        ZAP_SIMPLE_DEFAULT(0x0000),    2, ATTR_RO},                 // NumberOfActuationsLift
        {0x00000006, ZAP_TYPE(INT16U),        ZAP_SIMPLE_DEFAULT(0x0000),    2, ATTR_RO},                 // NumberOfActuationsTilt
        {0x00000007, ZAP_TYPE(BITMAP8),       ZAP_SIMPLE_DEFAULT(0x0F),      1, ATTR_RO},                 // ConfigStatus
        {0x00000008, ZAP_TYPE(PERCENT),       ZAP_SIMPLE_DEFAULT(50),        1, ATTR_RO | ATTR_NULLABLE}, // CurrentPositionLiftPercentage
        {0x00000009, ZAP_TYPE(PERCENT),       ZAP_EMPTY_DEFAULT(),           1, ATTR_RO | ATTR_NULLABLE}, // CurrentPositionTiltPercentage
        {0x0000000A, ZAP_TYPE(BITMAP8),       ZAP_SIMPLE_DEFAULT(0x00),      1, ATTR_RO},                 // OperationalStatus
        {0x0000000B, ZAP_TYPE(PERCENT100THS), ZAP_SIMPLE_DEFAULT(5000),      2, ATTR_RO | ATTR_NULLABLE}, // TargetPositionLiftPercent100ths
        {0x0000000C, ZAP_TYPE(PERCENT100THS), ZAP_EMPTY_DEFAULT(),           2, ATTR_RO | ATTR_NULLABLE}, // TargetPositionTiltPercent100ths
        {0x0000000D, ZAP_TYPE(ENUM8),         ZAP_SIMPLE_DEFAULT(0x00),      1, ATTR_RO},                 // EndProductType
        {0x0000000E, ZAP_TYPE(PERCENT100THS), ZAP_SIMPLE_DEFAULT(5000),      2, ATTR_RO | ATTR_NULLABLE}, // CurrentPositionLiftPercent100ths
        {0x0000000F, ZAP_TYPE(PERCENT100THS), ZAP_EMPTY_DEFAULT(),           2, ATTR_RO | ATTR_NULLABLE}, // CurrentPositionTiltPercent100ths
        {0x00000017, ZAP_TYPE(BITMAP8),       &windowcoveringValue,          1, ATTR_RW | ATTR_MIN_MAX},  // Mode
        {0x0000001A, ZAP_TYPE(BITMAP16),      ZAP_SIMPLE_DEFAULT(0x00),      2, ATTR_RO},                 // SafetyStatus
        {0x0000FFFC, ZAP_TYPE(BITMAP32),      ZAP_SIMPLE_DEFAULT(23),        4, ATTR_RO},                 // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),        ZAP_EMPTY_DEFAULT(),           2, ATTR_RO}                  // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // UpOrOpen
        {0x00000001, COMMAND_MASK_ACCEPTED},  // DownOrClose
        {0x00000002, COMMAND_MASK_ACCEPTED},  // StopMotion
        {0x00000005, COMMAND_MASK_ACCEPTED},  // GoToLiftPercentage
        {0x00000008, COMMAND_MASK_ACCEPTED},  // GoToTiltPercentage
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterWindowCoveringClusterServerAttributeChangedCallback);
}

void cluster_server_pump_configuration_and_control(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000200;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(INIT_FUNCTION) | \
                          ZAP_CLUSTER_MASK(ATTRIBUTE_CHANGED_FUNCTION) | \
                          ZAP_CLUSTER_MASK(PRE_ATTRIBUTE_CHANGED_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(INT16S),   ZAP_EMPTY_DEFAULT(),           2, ATTR_RO | ATTR_NULLABLE}, // MaxPressure
        {0x00000001, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),           2, ATTR_RO | ATTR_NULLABLE}, // MaxSpeed
        {0x00000002, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),           2, ATTR_RO | ATTR_NULLABLE}, // MaxFlow
        {0x00000003, ZAP_TYPE(INT16S),   ZAP_EMPTY_DEFAULT(),           2, ATTR_RO | ATTR_NULLABLE}, // MinConstPressure
        {0x00000004, ZAP_TYPE(INT16S),   ZAP_EMPTY_DEFAULT(),           2, ATTR_RO | ATTR_NULLABLE}, // MaxConstPressure
        {0x00000011, ZAP_TYPE(ENUM8),    ZAP_EMPTY_DEFAULT(),           1, ATTR_RO},                 // EffectiveOperationMode
        {0x00000012, ZAP_TYPE(ENUM8),    ZAP_EMPTY_DEFAULT(),           1, ATTR_RO},                 // EffectiveControlMode
        {0x00000013, ZAP_TYPE(INT16S),   ZAP_EMPTY_DEFAULT(),           2, ATTR_RO | ATTR_NULLABLE}, // Capacity
        {0x00000020, ZAP_TYPE(ENUM8),    &pumpconfigandcontrolValue,    1, ATTR_RW | ATTR_MIN_MAX},  // OperationMode
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(1),         4, ATTR_RO},                 // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(4),         2, ATTR_RO}                  // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) emberAfPumpConfigurationAndControlClusterServerInitCallback);
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterPumpConfigurationAndControlClusterServerAttributeChangedCallback);
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterPumpConfigurationAndControlClusterServerPreAttributeChangedCallback);
}

void cluster_server_thermostat(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000201;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(INIT_FUNCTION) | \
                          ZAP_CLUSTER_MASK(ATTRIBUTE_CHANGED_FUNCTION) | \
                          ZAP_CLUSTER_MASK(SHUTDOWN_FUNCTION) | \
                          ZAP_CLUSTER_MASK(PRE_ATTRIBUTE_CHANGED_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(TEMPERATURE), ZAP_EMPTY_DEFAULT(),        2, ATTR_RO | ATTR_NULLABLE},  // LocalTemperature
        {0x00000003, ZAP_TYPE(TEMPERATURE), ZAP_SIMPLE_DEFAULT(0x02BC), 2, ATTR_RO},                  // AbsMinHeatSetpointLimit
        {0x00000004, ZAP_TYPE(TEMPERATURE), ZAP_SIMPLE_DEFAULT(0x0BB8), 2, ATTR_RO},                  // AbsMaxHeatSetpointLimit
        {0x00000005, ZAP_TYPE(TEMPERATURE), ZAP_SIMPLE_DEFAULT(0x0640), 2, ATTR_RO},                  // AbsMinCoolSetpointLimit
        {0x00000006, ZAP_TYPE(TEMPERATURE), ZAP_SIMPLE_DEFAULT(0x0C80), 2, ATTR_RO},                  // AbsMaxCoolSetpointLimit
        {0x00000007, ZAP_TYPE(INT8U),       ZAP_EMPTY_DEFAULT(),        1, ATTR_RO},                  // PICoolingDemand
        {0x00000008, ZAP_TYPE(INT8U),       ZAP_EMPTY_DEFAULT(),        1, ATTR_RO},                  // PIHeatingDemand
        {0x00000011, ZAP_TYPE(TEMPERATURE), &thermostatValue[0],        2, ATTR_RW | ATTR_MIN_MAX},   // OccupiedCoolingSetpoint
        {0x00000012, ZAP_TYPE(TEMPERATURE), &thermostatValue[1],        2, ATTR_RW | ATTR_MIN_MAX},   // OccupiedHeatingSetpoint
        {0x00000015, ZAP_TYPE(TEMPERATURE), &thermostatValue[2],        2, ATTR_RW | ATTR_MIN_MAX},   // MinHeatSetpointLimit
        {0x00000016, ZAP_TYPE(TEMPERATURE), &thermostatValue[3],        2, ATTR_RW | ATTR_MIN_MAX},   // MaxHeatSetpointLimit
        {0x00000017, ZAP_TYPE(TEMPERATURE), &thermostatValue[4],        2, ATTR_RW | ATTR_MIN_MAX},   // MinCoolSetpointLimit
        {0x00000018, ZAP_TYPE(TEMPERATURE), &thermostatValue[5],        2, ATTR_RW | ATTR_MIN_MAX},   // MaxCoolSetpointLimit
        {0x0000001B, ZAP_TYPE(ENUM8),       &thermostatValue[6],        1, ATTR_RW | ATTR_MIN_MAX},   // ControlSequenceOfOperation
        {0x0000001C, ZAP_TYPE(ENUM8),       &thermostatValue[7],        1, ATTR_RW | ATTR_MIN_MAX},   // SystemMode
        {0x0000FFFC, ZAP_TYPE(BITMAP32),    ZAP_SIMPLE_DEFAULT(3),      4, ATTR_RO},                  // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),      ZAP_EMPTY_DEFAULT(),        2, ATTR_RO}                   // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // SetpointRaiseLower
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) emberAfThermostatClusterServerInitCallback);
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterThermostatClusterServerAttributeChangedCallback);
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterThermostatClusterServerShutdownCallback);
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterThermostatClusterServerPreAttributeChangedCallback);
}

void cluster_server_fan_control(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000202;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(ATTRIBUTE_CHANGED_FUNCTION) | ZAP_CLUSTER_MASK(PRE_ATTRIBUTE_CHANGED_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ENUM8),    &fancontrolValue[0],      1, ATTR_RW | ATTR_MIN_MAX},                 // FanMode
        {0x00000001, ZAP_TYPE(ENUM8),    ZAP_SIMPLE_DEFAULT(0x02), 1, ATTR_RO},                                // FanModeSequence
        {0x00000002, ZAP_TYPE(PERCENT),  &fancontrolValue[1],      1, ATTR_RW | ATTR_NULLABLE | ATTR_MIN_MAX}, // PercentSetting
        {0x00000003, ZAP_TYPE(PERCENT),  ZAP_SIMPLE_DEFAULT(0x00), 1, ATTR_RO},                                // PercentCurrent
        {0x00000004, ZAP_TYPE(INT8U),    ZAP_SIMPLE_DEFAULT(10),   1, ATTR_RO},                                // SpeedMax
        {0x00000005, ZAP_TYPE(INT8U),    &fancontrolValue[2],      1, ATTR_RW | ATTR_NULLABLE | ATTR_MIN_MAX}, // SpeedSetting
        {0x00000006, ZAP_TYPE(INT8U),    ZAP_SIMPLE_DEFAULT(0x00), 1, ATTR_RO},                                // SpeedCurrent
        {0x00000007, ZAP_TYPE(BITMAP8),  ZAP_SIMPLE_DEFAULT(0x01), 1, ATTR_RO},                                // RockSupport
        {0x00000008, ZAP_TYPE(BITMAP8),  &fancontrolValue[3],      1, ATTR_RW | ATTR_MIN_MAX},                 // RockSetting
        {0x00000009, ZAP_TYPE(BITMAP8),  ZAP_SIMPLE_DEFAULT(0x03), 1, ATTR_RO},                                // WindSupport
        {0x0000000A, ZAP_TYPE(BITMAP8),  &fancontrolValue[4],      1, ATTR_RW | ATTR_MIN_MAX},                 // WindSetting
        {0x0000000B, ZAP_TYPE(ENUM8),    &fancontrolValue[5],      1, ATTR_RW | ATTR_MIN_MAX},                 // AirflowDirection
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0x3F), 4, ATTR_RO},                                // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(5),    2, ATTR_RO}                                 // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // Step
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterFanControlClusterServerAttributeChangedCallback);
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterFanControlClusterServerPreAttributeChangedCallback);
}

void cluster_server_thermostat_user_interface_configuration(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000204;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(PRE_ATTRIBUTE_CHANGED_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(ENUM8),    &thermostatUIValue[0], 1, ATTR_RW | ATTR_MIN_MAX}, // TemperatureDisplayMode
        {0x00000001, ZAP_TYPE(ENUM8),    &thermostatUIValue[1], 1, ATTR_RW | ATTR_MIN_MAX}, // KeypadLockout
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0),          4, ATTR_RO},       // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(2),          2, ATTR_RO}        // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterThermostatUserInterfaceConfigurationClusterServerPreAttributeChangedCallback);
}

void cluster_server_color_control(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000300;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(INIT_FUNCTION) | ZAP_CLUSTER_MASK(SHUTDOWN_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(INT8U),    ZAP_SIMPLE_DEFAULT(0x00),   1, ATTR_RO},                                // CurrentHue
        {0x00000001, ZAP_TYPE(INT8U),    ZAP_SIMPLE_DEFAULT(0x00),   1, ATTR_RO},                                // CurrentSaturation
        {0x00000002, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x0000), 2, ATTR_RO},                                // RemainingTime
        {0x00000003, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x616B), 2, ATTR_RO},                                // CurrentX
        {0x00000004, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x607D), 2, ATTR_RO},                                // CurrentY
        {0x00000007, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x00FA), 2, ATTR_RO},                                // ColorTemperatureMireds
        {0x00000008, ZAP_TYPE(ENUM8),    ZAP_SIMPLE_DEFAULT(0x01),   1, ATTR_RO},                                // ColorMode
        {0x0000000F, ZAP_TYPE(BITMAP8),  ZAP_SIMPLE_DEFAULT(0x00),   1, ATTR_RW},                                // Options
        {0x00000010, ZAP_TYPE(INT8U),    ZAP_EMPTY_DEFAULT(),        1, ATTR_RO | ATTR_NULLABLE},                // NumberOfPrimaries
        {0x00004000, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x0000), 2, ATTR_RO},                                // EnhancedCurrentHue
        {0x00004001, ZAP_TYPE(ENUM8),    ZAP_SIMPLE_DEFAULT(0x01),   1, ATTR_RO},                                // EnhancedColorMode
        {0x00004002, ZAP_TYPE(INT8U),    ZAP_SIMPLE_DEFAULT(0x00),   1, ATTR_RO},                                // ColorLoopActive
        {0x00004003, ZAP_TYPE(INT8U),    ZAP_SIMPLE_DEFAULT(0x00),   1, ATTR_RO},                                // ColorLoopDirection
        {0x00004004, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x0019), 2, ATTR_RO},                                // ColorLoopTime
        {0x00004005, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x2300), 2, ATTR_RO},                                // ColorLoopStartEnhancedHue
        {0x00004006, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x0000), 2, ATTR_RO},                                // ColorLoopStoredEnhancedHue
        {0x0000400A, ZAP_TYPE(BITMAP16), ZAP_SIMPLE_DEFAULT(0x1F),   2, ATTR_RO},                                // ColorCapabilities
        {0x0000400B, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x0001), 2, ATTR_RO},                                // MinMireds
        {0x0000400C, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0xFEFF), 2, ATTR_RO},                                // MaxMireds
        {0x0000400D, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(1),      2, ATTR_RO},                                // CoupleColorTemp
        {0x00004010, ZAP_TYPE(INT16U),   &colorcontrolValue,         2, ATTR_RW | ATTR_NULLABLE | ATTR_MIN_MAX}, // StartUpColorTemperature
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0x1F),   4, ATTR_RO},                                // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(8),      2, ATTR_RO}                                 // ClusterRevision
    };

    static const CommandConfig commands[] = {
        // Accepted commands
        {0x00000000, COMMAND_MASK_ACCEPTED},  // MoveToHue
        {0x00000001, COMMAND_MASK_ACCEPTED},  // MoveHue
        {0x00000002, COMMAND_MASK_ACCEPTED},  // StepHue
        {0x00000003, COMMAND_MASK_ACCEPTED},  // MoveToSaturation
        {0x00000004, COMMAND_MASK_ACCEPTED},  // MoveSaturation
        {0x00000005, COMMAND_MASK_ACCEPTED},  // StepSaturation
        {0x00000006, COMMAND_MASK_ACCEPTED},  // MoveToHueAndSaturation
        {0x00000007, COMMAND_MASK_ACCEPTED},  // MoveToColor
        {0x00000008, COMMAND_MASK_ACCEPTED},  // MoveColor
        {0x00000009, COMMAND_MASK_ACCEPTED},  // StepColor
        {0x0000000A, COMMAND_MASK_ACCEPTED},  // MoveToColorTemperature
        {0x00000040, COMMAND_MASK_ACCEPTED},  // EnhancedMoveToHue
        {0x00000041, COMMAND_MASK_ACCEPTED},  // EnhancedMoveHue
        {0x00000042, COMMAND_MASK_ACCEPTED},  // EnhancedStepHue
        {0x00000043, COMMAND_MASK_ACCEPTED},  // EnhancedMoveToHueAndSaturation
        {0x00000044, COMMAND_MASK_ACCEPTED},  // ColorLoopSet
        {0x00000047, COMMAND_MASK_ACCEPTED},  // StopMoveStep
        {0x0000004B, COMMAND_MASK_ACCEPTED},  // MoveColorTemperature
        {0x0000004C, COMMAND_MASK_ACCEPTED},  // StepColorTemperature
        {chip::kInvalidCommandId, COMMAND_MASK_ACCEPTED},
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->commandConfigs.assign(std::begin(commands), std::end(commands));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) emberAfColorControlClusterServerInitCallback);
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) MatterColorControlClusterServerShutdownCallback);
}

void cluster_server_illuminance_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000400;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x0000), 2, ATTR_RO | ATTR_NULLABLE}, // MeasuredValue
        {0x00000001, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x01),   2, ATTR_RO | ATTR_NULLABLE}, // MinMeasuredValue
        {0x00000002, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0xFFFE), 2, ATTR_RO | ATTR_NULLABLE}, // MaxMeasuredValue
        {0x00000003, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),        2, ATTR_RO},                 // Tolerance
        {0x00000004, ZAP_TYPE(ENUM8),    ZAP_SIMPLE_DEFAULT(0xFF),   1, ATTR_RO | ATTR_NULLABLE}, // LightSensorType
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0),      4, ATTR_RO},                 // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(3),      2, ATTR_RO}                  // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
}

void cluster_server_temperature_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000402;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(TEMPERATURE), ZAP_SIMPLE_DEFAULT(0x8000), 2, ATTR_RO | ATTR_NULLABLE}, // MeasuredValue
        {0x00000001, ZAP_TYPE(TEMPERATURE), ZAP_SIMPLE_DEFAULT(0x8000), 2, ATTR_RO | ATTR_NULLABLE}, // MinMeasuredValue
        {0x00000002, ZAP_TYPE(TEMPERATURE), ZAP_SIMPLE_DEFAULT(0x8000), 2, ATTR_RO | ATTR_NULLABLE}, // MaxMeasuredValue
        {0x00000003, ZAP_TYPE(INT16U),      ZAP_EMPTY_DEFAULT(),        2, ATTR_RO},                 // Tolerance
        {0x0000FFFC, ZAP_TYPE(BITMAP32),    ZAP_SIMPLE_DEFAULT(0),      4, ATTR_RO},                 // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),      ZAP_SIMPLE_DEFAULT(4),      2, ATTR_RO}                  // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
}

void cluster_server_pressure_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x0000403;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(INT16S),   ZAP_SIMPLE_DEFAULT(0x0000), 2, ATTR_RO | ATTR_NULLABLE}, // MeasuredValue
        {0x00000001, ZAP_TYPE(INT16S),   ZAP_EMPTY_DEFAULT(),        2, ATTR_RO | ATTR_NULLABLE}, // MinMeasuredValue
        {0x00000002, ZAP_TYPE(INT16S),   ZAP_EMPTY_DEFAULT(),        2, ATTR_RO | ATTR_NULLABLE}, // MaxMeasuredValue
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0),      4, ATTR_RO},                 // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(3),      2, ATTR_RO}                  // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
}

void cluster_server_flow_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000404;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(5),   2, ATTR_RO | ATTR_NULLABLE}, // MeasuredValue
        {0x00000001, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0),   2, ATTR_RO | ATTR_NULLABLE}, // MinMeasuredValue
        {0x00000002, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(100), 2, ATTR_RO | ATTR_NULLABLE}, // MaxMeasuredValue
        {0x00000003, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0),   2, ATTR_RO},                 // Tolerance
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0),   4, ATTR_RO},                 // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(3),   2, ATTR_RO}                  // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
}

void cluster_server_relative_humidity_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000405;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),        2, ATTR_RO | ATTR_NULLABLE}, // MeasuredValue
        {0x00000001, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0),      2, ATTR_RO | ATTR_NULLABLE}, // MinMeasuredValue
        {0x00000002, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(0x2710), 2, ATTR_RO | ATTR_NULLABLE}, // MaxMeasuredValue
        {0x00000003, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),        2, ATTR_RO},                 // Tolerance
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_SIMPLE_DEFAULT(0),      4, ATTR_RO},                 // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_SIMPLE_DEFAULT(3),      2, ATTR_RO}                  // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
}

void cluster_server_occupancy_sensing(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000406;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER) | ZAP_CLUSTER_MASK(INIT_FUNCTION);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(BITMAP8),  ZAP_EMPTY_DEFAULT(),   1, ATTR_RO}, // Occupancy
        {0x00000001, ZAP_TYPE(ENUM8),    ZAP_SIMPLE_DEFAULT(1), 1, ATTR_RO}, // OccupancySensorType
        {0x00000002, ZAP_TYPE(BITMAP8),  ZAP_SIMPLE_DEFAULT(2), 1, ATTR_RO}, // OccupancySensorTypeBitmap
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(),   4, ATTR_RO}, // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(),   2, ATTR_RO}  // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
    clusterConfig->functionConfigs.push_back((EmberAfGenericClusterFunction) emberAfOccupancySensingClusterServerInitCallback);
}

static const AttributeConfig kConcentrationMeasurementAttributes[] = {
    {0x00000000, ZAP_TYPE(SINGLE),    ZAP_EMPTY_DEFAULT(),   4, ATTR_RO | ATTR_NULLABLE},  // MeasuredValue
    {0x00000001, ZAP_TYPE(SINGLE),    ZAP_EMPTY_DEFAULT(),   4, ATTR_RO | ATTR_NULLABLE},  // MinMeasuredValue
    {0x00000002, ZAP_TYPE(SINGLE),    ZAP_EMPTY_DEFAULT(),   4, ATTR_RO | ATTR_NULLABLE},  // MaxMeasuredValue
    {0x00000003, ZAP_TYPE(SINGLE),    ZAP_EMPTY_DEFAULT(),   4, ATTR_RO | ATTR_NULLABLE},  // PeakMeasuredValue
    {0x00000004, ZAP_TYPE(ELAPSED_S), ZAP_EMPTY_DEFAULT(),   4, ATTR_RO},                  // PeakMeasuredValueWindow
    {0x00000005, ZAP_TYPE(SINGLE),    ZAP_EMPTY_DEFAULT(),   4, ATTR_RO | ATTR_NULLABLE},  // AverageMeasuredValue
    {0x00000006, ZAP_TYPE(ELAPSED_S), ZAP_EMPTY_DEFAULT(),   4, ATTR_RO},                  // AverageMeasuredValueWindow
    {0x00000007, ZAP_TYPE(SINGLE),    ZAP_EMPTY_DEFAULT(),   4, ATTR_RO},                  // Uncertainty
    {0x00000008, ZAP_TYPE(ENUM8),     ZAP_EMPTY_DEFAULT(),   1, ATTR_RO},                  // MeasurementUnit
    {0x00000009, ZAP_TYPE(ENUM8),     ZAP_EMPTY_DEFAULT(),   1, ATTR_RO},                  // MeasurementMedium
    {0x0000000A, ZAP_TYPE(ENUM8),     ZAP_EMPTY_DEFAULT(),   1, ATTR_RO},                  // LevelValue
    {0x0000FFFC, ZAP_TYPE(BITMAP32),  ZAP_EMPTY_DEFAULT(),   4, ATTR_RO},                  // FeatureMap
    {0x0000FFFD, ZAP_TYPE(INT16U),    ZAP_SIMPLE_DEFAULT(3), 2, ATTR_RO}                   // ClusterRevision
};

void cluster_server_carbon_monoxide_concentration_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x0000040C;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);
    clusterConfig->attributeConfigs.assign(
                    std::begin(kConcentrationMeasurementAttributes), std::end(kConcentrationMeasurementAttributes));
}

void cluster_server_carbon_dioxide_concentration_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x0000040D;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);
    clusterConfig->attributeConfigs.assign(
                    std::begin(kConcentrationMeasurementAttributes), std::end(kConcentrationMeasurementAttributes));
}

void cluster_server_nitrogen_dioxide_concentration_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000413;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);
    clusterConfig->attributeConfigs.assign(
                    std::begin(kConcentrationMeasurementAttributes), std::end(kConcentrationMeasurementAttributes));
}

void cluster_server_ozone_concentration_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000415;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);
    clusterConfig->attributeConfigs.assign(
                    std::begin(kConcentrationMeasurementAttributes), std::end(kConcentrationMeasurementAttributes));
}

void cluster_server_pm25_concentration_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x0000042A;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);
    clusterConfig->attributeConfigs.assign(
                    std::begin(kConcentrationMeasurementAttributes), std::end(kConcentrationMeasurementAttributes));
}

void cluster_server_formaldehyde_concentration_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x0000042B;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);
    clusterConfig->attributeConfigs.assign(
                    std::begin(kConcentrationMeasurementAttributes), std::end(kConcentrationMeasurementAttributes));
}

void cluster_server_pm1_concentration_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x0000042C;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);
    clusterConfig->attributeConfigs.assign(
                    std::begin(kConcentrationMeasurementAttributes), std::end(kConcentrationMeasurementAttributes));
}

void cluster_server_pm10_concentration_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x0000042D;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);
    clusterConfig->attributeConfigs.assign(
                    std::begin(kConcentrationMeasurementAttributes), std::end(kConcentrationMeasurementAttributes));
}

void cluster_server_total_volatile_concentration_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x0000042E;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);
    clusterConfig->attributeConfigs.assign(
                    std::begin(kConcentrationMeasurementAttributes), std::end(kConcentrationMeasurementAttributes));
}

void cluster_server_radon_concentration_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x0000042F;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);
    clusterConfig->attributeConfigs.assign(
                    std::begin(kConcentrationMeasurementAttributes), std::end(kConcentrationMeasurementAttributes));
}

void cluster_server_soil_measurement(ClusterConfig *clusterConfig)
{
    clusterConfig->clusterId = 0x00000430;
    clusterConfig->mask = ZAP_CLUSTER_MASK(SERVER);

    static const AttributeConfig attributes[] = {
        {0x00000000, ZAP_TYPE(STRUCT),   ZAP_EMPTY_DEFAULT(), 0, ATTR_RO},                  // SoilMoistureMeasurementLimits
        {0x00000001, ZAP_TYPE(PERCENT),  ZAP_EMPTY_DEFAULT(), 1, ATTR_RO | ATTR_NULLABLE},  // SoilMoistureMeasuredValue
        {0x0000FFFC, ZAP_TYPE(BITMAP32), ZAP_EMPTY_DEFAULT(), 4, ATTR_RO},                  // FeatureMap
        {0x0000FFFD, ZAP_TYPE(INT16U),   ZAP_EMPTY_DEFAULT(), 2, ATTR_RO}                   // ClusterRevision
    };

    clusterConfig->attributeConfigs.assign(std::begin(attributes), std::end(attributes));
}

} // Clusters

namespace Endpoints
{

using PresetFunc = void (*)(ClusterConfig *);
inline void addCluster(EndpointConfig *endpoint, PresetFunc preset)
{
    ClusterConfig cluster;
    preset(&cluster);
    endpoint->clusterConfigs.push_back(cluster);
}

static void matter_default_light_preset(EndpointConfig *mEndpointConfig)
{
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_descriptor);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_identify);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_groups);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_onoff);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_level_control);
}

void matter_air_purifier_preset(EndpointConfig *mEndpointConfig)
{
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_identify);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_descriptor);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_hepa_filter_monitoring);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_activated_carbon_filter_monitoring);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_fan_control);
}

void matter_room_air_con_preset(EndpointConfig *mEndpointConfig)
{
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_identify);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_onoff);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_descriptor);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_thermostat);
}

void matter_onoff_light_preset(EndpointConfig *mEndpointConfig)
{
    matter_default_light_preset(mEndpointConfig);
}

void matter_dimmable_light_preset(EndpointConfig *mEndpointConfig)
{
    matter_default_light_preset(mEndpointConfig);
}

void matter_color_temperature_light_preset(EndpointConfig *mEndpointConfig)
{
    matter_default_light_preset(mEndpointConfig);
    addCluster(mEndpointConfig, Presets::Clusters::cluster_server_color_control);
}

} // Endpoints

} // Presets
