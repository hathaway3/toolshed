include ../rules.mak

BINARY = cocofuse

vpath %.c ../../../$(BINARY)

ifdef __APPLE__
CFLAGS	+= -g -I../../../include -Wall -I/usr/local/include/osxfuse/fuse -DDEBUG
LDFLAGS	+= -g -L/usr/local/lib -L../libtoolshed -L../libcoco -L../libnative -L../libcecb -L../libdecb -L../libmisc -L../librbf -L../libsys -ltoolshed -lcoco -lnative -lcecb -ldecb -lrbf -lmisc -lsys -lm -losxfuse
else
CFLAGS	+= -g -I../../../include -Wall -DDEBUG
LDFLAGS	+= -g -L/usr/local/lib -L../libtoolshed -L../libcoco -L../libnative -L../libcecb -L../libdecb -L../libmisc -L../librbf -L../libsys -ltoolshed -lcoco -lnative -lcecb -ldecb -lrbf -lmisc -lsys -lm -lfuse
endif

$(BINARY):	$(BINARY).o
	-$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.o $(BINARY)
