virusDetector: virusDetector.o
	gcc -m32 -g -Wall virusDetector.o -o virusDetector

virusDetector.o: virusDetector.c 
	gcc -g -m32 -Wall -c virusDetector.c -o virusDetector.o

.PHONY: clean
clean: 
	rm -f *.o