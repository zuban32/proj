#include "string.h"
#include "console.h"

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
kmemset(void *s0, int val, uint32_t size)
{
	const char *s = (char *)s0 + size;
	char *s1 = s0;
	while(s1 < s)
		*s1++ = val;
}

uint32_t 
kstoi(const char *s)
{
	kprintf(1, "kstoi started: %s\n", s);
	char *str = s;
	while(*str++);
	str -= 2;
	uint32_t res = 0;
	uint32_t mult = 1;
	while(str >= s)
	{
		res += mult * (*str-- - '0');
		mult *= 10;
	}
	kprintf(1, "%s -> %d\n", s, res);
	return res;
}