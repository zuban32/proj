#include <inc/alloc.h>
#include <inc/paging.h>
#include <inc/process.h>
#include <inc/string.h>

char heap_mem[4 * 1024 * 1024] __attribute__ ((aligned (PAGE_SIZE)));

static HeapManager heap_mgr((uintptr_t)0);

void *HeapManager::alloc(size_t size)
{
	auto hb = heap_mgr.blocks;
	while(!(hb++)->is_free() || hb->size < size);

	// haven't found appropriate free block
	if(hb >= heap_mgr.blocks + MAX_HEAP_BLOCKS)
		return nullptr;

	// can't split this free block - not enough space
	if(size < hb->size && heap_mgr.cur_block_top == MAX_HEAP_BLOCKS - 1)
		return nullptr;

	hb->owner = get_cur_process()->id;

	if(size < hb->size) {
		auto new_block = heap_mgr.blocks + heap_mgr.cur_block_top++;
		new_block->offset = hb->offset + size;
		new_block->size = hb->size - size;
		new_block->owner = -1;
	}

	return (void*)(heap_mgr.heap_top + hb->offset);
}

void *HeapManager::alloc(unsigned count, size_t size)
{
	return heap_mgr.alloc(count * size);
}

int HeapManager::free(void *_addr)
{
	uintptr_t addr = (uintptr_t)_addr;
	for(int i = 0; i < heap_mgr.cur_block_top; i++) {
		HeapBlock *cur = heap_mgr.blocks + i;
		if(heap_mgr.heap_top + cur->offset == addr) {
			cur->owner = -1;
			if(i+1 < heap_mgr.cur_block_top && heap_mgr.blocks[i+1].is_free()) {
				cur->size += heap_mgr.blocks[i+1].size;
				kmemcpy(heap_mgr.blocks + (i + 1), heap_mgr.blocks + (i + 2),
						(heap_mgr.cur_block_top -1 - (i+2) + 1)* sizeof(*heap_mgr.blocks));
				heap_mgr.cur_block_top--;
			}
			return 0;
		}
	}
	return 1;
}
