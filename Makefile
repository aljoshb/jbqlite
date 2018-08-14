CC = gcc
CFLAGS  = -g -Wall

default: jbqlite

# To create the executable file jbqlite, object files:
# main.o and input.o are needed.
jbqlite: main.o input.o
	$(CC) $(CFLAGS) -o jbqlite main.o input.o

# To create the object file main.o, source file:
# src/main.c is needed
main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c

# To create the object file input.o, source file:
# lib/input.c is needed
input.o: lib/input.c
	$(CC) $(CFLAGS) -c lib/input.c

clean:
	$(RM) jbqlite *.o *~