all:task1

task1: task1.o 
	gcc -m32 -no-pie -g -Wall task1.o -o task1

task1.o: task1.s
	nasm -f elf -w+all task1.s -o task1.o


clean: 
	rm -f *.o
