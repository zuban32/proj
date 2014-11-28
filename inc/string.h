#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include "common.h"

int kstrcmp(char *, char *);
void kmemset(void *, int, uint32_t);
uint32_t kstoi(const char *);
int k_isspace(char);

#endif