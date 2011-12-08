# Modify global directives in global namespace
OBJDIRS += $(LOWERC_ROOT)/Student
TESTS += PrimitivesStudentTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
$(UPPERC_ROOT)_STUDENT_SRCS := $(LOWERC_ROOT)/Student/Dummy.cc

# Set makefile template specific vars
UPPERC_DIR := $(UPPERC_ROOT)_STUDENT
LOWERC_DIR := $(LOWERC_ROOT)/Student

SRC_LINKED_OBJECTS := 

EXECUTABLE :=
EXECUTABLE_OBJS :=

EXTRA_TEST_OBJS :=

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
PrimitivesStudentTest: $($(UPPERC_ROOT)_STUDENT_TESTS)
	@for a in $($(UPPERC_ROOT)_STUDENT_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
