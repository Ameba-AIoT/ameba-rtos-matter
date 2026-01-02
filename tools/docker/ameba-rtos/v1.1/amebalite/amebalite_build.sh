#!/bin/bash

set -e

handle_error() {
  echo "Error: $1"
  exit 1
}

if [ -z "$1" ]; then
  echo "Error: No base directory provided."
  echo "Usage: $0 <base_directory>"
  exit 1
fi

basedir=$1
chipdir=${basedir}/connectedhomeip
amebadir=${basedir}/ameba-rtos

if [ ! -d "$chipdir" ]; then
  handle_error "Directory ${chipdir} does not exist."
fi

if [ ! -d "$amebadir" ]; then
  handle_error "Directory ${amebadir} does not exist."
fi

cd ${chipdir}
source ${chipdir}/scripts/activate.sh

cd ${amebadir}

chmod u+x matter_setup.sh
.//matter_setup.sh ameba-rtos v1.5 || handle_error "Failed to run matter_setup.sh for ameba-rtos"

mkdir ${amebadir}/amebalite_gcc_project/menuconfig/
mkdir ${amebadir}/amebalite_gcc_project/menuconfig/project_kr4/
mkdir ${amebadir}/amebalite_gcc_project/menuconfig/project_km4/

cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebalite/inc/kr4/platform_autoconf.h ${amebadir}/amebalite_gcc_project/menuconfig/project_kr4/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebalite/inc/km4/platform_autoconf.h ${amebadir}/amebalite_gcc_project/menuconfig/project_km4/

cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebalite/menuconfig/.config     ${amebadir}/amebalite_gcc_project/menuconfig/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebalite/menuconfig/.config_kr4 ${amebadir}/amebalite_gcc_project/menuconfig/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebalite/menuconfig/.config_km4 ${amebadir}/amebalite_gcc_project/menuconfig/

echo "Build firmware"
cd ${amebadir}/amebalite_gcc_project/

echo "Building all_clusters examples"
python build.py -D MATTER_EXAMPLE=all_clusters || handle_error "Failed to build all_clusters"

echo "Build all_clusters examples completed"
cd build/ && ninja clean_matter_libs clean && cd ../ && rm -rf build/ || handle_error "Failed to clean"

echo "Building light_port example"
python build.py -D MATTER_EXAMPLE=light_port || handle_error "Failed to build light_port"

echo "Build light_port example completed"
cd build/ && ninja clean_matter_libs clean && cd ../ && rm -rf build/ || handle_error "Failed to clean"
