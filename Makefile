CC = g++
LD = ld
AS = nasm

QEMU = /home/zuban32/qemu-install/bin/qemu-system-i386
QEMU_FLAGS = -m 4G -d guest_errors
CFLAGS = -m32 -std=c++0x -Os -ffreestanding -fno-stack-protector\
-pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Werror -fno-exceptions\
 -fno-rtti
CBOOT_FLAGS = -m32 -std=c99 -c -Os -fno-builtin -ffreestanding -I../proj\
-pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Werror -nostdlib

CRTI_OBJ=$(shell $(CC) $(CFLAGS) -print-file-name=crti.o)
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ=$(shell $(CC) $(CFLAGS) -print-file-name=crtn.o)

KERNEL_CFLAGS = -c -I../proj
USER_CFLAGS =  -nostdlib -I../proj/lib -Wl,-eusermain
LDFLAGS = -melf_i386 -e kern_start -Ttext=0x100000 -nostdlib
DLDFLAGS = -melf_i386 -e kern_start -Ttext=0x100000 -nostdlib
GCC_LIB = $(shell $(CC) $(CFLAGS) --print-libgcc-file-name)
ASBOOTFLAGS = -D KERNEL_SIZE=$(shell stat -c%s kernel.bin) -fbin
ASKERNFLAGS = -felf32
OBJDIR = obj/
TESTDIR = test/
BOOT1_SRCS = boot/boot.asm boot/gdt.asm boot/switch_pm.asm
BOOT2_SRCS = boot/boot2.c
KERNEL_ASM = kernel/1st_entry.asm kernel/isr_entry.asm#$(wildcard kernel/*.asm)
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

all: kernel.bin boot1.bin boot2.bin user
	@cat boot1.bin boot2.bin > os.disk
	@dd if=/dev/zero bs=1 count=$$((0x200 - $(shell stat -c%s boot2.bin))) >> os.disk 2> /dev/null
	@cat kernel.bin >> os.disk
	@dd if=/dev/zero bs=1 count=$$((0x10000 - 0x400 - $(shell stat -c%s kernel.bin))) >> os.disk 2> /dev/null
	@cat $(TEST_ELF) >> os.disk
	@dd if=/dev/zero bs=1 count=$$((($(TEST_ELF_SIZE)/512 + 1) * 512 - $(TEST_ELF_SIZE))) >> os.disk 2> /dev/null
	@echo "Build finished"

gdb: kernel.bin boot1.bin boot2.bin user kernel.asm
	@cat boot1.bin boot2.bin > os.disk
	@dd if=/dev/zero bs=1 count=$$((0x200 - $(shell stat -c%s boot2.bin))) >> os.disk 2> /dev/null
	@cat kernel.bin >> os.disk
	@dd if=/dev/zero bs=1 count=$$((0x10000 - 0x400 - $(shell stat -c%s kernel.bin))) >> os.disk 2> /dev/null
	@cat $(TEST_ELF) >> os.disk
	@dd if=/dev/zero bs=1 count=$$((($(TEST_ELF_SIZE)/512 + 1) * 512 - $(TEST_ELF_SIZE))) >> os.disk 2> /dev/null
	@$(QEMU) $(QEMU_FLAGS) -hda os.disk -S -gdb tcp::1234 -serial stdio -vga std
	@echo "Debug build finished"

boot1.bin: $(BOOT1_SRCS)
	@echo "Compiling bootloader1"
	@$(AS) $(ASBOOTFLAGS) $< -o $@
	
boot2.bin:
	@echo "Compiling bootloader2"
	@gcc $(CBOOT_FLAGS) $(BOOT2_SRCS)
	@$(LD) -nostdlib -melf_i386 -e boot2_main --oformat binary boot2.o -o $@
#	@objcopy -O binary boot2.out $@

kernel.obj:	$(KERNEL_ASM) $(KERNEL_C)
	@mkdir -p $(OBJDIR) 2>/dev/null
	@$(foreach var, $(KERNEL_ASM), $(AS) $(ASKERNFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.asm=.o));)
	@echo 'Compiling kernel'
	@$(foreach var, $(KERNEL_C), $(CC) $(CFLAGS) $(KERNEL_CFLAGS) $(var) -o $(OBJDIR)$(notdir $(var:.cpp=.o));)

kernel.asm: kernel.obj
	@$(LD) $(DLDFLAGS) $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(KERNEL_OBJ1) $(KERNEL_OBJ2) $(CRTEND_OBJ) $(CRTN_OBJ) $(GCC_LIB)  -o $(OBJDIR)$<
	@echo "Dumping kernel"
	@objdump -D -M intel $(OBJDIR)$< >$(OBJDIR)$@
	@echo "target remote :1234\nsymbol-file $(OBJDIR)$^" > .gdbinit

kernel.bin: kernel.obj
	@echo "Linking kernel"
	@$(LD) $(LDFLAGS) $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(KERNEL_OBJ1) $(KERNEL_OBJ2) $(CRTEND_OBJ) $(CRTN_OBJ) $(GCC_LIB)  -o $@
	
kernel.bin1:
	@echo "Compiling & linking kernel"
	@$(CC) $(CFLAGS)
	
usrlib:
	@$(foreach var, $(LIB_C), $(CC) -c $(CFLAGS) $(USER_CFLAGS) $(var) -o $(notdir $(var:.cpp=.o));)
	@ar r $(LIB_OUT).a $(LIB_OBJ)

user: usrlib
	@echo "Compiling user test"
	@$(CC) $(CFLAGS) $(USER_CFLAGS) $(TEST_SRC) $(LIB_OUT).a -o $(TEST_ELF)

clean:
	@rm -r -f $(OBJDIR)
	@cd test && find . ! -name '*.cpp' -type f -exec rm -f {} +
	@rm -f *.o *.a *~ *.disk *.bin *.elf .gdbinit

run: all
	@echo ------------------------------------------------------
	@${QEMU} $(QEMU_FLAGS) -hda os.disk -serial stdio
