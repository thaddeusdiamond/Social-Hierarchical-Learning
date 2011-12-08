# Modify global directives in global namespace
OBJDIRS += $(LOWERC_ROOT)/QLearner
TESTS += PrimitivesQLearnerTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
$(UPPERC_ROOT)_QLEARNER_SRCS := $(LOWERC_ROOT)/QLearner/StandardQLearner.cc \
                                $(LOWERC_ROOT)/QLearner/QTable.cc
$(UPPERC_ROOT)_QLEARNER_EXECUTABLES :=

# Set makefile template specific vars
UPPERC_DIR := $(UPPERC_ROOT)_QLEARNER
LOWERC_DIR := $(LOWERC_ROOT)/QLearner

EXECUTABLE_OBJS :=
TEST_OBJS :=

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
PrimitivesQLearnerTest: $($(UPPERC_ROOT)_QLEARNER_TESTS)
	@for a in $(PRIMITIVES_QLEARNER_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
