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
amebadir=${basedir}/ameba-rtos-z2
chipdir=${basedir}/connectedhomeip
matterdir=${amebadir}/component/common/application/matter
tooldir=${matterdir}/tools

platform_opts=${matterdir}/common/include/platform_opts_matter.h

opt=(
    "CONFIG_EXAMPLE_MATTER_CHIPTEST"
    "CONFIG_EXAMPLE_MATTER_LIGHT"
)

select_opts() {
    for option in "${opt[@]}"; do
        if grep -q "^#define $option[[:space:]]*1" "$platform_opts"; then
            if [ "$option" == "$1" ]; then
                continue
            fi
        fi

        if [ "$option" == "$1" ]; then
            sed -i "s/^#define $option[[:space:]]*[01]*/#define $option 1/" "$platform_opts"
        else
            if grep -q "^#define $option[[:space:]]*1" "$platform_opts"; then
                sed -i "s/^#define $option[[:space:]]*[01]*/#define $option 0/" "$platform_opts"
            fi
        fi
    done
}

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

cp ${tooldir}/docker/matter_version_selection.sh ${tooldir}/scripts/matter_version_selection.sh

chmod u+x matter_setup.sh

# AmebaZ2 build flow
./matter_setup.sh amebaz2 || handle_error "Failed to run matter_setup.sh for amebaz2"

cd ${amebadir}/project/realtek_amebaz2_v0_example/GCC-RELEASE/

# All-clusters-app
echo "Building all_clusters"
select_opts CONFIG_EXAMPLE_MATTER_CHIPTEST
make all_clusters || handle_error "Failed to build all_clusters"

echo "Building firmware image"
make is_matter || handle_error "Failed to build firmware image for all_clusters"

echo "Build all-clusters-app completed"
make clean_matter_libs || handle_error "Failed to clean matter libraries"
make clean_matter || handle_error "Failed to clean matter"

# Lighting-app
echo "Building porting layer lighting-app"
select_opts CONFIG_EXAMPLE_MATTER_LIGHT
make light_port || handle_error "Failed to build light_port"

echo "Building firmware image"
make is_matter || handle_error "Failed to build firmware image for light_port"

echo "Build lighting completed"
make clean_matter_libs || handle_error "Failed to clean matter libraries"
make clean_matter || handle_error "Failed to clean matter"

cd ${amebadir}/

# AmebaZ2plus build flow
./matter_setup.sh amebaz2plus || handle_error "Failed to run matter_setup.sh for amebaz2plus"

cd ${amebadir}/project/realtek_amebaz2plus_v0_example/GCC-RELEASE/

# All-clusters-app
echo "Building all_clusters"
select_opts CONFIG_EXAMPLE_MATTER_CHIPTEST
make all_clusters || handle_error "Failed to build all_clusters"

echo "Building firmware image"
make is_matter || handle_error "Failed to build firmware image for all_clusters"

echo "Build all-clusters-app completed"
make clean_matter_libs || handle_error "Failed to clean matter libraries"
make clean_matter || handle_error "Failed to clean matter"

# Lighting-app
echo "Building porting layer lighting-app"
select_opts CONFIG_EXAMPLE_MATTER_LIGHT
make light_port || handle_error "Failed to build light_port"

echo "Building firmware image"
make is_matter || handle_error "Failed to build firmware image for light_port"

echo "Build lighting completed"
make clean_matter_libs || handle_error "Failed to clean matter libraries"
make clean_matter || handle_error "Failed to clean matter"
