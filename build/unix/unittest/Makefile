# Makefile for Unit Tests
include ../rules.mak

vpath %.c ../../../unittest

LDFLAGS	+= -L../libtoolshed -L../libcecb -L../libcoco -L../libnative -L../libdecb -L../libmisc -L../librbf -L../libsys -ltoolshed -lcoco -lcecb -ldecb -lrbf -lmisc -lnative -lsys -lm

testall: librbftest libdecbtest libcecbtest
ifeq (,$(NOTEST))
	./librbftest
	./libdecbtest
	./libcecbtest
endif

librbftest:	librbftest.o
	$(CC) -o $@ $^ $(LDFLAGS)

libdecbtest: libdecbtest.o
	$(CC) -o $@ $^ $(LDFLAGS)

libcecbtest: libcecbtest.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.o *.dsk *.wav *.cas librbftest libdecbtest libcecbtest
