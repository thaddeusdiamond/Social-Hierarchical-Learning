# Modify global directives in global namespace
OBJDIRS := $(OBJDIRS) $(LOWERC_ROOT)/Recognizer
TESTS   := $(TESTS)   $(LOWERC_ROOT)RecognizerTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
$(UPPERC_ROOT)_RECOGNIZER_SRCS := 
$(UPPERC_ROOT)_RECOGNIZER_EXECUTABLES := $(LOWERC_ROOT)/Recognizer/aaai12.cc \
					 $(LOWERC_ROOT)/Recognizer/iros12.cc


# Set makefile template specific vars
UPPERC_DIR := $(UPPERC_ROOT)_RECOGNIZER
LOWERC_DIR := $(LOWERC_ROOT)/Recognizer

EXECUTABLE_OBJS := $(PRIMITIVES_QLEARNER_OBJS) $(PRIMITIVES_STUDENT_OBJS) $(OBSERVATION_OBSERVER_OBJS) $(OBSERVATION_PLAYBACK_OBJS) $(PROTO_OBJS)
TEST_OBJS       := $(PRIMITIVES_QLEARNER_OBJS)

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
$(LOWERC_ROOT)RecognizerTest: $($(UPPERC_ROOT)_RECOGNIZER_TESTS)
	@for a in $(OBSERVATION_RECOGNIZER_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
