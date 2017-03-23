#ifndef INC_X86_MEM_H_
#define INC_X86_MEM_H_

#include <stdint.h>

#include <console.h>

typedef struct
{
	uint64_t base;
	uint64_t len;
	uint32_t type;
} RAMMapEntry;

typedef struct
{
	RAMMapEntry *head;
	int num;
} RAMMap;

enum RAMTypes {
	NORMAL = 1,
	RESERVED,
	ACPI_RECLAIM,
	ACPI_NVS,
	BAD
};

static inline void dump_entry(RAMMapEntry *entry)
{
	kprintf("Base: %x\nLen: %x\nType: %x\n\n", (uint32_t)entry->base, (uint32_t)entry->len, entry->type);
}

static inline void dump_map(RAMMap *map)
{
	kprintf("Map len = %d\n", map->num);
	for(int i = 0; i < map->num; i++) {
		dump_entry(map->head + i);
	}
}

// Returns memory size in megabytes
static inline int get_memory_size(RAMMap *map)
{
	uint64_t max = 0;
	// skip last entry because it always points to 4GB
	for(int i = 0; i < map->num - 1; i++) {
		RAMMapEntry *cur = map->head + i;
		if(cur->base + cur->len > max) {
			max = cur->base + cur->len;
		}
	}
	return max / (1024 * 1024);
}

#endif /* INC_X86_MEM_H_ */
