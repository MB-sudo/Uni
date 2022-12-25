task2b: task2b.o
	gcc -m32 -g -Wall task2b.o -o task2b

task2b.o: task2b.c 
	gcc -g -m32 -Wall -c task2b.c -o task2b.o

.PHONY: clean
clean: 
	rm -f *.o