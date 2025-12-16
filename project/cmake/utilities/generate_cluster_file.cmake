cmake_minimum_required(VERSION 3.6)

# Generate Cluster File

file(MAKE_DIRECTORY ${CODEGEN_DIR}/zap-generated)
execute_process(COMMAND python3 ${CHIP_DIR}/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json -z ${CHIP_DIR}/src/app/zap-templates/zcl/zcl.json --output-dir ${CODEGEN_DIR}/zap-generated ${ZAP_FILE})
execute_process(COMMAND python3 ${CHIP_DIR}/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json -z ${CHIP_DIR}/src/app/zap-templates/zcl/zcl.json --output-dir ${CODEGEN_DIR}/zap-generated ${ZAP_FILE})
execute_process(COMMAND python3 ${CHIP_DIR}/scripts/codegen.py --generator cpp-app --output-dir ${CODEGEN_DIR} --expected-outputs ${MATTER_TOOL_DIR}/codegen_helpers/expected.outputs ${MATTER_FILE})
execute_process(COMMAND python3 ${CHIP_DIR}/src/app/zap_cluster_list.py --zap_file ${ZAP_FILE} OUTPUT_FILE ${CLUSTER_FILE})
execute_process(COMMAND python3 ${MATTER_TOOL_DIR}/codegen_helpers/parse_clusters.py --cluster_file ${CLUSTER_FILE} --chip_path ${CHIP_DIR})
