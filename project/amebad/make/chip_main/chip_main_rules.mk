all: CORE_TARGETS

# -------------------------------------------------------------------
# Generate obj list
# -------------------------------------------------------------------
CPPOBJS = $(CPPSRC:.cpp=.o)
COBJS = $(notdir $(CSRC:.c=.o))
OBJS = $(CPPOBJS) $(COBJS)
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
clean:
	@find $(shell pwd) -mindepth 1 -type d -exec rm -r {} +
	@rm -f $(patsubst %.cpp,%.d,$(CPPSRC:.cpp=.d))
	@rm -f $(patsubst %.cpp,%.su,$(CPPSRC:.cpp=.su))
	@rm -f $(patsubst %.cpp,%.o,$(CPPSRC:.cpp=.o))
	@rm -f *.d *.su *.o

# -------------------------------------------------------------------
# Dependency includes
# -------------------------------------------------------------------
-include $(DEPS)