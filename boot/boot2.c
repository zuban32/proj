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
			kmemcpy((char *)cur_ph->p_vaddr, (char *)file + cur_ph->p_offset,
					cur_ph->p_filesz);
		}
	}
}

void boot2_main(char *file, uint32_t gdt_start)
{
	load_kernel_code((Elf32_Ehdr *)file);
	((void (*)(uint32_t))((Elf32_Ehdr *)file)->e_entry)(gdt_start);
}
