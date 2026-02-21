#include "tinytest.h"

int tt_passes = 0;
int tt_fails = 0;
int tt_current_test_failed = 0;
const char *tt_current_msg = NULL;
long tt_current_value = 0;
const char *tt_current_expression = NULL;
const char *tt_current_file = NULL;
int tt_current_line = 0;

void tt_execute(const char *name, void (*test_function)()) {
  tt_current_test_failed = 0;
  test_function();
  if (tt_current_test_failed) {
    printf("failure: %s:%d: In test %s():\n    %s (%ld) (%s)\n",
           tt_current_file, tt_current_line, name, tt_current_msg,
           tt_current_value, tt_current_expression);
    tt_fails++;
  } else {
    tt_passes++;
  }
}

int tt_assert(const char *file, int line, const char *msg,
              const char *expression, int pass) {
  tt_current_msg = msg;
  tt_current_value = 0;
  tt_current_expression = expression;
  tt_current_file = file;
  tt_current_line = line;
  tt_current_test_failed = !pass;
  return pass;
}

int tt_assert_v(const char *file, int line, const char *msg, long value,
                const char *expression, int pass) {
  tt_current_msg = msg;
  tt_current_value = value;
  tt_current_expression = expression;
  tt_current_file = file;
  tt_current_line = line;
  tt_current_test_failed = !pass;
  return pass;
}

int tt_report(void) {
  if (tt_fails) {
    printf("%c%sFAILED%c%s [%s] (passed:%d, failed:%d, total:%d)\n",
           TT_COLOR_CODE, TT_COLOR_RED, TT_COLOR_CODE, TT_COLOR_RESET,
           tt_current_file, tt_passes, tt_fails, tt_passes + tt_fails);
    return -1;
  } else {
    printf("%c%sPASSED%c%s [%s] (total:%d)\n", TT_COLOR_CODE, TT_COLOR_GREEN,
           TT_COLOR_CODE, TT_COLOR_RESET, tt_current_file, tt_passes);
    return 0;
  }
}
