#pragma once

#include "int.h"

#ifdef __cplusplus
extern "C" {
#endif

void log(const char *format, ...) __attribute__((format(printf, 1, 2)));

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
