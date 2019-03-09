default: main

main.o: main.c $(HEADERS)
	gcc -c main.c -o main.o

main: main.o
	gcc main.o -o pdp11

clean:
	-rm -f main.o
	-rm -f pdp11
