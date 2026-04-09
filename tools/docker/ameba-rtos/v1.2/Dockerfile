# Inherit from the common base image
FROM ameba-rtos-matter:common

# Define toolchain directory
ARG TOOLCHAIN_DIR='/opt/rtk-toolchain/'

# Define toolchain ASDK arguments
ARG TOOLCHAIN_ASDK_LINUX_URL='https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/prebuilts-v1.0.3/prebuilts-linux-1.0.3.tar.gz'
ARG TOOLCHAIN_ASDK_LINUX_URL_ALIYUN='https://rs-wn.oss-cn-shanghai.aliyuncs.com/prebuilts-linux-1.0.3.tar.gz'
ARG TOOLCHAIN_ASDK_FILE_PATH='/opt/rtk-toolchain/prebuilts-linux-1.0.3.tar.gz'

# Redefine following build arguments to respective repo and tag/branch
ARG AMEBA_MATTER_REPO=https://github.com/mikaelajiwidodo/ameba-rtos-matter.git
ARG AMEBA_MATTER_TAG_NAME=ameba-rtos/v1.5-PR/green2_support

# Define fixed build arguments
ARG AMBRTOS_REPO=https://github.com/Ameba-AIoT/ameba-rtos.git
ARG AMBRTOS_TAG_NAME=release/v1.2
ARG AMEBA_MATTER_DIR=/opt/ameba/ameba-rtos-matter
ARG AMEBA_DIR=/opt/ameba

# Clone the repo
RUN git clone --branch ${AMBRTOS_TAG_NAME} --single-branch ${AMBRTOS_REPO} ameba-rtos \
    && git clone --branch ${AMEBA_MATTER_TAG_NAME} --single-branch ${AMEBA_MATTER_REPO} ${AMEBA_MATTER_DIR}

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
    && tar xzvf ${TOOLCHAIN_ASDK_FILE_PATH} -C ${TOOLCHAIN_DIR}

# Setup Ameba Amazon FreeRTOS environment
RUN cd ${AMEBA_MATTER_DIR} \
    && chmod u+x matter_setup.sh \
    && ./matter_setup.sh ../ameba-rtos \
    && source ${AMEBA_MATTER_DIR}/matter_env.sh

# Set default working directory
WORKDIR ${AMEBA_MATTER_DIR}

# Set default command to bash for interaction
CMD ["/bin/bash"]
