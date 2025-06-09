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
amebadir=${basedir}/ameba-rtos-d

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

cp ${amebadir}/component/common/application/matter/tools/docker/matter_version_selection.sh ${amebadir}/component/common/application/matter/tools/scripts/matter_version_selection.sh

chmod u+x matter_setup.sh
.//matter_setup.sh amebad || handle_error "Failed to run matter_setup.sh for amebad"

cp ${amebadir}/component/common/application/matter/tools/docker/amebad/platform_autoconf.h ${amebadir}/project/realtek_amebaD_va0_example/inc/inc_hp

cp ${amebadir}/component/common/application/matter/tools/docker/amebad/.config ${amebadir}/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/asdk/config

#echo "Build project_lp"
#cd ${amebadir}/project/realtek_amebaD_va0_example/GCC-RELEASE/project_lp
#make all

echo "Build project_hp"
cd ${amebadir}/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp

echo "Building all_clusters"
make -C asdk all_clusters || handle_error "Failed to build all_clusters"

echo "Building firmware image"
make all || handle_error "Failed to build project_hp firmware image"

echo "Build all-clusters-app completed"
make clean || handle_error "Failed to clean"

echo "Building lighting"
make -C asdk light || handle_error "Failed to build light"

echo "Building firmware image"
make all || handle_error "Failed to build project_hp firmware image"

echo "Build lighting completed"
make clean || handle_error "Failed to clean"
