# Makefile for libdecb.a
include ../rules.mak

vpath %.c ../../../libdecb

CFLAGS	+= -I../../../include -Wall

%.a:
	$(AR) -r $@ $^
	$(RANLIB) $@

libdecb.a:	libdecbgs.o libdecbkill.o libdecbopen.o libdecbread.o libdecbrename.o \
            libdecbseek.o libdecbss.o libdecbread.o libdecbwrite.o libdecbtokenize.o \
            libdecbbinconcat.o libdecbsrec.o libdecbdskini.o

clean:
	$(RM) *.o *.a
