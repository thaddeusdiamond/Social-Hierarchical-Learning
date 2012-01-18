# Global configuration (report name, system path names)
REPORT  := report
LATEX   := latex
BIBTEX  := bibtex

# Directory structure for output
OUTPUT	:= output
INTER   := $(OUTPUT)/intermediate
FIGURES	:= figures

# Collect the appropriate files for compilation
STYLES  := $(wildcard *.sty)
SRCS    := $(wildcard *.tex *.cls)
SRCS    += $(wildcard $(FIGURES)/*.eps)
REFS    := $(wildcard *.bib)

all: view

$(REPORT).dvi: $(SRCS) $(REFS)
	@mkdir -p $(INTER)
	@cp $(REFS) $(SRCS) $(STYLES) $(INTER)
	@cd $(INTER); \
		$(LATEX) $(REPORT); $(BIBTEX) $(REPORT); \
		$(LATEX) $(REPORT); $(LATEX)  $(REPORT);

$(REPORT).ps: $(REPORT).dvi
	dvips -Pcmz -t letter $(INTER)/$(REPORT).dvi -o $(OUTPUT)/$(REPORT).ps

pdf: $(REPORT).ps
	ps2pdf $(OUTPUT)/$(REPORT).ps $(OUTPUT)/$(REPORT).pdf

without-display: pdf

view: pdf
	acroread $(OUTPUT)/$(REPORT).pdf & 2>/dev/null

view-dvi: $(REPORT).dvi
	xdvi $(INTER)/$(REPORT).dvi

print: $(REPORT).dvi
	dvips $(INTER)/$(REPORT).dvi -o $(OUTPUT)/$(REPORT).ps

clean:
	rm -rf $(OUTPUT)

