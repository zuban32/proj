#include "common.h"
#include "kbd.h"

char 
get_key()
{
	prints("getting key ");
	return inb(0x60);
}