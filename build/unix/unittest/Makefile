include ../rules.mak

vpath %.c ../../../unittest
vpath %.c ../../../tests

# Allow tests in tests/ to include headers from unittest/ (e.g. tinytest.h)
CFLAGS += -I../../../unittest

# Object files needed by all tests
TINYTEST_OBJ = tinytest.o
TESTS = librbftest libdecbtest libcecbtest libtoolshedtest os9commandtest os9_tool_test

all: $(TINYTEST_OBJ) $(TESTS)

testall: all
ifeq (,$(NOTEST))
	./librbftest
	./libdecbtest
	./libcecbtest
	./libtoolshedtest
	./os9commandtest
	./os9_tool_test
endif

# Rule to build binaries, linking tinytest.o
$(TESTS): %: %.o $(TINYTEST_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# Summarized test report
test: testall
	@echo "--------------------------------------------------"
	@echo "Unit Test Summary"
	@echo "--------------------------------------------------"
	@for test in $(TESTS); do \
		./$$test > /dev/null 2>&1 && echo "$$test: PASSED" || echo "$$test: FAILED"; \
	done
	@echo "--------------------------------------------------"
	@echo "CLI Integration Test Summary"
	@echo "--------------------------------------------------"
	@bash ../../../tests/os9_test.sh && echo "os9_test.sh: PASSED" || echo "os9_test.sh: FAILED"
