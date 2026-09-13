# Makefile wrapper for ToolShed

FIRMWARE_DIRS = dwdos cocoroms hdbdos superdos

.PHONY: all test install clean package

all:
	$(MAKE) -C build/unix all

test:
	$(MAKE) -C build/unix test

install:
	$(MAKE) -C build/unix install

clean:
	$(MAKE) -C build/unix clean
	$(foreach dir, $(FIRMWARE_DIRS), $(MAKE) -C $(dir) clean;)

package:
	$(MAKE) -C build/unix package

# NOTE: build/win32 is deprecated. Use build/unix even for Windows cross-builds.
