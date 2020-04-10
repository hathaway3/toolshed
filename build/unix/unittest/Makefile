# Makefile for Unit Tests
include ../rules.mak

vpath %.c ../../../unittest

LDFLAGS	+= -g -L../libtoolshed -L../libcecb -L../libcoco -L../libnative -L../libdecb -L../libmisc -L../librbf -L../libsys -ltoolshed -lcoco -lnative -ldecb -lcecb -lmisc -lrbf -lsys -lm

testall: librbftest
	os9 format -q test.dsk
	echo "Test" > /tmp/test.txt
	os9 copy /tmp/test.txt test.dsk,test.txt
	./librbftest

librbftest:	librbftest.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.o librbftest
