all:task3

task3: task3.o 
	gcc -m32 -no-pie -g -Wall task3.o -o task3

task3.o: task3.s
	nasm -f elf -w+all  task3.s -o task3.o

.PHONY: clean
clean: 
	rm -f *.o
