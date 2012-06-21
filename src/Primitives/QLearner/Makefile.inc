# Modify global directives in global namespace
OBJDIRS := $(OBJDIRS) $(LOWERC_ROOT)/QLearner
TESTS   := $(TESTS)   $(LOWERC_ROOT)QLearnerTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/ *.cc
$(UPPERC_ROOT)_QLEARNER_SRCS := $(LOWERC_ROOT)/QLearner/State.cc \
                                $(LOWERC_ROOT)/QLearner/QTable.cc \
                                $(LOWERC_ROOT)/QLearner/Action.cc \
                                $(LOWERC_ROOT)/QLearner/Condition.cc \
                                $(LOWERC_ROOT)/QLearner/StandardQLearner.cc
$(UPPERC_ROOT)_QLEARNER_EXECUTABLES :=

# Set makefile template specific vars
UPPERC_DIR := $(UPPERC_ROOT)_QLEARNER
LOWERC_DIR := $(LOWERC_ROOT)/QLearner

EXECUTABLE_OBJS :=
TEST_OBJS       := $($(UPPERC_ROOT)_EXPLORATION_OBJS)

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
$(LOWERC_ROOT)QLearnerTest: $($(UPPERC_ROOT)_QLEARNER_TESTS)
	@for a in $(PRIMITIVES_QLEARNER_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
