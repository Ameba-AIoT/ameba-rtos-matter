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
source ${chipdir}/scripts/bootstrap.sh
source ${chipdir}/scripts/activate.sh

cd ${amebadir}

chmod u+x matter_setup.sh
.//matter_setup.sh ameba-rtos v1.4 || handle_error "Failed to run matter_setup.sh for ameba-rtos"

cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.0/amebasmart/inc/lp/platform_autoconf.h ${amebadir}/amebasmart_gcc_project/project_lp/inc/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.0/amebasmart/inc/hp/platform_autoconf.h ${amebadir}/amebasmart_gcc_project/project_hp/inc/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.0/amebasmart/inc/ap/platform_autoconf.h ${amebadir}/amebasmart_gcc_project/project_ap/inc/

cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.0/amebasmart/menuconfig/.config    ${amebadir}/amebasmart_gcc_project/menuconfig/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.0/amebasmart/menuconfig/.config_lp ${amebadir}/amebasmart_gcc_project/menuconfig/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.0/amebasmart/menuconfig/.config_hp ${amebadir}/amebasmart_gcc_project/menuconfig/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/v1.0/amebasmart/menuconfig/.config_ap ${amebadir}/amebasmart_gcc_project/menuconfig/

echo "Clean SDK"
cd ${amebadir}/amebasmart_gcc_project/
make clean || handle_error "Failed to clean SDK"

echo "Build Matter libraries and firmware image"
cd ${amebadir}/amebasmart_gcc_project/

echo "Building all_clusters"
make -C project_ap/asdk all_clusters || handle_error "Failed to build all_clusters"

echo "Building firmware image"
make all MATTER_EXAMPLE=chiptest || handle_error "Failed to build firmware image"

echo "Build all-clusters-app completed"
make clean || handle_error "Failed to clean"

echo "Building lighting"
make -C project_ap/asdk light_port || handle_error "Failed to build light_port"

echo "Building firmware image"
make all MATTER_EXAMPLE=light || handle_error "Failed to build firmware image"

echo "Build light_port completed"
make clean || handle_error "Failed to clean"