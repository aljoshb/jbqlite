CC = gcc
CFLAGS  = -g -Wall

default: jbqlite

# To create the executable file jbqlite, object files:
# main.o, input.o and welcome.o are needed.
jbqlite: main.o input.o interface.o statements.o
	$(CC) $(CFLAGS) -o jbqlite main.o input.o interface.o statements.o

# To create the object file main.o, source file:
# src/main.c is needed
main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c

# To create the object file input.o, source file:
# lib/input.c is needed
input.o: lib/input.c
	$(CC) $(CFLAGS) -c lib/input.c

# To create the object file welcome.o, source file:
# lib/welcome.c is needed
interface.o: lib/interface.c
	$(CC) $(CFLAGS) -c lib/interface.c

statements.o: lib/statements.c
	$(CC) $(CFLAGS) -c lib/statements.c

clean:
	$(RM) jbqlite *.o *~