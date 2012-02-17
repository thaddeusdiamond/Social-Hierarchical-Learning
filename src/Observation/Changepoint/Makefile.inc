# Modify global directives in global namespace
OBJDIRS := $(OBJDIRS) $(LOWERC_ROOT)/Changepoint
TESTS   := $(TESTS)
# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
$(UPPERC_ROOT)_CHANGEPOINT_SRCS := $(LOWERC_ROOT)/Changepoint/VarianceDetector.cc
$(UPPERC_ROOT)_CHANGEPOINT_EXECUTABLES := 

# Set makefile template specific vars
UPPERC_DIR := $(UPPERC_ROOT)_CHANGEPOINT
LOWERC_DIR := $(LOWERC_ROOT)/Changepoint

EXECUTABLE_OBJS :=
TEST_OBJS       := $(PRIMITIVES_QLEARNER_OBJS)

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
$(LOWERC_ROOT)ChangepointTest: $($(UPPERC_ROOT)_CHANGEPOINT_TESTS)
	@for a in $(OBSERVATION_CHANGEPOINT_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
