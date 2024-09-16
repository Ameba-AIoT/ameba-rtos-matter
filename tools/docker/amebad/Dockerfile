# Inherit from the common base image
FROM ameba-rtos-matter:common

# Redefine following build arguments to respective repo and tag/branch
ARG AMEBA_MATTER_REPO=https://github.com/xshuqun/ameba-rtos-matter.git
ARG TAG_NAME=ameba/update_241022

# Define fixed build arguments
ARG AMBD_REPO=https://github.com/ambiot/ambd_matter.git
ARG AMEBA_MATTER_DIR=component/common/application/matter
ARG AMEBA_DIR=/opt/ameba

# Clone the repo
RUN git clone --branch ${TAG_NAME} --single-branch ${AMBD_REPO} ambd_matter \
    && cd ambd_matter \
    && git clone --branch ${TAG_NAME} --single-branch ${AMEBA_MATTER_REPO} ${AMEBA_MATTER_DIR} \
    && cat ${AMEBA_MATTER_DIR}/tools/toolchain/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2.part* > ${AMEBA_MATTER_DIR}/tools/toolchain/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2 \
    && mkdir -p project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/toolchain/linux \
    && tar -jxf ${AMEBA_MATTER_DIR}/tools/toolchain/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2 -C project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/toolchain/linux

# Set default working directory
WORKDIR ${AMEBA_DIR}/ambd_matter

# Set default command to bash for interaction
CMD ["/bin/bash"]

