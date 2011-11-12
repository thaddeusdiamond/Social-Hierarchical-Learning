LOWERC_ROOT := Primitives
UPPERC_ROOT := PRIMITIVES

# Component fragments from */Makefile.inc
include $(LOWERC_ROOT)/DataLoader/Makefile.inc
include $(LOWERC_ROOT)/Learner/Makefile.inc
include $(LOWERC_ROOT)/PostProcessor/Makefile.inc
