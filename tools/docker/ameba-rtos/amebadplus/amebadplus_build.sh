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
.//matter_setup.sh ameba-rtos v1.3 || handle_error "Failed to run matter_setup.sh for ameba-rtos"

cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/amebadplus/inc/km0/platform_autoconf.h ${amebadir}/amebadplus_gcc_project/project_km0/inc/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/amebadplus/inc/km4/platform_autoconf.h ${amebadir}/amebadplus_gcc_project/project_km4/inc/

cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/amebadplus/menuconfig/.config     ${amebadir}/amebadplus_gcc_project/menuconfig/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/amebadplus/menuconfig/.config_km0 ${amebadir}/amebadplus_gcc_project/menuconfig/
cp ${amebadir}/component/application/matter/tools/docker/ameba-rtos/amebadplus/menuconfig/.config_km4 ${amebadir}/amebadplus_gcc_project/menuconfig/

echo "Clean SDK"
cd ${amebadir}/amebadplus_gcc_project/
make clean || handle_error "Failed to clean SDK"

echo "Build project_km4"
cd ${amebadir}/amebadplus_gcc_project/project_km4/

echo "Building all_clusters"
make -C asdk all_clusters || handle_error "Failed to build all_clusters"

echo "Building firmware image"
make EXAMPLE=all_clusters || handle_error "Failed to build project_km4 firmware image"

echo "Build all-clusters-app completed"
make clean || handle_error "Failed to clean"

echo "Building lighting"
make -C asdk light_port || handle_error "Failed to build light_port"

echo "Building firmware image"
make EXAMPLE=light || handle_error "Failed to build project_km4 firmware image"

echo "Build light_port completed"
make clean || handle_error "Failed to clean"