# Modify global directives in global namespace
OBJDIRS += $(LOWERC_ROOT)/Learner
TESTS += PrimitivesLearnerTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
$(UPPERC_ROOT)_LEARNER_SRCS := $(LOWERC_ROOT)/Learner/Tamer.cc

# Set makefile template specific vars
UPPERC_DIR = $(UPPERC_ROOT)_LEARNER
LOWERC_DIR = $(LOWERC_ROOT)/Learner
EXECUTABLE =
EXECUTABLE_OBJS =
EXTRA_TEST_OBJS =

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
PrimitivesLearnerTest: $($(UPPERC_ROOT)_LEARNER_TESTS)
	@for a in $($(UPPERC_ROOT)_LEARNER_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
