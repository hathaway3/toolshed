include ../rules.mak

vpath %.c ../../../unittest

# Object files needed by all tests
TINYTEST_OBJ = tinytest.o
TESTS = librbftest libdecbtest libcecbtest libtoolshedtest os9commandtest

all: $(TINYTEST_OBJ) $(TESTS)

testall: all
ifeq (,$(NOTEST))
	./librbftest
	./libdecbtest
	./libcecbtest
	./libtoolshedtest
	./os9commandtest
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
