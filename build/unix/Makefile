# Makefile for ToolShed

include rules.mak

PACKAGENAME_UNIX	= toolshed-unix64-$(VERSION).tgz
PACKAGENAME_WIN		= toolshed-win64-$(VERSION).zip

PREFIX      ?= $(HOME)/.local
prefix      ?= $(PREFIX)
bindir      ?= $(prefix)/bin
docdir      ?= $(prefix)/share/doc/toolshed

ifneq ($(DESTDIR),)
INSTALLDIR  ?= $(DESTDIR)$(bindir)
DOCDIR      ?= $(DESTDIR)$(docdir)
else
INSTALLDIR  ?= $(bindir)
DOCDIR      ?= $(docdir)
endif

APPS 		= ar2 os9 cecb decb tocgen makewav dis68 lst2cmt dgndos

DIRS		= libtoolshed libnative libcecb librbf libcoco libdecb libdiskimage libdragondos libmisc libsys \
		decb cecb os9 makewav tocgen lst2cmt dgndos \
		dis68 ar2 \
		doc

ifneq ($(WIN),)
PACKAGENAME	= $(PACKAGENAME_WIN)
MAKEPACKAGE = zip -9
else
PACKAGENAME	= $(PACKAGENAME_UNIX)
MAKEPACKAGE = tar czvf
DIRS		:= $(DIRS) unittest
ifeq ($(NO_FUSE),1)
NO_COCOFUSE = 1
endif
ifeq ($(NO_COCOFUSE),)
HAS_FUSE := $(shell $(CC) -E -D_FILE_OFFSET_BITS=64 -I../../include -F/Library/Frameworks -I/usr/local/include/fuse -I/usr/local/include/fuse/fuse -I/usr/local/include/osxfuse/fuse -I/opt/homebrew/include/fuse -I/opt/homebrew/include/fuse/fuse ../../cocofuse/cocofuse.c >/dev/null 2>&1 && echo 1 || echo 0)
ifeq ($(HAS_FUSE),1)
DIRS		:= $(DIRS) cocofuse
APPS		:= $(APPS) cocofuse
endif
endif
endif

APPS		:= $(foreach item,$(APPS),$(item)/$(item)$(SUFEXE))

.PHONY: all test install package cleandirs clean unixpackage winpackage allpackages print-version

all:
	@set -e; for dir in $(DIRS); do $(MAKE) -C $$dir; done

print-version:
	@echo "$(VERSION)"

test: all
ifneq ($(WIN),)
	@echo "Tests skipped for Windows target"
else
	$(MAKE) -C unittest test
endif

install: test
	$(INSTALL) -d $(INSTALLDIR) $(DOCDIR)
	$(INSTALL) $(APPS) $(INSTALLDIR)
	$(INSTALL) -m 0644 doc/ToolShed.html $(DOCDIR)

package: INSTALLDIR=toolshed-$(VERSION)
package: DOCDIR = $(INSTALLDIR)
package: install
	$(MAKEPACKAGE) $(PACKAGENAME) toolshed-$(VERSION)/*
	rm -rf toolshed-$(VERSION)

cleandirs:
	$(foreach dir, $(DIRS) cocofuse unittest, [ ! -d $(dir) ] || $(MAKE) -C $(dir) clean;)

clean:: cleandirs
	$(RM) $(PACKAGENAME_WIN) $(PACKAGENAME_UNIX) .version

unixpackage:
	$(MAKE) -f ./Makefile package

winpackage:
	$(MAKE) -f ./Makefile cleandirs
	$(MAKE) -f ./Makefile CC=x86_64-w64-mingw32-gcc WIN=1 package

allpackages: clean unixpackage winpackage
