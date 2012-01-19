# Modify global directives in global namespace
OBJDIRS := $(OBJDIRS) $(LOWERC_ROOT)/Playback
TESTS   := $(TESTS)   $(LOWERC_ROOT)PlaybackTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
$(UPPERC_ROOT)_OBSERVER_SRCS := $(LOWERC_ROOT)/Playback/PlaybackSensor.cc
$(UPPERC_ROOT)_OBSERVER_EXECUTABLES := 

# Set makefile template specific vars
UPPERC_DIR := $(UPPERC_ROOT)_PLAYBACK
LOWERC_DIR := $(LOWERC_ROOT)/Playback

EXECUTABLE_OBJS :=
TEST_OBJS       :=

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
$(LOWERC_ROOT)PlaybackTest: $($(UPPERC_ROOT)_PLAYBACK_TESTS)
	@for a in $(OBSERVATION_PLAYBACK_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
