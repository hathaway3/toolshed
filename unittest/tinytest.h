/* TinyTest: A really really really tiny and simple no-hassle C unit-testing
 * framework.
 *
 * Features:
 *   - No library dependencies. Not even itself. Just a header file.
 *   - Simple ANSI C. Should work with virtually every C or C++ compiler on
 *     virtually any platform.
 *   - Reports assertion failures, including expressions and line numbers.
 *   - Stops test on first failed assertion.
 *   - ANSI color output for maximum visibility.
 *   - Easy to embed in apps for runtime tests (e.g. environment tests).
 *
 * Example Usage:
 *
 *    #include "tinytest.h"
 *    #include "mylib.h"
 *
 *    void test_sheep()
 *    {
 *      ASSERT("Sheep are cool", are_sheep_cool());
 *      ASSERT_EQUALS(4, sheep.legs);
 *    }
 *
 *    void test_cheese()
 *    {
 *      ASSERT("Cheese is tangy", cheese.tanginess > 0);
 *      ASSERT_STRING_EQUALS("Wensleydale", cheese.name);
 *    }
 *
 *    int main()
 *    {
 *      RUN(test_sheep);
 *      RUN(test_cheese);
 *      return TEST_REPORT();
 *    }
 *
 * To run the tests, compile the tests as a binary and run it.
 *
 * Project home page: http://github.com/joewalnes/tinytest
 *
 * 2010, -Joe Walnes <joe@walnes.com> http://joewalnes.com
 */

#ifndef _TINYTEST_INCLUDED
#define _TINYTEST_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Main assertion method */
#define ASSERTV(msg, value, expression)                                        \
  if (!tt_assert_v(__FILE__, __LINE__, (msg), value, (#expression),            \
                   (expression) ? 1 : 0))                                      \
  return
#define ASSERT(msg, expression)                                                \
  if (!tt_assert(__FILE__, __LINE__, (msg), (#expression),                     \
                 (expression) ? 1 : 0))                                        \
  return

/* Convenient assertion methods */
#define ASSERT_EQUALS(expected, actual)                                        \
  ASSERTV((#actual), actual, (expected) == (actual))
#define ASSERT_NEQUALS(expected, actual)                                       \
  ASSERTV((#actual), actual, (expected) != (actual))
#define ASSERT_STRING_EQUALS(expected, actual)                                 \
  ASSERT((#actual), strcmp((expected), (actual)) == 0)
#define ASSERT_MEM_EQUALS(expected, actual, size)                              \
  ASSERT((#actual), memcmp((expected), (actual), (size)) == 0)
#define ASSERT_TRUE(expression) ASSERT((#expression), (expression))
#define ASSERT_FALSE(expression) ASSERT((#expression), !(expression))

/* Run a test() function */
#define RUN(test_function) tt_execute((#test_function), (test_function))
#define TEST_REPORT() tt_report()

#define TT_COLOR_CODE 0x1B
#define TT_COLOR_RED "[1;31m"
#define TT_COLOR_GREEN "[1;32m"
#define TT_COLOR_RESET "[0m"

extern int tt_passes;
extern int tt_fails;
extern int tt_current_test_failed;
extern const char *tt_current_msg;
extern long tt_current_value;
extern const char *tt_current_expression;
extern const char *tt_current_file;
extern int tt_current_line;

void tt_execute(const char *name, void (*test_function)());
int tt_assert(const char *file, int line, const char *msg,
              const char *expression, int pass);
int tt_assert_v(const char *file, int line, const char *msg, long value,
                const char *expression, int pass);
int tt_report(void);

#endif
