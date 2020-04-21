# Makefile for Unit Tests
include ../rules.mak

vpath %.c ../../../unittest

LDFLAGS	+= -L../libtoolshed -L../libcecb -L../libcoco -L../libnative -L../libdecb -L../libmisc -L../librbf -L../libsys -ltoolshed -lcoco -ldecb -lcecb -lrbf -lmisc -lnative -lsys -lm

testall: librbftest libdecbtest
ifeq (,$(NOTEST))
	./librbftest
	./libdecbtest
endif

librbftest:	librbftest.o
	$(CC) -o $@ $^ $(LDFLAGS)

libdecbtest: libdecbtest.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.o librbftest libdecbtest
