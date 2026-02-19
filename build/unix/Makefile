# Makefile for ToolShed

include rules.mak

vpath %.c ../../mamou:../../os9

PACKAGENAME_UNIX	= toolshed-unix64-$(VERSION).tgz
PACKAGENAME_WIN		= toolshed-win64-$(VERSION).zip

PREFIX		?= /usr/local

ifneq ($(DESTDIR),)
INSTALLDIR	?= $(DESTDIR)/usr/bin
DOCDIR		?= $(DESTDIR)/usr/share/doc/toolshed
else
INSTALLDIR	?= $(PREFIX)/bin
DOCDIR		?= $(PREFIX)/share/doc/toolshed
endif
APPS 		= ar2 os9 cecb decb tocgen makewav dis68 lst2cmt mamou

DIRS		= libtoolshed libnative libcecb librbf libcoco libdecb libmisc libsys \
		decb cecb os9 makewav tocgen lst2cmt \
		dis68 ar2 mamou \
		doc

ifneq ($(WIN),)
PACKAGENAME	= $(PACKAGENAME_WIN)
MAKEPACKAGE = zip -9
else
PACKAGENAME	= $(PACKAGENAME_UNIX)
MAKEPACKAGE = tar czvf
DIRS		:= $(DIRS) cocofuse unittest
APPS		:= $(APPS) cocofuse
endif

APPS		:= $(foreach item,$(APPS),$(item)/$(item)$(SUFEXE))

all:
	$(foreach dir, $(DIRS), (cd $(dir); $(MAKE));)

install: all
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
	
clean: cleandirs
	$(RM) $(PACKAGENAME_WIN) $(PACKAGENAME_UNIX)

unixpackage:
	$(MAKE) -f ./Makefile package

winpackage:
	$(MAKE) -f ./Makefile cleandirs
	$(MAKE) -f ./Makefile CC=x86_64-w64-mingw32-gcc WIN=1 package

allpackages: clean unixpackage winpackage 


