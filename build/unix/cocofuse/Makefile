BINARY = cocofuse
OBJS = $(BINARY).o

include ../rules.mak
vpath %.c ../../../$(BINARY)

# Platform-specific FUSE flags
OS := $(shell uname)
ifeq ($(OS),Darwin)
FUSE_CFLAGS := $(shell pkg-config --cflags fuse 2>/dev/null)
ifeq ($(FUSE_CFLAGS),)
CFLAGS  += -I/usr/local/include/fuse -I/usr/local/include/fuse/fuse -I/usr/local/include/osxfuse/fuse -I/opt/homebrew/include/fuse -I/opt/homebrew/include/fuse/fuse -DDEBUG
else
CFLAGS  += $(FUSE_CFLAGS) -DDEBUG
endif
FUSE_LIBS := $(shell pkg-config --libs fuse 2>/dev/null)
ifeq ($(FUSE_LIBS),)
LDFLAGS += -L/usr/local/lib -L/opt/homebrew/lib
LDLIBS  += -lfuse
else
LDLIBS  += $(FUSE_LIBS)
endif
else
CFLAGS  += -DDEBUG
LDFLAGS += -L/usr/local/lib
LDLIBS  += -lfuse
endif

all: $(BINARY)$(SUFEXE)
