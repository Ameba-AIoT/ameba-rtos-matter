# Use the latest Ubuntu LTS version for stability
FROM ubuntu:24.04

# Install common dependencies
# Add deadsnakes PPA to install Python 3.10.16 to remove wheel error during bootstrap
# Clean up to reduce image size
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    software-properties-common && \
    add-apt-repository ppa:deadsnakes/ppa && \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    git \
    gcc \
    g++ \
    python3.10 \
    python3.10-dev \
    python3.10-venv \
    python3.10-distutils \
    python3-pip \
    wget \
    bzip2 \
    libgirepository1.0-dev \
    libcairo2-dev \
    libreadline-dev \
    libssl-dev \
    && apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Set python3.10 as the default Python version
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.10 1

# Link `sh` to `bash`
RUN ln -sf /bin/bash /bin/sh

# Define build arguments for repositories
ARG CHIP_REPO=https://github.com/project-chip/connectedhomeip.git
ARG CHIP_REF=master
ARG CHIP_SHA=70d9a61475d31686f0fde8e7b56f352a0f59b299
ARG AMEBA_DIR=/opt/ameba

# Create the necessary directory, clone the connectedhomeip repo
RUN mkdir -p ${AMEBA_DIR} \
    && cd ${AMEBA_DIR} \
    && git clone --depth 1 --branch ${CHIP_REF} --single-branch ${CHIP_REPO} connectedhomeip \
    && cd connectedhomeip \
    && git fetch --depth=1 origin ${CHIP_SHA} \
    && git checkout ${CHIP_SHA} \
    && git submodule update --init --recursive --depth 1

# Set the working directory for the rest of the Dockerfile to work
WORKDIR ${AMEBA_DIR}

# Set default command to bash for interaction
CMD ["/bin/bash"]
