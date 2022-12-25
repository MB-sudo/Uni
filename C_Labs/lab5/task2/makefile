all: start.o main.o
	ld -m elf_i386 start.o main.o -o start

start.o: start.s
	nasm -f elf32 start.s -o start.o

main.o: main.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o


.PHONY: clean
clean:
	rm -f *.o
