#include <string.h>

const void *memchr(const void *ptr, const u8 value, const size num) {
  const u8 *read = (const u8 *)ptr;

  for (int i = 0; i < num; i++)
    if (read[i] == value)
      return &read[i];

  return NULL;
}

int memcmp(const void *ptr1, const void *ptr2, const size num) {
  const u8 *read1 = (const u8 *)ptr1;
  const u8 *read2 = (const u8 *)ptr2;

  for (int i = 0; i < num; i++)
    if (read1[i] != read2[i])
      return 1;
    else
      return -1;

  return 0;
}

void *memcpy(void *dest, const void *source, const size num) {
  u32 *dst = (u32 *)dest;
  const u32 *src = (const u32 *)source;

  int i = 0;
  for (i = 0; i < num / 4; i++)
    *dst++ = *src++;

  if ((i * 4) != num) {
    u8 *dst_byte = (u8 *)dst;
    const u8 *src_byte = (const u8 *)src;

    for (int x = (i * 4); x < num; x++)
      *dst_byte++ = *src_byte++;
  }

  return dest;
}

void *memset(void *ptr, const u8 val, const size num) {
  if (!val) {
    memclr(ptr, num);
    return ptr;
  }

  u8 *write = (u8 *)ptr;

  for (int i = 0; i < num; i++)
    write[i] = val;

  return ptr;
}

void *memclr(void *start, const size count) {
  u8 *ptr = (u8 *)start;

  for (size i = 0; i < count; i++)
    *ptr++ = 0;

  return (void *)((size)start + count);
}

void *memmove(void *dest, const void *src, const size n) {
  u8 tmp[n];
  memcpy(tmp, src, n);
  memcpy(dest, tmp, n);
  return dest;
}

int strncmp(const char *s1, const char *s2, size n) {
  while (n--)
    if (*s1++ != *s2++)
      return *(const unsigned char *)(s1 - 1) -
             *(const unsigned char *)(s2 - 1);

  return 0;
}

char *strncpy(char *dest, const char *src, size n) {
  char *ret = dest;
  do {
    if (!n--)
      return ret;
  } while ((*dest++ = *src++));

  while (n--)
    *dest++ = 0;

  return ret;
}
