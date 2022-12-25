
all: task4 task3 task2 task1

LineParser.o: LineParser.c
	gcc -g -m32 -Wall -c LineParser.c -o LineParser.o

task4: task4.o LineParser.o
	gcc -m32 -g -Wall task4.o LineParser.o -o  task4
	
task3: task3.o LineParser.o
	gcc -m32 -g -Wall task3.o LineParser.o -o  task3

task2: task2.o
	gcc -m32 -g -Wall task2.o -o  task2

task1: task1.o LineParser.o
	gcc -m32 -g -Wall task1.o LineParser.o -o  task1

task4.o: task4.c LineParser.h
	gcc -g -m32 -Wall -c task4.c -o task4.o
	
task3.o: task3.c LineParser.h
	gcc -g -m32 -Wall -c task3.c -o task3.o
	
task2.o: task2.c
	gcc -g -m32 -Wall -c task2.c -o task2.o
	
task1.o: task1.c LineParser.h
	gcc -g -m32 -Wall -c task1.c -o task1.o

.PHONY: clean
clean: 
	rm -f *.o 
