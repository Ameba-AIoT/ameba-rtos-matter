all: CORE_TARGETS

# -------------------------------------------------------------------
# Generate obj list
# -------------------------------------------------------------------
CPPOBJS = $(CPPSRC:.cpp=.o)
COBJS = $(notdir $(CSRC:.c=.o))
OBJS = $(CPPOBJS)
OBJS += $(COBJS)
STATIC_LIB = lib_main.a

# -------------------------------------------------------------------
# Object files include
# -------------------------------------------------------------------
-include $(OBJS:.o=.d)

# -------------------------------------------------------------------
# Build Library Target
# -------------------------------------------------------------------
CORE_TARGETS: $(CPPOBJS) $(COBJS) $(STATIC_LIB)

$(CPPOBJS): %.o: %.cpp
	$(CC) -ffunction-sections $(CPPFLAGS) $(MODULE_IFLAGS) -c $< -o $@
	@foldername=$$(basename $$(dirname $<)); \
	mkdir -p $${foldername}; \
	cp $@ $${foldername}/$(notdir $@)

$(STATIC_LIB):$(OBJS)
	$(AR) rvs $@ *.o */*.o
	$(MOVE) -f $@ $(ROOTDIR)/lib/application

# -------------------------------------------------------------------
# Clean Rule
# -------------------------------------------------------------------
clean: CLEAN_OBJS
	@echo "Cleaning up $(DEVICE_TYPE) main..."
	$(call CLEAN_FUNC)
	@find . -name '*.o' -delete
	@find . -mindepth 1 -type d -empty -delete

# -------------------------------------------------------------------
# Dependency includes
# -------------------------------------------------------------------
-include $(DEPS)
