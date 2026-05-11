#!/bin/bash

set -e

PATH_TO_AMEBA_RTOS=$(cat .PATH_TO_AMEBA_RTOS)
AMEBA_PY="python $PATH_TO_AMEBA_RTOS/ameba.py"

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
matterdir=${basedir}/ameba-rtos-matter

if [ ! -d "$chipdir" ]; then
  handle_error "Directory ${chipdir} does not exist."
fi

if [ ! -d "$amebadir" ]; then
  handle_error "Directory ${amebadir} does not exist."
fi

if [ ! -d "$matterdir" ]; then
  handle_error "Directory ${matterdir} does not exist."
fi

cd ${matterdir}

echo $PATH

source matter_env.sh

echo $PATH

echo "Building firmware"

$AMEBA_PY soc RTL8726E

cp ${matterdir}/tools/docker/ameba-rtos/v1.2/config/amebadplus_lite_green2_matter_proj.conf ${matterdir}/project/cmake/config/ameba-rtos-v1.2/matter_proj.conf

matter_apply_conf

echo "Building all_clusters example"
matter_build_proj all_clusters || handle_error "Failed to build all_clusters"

echo "Build all_clusters example completed"
matter_clean_proj

echo "Building light_port example"
matter_build_proj light_port || handle_error "Failed to build light_port"

echo "Build light_port example completed"
matter_clean_proj
