CC = gcc
LD = @ld
AS = nasm
CFLAGS = -m32 -c -I../proj -std=gnu99 -nostdinc
LDFLAGS = -melf_i386 -Ttext 0x1000 --oformat binary -e kern_start
DLDFLAGS = -melf_i386 -Ttext 0x1000 -e kern_start
ASBOOTFLAGS = -fbin
ASKERNFLAGS = -felf32
OBJDIR = obj/
TESTDIR = test/
BOOT_SRCS = $(wildcard boot/*)
KERNEL_ASM = $(wildcard kernel/*.asm)
KERNEL_C = $(wildcard kernel/*.c)
TEST_C = $(TESTDIR)test.c
KERNEL_OBJ1 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_ASM:.asm=.o)))
KERNEL_OBJ2 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_C:.c=.o)))
TEST_OBJ = $(TESTDIR)test.o
KERNEL_DUMP = kernel.elf


all: boot.bin kernel.bin
	@cat $^ > os.disk

gdb: boot.bin kernel.bin
	@cat $^ > os.disk
	@qemu-system-i386 -fda os.disk -S -gdb tcp::1234 -serial stdio

gdb1: boot.bin kernel.bin
	@cat $^ > os.disk
	@qemu-system-i386 -fda os.disk -S -serial stdio

boot.bin: $(BOOT_SRCS)
	@echo "Compiling bootloader"
	$(AS) $(ASBOOTFLAGS) $< -o $@

kernel.obj:	$(KERNEL_ASM) $(KERNEL_C)
	@mkdir -p $(OBJDIR) 2>/dev/null
	@echo $(KERNEL_OBJ1)
	@$(foreach var, $(KERNEL_ASM), $(AS) $(ASKERNFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.asm=.o));)
	@echo 'Compiling kernel'
	@$(foreach var, $(KERNEL_C), $(CC) $(CFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.c=.o));)
	@echo 'Compiling test' 
	@$(CC) $(CFLAGS) $(TEST_C) -o $(TEST_OBJ)

kernel.asm: kernel.obj
	$(LD) $(DLDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) $(TEST_OBJ) -o $(OBJDIR)$<
	@echo "Dumping kernel"
	@objdump -d -M intel $(OBJDIR)$< >$(OBJDIR)$@
	@echo "target remote :1234\nsymbol-file $(OBJDIR)$^" > .gdbinit

kernel.bin: kernel.obj kernel.asm
	@echo "Linking kernel"
	$(LD) $(LDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) $(TEST_OBJ) -o $@ 2>/dev/null

clean:
	@rm -r -f $(OBJDIR)
	@cd test && rm -f *.o *.bin *.asm
	@rm -f *.o *~ *.disk *.bin *.elf .gdbinit

run: all
	@echo ------------------------------------------------------
	@qemu-system-i386 -fda os.disk -serial stdio -d out_asm,in_asm 2> dump.asm
