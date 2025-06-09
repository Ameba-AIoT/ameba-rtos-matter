#!/bin/bash

set -e

run_docker_build_scripts() {
  # Define the common path for the tools directory
  local tools_dir="component/common/application/matter/tools/docker"

  # Check if the container exists
  if docker ps -a --format '{{.Names}}' | grep -w "ameba-container" > /dev/null; then
    # Stop and remove the container if it exists
    docker stop ameba-container
    docker rm ameba-container
  fi

  # Start the container
  docker run -d --name ameba-container ameba-rtos-matter:amebad tail -f /dev/null

  # Execute the setup and build script inside the container
  docker exec ameba-container bash -c "
    chmod u+x ${tools_dir}/amebad/amebad_build.sh &&
    ${tools_dir}/amebad/amebad_build.sh /opt/ameba
  "

  # Stop and remove the container after execution
  docker stop ameba-container
  docker rm ameba-container
}

# Run the function
run_docker_build_scripts
