CC = gcc
LD = ld
AS = nasm
CFLAGS = -m32 -c
LDFLAGS = -melf_i386 -Ttext 0x1000 --oformat binary
ASBOOTFLAGS = -fbin
ASKERNFLAGS = -felf32
BOOT_SRCS = $(wildcard boot/*)
KERNEL_ASM = $(wildcard kernel/*.asm)
KERNEL_C = $(wildcard kernel/*.c)
KERNEL_OBJ1 = $(KERNEL_ASM:.asm=.o)
KERNEL_OBJ2 = $(KERNEL_C:.c=.o)


all: boot.bin kernel.bin
	cat $^ > os.disk
	qemu-system-i386 -fda os.disk

gdb: boot.bin kernel.bin
	cat $^ > os.disk
	gdb "qemu-system-i386 -fda os.disk -S -gdb tcp::1234"

boot.bin: $(BOOT_SRCS)
	$(AS) $(ASBOOTFLAGS) $< -o $@

kernel.bin:	$(KERNEL_ASM) $(KERNEL_C)
	$(AS) $(KERNEL_ASM) $(ASKERNFLAGS) -o $(KERNEL_OBJ1)
	$(foreach var, $(KERNEL_C), $(CC) $(CFLAGS) $(var) -o $(var:.c=.o);)
	$(LD) $(LDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) -o $@

clean:
	cd kernel && rm -f *.o *~ *.disk *.bin
	cd boot && rm -f *.o *~ *.disk *.bin
	rm -f *.o *~ *.disk *.bin
