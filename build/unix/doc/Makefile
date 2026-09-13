
include ../rules.mak

DOCDIR      = ../../../doc
HTML        = ToolShed.html
MARKDOWN    = markdown
CLEAN_FILES = $(HTML)

all: $(HTML)

# Try to build from Markdown; if the markdown tool isn't available,
# fall back to copying the pre-built HTML from the source doc/ directory.
$(HTML):
	@if command -v $(MARKDOWN) >/dev/null 2>&1 && test -f $(DOCDIR)/ToolShed.md; then \
		$(MARKDOWN) $(DOCDIR)/ToolShed.md > $@; \
	elif command -v $(MARKDOWN) >/dev/null 2>&1 && test -f $(DOCDIR)/archive/ToolShed.md; then \
		$(MARKDOWN) $(DOCDIR)/archive/ToolShed.md > $@; \
	elif test -f $(DOCDIR)/ToolShed.html; then \
		cp $(DOCDIR)/ToolShed.html $@; \
	elif test -f $(DOCDIR)/archive/ToolShed.html; then \
		cp $(DOCDIR)/archive/ToolShed.html $@; \
	else \
		echo "WARNING: Neither 'markdown' tool nor pre-built ToolShed.html found; creating placeholder."; \
		printf '<html><body><h1>ToolShed Documentation</h1><p>See the doc/ directory for documentation.</p></body></html>\n' > $@; \
	fi
