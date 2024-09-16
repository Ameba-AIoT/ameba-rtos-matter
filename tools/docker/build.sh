#!/bin/bash

set -e

echo "Changing directory to build location"
cd /opt/ameba/ambz2_matter/project/realtek_amebaz2_v0_example/GCC-RELEASE/

echo "Building matter libraries"
make all_clusters

echo "Building firmware image"
make is_matter

echo "Build all-clusters-app completed"
make clean_matter

