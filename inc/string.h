#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include <inc/common.h>

int kstrcmp(const char *s1, const char *s2);
void kmemset(void *s0, int val, uint32_t size);
void kmemcpy(char *dest, char *src, int size);
uint32_t kstoi(const char *s);
int kisspace(const char c);
int kisdigit(const char c);
int kisletter(const char c);

#endif

