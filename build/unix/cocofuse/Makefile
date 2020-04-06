include ../rules.mak

BINARY = cocofuse

vpath %.c ../../../$(BINARY)

CFLAGS	+= -g -I../../../include -Wall -I/usr/local/include/osxfuse/fuse #-DDEBUG
LDFLAGS	+= -g -L/usr/local/lib -L../libtoolshed -L../libcoco -L../libnative -L../libcecb -L../libdecb -L../libmisc -L../librbf -L../libsys -ltoolshed -lcoco -lnative -lcecb -ldecb -lrbf -lmisc -lsys -lm -losxfuse

$(BINARY):	$(BINARY).o
	-$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.o $(BINARY)
