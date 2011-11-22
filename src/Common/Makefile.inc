# Modify global directives in global namespace
OBJDIRS += Common
TESTS += CommonTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
COMMON_SRCS := Common/*.proto

# Set makefile template specific vars
UPPERC_DIR := COMMON
LOWERC_DIR := Common

SRC_LINKED_OBJECTS := $(OBJDIR)/Common/%.cc

EXECUTABLE :=
EXECUTABLE_OBJS :=

EXTRA_TEST_OBJS :=

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
CommonTest: $(COMMON_TESTS)
	@echo + Ensuring Database Storage Layer Exists...
	@mkdir -p $(STORAGE)
	@for a in $(COMMON_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done

# Wierd protobuf stuff for compilation
$(OBJDIR)/Common/%.pb.o: $(OBJDIR)/Common/%.pb.h
$(OBJDIR)/Common/%.pb.cc $(OBJDIR)/Common/%.pb.h: Common/%.proto
	@echo + protoc $<
	@mkdir -p $(@D)
	$(V)$(LDLIBPATH) $(PROTOCEXE) --proto_path=Common --cpp_out=$(OBJDIR)/Common $<
