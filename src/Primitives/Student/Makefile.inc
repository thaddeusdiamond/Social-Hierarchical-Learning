# Modify global directives in global namespace
OBJDIRS += $(LOWERC_ROOT)/Student
TESTS += PrimitivesStudentTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
$(UPPERC_ROOT)_STUDENT_SRCS := $(LOWERC_ROOT)/Student/Dummy.cc \
                               $(LOWERC_ROOT)/Student/FeedbackSensor.cc
$(UPPERC_ROOT)_STUDENT_EXECUTABLES := 

# Set makefile template specific vars
UPPERC_DIR := $(UPPERC_ROOT)_STUDENT
LOWERC_DIR := $(LOWERC_ROOT)/Student

EXECUTABLE_OBJS :=
TEST_OBJS :=

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
PrimitivesStudentTest: $($(UPPERC_DIR)_TESTS)
	@for a in $(PRIMITIVES_STUDENT_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
