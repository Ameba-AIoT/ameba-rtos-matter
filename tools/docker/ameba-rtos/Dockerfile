# Inherit from the common base image
FROM ameba-rtos-matter:common

# Redefine following build arguments to respective repo and tag/branch
ARG AMEBA_MATTER_REPO=https://github.com/mikaelajiwidodo/ameba-rtos-matter.git
ARG TAG_NAME=ameba-rtos/v1.4/shift_factory_data

# Define fixed build arguments
ARG AMBRTOS_REPO=https://github.com/Ameba-AIoT/ameba-rtos.git
ARG AMBRTOS_TAG_NAME=release/v1.0+matter
ARG AMEBA_MATTER_DIR=component/application/matter
ARG AMEBA_DIR=/opt/ameba

# Clone the repo and download the toolchain
RUN git clone --branch ${AMBRTOS_TAG_NAME} --single-branch ${AMBRTOS_REPO} ameba-rtos \
    && cd ameba-rtos \
    && git clone --branch ${TAG_NAME} --single-branch ${AMEBA_MATTER_REPO} ${AMEBA_MATTER_DIR} \
    && cd amebadplus_gcc_project \
    && make toolchain

# Set default working directory
WORKDIR ${AMEBA_DIR}/ameba-rtos

# Set default command to bash for interaction
CMD ["/bin/bash"]
