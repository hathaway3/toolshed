# Makefile wrapper for ToolShed

all:
	$(MAKE) -C build/unix all

install:
	$(MAKE) -C build/unix install

clean:
	$(MAKE) -C build/unix clean

package:
	$(MAKE) -C build/unix package
