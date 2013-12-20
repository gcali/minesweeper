CC=gcc -std=gnu89 -pedantic -Wall -Wno-unused-but-set-variable
CFLAGS=-g
LDFLAGS=-lncurses

SDIR=src
ODIR=obj
VPATH:=$(SDIR)

DEBUG_NAME=debug.out
DEBUG_EXCLUSIVE_OBJECTS := $(addprefix $(ODIR)/, debug.o)
DEBUG_OBJECTS := $(DEBUG_EXCLUSIVE_OBJECTS)

NAME=campo_ex
OBJECTS := $(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(wildcard $(SDIR)/*.c))
OBJECTS := $(filter-out $(DEBUG_EXCLUSIVE_OBJECTS),$(OBJECTS))


$(NAME): $(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS) $(LDFLAGS)

$(ODIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(ODIR)/%o.: error.h

$(ODIR)/main.o: interface.h grid.h
$(ODIR)/debug.o: interface.h
#$(ODIR)/error.o:
$(ODIR)/utilities.o: utilities.h
$(ODIR)/interface.o: interface.h utilities.h
$(ODIR)/grid.o: grid.h

.PHONY: clean
clean:
	@-rm -f $(OBJECTS) $(NAME) $(DEBUG_NAME) $(DEBUG_OBJECTS)

.PHONY: debug
debug: $(DEBUG_OBJECTS)
	$(CC) -o $(DEBUG_NAME) $(DEBUG_OBJECTS) $(LDFLAGS)
