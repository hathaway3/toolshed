# Makefile for Unit Tests
include ../rules.mak

.DEFAULT_GOAL := testall
.PHONY: all testall test clean

vpath %.c ../../../unittest
vpath %.c ../../../tests

# Allow tests in tests/ to include headers from unittest/ (e.g. tinytest.h)
# and rebuild when an included header changes.
CFLAGS += -I../../../unittest -MMD -MP

LDLIBS += -L../libtoolshed -L../libcoco -L../libcecb -L../libdecb -L../libdragondos -L../libdiskimage \
          -L../librbf -L../libnative -L../libmisc -L../libsys \
          -ltoolshed -lcoco -lcecb -ldecb -ldragondos -ldiskimage -lrbf -lnative -lmisc -lsys -lm

TEST_LIBS = \
    ../libtoolshed/libtoolshed.a \
    ../libcoco/libcoco.a \
    ../libcecb/libcecb.a \
    ../libdecb/libdecb.a \
    ../libdragondos/libdragondos.a \
    ../libdiskimage/libdiskimage.a \
    ../librbf/librbf.a \
    ../libnative/libnative.a \
    ../libmisc/libmisc.a \
    ../libsys/libsys.a

TESTS = librbftest libdecbtest libcecbtest libtoolshedtest libnativetest libmisctest os9commandtest \
	decbcommandtest cecbcommandtest libsystest libdiskimagetest infocommandtest libdragondostest \
	dgndoscommandtest commandaliastest os9_tool_test

all: $(TESTS)

# Only the source and libraries are linked, not the headers the .d files add.
$(TESTS): %: %.c $(TEST_LIBS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< $(TEST_LIBS) $(LDLIBS) -o $@

testall: $(TESTS)
ifeq (,$(NOTEST))
	./librbftest
	./libdecbtest
	./libcecbtest
	./libtoolshedtest
	./libnativetest
	./libmisctest
	./libsystest
	./os9commandtest
	./decbcommandtest
	./cecbcommandtest
	./libdiskimagetest
	./infocommandtest
	./libdragondostest
	./dgndoscommandtest
	./commandaliastest
	./os9_tool_test
endif

# Summarized test report
test: testall
	@echo "--------------------------------------------------"
	@echo "Unit Test Summary"
	@echo "--------------------------------------------------"
	@FAILED=0; for test in $(TESTS); do \
		./$$test > /dev/null 2>&1 && echo "$$test: PASSED" || { echo "$$test: FAILED"; FAILED=1; }; \
	done; [ $$FAILED -eq 0 ] || exit 1
	@echo "--------------------------------------------------"
	@echo "CLI Integration Test Summary"
	@echo "--------------------------------------------------"
	@bash ../../../tests/os9_test.sh && echo "os9_test.sh: PASSED" || { echo "os9_test.sh: FAILED"; exit 1; }

clean:
	$(RM) *.o *.d *.dsk *.wav *.cas *.c10 $(TESTS)

-include $(TESTS:=.d)
