CC = gcc
LD = ld
AS = nasm
QEMU = /home/zuban32/qemu-install/bin/qemu-system-i386
QEMU_FLAGS = -m 4G
CFLAGS = -m32 -std=c11 -fno-builtin -ffreestanding\
-pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Werror
KERNEL_CFLAGS = -c -I../proj
USER_CFLAGS =  -nostdlib -I../proj/lib -Wl,-eusermain
LDFLAGS = -melf_i386 -nostdlib -Ttext 0x7f00 --oformat binary -e kern_start
DLDFLAGS = -melf_i386 -nostdlib -Ttext 0x7f00 -e kern_start
GCC_LIB = $(shell $(CC) $(CFLAGS) --print-libgcc-file-name)
BOOT1_FLAGS = -D KERNEL_SIZE=$(shell stat -c%s kernel.bin) -fbin
ASKERNFLAGS = -felf32
OBJDIR = obj/
TESTDIR = test/
BOOT1_SRCS = boot/boot.asm boot/gdt.asm boot/switch_pm.asm
BOOT2_SRCS = boot/boot2.c
KERNEL_ASM = kernel/1st_entry.asm kernel/isr_entry.asm
KERNEL_C = $(wildcard kernel/*.c kernel/hw/*.c)
LIB_C = $(wildcard lib/*.c)
LIB_OBJ = $(notdir $(LIB_C:.c=.o))
LIB_OUT = usrlib
KERNEL_OBJ1 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_ASM:.asm=.o)))
KERNEL_OBJ2 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_C:.c=.o)))
KERNEL_DUMP = kernel.elf
TEST_SRC = test/hello.c
TEST_ELF = test/hello
TEST_ELF_SIZE = $(shell stat -c%s $(TEST_ELF))

all: kernel.bin boot1.bin user
	@cat boot1.bin > os.disk
	@cat kernel.bin >> os.disk
	@dd if=/dev/zero bs=1 count=$$((0x20000 - 512 - $(shell stat -c%s kernel.bin))) >> os.disk 2> /dev/null
	@cat $(TEST_ELF) >> os.disk
	@dd if=/dev/zero bs=1 count=$$((($(TEST_ELF_SIZE)/512 + 1) * 512 - $(TEST_ELF_SIZE))) >> os.disk 2> /dev/null

gdb: kernel.bin boot1.bin user kernel.asm
	@cat boot1.bin > os.disk
	@cat kernel.bin >> os.disk
	@dd if=/dev/zero bs=1 count=$$((0x20000 - 512 - $(shell stat -c%s kernel.bin))) >> os.disk 2> /dev/null
	@cat $(TEST_ELF) >> os.disk
	@dd if=/dev/zero bs=1 count=$$((($(TEST_ELF_SIZE)/512 + 1) * 512 - $(TEST_ELF_SIZE))) >> os.disk 2> /dev/null
	@$(QEMU) $(QEMU_FLAGS) -hda os.disk -S -gdb tcp::1234 -serial stdio -vga std

boot1.bin: $(BOOT1_SRCS)
	@echo "Compiling bootloader 1"
	@$(AS) $(BOOT1_FLAGS) $< -o $@

kernel.obj:	$(KERNEL_ASM) $(KERNEL_C)
	@mkdir -p $(OBJDIR) 2>/dev/null
	@$(foreach var, $(KERNEL_ASM), $(AS) $(ASKERNFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.asm=.o));)
	@echo 'Compiling kernel'
	@$(foreach var, $(KERNEL_C), $(CC) $(CFLAGS) $(KERNEL_CFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.c=.o));)

kernel.asm: kernel.obj
	@$(LD) $(DLDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) $(GCC_LIB) -o $(OBJDIR)$<
	@echo "Dumping kernel"
	@objdump -d -M intel $(OBJDIR)$< >$(OBJDIR)$@
	@echo "target remote :1234\nsymbol-file $(OBJDIR)$^" > .gdbinit

kernel.bin: kernel.obj
	@echo "Linking kernel"
	@$(LD) $(LDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) $(GCC_LIB) -o $@
	
usrlib:
	@$(foreach var, $(LIB_C), $(CC) -c $(CFLAGS) $(USER_CFLAGS) $(var) -o $(notdir $(var:.c=.o));)
	ar r $(LIB_OUT).a $(LIB_OBJ)

user: usrlib
	@echo "Compiling user test"
	$(CC) $(CFLAGS) $(USER_CFLAGS) $(TEST_SRC) $(LIB_OUT).a -o $(TEST_ELF)

clean:
	@rm -r -f $(OBJDIR)
	@cd test && find . ! -name '*.c' -type f -exec rm -f {} +
	@rm -f *.o *.a *~ *.disk *.bin *.elf .gdbinit

run: all
	@echo ------------------------------------------------------
	@${QEMU} $(QEMU_FLAGS) -drive file=os.disk,format=raw -serial stdio
