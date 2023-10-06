# Makefile for libtoolshed.a
include ../rules.mak

vpath %.c ../../../libtoolshed

CFLAGS	+= -I../../../include -Wall

%.a:
	$(AR) -r $@ $^
	$(RANLIB) $@

libtoolshed.a:	toolshed.o

clean:
	$(RM) *.o *.a
