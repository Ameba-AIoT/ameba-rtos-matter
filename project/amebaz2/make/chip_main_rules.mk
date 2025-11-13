# -------------------------------------------------------------------
# Initialize target name and target object files
# -------------------------------------------------------------------
TARGET=lib_main
OBJ_DIR=$(TARGET)/Debug/obj
BIN_DIR=$(TARGET)/Debug/bin
INFO_DIR=$(TARGET)/Debug/info

# -------------------------------------------------------------------
# Generate obj list
# -------------------------------------------------------------------
SRC_O = $(patsubst %.c,%_$(TARGET).o,$(SRC_C))
SRC_OO += $(patsubst %.cpp,%_$(TARGET).oo,$(SRC_CPP))
VER_O = $(patsubst %.c,%_$(TARGET).o,$(VER_C))

SRC_C_LIST = $(notdir $(SRC_C))
OBJ_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%_$(TARGET).o,$(SRC_C_LIST)))
DEPENDENCY_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%_$(TARGET).d,$(SRC_C_LIST)))

SRC_CPP_LIST = $(notdir $(SRC_CPP))
OBJ_CPP_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%_$(TARGET).oo,$(SRC_CPP_LIST)))
DEPENDENCY_LIST += $(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%_$(TARGET).d,$(SRC_CPP_LIST)))

# -------------------------------------------------------------------
# Build Library Target
# -------------------------------------------------------------------
.PHONY: lib_main
lib_main: prerequirement $(SRC_O) $(SRC_OO)
	$(AR) crv $(BIN_DIR)/$(TARGET).a $(OBJ_CPP_LIST) $(OBJ_LIST) $(VER_O)
	cp $(BIN_DIR)/$(TARGET).a $(SDKROOTDIR)/component/soc/realtek/8710c/misc/bsp/lib/common/GCC/$(TARGET).a

# -------------------------------------------------------------------
# Prerequirement Rule
# -------------------------------------------------------------------
.PHONY: prerequirement
prerequirement:
	@rm -f $(TARGET)_version*.o
	@echo const char $(TARGET)_rev[] = \"$(TARGET)_ver_`git rev-parse HEAD`_`date +%Y/%m/%d-%T`\"\; > $(TARGET)_version.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $(VER_C) -o $(VER_O)
	@if [ ! -d $(ARM_GCC_TOOLCHAIN) ]; then \
		echo ===========================================================; \
		echo Toolchain not found, \"make toolchain\" first!; \
		echo ===========================================================; \
		exit -1; \
	fi
	@echo ===========================================================
	@echo Build $(TARGET)
	@echo ===========================================================
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(INFO_DIR)

# -------------------------------------------------------------------
# Compilation Rules
# -------------------------------------------------------------------
$(SRC_OO): %_$(TARGET).oo : %.cpp | prerequirement
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(OBJ_DIR)/$(notdir $(patsubst %.oo,%.d,$@))
	@cp $@ $(OBJ_DIR)/$(notdir $@)
ifneq ($(findstring _dm,$(DEVICE_TYPE)),_dm)
	@cp $*_$(TARGET).s $(INFO_DIR)
	@cp $*_$(TARGET).ii $(INFO_DIR)
endif
	@chmod 777 $(OBJ_DIR)/$(notdir $@)

$(SRC_O): %_$(TARGET).o : %.c | prerequirement
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(OBJ_DIR)/$(notdir $(patsubst %.o,%.d,$@))
	@cp $@ $(OBJ_DIR)/$(notdir $@)
ifneq ($(findstring _dm,$(DEVICE_TYPE)),_dm)
	@cp $*_$(TARGET).s $(INFO_DIR)
	@cp $*_$(TARGET).i $(INFO_DIR)
endif
	@chmod 777 $(OBJ_DIR)/$(notdir $@)

# -------------------------------------------------------------------
# Dependency includes
# -------------------------------------------------------------------
-include $(DEPENDENCY_LIST)

# -------------------------------------------------------------------
# Clean Rule
# -------------------------------------------------------------------
.PHONY: clean
clean:
	@echo "Cleaning up $(DEVICE_TYPE) main..."
	@rm -rf $(TARGET)
	@rm -f $(SRC_O) $(DRAM_O) $(VER_O) $(SRC_OO)
	@rm -f $(patsubst %.o,%.d,$(SRC_O)) $(patsubst %.o,%.d,$(VER_O)) $(patsubst %.oo,%.d,$(SRC_OO))
	@rm -f $(patsubst %.o,%.su,$(SRC_O)) $(patsubst %.o,%.su,$(VER_O)) $(patsubst %.oo,%.su,$(SRC_OO))
	@rm -f $(patsubst %.o,%.i,$(SRC_O)) $(patsubst %.o,%.i,$(VER_O)) $(patsubst %.oo,%.ii,$(SRC_OO))
	@rm -f $(patsubst %.o,%.s,$(SRC_O))$(patsubst %.o,%.s,$(VER_O)) $(patsubst %.oo,%.s,$(SRC_OO))
	@rm -f *.i
	@rm -f *.s
	@rm -f $(VER_C)
