#ifndef INC_ALLOC_H_
#define INC_ALLOC_H_

#include <inc/common.h>

enum {
	MAX_HEAP_BLOCKS = 1024,
};

struct HeapBlock
{
	int offset;
	unsigned size;
	int owner;

	HeapBlock(): offset(0), size(0), owner(-1) {}
	inline bool is_free() {
		return owner == -1 && size > 0 && offset >= 0;
	}
};

class HeapManager
{
	HeapBlock blocks[MAX_HEAP_BLOCKS];
	uintptr_t heap_top;
	int cur_block_top = 0;

public:
	HeapManager(uintptr_t htop): heap_top(htop) {}

	void *alloc(size_t size);
	void *alloc(unsigned count, size_t size);
	int free(void *_addr);
};

#endif /* INC_ALLOC_H_ */



