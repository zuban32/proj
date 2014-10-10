all:
	gcc -m32 -c kernel.c -o kernel.o
	ld -melf_i386 -o kernel.bin -e main -Ttext 0x1000 kernel.o --oformat binary
	nasm boot.asm -fbin -o boot.bin
	cat boot.bin kernel.bin > os.disk
	qemu-system-i386 -fda os.disk
	
clean:
	rm -f *.o *~ *.disk *.bin
