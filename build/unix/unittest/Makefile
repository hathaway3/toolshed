# Makefile for Unit Tests
include ../rules.mak

vpath %.c ../../../unittest

LDFLAGS	+= -L../libtoolshed -L../libcecb -L../libcoco -L../libnative -L../libdecb -L../libmisc -L../librbf -L../libsys -ltoolshed -lcoco -lnative -ldecb -lcecb -lmisc -lrbf -lsys -lm

testall: librbftest
	./librbftest

librbftest:	librbftest.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.o librbftest
