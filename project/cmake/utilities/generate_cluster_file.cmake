cmake_minimum_required(VERSION 3.6)

# Generate Cluster File

file(MAKE_DIRECTORY ${CODEGEN_DIR}/zap-generated)
execute_process(COMMAND python3 ${CHIPDIR}/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json -z ${CHIPDIR}/src/app/zap-templates/zcl/zcl.json --output-dir ${CODEGEN_DIR}/zap-generated ${ZAP_FILE})
execute_process(COMMAND python3 ${CHIPDIR}/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json -z ${CHIPDIR}/src/app/zap-templates/zcl/zcl.json --output-dir ${CODEGEN_DIR}/zap-generated ${ZAP_FILE})
execute_process(COMMAND python3 ${CHIPDIR}/scripts/codegen.py --generator cpp-app --output-dir ${CODEGEN_DIR} --expected-outputs ${MATTER_TOOLDIR}/codegen_helpers/expected.outputs ${MATTER_FILE})
execute_process(COMMAND python3 ${CHIPDIR}/src/app/zap_cluster_list.py --zap_file ${ZAP_FILE} OUTPUT_FILE ${CLUSTER_FILE})
execute_process(COMMAND python3 ${MATTER_TOOLDIR}/codegen_helpers/parse_clusters.py --cluster_file ${CLUSTER_FILE} --chip_path ${CHIPDIR})