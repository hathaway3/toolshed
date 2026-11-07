# Project-Wide Rules

FALLBACK_VERSION = 2.6.1
GIT_VERSION := $(shell git describe --tags --match '[vV][0-9]*' --abbrev=0 2>/dev/null | sed 's/^[vV]//')
VERSION ?= $(if $(GIT_VERSION),$(GIT_VERSION),$(FALLBACK_VERSION))

RULES_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
VERSION_FILE := $(RULES_DIR).version
CURRENT_SAVED_VERSION := $(shell cat $(VERSION_FILE) 2>/dev/null)
ifneq ($(CURRENT_SAVED_VERSION),$(VERSION))
$(shell printf '%s\n' "$(VERSION)" > $(VERSION_FILE))
endif

MERGE		= cat
SOFTLINK	= ln -s
RM			= rm -f
INSTALL		= install
MAKE		= make
CFLAGS		= -Dunix -DUNIX -O3 -I. -I../../../include -Wall -DTOOLSHED_VERSION=\"$(VERSION)\" -D_FILE_OFFSET_BITS=64 -Wno-unused-result -Werror
# Optional extra flags (e.g. for an AddressSanitizer build: EXTRA_CFLAGS="-g -O1 -fsanitize=address"
# EXTRA_LDFLAGS="-fsanitize=address"). Empty by default, so normal builds are unchanged.
CFLAGS		+= $(EXTRA_CFLAGS)
LDFLAGS	+= $(EXTRA_LDFLAGS)
ASM			= rma
AR			= $(CROSS)ar
RANLIB		= $(CROSS)ranlib
CC			= $(CROSS)cc

ifneq ($(WIN),)
SUFEXE		= .exe
CFLAGS		+= -D__USE_MINGW_ANSI_STDIO=1 -Wno-format
endif

%.l: %.r
	$(MERGE) $< > $@

%.r: %.a
	$(ASM) $< -o=$@

%.o: %.c $(VERSION_FILE)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<
