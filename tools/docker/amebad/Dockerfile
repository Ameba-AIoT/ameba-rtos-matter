# Inherit from the common base image
FROM ameba-rtos-matter:common

# Redefine following build arguments to respective repo and tag/branch
ARG AMEBA_MATTER_REPO=https://github.com/xshuqun/ameba-rtos-matter.git
ARG TAG_NAME=main-PR/update_readme

# Define fixed build arguments
ARG AMBD_REPO=https://github.com/Ameba-AIoT/ameba-rtos-d.git
ARG AMBD_TAG_NAME=main
ARG AMEBA_MATTER_DIR=component/common/application/matter
ARG AMEBA_DIR=/opt/ameba

# Clone the repo
RUN git clone --branch ${AMBD_TAG_NAME} --single-branch ${AMBD_REPO} ameba-rtos-d \
    && cd ameba-rtos-d \
    && git clone --branch ${TAG_NAME} --single-branch ${AMEBA_MATTER_REPO} ${AMEBA_MATTER_DIR} \
    && cat ${AMEBA_MATTER_DIR}/tools/toolchain/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2.part* > ${AMEBA_MATTER_DIR}/tools/toolchain/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2 \
    && mkdir -p project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/toolchain/linux \
    && tar -jxf ${AMEBA_MATTER_DIR}/tools/toolchain/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2 -C project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/toolchain/linux

# Set default working directory
WORKDIR ${AMEBA_DIR}/ameba-rtos-d

# Set default command to bash for interaction
CMD ["/bin/bash"]
