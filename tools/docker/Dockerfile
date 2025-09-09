# Use the latest Ubuntu LTS version for stability
FROM ubuntu:24.04

# Install common dependencies and clean up to reduce image size
RUN set -x \
    && apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -fy --no-install-recommends \
    git \
    gcc \
    g++ \
    python3-venv \
    python3-dev \
    python3-pip \
    wget \
    bzip2 \
    cmake \
    curl \
    libgirepository1.0-dev \
    libcairo2-dev \
    libreadline-dev \
    libssl-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set python3 as the default python version, and then check the version
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1 \
    && python --version \
    && python3 --version

# Link `sh` to `bash`
RUN ln -sf /bin/bash /bin/sh

# Define build arguments for repositories
ARG CHIP_REPO=https://github.com/project-chip/connectedhomeip.git
ARG CHIP_REF=v1.5-branch
ARG AMEBA_DIR=/opt/ameba

# Create the necessary directory, clone the connectedhomeip repo
RUN mkdir -p ${AMEBA_DIR} \
    && cd ${AMEBA_DIR} \
    && git clone --depth 1 --branch ${CHIP_REF} --single-branch ${CHIP_REPO} connectedhomeip \
    && cd connectedhomeip \
    && git submodule update --init --recursive --depth 1

# Set the working directory for the rest of the Dockerfile to work
WORKDIR ${AMEBA_DIR}

# Set default command to bash for interaction
CMD ["/bin/bash"]

