# Modify global directives in global namespace
OBJDIRS := $(OBJDIRS) $(LOWERC_ROOT)/Exploration
TESTS   := $(TESTS)   $(LOWERC_ROOT)ExplorationTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/ *.cC
$(UPPERC_ROOT)_EXPLORATION_SRCS := $(LOWERC_ROOT)/Exploration/GreedyExplorer.cc
$(UPPERC_ROOT)_EXPLORATION_EXECUTABLES :=

# Set makefile template specific vars
UPPERC_DIR := $(UPPERC_ROOT)_EXPLORATION
LOWERC_DIR := $(LOWERC_ROOT)/Exploration

EXECUTABLE_OBJS := $($(UPPERC_ROOT)_QLEARNER_OBJS) $(PROTO_OBJS)
TEST_OBJS	      := $($(UPPERC_ROOT)_QLEARNER_OBJS) $(PROTO_OBJS)

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
$(LOWERC_ROOT)ExplorationTest: $($(UPPERC_DIR)_TESTS)
	@for a in $(PRIMITIVES_EXPLORATION_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
