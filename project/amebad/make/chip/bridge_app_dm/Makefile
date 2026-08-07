include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE := bridge_dm
OUTPUT_DIR  := $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/build/chip

# -------------------------------------------------------------------
# Compilation flag
# -------------------------------------------------------------------
GLOBAL_CFLAGS += -DCHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT=20

# -------------------------------------------------------------------
# Build Declaration and Source
# -------------------------------------------------------------------
include $(MATTER_CORE_SRC)

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_CORE_RULES)