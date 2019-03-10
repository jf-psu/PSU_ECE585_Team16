default: main

main.o: main.c $(HEADERS)
	gcc -c main.c -o main.o

instructions.o: instructions.c $(HEADERS)
	gcc -c instructions.c -o instructions.o

main: main.o instructions.o 
	gcc -o pdp11 main.o instructions.o

clean:
	-rm -f main.o
	-rm -f instructions.o
	-rm -f pdp11
