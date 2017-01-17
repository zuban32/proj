#include <inc/common.h>
#include <inc/elf.h>

static void kmemcpy(char *dest, char *src, int size)
{
	char *end = src + size;
	while(src < end) {
		*dest++ = *src++;
	}
}

static void load_kernel_code(Elf32_Ehdr *file)
{
	Elf32_Phdr *ph = (Elf32_Phdr *)((uint8_t *)file + file->e_phoff);
	for(int i = 0; i < file->e_phnum; i++) {
		Elf32_Phdr *cur_ph = ph + i;
		if(cur_ph->p_type == 1) {
	//		page_alloc(cur_ph->p_vaddr, 1);
//			kmemset((void *)cur_ph->p_vaddr, 0, cur_ph->p_memsz);
			kmemcpy((char *)cur_ph->p_vaddr, (char *)file + cur_ph->p_offset,
					cur_ph->p_filesz);
		}
	}
}

void boot2_main(char *file, uint32_t gdt_start)
{
//	int count = 1, cur_buf_ind = 0;
////	while(1);
//	for(int i = 0; i < count; i++) {
//		ata_request_readsector(KERNEL_CODE_OFF / 512, 1);
////		while(1) {
////			int res = inb(0x1f7);
////			if(!(res & 0x80) || (res & 0x8))
////				break;
////		}
//		delay();
//		while ((inb(0x1F7) & 0xC0) != 0x40);
//		uint16_t *out = read_buffer + cur_buf_ind * SECTOR_SIZE;
//		for(int j = 0; j < SECTOR_SIZE; j++) {
//			*(out + j) = inw(PRIMARY_BASE_START);
//		}
//		delay();
//		cur_buf_ind++;
//	}

	load_kernel_code((Elf32_Ehdr *)file);
	((void (*)(uint32_t))((Elf32_Ehdr *)file)->e_entry)(gdt_start);
}
