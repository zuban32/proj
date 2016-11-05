CC = g++
LD = ld
AS = nasm
QEMU = /home/zuban32/qemu-install/bin/qemu-system-i386
QEMU_FLAGS = -m 4G
CFLAGS = -m32 -std=c++0x -fno-builtin -ffreestanding\
-pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Werror
KERNEL_CFLAGS = -c -I../proj
USER_CFLAGS =  -nostdlib -I../proj/lib -Wl,-eusermain
LDFLAGS = -melf_i386 -nostdlib -Ttext 0x1000 --oformat binary -e kern_start
DLDFLAGS = -melf_i386 -nostdlib -Ttext 0x1000 -e kern_start
GCC_LIB = $(shell $(CC) $(CFLAGS) --print-libgcc-file-name)
ASBOOTFLAGS = -D KERNEL_SIZE=$(shell stat -c%s kernel.bin) -fbin
ASKERNFLAGS = -felf32
OBJDIR = obj/
TESTDIR = test/
BOOT_SRCS = $(wildcard boot/*)
KERNEL_ASM = $(wildcard kernel/*.asm)
KERNEL_C = $(wildcard kernel/*.cpp kernel/hw/*.cpp)
LIB_C = $(wildcard lib/*.cpp)
LIB_OBJ = $(notdir $(LIB_C:.cpp=.o))
LIB_OUT = usrlib
KERNEL_OBJ1 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_ASM:.asm=.o)))
KERNEL_OBJ2 = $(addprefix $(OBJDIR), $(notdir $(KERNEL_C:.cpp=.o)))
KERNEL_DUMP = kernel.elf
TEST_SRC = test/hello.cpp
TEST_ELF = test/hello
TEST_ELF_SIZE = $(shell stat -c%s $(TEST_ELF))

all: kernel.bin boot.bin user
	cat boot.bin kernel.bin > os.disk
	dd if=/dev/zero bs=1 count=$$((0x6000 - 512 - $(shell stat -c%s kernel.bin))) >> os.disk 2> /dev/null
	cat $(TEST_ELF) >> os.disk
	dd if=/dev/zero bs=1 count=$$((($(TEST_ELF_SIZE)/512 + 1) * 512 - $(TEST_ELF_SIZE))) >> os.disk 2> /dev/null

gdb: kernel.bin boot.bin user kernel.asm
	@cat boot.bin kernel.bin > os.disk
	@dd if=/dev/zero bs=1 count=$$((0x6000 - 512 - $(shell stat -c%s kernel.bin))) >> os.disk 2> /dev/null
	@cat $(TEST_ELF) >> os.disk
	@dd if=/dev/zero bs=1 count=$$((($(TEST_ELF_SIZE)/512 + 1) * 512 - $(TEST_ELF_SIZE))) >> os.disk 2> /dev/null
	@$(QEMU) $(QEMU_FLAGS) -hda os.disk -S -gdb tcp::1234 -serial stdio -vga std

boot.bin: $(BOOT_SRCS)
	@echo "Compiling bootloader"
	@$(AS) $(ASBOOTFLAGS) $< -o $@

kernel.obj:	$(KERNEL_ASM) $(KERNEL_C)
	@mkdir -p $(OBJDIR) 2>/dev/null
	@$(foreach var, $(KERNEL_ASM), $(AS) $(ASKERNFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.asm=.o));)
	@echo 'Compiling kernel'
	@$(foreach var, $(KERNEL_C), $(CC) $(CFLAGS) $(KERNEL_CFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.cpp=.o));)

kernel.asm: kernel.obj
	@$(LD) $(DLDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) $(GCC_LIB) -o $(OBJDIR)$<
	@echo "Dumping kernel"
	@objdump -d -M intel $(OBJDIR)$< >$(OBJDIR)$@
	@echo "target remote :1234\nsymbol-file $(OBJDIR)$^" > .gdbinit

kernel.bin: kernel.obj
	@echo "Linking kernel"
	@$(LD) $(LDFLAGS) $(KERNEL_OBJ1) $(KERNEL_OBJ2) $(GCC_LIB) -o $@
	
usrlib:
	$(foreach var, $(LIB_C), $(CC) -c $(CFLAGS) $(USER_CFLAGS) $(var) -o $(notdir $(var:.cpp=.o));)
	ar r $(LIB_OUT).a $(LIB_OBJ)

user: usrlib
	@echo "Compiling user test"
	$(CC) $(CFLAGS) $(USER_CFLAGS) $(TEST_SRC) $(LIB_OUT).a -o $(TEST_ELF)

clean:
	@rm -r -f $(OBJDIR)
	@cd test && find . ! -name '*.cpp' -type f -exec rm -f {} +
	@rm -f *.o *.a *~ *.disk *.bin *.elf .gdbinit

run: all
	@echo ------------------------------------------------------
	@${QEMU} $(QEMU_FLAGS) -hda os.disk -serial stdio
