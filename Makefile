CC=gcc -std=gnu89 -pedantic -Wall -Wno-unused-but-set-variable
CFLAGS=-c -g
LDFLAGS=-lncurses

NAME=campo_ex
OBJECTS=error.o interface.o utilities.o main.o grid.o

DEBUG_NAME=debug
DEBUG_OBJECTS=error.o interface.o utilities.o debug.o

$(NAME): $(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS) $(LDFLAGS)

main.o: main.c interface.h grid.h
	$(CC) $(CFLAGS) main.c

debug.o: debug.c interface.h
	$(CC) $(CFLAGS) debug.c

error.o: error.c error.h
	$(CC) $(CFLAGS) error.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) utilities.c

interface.o: interface.c interface.h error.h utilities.h
	$(CC) $(CFLAGS) interface.c

grid.o: grid.c grid.h error.h
	$(CC) $(CFLAGS) grid.c

.PHONY: clean
clean:
	@-rm -f $(OBJECTS) $(NAME) $(DEBUG_NAME) $(DEBUG_OBJECTS)

.PHONY: debug
debug: $(DEBUG_OBJECTS)
	$(CC) -o $(DEBUG_NAME) $(DEBUG_OBJECTS) $(LDFLAGS)
