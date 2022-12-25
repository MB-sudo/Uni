menu_map: menu_map.o
	gcc -m32 -g -Wall menu_map.o -o menu_map

menu_map.o: menu_map.c 
	gcc -g -m32 -Wall -c menu_map.c -o menu_map.o

.PHONY: clean
clean: 
	rm -f *.o