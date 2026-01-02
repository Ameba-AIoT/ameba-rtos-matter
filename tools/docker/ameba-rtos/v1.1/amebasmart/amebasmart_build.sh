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

mkdir ${amebadir}/amebasmart_gcc_project/menuconfig/
mkdir ${amebadir}/amebasmart_gcc_project/menuconfig/project_lp/
mkdir ${amebadir}/amebasmart_gcc_project/menuconfig/project_hp/
mkdir ${amebadir}/amebasmart_gcc_project/menuconfig/project_ap/

cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebasmart/inc/lp/platform_autoconf.h ${amebadir}/amebasmart_gcc_project/menuconfig/project_lp/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebasmart/inc/hp/platform_autoconf.h ${amebadir}/amebasmart_gcc_project/menuconfig/project_hp/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebasmart/inc/ap/platform_autoconf.h ${amebadir}/amebasmart_gcc_project/menuconfig/project_ap/

cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebasmart/menuconfig/.config      ${amebadir}/amebasmart_gcc_project/menuconfig/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebasmart/menuconfig/.config_km0  ${amebadir}/amebasmart_gcc_project/menuconfig/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebasmart/menuconfig/.config_km4  ${amebadir}/amebasmart_gcc_project/menuconfig/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.1/amebasmart/menuconfig/.config_ca32 ${amebadir}/amebasmart_gcc_project/menuconfig/

echo "Build firmware"
cd ${amebadir}/amebasmart_gcc_project/

echo "Building all_clusters examples"
python build.py -D MATTER_EXAMPLE=all_clusters || handle_error "Failed to build all_clusters"

echo "Build all_clusters examples completed"
cd build/ && ninja clean_matter_libs clean && cd ../ && rm -rf build/ || handle_error "Failed to clean"

echo "Building light_port example"
python build.py -D MATTER_EXAMPLE=light_port || handle_error "Failed to build light_port"

echo "Build light_port example completed"
cd build/ && ninja clean_matter_libs clean && cd ../ && rm -rf build/ || handle_error "Failed to clean"
