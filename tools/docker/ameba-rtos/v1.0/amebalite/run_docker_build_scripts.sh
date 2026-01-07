#!/bin/bash

run_docker_build_scripts() {
  # Define the common path for the tools directory
  local tools_dir="component/application/matter/tools/docker"

  # Check if the container exists
  if docker ps -a --format '{{.Names}}' | grep -w "ameba-container" > /dev/null; then
    # Stop and remove the container if it exists
    docker stop ameba-container
    docker rm ameba-container
  fi

  # Start the container
  docker run -d --name ameba-container ameba-rtos-matter:ameba-rtos-v1.0 tail -f /dev/null

  # Execute the setup and build script inside the container
  if ! docker exec ameba-container bash -c "
    chmod u+x ${tools_dir}/ameba-rtos/v1.0/amebalite/amebalite_build.sh  &&
    ${tools_dir}/ameba-rtos/v1.0/amebalite/amebalite_build.sh /opt/ameba
  "; then
    echo "Build script failed. Exiting."
    docker stop ameba-container
    docker rm ameba-container
    exit 1
  fi

  # Stop and remove the container after execution
  docker stop ameba-container
  docker rm ameba-container
}

# Run the function
run_docker_build_scripts