CC = gcc
LD = ld
AS = nasm
QEMU = /home/zuban32/qemu-install/bin/qemu-system-i386
QEMU_FLAGS = -m 256M
CFLAGS = -m32 -c -I../proj -std=c11 -DTEST -fno-builtin -ffreestanding\
-pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Werror
KERNEL_CFLAGS = -I../proj
USER_CFLAGS = -I../lib
LDFLAGS = -melf_i386 -nostdlib -Ttext 0x1000 --oformat binary -e kern_start
DLDFLAGS = -melf_i386 -nostdlib -Ttext 0x1000 -e kern_start
GCC_LIB = $(shell $(CC) $(CFLAGS) --print-libgcc-file-name)
ASBOOTFLAGS = -D KERNEL_SIZE=$(shell stat -c%s kernel.bin) -fbin
ASKERNFLAGS = -felf32
OBJDIR = obj/
TESTDIR = test/
BOOT_SRCS = $(wildcard boot/*)
KERNEL_ASM = $(wildcard kernel/*.asm)
KERNEL_C = $(wildcard kernel/*.c kernel/hw/*.c)
KERNEL_OBJ1 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_ASM:.asm=.o)))
KERNEL_OBJ2 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_C:.c=.o)))
KERNEL_DUMP = kernel.elf
TEST_SRC = test/hello.c
TEST_ELF = test/hello
TEST_ELF_SIZE = $(shell stat -c%s $(TEST_ELF))

all: kernel.bin boot.bin user
	@cat boot.bin kernel.bin > os.disk
	@dd if=/dev/zero bs=1 count=$$((0x5000 - 512 - $(shell stat -c%s kernel.bin))) >> os.disk 2> /dev/null
	@cat /home/zuban32/a.out >> os.disk
	@dd if=/dev/zero bs=1 count=$$((($(TEST_ELF_SIZE)/512 + 1) * 512 - $(TEST_ELF_SIZE))) >> os.disk 2> /dev/null

gdb: boot.bin kernel.bin
	@cat $^ > os.disk
	@$(QEMU) -hda os.disk -S -gdb tcp::1234 -serial stdio -vga std

boot.bin: $(BOOT_SRCS)
	@echo "Compiling bootloader"
	@$(AS) $(ASBOOTFLAGS) $< -o $@

kernel.obj:	$(KERNEL_ASM) $(KERNEL_C)
	@mkdir -p $(OBJDIR) 2>/dev/null
	@$(foreach var, $(KERNEL_ASM), $(AS) $(ASKERNFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.asm=.o));)
	@echo 'Compiling kernel'
	@$(foreach var, $(KERNEL_C), $(CC) $(CFLAGS) $(KERNEL_CFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.c=.o));)

kernel.asm: kernel.obj
	@$(LD) $(DLDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) $(TEST_OBJ) -o $(OBJDIR)$<
	@echo "Dumping kernel"
	@objdump -d -M intel $(OBJDIR)$< >$(OBJDIR)$@
	@echo "target remote :1234\nsymbol-file $(OBJDIR)$^" > .gdbinit

kernel.bin: kernel.obj
	@echo "Linking kernel"
	@$(LD) $(LDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) $(GCC_LIB) -o $@
	
user:
	@$(CC) $(CFLAGS) $(USER_CFLAGS) $(TEST_SRC) -o $(TEST_ELF)

clean:
	@rm -r -f $(OBJDIR)
	@cd test && find . ! -name '*.c' -type f -exec rm -f {} +
	@rm -f *.o *~ *.disk *.bin *.elf .gdbinit

run: all
	@echo ------------------------------------------------------
	@${QEMU} -hda os.disk -serial stdio -vga std
