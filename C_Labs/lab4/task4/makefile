all:task4

task4: task4.o 
	gcc -m32 -no-pie -g -Wall task4.o -o task4

task4.o: task4.s
	nasm -f elf -w+all task4.s -o task4.o

.PHONY: clean
clean: 
	rm -f *.o
