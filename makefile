CFLAGS=-g -std=c99 -Wall -Wconversion -Wno-sign-conversion
OBJ= hash.c uniq.c strutil.c lista.c
CC=gcc
EXEC=pruebas

all:
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

valgrind:
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./pruebas

clean:
	rm -f $(EXEC)
