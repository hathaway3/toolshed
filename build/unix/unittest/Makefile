# Makefile for Unit Tests
include ../rules.mak

vpath %.c ../../../unittest

LDFLAGS	+= -L../libtoolshed -L../libcecb -L../libcoco -L../libnative -L../libdecb -L../libmisc -L../librbf -L../libsys -ltoolshed -lcoco -ldecb -lcecb -lrbf -lmisc -lnative -lsys -lm

testall: librbftest
ifeq (,$(NOTEST))
	./librbftest
endif

librbftest:	librbftest.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.o librbftest
