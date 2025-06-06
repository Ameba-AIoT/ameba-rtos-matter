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
amebadir=${basedir}/ameba-rtos-z2

if [ ! -d "$chipdir" ]; then
  handle_error "Directory ${chipdir} does not exist."
fi

if [ ! -d "$amebadir" ]; then
  handle_error "Directory ${amebadir} does not exist."
fi

cd ${chipdir}
source ${chipdir}/scripts/bootstrap.sh
source ${chipdir}/scripts/activate.sh

echo "Changing directory to build location ${amebadir}"

cd ${amebadir}

cp ${amebadir}/component/common/application/matter/tools/docker/matter_version_selection.sh ${amebadir}/component/common/application/matter/tools/scripts/matter_version_selection.sh

chmod u+x matter_setup.sh
./matter_setup.sh amebaz2 || handle_error "Failed to run matter_setup.sh for amebaz2"

cd ${amebadir}/project/realtek_amebaz2_v0_example/GCC-RELEASE/

echo "Building all_clusters"
make all_clusters || handle_error "Failed to build all_clusters"

echo "Building firmware image"
make is_matter || handle_error "Failed to build firmware image for all_clusters"

echo "Build all-clusters-app completed"
make clean_matter_libs || handle_error "Failed to clean matter libraries"
make clean_matter || handle_error "Failed to clean matter"

echo "Building lighting"
make light || handle_error "Failed to build lighting"

echo "Building firmware image"
make is_matter || handle_error "Failed to build firmware image for all_clusters"

echo "Build lighting completed"
make clean_matter_libs || handle_error "Failed to clean matter libraries"
make clean_matter || handle_error "Failed to clean matter"

cd ${amebadir}/
chmod u+x matter_setup.sh
./matter_setup.sh amebaz2plus || handle_error "Failed to run matter_setup.sh for amebaz2plus"

cd ${amebadir}/project/realtek_amebaz2plus_v0_example/GCC-RELEASE/

echo "Building all_clusters"
make all_clusters || handle_error "Failed to build all_clusters"

echo "Building firmware image"
make is_matter || handle_error "Failed to build firmware image for all_clusters"

echo "Build all-clusters-app completed"
make clean_matter_libs || handle_error "Failed to clean matter libraries"
make clean_matter || handle_error "Failed to clean matter"

echo "Building lighting"
make light || handle_error "Failed to build lighting"

echo "Building firmware image"
make is_matter || handle_error "Failed to build firmware image for all_clusters"

echo "Build lighting completed"
make clean_matter_libs || handle_error "Failed to clean matter libraries"
make clean_matter || handle_error "Failed to clean matter"
