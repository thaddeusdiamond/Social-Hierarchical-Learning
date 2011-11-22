# Modify global directives in global namespace
OBJDIRS += Common
OBJDIRS += Proto
TESTS += CommonTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
COMMON_SRCS := Common/*.proto

# Set makefile template specific vars
UPPERC_DIR := COMMON
LOWERC_DIR := Common

SRC_LINKED_OBJECTS := $(OBJDIR)/Proto/%.cc

EXECUTABLE :=
EXECUTABLE_OBJS :=

EXTRA_TEST_OBJS :=

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
CommonTest: $(COMMON_TESTS)
	@for a in $(COMMON_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done

# Wierd protobuf stuff for compilation
$(OBJDIR)/Proto/%.o: $(OBJDIR)/Proto/%.pb.h
$(OBJDIR)/Proto/%.pb.cc $(OBJDIR)/Proto/%.pb.h: Common/%.proto
	@echo + protoc $<
	@mkdir -p $(@D)
	$(V)$(LDLIBPATH) $(PROTOCEXE) --proto_path=Common --cpp_out=$(OBJDIR)/Proto $<
