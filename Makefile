CC = gcc
CFLAGS  = -g -Wall

default: jbqlite

# To create the executable file jbqlite, object files:
# main.o, input.o and welcome.o are needed.
jbqlite: main.o input.o interface.o statements.o commands.o table.o pager.o
	$(CC) $(CFLAGS) -o jbqlite main.o input.o interface.o statements.o commands.o table.o pager.o

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

# To create the object file statements.o, source file:
# lib/statements.c is needed
statements.o: lib/statements.c
	$(CC) $(CFLAGS) -c lib/statements.c

# To create the object file commands.o, source file:
# lib/commands.c is needed
commands.o: lib/commands.c
	$(CC) $(CFLAGS) -c lib/commands.c

table.o: lib/table.c
	$(CC) $(CFLAGS) -c lib/table.c

pager.o: lib/pager.c
	$(CC) $(CFLAGS) -c lib/pager.c

clean:
	$(RM) jbqlite *.o *~