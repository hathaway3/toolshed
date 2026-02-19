# Project-Wide Rules

VERSION = 2.4.2

MERGE		= cat
SOFTLINK	= ln -s
RM		= rm -f
INSTALL		= install
MAKE		= make
ASM		= rma
AR		= $(CROSS)ar
RANLIB		= $(CROSS)ranlib
CC		= $(CROSS)cc

# Common include directory
INCDIR		= ../../../include

# Common CFLAGS
# -MMD -MP creates dependency files (.d) automatically
CFLAGS		+= -Dunix -DUNIX -O3 -I. -I$(INCDIR) -Wall -DTOOLSHED_VERSION=\"$(VERSION)\" -D_FILE_OFFSET_BITS=64 -Wno-unused-result -Werror -MMD -MP

# Library search paths
LDFLAGS		+= -L../libtoolshed -L../libcoco -L../libnative -L../libcecb -L../librbf -L../libdecb -L../libmisc -L../libsys

# Default libraries to link against
LDLIBS		+= -ltoolshed -lcoco -lnative -lcecb -lrbf -ldecb -lmisc -lsys -lm

ifneq ($(WIN),)
SUFEXE		= .exe
endif

# --- Standard Targets ---

.PHONY: all clean

# Default target
all: $(BINARY)$(SUFEXE) $(LIBRARY)

# Default clean target
clean:
	$(RM) *.o *.a *.d $(CLEAN_FILES) $(BINARY) $(BINARY)$(SUFEXE) $(LIBRARY)

# Include dependency files if they exist
-include $(wildcard *.d)

# --- Pattern Rules ---

# Building static libraries
$(LIBRARY): $(OBJS)
	$(AR) -rv $@ $^
	$(RANLIB) $@

# Building archives (generic rule)
%.a:
	$(AR) -rv $@ $^
	$(RANLIB) $@

# Building executables (standard tool rule)
$(BINARY)$(SUFEXE): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.l: %.r
	$(MERGE) $< > $@

%.r: %.a
	$(ASM) $< -o=$@
