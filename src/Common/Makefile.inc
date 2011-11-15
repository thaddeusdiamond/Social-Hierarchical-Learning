# Modify global directives in global namespace
OBJDIRS += Common
TESTS += CommonTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
COMMON_SRCS :=

# Set makefile template specific vars
UPPERC_DIR = COMMON
LOWERC_DIR = Common
EXECUTABLE =
EXECUTABLE_OBJS =
EXTRA_TEST_OBJS =

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
CommonTest: $(COMMON_TESTS)
	@for a in $(COMMON_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done
