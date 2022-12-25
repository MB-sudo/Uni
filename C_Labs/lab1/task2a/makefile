encoder: encoder.o
	gcc -m32 -g -Wall encoder.o -o encoder

encoder.o: encoder.c 
	gcc -g -m32 -Wall -c encoder.c -o encoder.o

.PHONY: clean
clean: 
	rm -f *.o