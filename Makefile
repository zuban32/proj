CC = gcc
LD = ld
AS = nasm
CFLAGS = -m32 -c -std=gnu99
LDFLAGS = -melf_i386 -Ttext 0x1000 --oformat binary
ASBOOTFLAGS = -fbin
ASKERNFLAGS = -felf32
OBJDIR = obj/
BOOT_SRCS = $(wildcard boot/*)
KERNEL_ASM = $(wildcard kernel/*.asm)
KERNEL_C = $(wildcard kernel/*.c)
KERNEL_OBJ1 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_ASM:.asm=.o)))
KERNEL_OBJ2 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_C:.c=.o)))


all: boot.bin kernel.bin
	cat $^ > os.disk
	qemu-system-i386 -fda os.disk -serial stdio

gdb: boot.bin kernel.bin
	cat $^ > os.disk
	qemu-system-i386 -fda os.disk -S -gdb tcp::1234 -serial stdio

boot.bin: $(BOOT_SRCS)
	$(AS) $(ASBOOTFLAGS) $< -o $@

kernel.bin:	$(KERNEL_ASM) $(KERNEL_C)
	echo $(KERNEL_OBJ1)
	echo $(KERNEL_OBJ2)
	$(AS) $(KERNEL_ASM) $(ASKERNFLAGS) -o $(KERNEL_OBJ1)
	$(foreach var, $(KERNEL_C), $(CC) $(CFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.c=.o);))
	$(LD) $(LDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) -o $@

clean:
	cd $(OBJDIR) && rm -f *.o *~ *.disk *.bin
	rm -f *.o *~ *.disk *.bin
