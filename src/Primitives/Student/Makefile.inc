# Modify global directives in global namespace
OBJDIRS := $(OBJDIRS) $(LOWERC_ROOT)/Student
TESTS   := $(TESTS)   $(LOWERC_ROOT)StudentTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/ *.cc
$(UPPERC_ROOT)_STUDENT_SRCS := $(LOWERC_ROOT)/Student/Dummy.cc \
                               $(LOWERC_ROOT)/Student/LBDStudent.cc \
                               $(LOWERC_ROOT)/Student/FeedbackSensor.cc \
                               $(LOWERC_ROOT)/Student/KinectSensor.cc \
                               $(LOWERC_ROOT)/Student/CreateSensor.cc
$(UPPERC_ROOT)_STUDENT_EXECUTABLES := 

# Set makefile template specific vars
UPPERC_DIR := $(UPPERC_ROOT)_STUDENT
LOWERC_DIR := $(LOWERC_ROOT)/Student


EXECUTABLE_OBJS :=
TEST_OBJS	:= $($(UPPERC_ROOT)_QLEARNER_OBJS) $(PROTO_OBJS)

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
$(LOWERC_ROOT)StudentTest: $($(UPPERC_DIR)_TESTS)
	@for a in $(PRIMITIVES_STUDENT_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
