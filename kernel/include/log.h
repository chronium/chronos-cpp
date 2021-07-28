#pragma once

#include "int.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SERIAL "\xDA"
#define TERMINAL "\xDB"
#define BOTH "\xDC"
#define CSERIAL "\xEA"
#define CTERMINAL "\xEB"
#define CBOTH "\xEC"

void log(const char *format, ...) __attribute__((format(printf, 1, 2)));
void glog(const char *format, ...) __attribute__((format(printf, 1, 2)));

void panic(const char *format, ...)
    __attribute__((format(printf, 1, 2), noreturn));

#ifdef __cplusplus
}
#endif

#define REQUIRE(cond, ...)                                                     \
  {                                                                            \
    if (!(cond)) {                                                             \
      panic(__VA_ARGS__);                                                      \
    }                                                                          \
  }
