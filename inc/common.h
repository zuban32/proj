#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <stdint.h>
#define NULL (0)

static inline uint8_t inb(int port)
{
	uint8_t data;
	__asm __volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline uint16_t inw(int port)
{
	uint16_t data;
	__asm __volatile("inw %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline uint16_t insw(int port)
{
	uint16_t data;
	__asm __volatile("inw %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

static inline void outb(int port, uint8_t data)
{
	__asm __volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

#endif

