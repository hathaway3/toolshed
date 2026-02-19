# Makefile for decb
BINARY = decb
OBJS = decbcopy.o decbdskini.o decbdir.o decblist.o decbhdbconv.o decbdsave.o \
       decbfstat.o decbrename.o decbkill.o decbfree.o decb_main.o decbattr.o \
       os9dump.o os9dsave.o

include ../rules.mak
vpath %.c ../../../decb ../../../os9

all: $(BINARY)$(SUFEXE)
