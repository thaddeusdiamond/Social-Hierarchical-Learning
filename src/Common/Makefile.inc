# Modify global directives in global namespace
OBJDIRS += Common
TESTS += CommonTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
COMMON_SRCS :=
COMMON_EXECUTABLES :=

# Set makefile template specific vars
UPPERC_DIR := COMMON
LOWERC_DIR := Common

EXECUTABLE_OBJS :=
TEST_OBJS := $(PROTO_OBJS)

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
CommonTest: $(COMMON_TESTS)
	@echo + Ensuring Database Storage Layer Exists...
	@mkdir -p $(STORAGE)
	@for a in $(COMMON_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
