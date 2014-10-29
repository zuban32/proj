#include "string.h"

int k_isspace(char c)
{
	return (c == ' ' || c == '\n');
}

int
kstrcmp(char *s1, char *s2)
{
	while(*s1 == *s2 && *s1 && *s2 && !k_isspace(*s1) && !k_isspace(*s2))
		s1++, s2++;
	return *s1 - *s2;
}

void
kmemset(char *s1, int val, uint32_t size)
{
	const char *s = s1 + size;
	while(s1 < s)
		*s1++ = val;
}