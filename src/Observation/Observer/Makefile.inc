# Modify global directives in global namespace
OBJDIRS := $(OBJDIRS) $(LOWERC_ROOT)/Observer
TESTS   := $(TESTS)   $(LOWERC_ROOT)ObserverTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
$(UPPERC_ROOT)_OBSERVER_SRCS := $(LOWERC_ROOT)/Observer/Dummy.cc \
				$(LOWERC_ROOT)/Observer/RealtimeObserver.cc
$(UPPERC_ROOT)_OBSERVER_EXECUTABLES := 

# Set makefile template specific vars
UPPERC_DIR := $(UPPERC_ROOT)_OBSERVER
LOWERC_DIR := $(LOWERC_ROOT)/Observer

EXECUTABLE_OBJS :=
TEST_OBJS       := $(PRIMITIVES_QLEARNER_OBJS)

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
$(LOWERC_ROOT)ObserverTest: $($(UPPERC_ROOT)_OBSERVER_TESTS)
	@for a in $(OBSERVATION_OBSERVER_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
