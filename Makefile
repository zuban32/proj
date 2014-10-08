all:
	gcc -ffreestanding -c kernel.c -o kernel.o
	ld -o kernel.bin -Ttext 0x1000 kernel.o --oformat binary
	nasm boot.asm -fbin -o boot.bin
	cat boot.bin kernel.bin > os.disk
	qemu -fda os.disk --vga cirrus
	
clean:
	rm -f *.o *~ *.disk *.bin
