all:task2

task2: task2.o 
	gcc -m32 -no-pie -g -Wall task2.o -o task2

task2.o: task2.s
	nasm -f elf -w+all task2.s -o task2.o

.PHONY: clean
clean: 
	rm -f *.o
