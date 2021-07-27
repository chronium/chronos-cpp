#pragma once

#include <int.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

const void *memchr(const void *, const u8, const size);
int memcmp(const void *, const void *, const size);
void *memcpy(void *, const void *, const size);
void *memset(void *, const u8, const size);
void *memclr(void *, const size);
void *memmove(void *, const void *, const size);

int strncmp(const char *, const char *, size);
char *strncpy(char *, const char *, size);

#ifdef __cplusplus
}
#endif