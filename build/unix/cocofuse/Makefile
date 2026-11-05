include ../rules.mak

BINARY = cocofuse

vpath %.c ../../../$(BINARY)

OS := $(shell uname)
ifeq ($(OS),Darwin)
CFLAGS	+= -I../../../include -Wall -DDEBUG
FUSE_CFLAGS := $(shell pkg-config --cflags fuse 2>/dev/null)
ifeq ($(FUSE_CFLAGS),)
CFLAGS  += -F/Library/Frameworks -I/usr/local/include/fuse -I/usr/local/include/fuse/fuse -I/usr/local/include/osxfuse/fuse -I/opt/homebrew/include/fuse -I/opt/homebrew/include/fuse/fuse
else
CFLAGS  += $(FUSE_CFLAGS)
endif
LDFLAGS	+= -L/usr/local/lib -L/opt/homebrew/lib -L../libtoolshed -L../libcoco -L../libdragondos -L../libdiskimage -L../libnative -L../libcecb -L../libdecb -L../libmisc -L../librbf -L../libsys -ltoolshed -lcoco -ldragondos -ldiskimage -lnative -lcecb -ldecb -lrbf -lmisc -lsys -lm
FUSE_LIBS := $(shell pkg-config --libs fuse 2>/dev/null)
ifeq ($(FUSE_LIBS),)
ifneq ($(wildcard /usr/local/lib/libfuse-t* /opt/homebrew/lib/libfuse-t* /Library/Frameworks/fuse_t.framework),)
LDFLAGS += -lfuse-t
else
LDFLAGS += -lfuse
endif
else
LDFLAGS += $(FUSE_LIBS)
endif
else
CFLAGS	+= -I../../../include -Wall -DDEBUG
LDFLAGS	+= -L/usr/local/lib -L../libtoolshed -L../libcoco -L../libdragondos -L../libdiskimage -L../libnative -L../libcecb -L../libdecb -L../libmisc -L../librbf -L../libsys -ltoolshed -lcoco -ldragondos -ldiskimage -lnative -lcecb -ldecb -lrbf -lmisc -lsys -lm -lfuse
endif

all: $(BINARY)$(SUFEXE)

$(BINARY)$(SUFEXE): $(BINARY).o
	-$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.o *.d $(BINARY) $(BINARY)$(SUFEXE)
