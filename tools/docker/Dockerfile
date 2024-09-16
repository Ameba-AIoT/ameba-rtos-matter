# Use the latest Ubuntu LTS version for stability
FROM ubuntu:23.04

# Install dependencies and clean up to reduce image size
RUN set -x \
    && apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -fy --no-install-recommends \
    git \
    gcc \
    g++ \
    python3-venv \
    python3-dev \
    python3-pip \
    bzip2 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && : # last line

# Define build arguments with default values
ARG CHIP_REPO=https://github.com/project-chip/connectedhomeip.git
ARG CHIP_REF=master
ARG AMEBA_DIR=/opt/ameba
ARG AMBZ2_REPO=https://github.com/ambiot/ambz2_matter.git
ARG TAG_NAME=v1.4_TE2_240812
ARG AMEBA_MATTER_DIR=component/common/application/matter

# Set environment variables
ENV AMEBA_PATH=${AMEBA_DIR}/ambz2_matter

# Create the necessary directory, clone repositories, and run environment setup script
RUN mkdir -p ${AMEBA_DIR} \
    && cd ${AMEBA_DIR} \
    && git clone --depth 1 --branch ${CHIP_REF} --single-branch ${CHIP_REPO} connectedhomeip \
    && cd connectedhomeip \
    && git submodule update --init --recursive --depth 1 \
    && cd .. \
    && git clone --branch ${TAG_NAME} --single-branch ${AMBZ2_REPO} ambz2_matter \
    && cd ambz2_matter \
    && git submodule update --init --recursive --depth 1 \
    && cat ${AMEBA_MATTER_DIR}/tools/toolchain/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2.part* > ${AMEBA_MATTER_DIR}/tools/toolchain/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2 \
    && mkdir -p tools/arm-none-eabi-gcc/asdk-10.3.1 && tar -jxf ${AMEBA_MATTER_DIR}/tools/toolchain/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2 -C tools/arm-none-eabi-gcc/ \
    && rm -rf ${AMEBA_MATTER_DIR}/tools/toolchain/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2.part* \
    && : # last line

# Set the working directory
WORKDIR ${AMEBA_DIR}

# Set default command to bash so you can interact with the container
CMD ["/bin/bash"]
