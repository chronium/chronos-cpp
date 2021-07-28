#include <log.h>

void kinit() {
  log("We have serial!");
  log(CBOTH "int i = 0;\nint ii = 10 / i;\nResult:\n");
  int i = 0;
  int ii = 10 / i;
  (void)ii;
}
