all: start.o
	ld -m elf_i386 start.o -o start

start.o: start.s
	nasm -f elf32 -w+all start.s -o start.o

.PHONY: clean
clean:
	rm -f *.o
