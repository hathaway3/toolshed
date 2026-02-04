# Makefile wrapper for ToolShed

FIRMWARE_DIRS = dwdos cocoroms hdbdos superdos

all:
	$(MAKE) -C build/unix all

install:
	$(MAKE) -C build/unix install

clean:
	$(MAKE) -C build/unix clean
	$(foreach dir, $(FIRMWARE_DIRS), $(MAKE) -C $(dir) clean;)

package:
	$(MAKE) -C build/unix package

# NOTE: build/win32 is deprecated. Use build/unix even for Windows cross-builds.
