BINARY = cocofuse
OBJS = $(BINARY).o

include ../rules.mak
vpath %.c ../../../$(BINARY)

# Platform-specific FUSE flags
OS := $(shell uname)
ifeq ($(OS),Darwin)
CFLAGS  += -I/usr/local/include/fuse/fuse -DDEBUG
else
CFLAGS  += -DDEBUG
endif

LDFLAGS += -L/usr/local/lib
LDLIBS  += -lfuse

all: $(BINARY)$(SUFEXE)
