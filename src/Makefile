#
# This makefile system follows the structuring conventions
# recommended by Peter Miller in his excellent paper:
#
#       Recursive Make Considered Harmful
#       http://aegis.sourceforge.net/auug97.pdf
#
# Later pilfered from Alex Thomson's Deterministic DBMS project

# Root directories to be created and cleaned after
OBJDIR    := ../obj
BINDIR    := ../bin
LOGDIR    := ../logs
EXTDIR    := ../ext
DOCDIR    := ../doc
STORAGE   := ../db

# External programs that are linked and used automatically in the process
GTEST     := $(EXTDIR)/googletest
PROTOB    := $(EXTDIR)/protobuf
NAOSDK    := /home/brad/nao-sdk/naoqi-sdk-1.12-linux32
HASHLIB   := /usr/include/hashlib++

# Doxygen specific directives
DOCOUT    := ../doc/html
DOXY_EXE  := doxygen
DOXY_CONF := SHL.doxygen

# Executable for protocol buffers
PROTOCEXE := $(PROTOB)/src/.libs/protoc

# Directives for where the source files should look to include headers from
TOP       := .
PRIMDIR   := ./Primitives
OBSVDIR   := ./Observation
MGRDIR	  := ./Manager

# '$(V)' controls whether the lab makefiles print verbose commands (the
# actual shell commands run by Make), as well as the "overview" commands
# (such as '+ cc lib/readline.c').
#
# For overview commands only, the line should read 'V = @'.
# For overview and verbose commands, the line should read 'V ='.
V = @ 

# Set std compiler options (linking for google test and pthread), LD_LIB
CXX       := g++
CXXFLAGS  := -g -I$(PROTOB)/src -I$(GTEST)/include -Wall -Werror
CXXFLAGS  += -I$(TOP) -I$(OBJDIR) -I$(PRIMDIR) -I$(OBSVDIR)
CXXFLAGS  += -I$(MGRDIR) -I$(NAOSDK)/include -I$(HASHLIB)
MDFLAGS   := -MD
LDFLAGS   := -lrt -lpthread -lgtest -L$(GTEST)/lib/.libs -lprotobuf 
LDFLAGS   += -L$(PROTOB)/src/.libs -lhl++
LDLIBPATH := LD_LIBRARY_PATH=$(GTEST)/lib/.libs:$(PROTOB)/src/.libs:$(NAOSDK)/lib

# Lists that the */Makefile.inc makefile fragments will add to
TESTS :=

# Make sure that 'all' is the first target
all:
	@echo + Ensuring Database Storage Layer Exists...
	@mkdir -p $(STORAGE)
	
# Perform doxygen every time, no matter what is called
#$(warning + doc $(DOXY_EXE) $(DOCDIR)/$(DOXY_CONF) > $(DOCOUT))
#$(shell cd $(DOCDIR); $(DOXY_EXE) $(DOXY_CONF))

# We define a common "Makefrag"-style template so that we can just set dir
# specific variables and include that template
MAKEFILE_TEMPLATE := Makefile.template

# Makefile fragments for library code
include Proto/Makefile.inc
include Common/Makefile.inc
include Backend/Makefile.inc
include Primitives/Makefile.inc
include Observation/Makefile.inc
include Performance/Makefile.inc
include Manager/Makefile.inc

Test: $(TESTS)

clean:
	rm -rf $(OBJDIR) $(LOGDIR) $(BINDIR) $(STORAGE) $(DOCOUT)
	
# This magic automatically generates makefile dependencies
# for header files included from C source files we compile,
# and keeps those dependencies up-to-date every time we recompile.
# See 'mergedep.pl' for more information.
$(OBJDIR)/.deps: $(foreach dir, $(OBJDIRS), $(wildcard $(OBJDIR)/$(dir)/*.d))
	@mkdir -p $(@D)
	@cat $^ /dev/null > $@
#	@$(PERL) mergedep.pl $@ $^
-include $(OBJDIR)/.deps

always:
	@:

# Eliminate default suffix rules
.SUFFIXES:

# make it so that no intermediate .o files are ever deleted
.PRECIOUS: $(foreach dir, $(OBJDIRS), $(OBJDIR)/$(dir)/%.o) \
           $(foreach dir, $(OBJDIRS), $(OBJDIR)/$(dir)/%UnitTest.o) \
           $(foreach dir, $(OBJDIRS), $(OBJDIR)/$(dir)/%.pb.cc) \
           $(foreach dir, $(OBJDIRS), $(OBJDIR)/$(dir)/%.pb.h)

.PHONY: all always clean test
