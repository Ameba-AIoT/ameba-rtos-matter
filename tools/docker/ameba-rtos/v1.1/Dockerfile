# Inherit from the common base image
FROM ameba-rtos-matter:common

# Define toolchain directory
ARG TOOLCHAIN_DIR='/opt/rtk-toolchain/'

# Define toolchain ASDK arguments
ARG TOOLCHAIN_ASDK_LINUX_URL='https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/10.3.1_v5/asdk-10.3.1-linux-newlib-build-4354-x86_64_with_small_reent.tar.bz2'
ARG TOOLCHAIN_ASDK_LINUX_URL_ALIYUN='https://rs-wn.oss-cn-shanghai.aliyuncs.com/asdk-10.3.1-linux-newlib-build-4354-x86_64_with_small_reent.tar.bz2'
ARG TOOLCHAIN_ASDK_FILE_PATH='/opt/rtk-toolchain/asdk-10.3.1-linux-newlib-build-4354-x86_64_with_small_reent.tar.bz2'
ARG TOOLCHAIN_ASDK_OLD_DIR='/opt/rtk-toolchain/asdk-10.3.1'
ARG TOOLCHAIN_ASDK_NEW_DIR='/opt/rtk-toolchain/asdk-10.3.1-4354'

# Redefine following build arguments to respective repo and tag/branch
ARG AMEBA_MATTER_REPO=https://github.com/mikaelajiwidodo/ameba-rtos-matter.git
ARG TAG_NAME=ameba-rtos/v1.5-PR/rtos_v1.0_support

# Define fixed build arguments
ARG AMBRTOS_REPO=https://github.com/mikaelajiwidodo/ameba-rtos.git
ARG AMBRTOS_TAG_NAME=release/v1.1+matter_v1.5
ARG AMEBA_MATTER_DIR=component/application/matter
ARG AMEBA_DIR=/opt/ameba

# Clone the repo
RUN git clone --branch ${AMBRTOS_TAG_NAME} --single-branch ${AMBRTOS_REPO} ameba-rtos \
    && cd ameba-rtos \
    && git clone --branch ${TAG_NAME} --single-branch ${AMEBA_MATTER_REPO} ${AMEBA_MATTER_DIR}

# Setup connectedhomeip environment
RUN cd connectedhomeip \
    && source scripts/bootstrap.sh \
    && source scripts/activate.sh \
    && cd ../ameba-rtos \
    && pip install -r tools/requirements.txt

# Download the toolchain ASDK
RUN mkdir -p ${TOOLCHAIN_DIR} \
    && ( \
        wget -q --show-progress -P ${TOOLCHAIN_DIR} ${TOOLCHAIN_ASDK_LINUX_URL} \
        || { \
            echo "Primary download failed, attempting mirror..." \
            && wget -q --show-progress -P ${TOOLCHAIN_DIR} ${TOOLCHAIN_ASDK_LINUX_URL_ALIYUN} \
            || ( echo "All download attempts failed"; exit 1 ) \
        } \
    ) \
    && tar -jxf ${TOOLCHAIN_ASDK_FILE_PATH} -C ${TOOLCHAIN_DIR} \
    && mv ${TOOLCHAIN_ASDK_OLD_DIR} ${TOOLCHAIN_ASDK_NEW_DIR}

# Set default working directory
WORKDIR ${AMEBA_DIR}/ameba-rtos

# Set default command to bash for interaction
CMD ["/bin/bash"]
