# Makefile for libcecb.a

vpath %.c ../../../libcecb

CFLAGS	+= -I../../../include

%.a:
	ar -r $@ $^
	ranlib $@

libcecb.a:	libcecbbulkerase.o libcecbwrite.o libcecbwav.o libcecbread.o \
					libcecbgs.o libcecbcas.o libcebcopen.o libcecbtokenize.o

clean:
	rm -f *.o *.a
