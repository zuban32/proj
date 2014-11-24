CC = gcc
LD = @ld
AS = @nasm
CFLAGS = -m32 -c -std=gnu99
LDFLAGS = -melf_i386 -Ttext 0x1000 --oformat binary
DLDFLAGS = -melf_i386 -Ttext 0x1000
ASBOOTFLAGS = -fbin
ASKERNFLAGS = -felf32
OBJDIR = obj/
BOOT_SRCS = $(wildcard boot/*)
KERNEL_ASM = $(wildcard kernel/*.asm)
KERNEL_C = $(wildcard kernel/*.c)
KERNEL_OBJ1 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_ASM:.asm=.o)))
KERNEL_OBJ2 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_C:.c=.o)))
KERNEL_DUMP = kernel.elf


all: boot.bin kernel.bin
	cat $^ > os.disk
	qemu-system-i386 -fda os.disk -serial stdio

gdb: boot.bin kernel.bin
	cat $^ > os.disk
	qemu-system-i386 -fda os.disk -S -gdb tcp::1234 -serial stdio

boot.bin: $(BOOT_SRCS)
	@echo $@
	$(AS) $(ASBOOTFLAGS) $< -o $@

kernel.obj:	$(KERNEL_ASM) $(KERNEL_C)
	@echo $@
	@mkdir -p $(OBJDIR) 2>/dev/null
	$(AS) $(KERNEL_ASM) $(ASKERNFLAGS) -o $(KERNEL_OBJ1)
	@echo 'Compiling kernel'
	$(foreach var, $(KERNEL_C), $(CC) $(CFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.c=.o));)

kernel.asm: kernel.obj
	@echo $@
	$(LD) $(DLDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) -o $(OBJDIR)$<
	objdump -d -M intel $(OBJDIR)$< >$(OBJDIR)$@
	@echo "target remote :1234\nsymbol-file $(OBJDIR)$^" > .gdbinit

kernel.bin: kernel.obj kernel.asm
	@echo $@
	$(LD) $(LDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) -o $@	

clean:
	cd $(OBJDIR) && rm -f *.o *~ *.disk *.bin *.elf
	rm -r $(OBJDIR)
	rm -f *.o *~ *.disk *.bin *.elf .gdbinit
