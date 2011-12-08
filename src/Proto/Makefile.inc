# Modify global directives in global namespace
OBJDIRS += Proto
TESTS += ProtoTest

# relative to $(TOP), i.e. $(LOWERC_DIR)/*.cc
PROTO_SRCS := Proto/*.proto
PROTO_EXECUTABLES :=

# Set makefile template specific vars
UPPERC_DIR := PROTO
LOWERC_DIR := Proto

EXECUTABLE_OBJS :=
TEST_OBJS :=

include $(MAKEFILE_TEMPLATE)

# Directive to make the test case
ProtoTest: $(PROTO_TESTS)
	@echo + Ensuring Database Storage Layer Exists...
	@mkdir -p $(STORAGE)
	@for a in $(PROTO_TESTS); do \
		echo == $$a ==; \
		$(LDLIBPATH) $$a 2>$(LOGDIR)$${a#$(BINDIR)}; \
	done

# Weird protobuf stuff for compilation
$(OBJDIR)/Proto/%.pb.o: $(OBJDIR)/Proto/%.pb.h
$(OBJDIR)/Proto/%.pb.cc $(OBJDIR)/Proto/%.pb.h: Proto/%.proto
	@echo + protoc $<
	@mkdir -p $(@D)
	$(V)$(LDLIBPATH) $(PROTOCEXE) --proto_path=Proto --cpp_out=$(OBJDIR)/Proto $<
