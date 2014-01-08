CC=gcc -std=gnu89 -pedantic -Wall -Wno-unused-but-set-variable
CFLAGS=-g
LDFLAGS=-lncurses

SDIR=src
ODIR=obj
VPATH:=$(SDIR)

DEBUG_NAME=debug.out
DEBUG_EXCLUSIVE_OBJECTS := debug.o
DEBUG_EXCLUSIVE_OBJECTS := $(addprefix $(ODIR)/, $(DEBUG_EXCLUSIVE_OBJECTS))
DEBUG_OBJECTS := misc.o highscore.o utilities.o error.o interface.o grid.o timer.o
DEBUG_OBJECTS := $(addprefix $(ODIR)/, $(DEBUG_OBJECTS))
DEBUG_OBJECTS := $(DEBUG_EXCLUSIVE_OBJECTS) $(DEBUG_OBJECTS)

NAME=campo_ex
OBJECTS := $(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(wildcard $(SDIR)/*.c))
OBJECTS := $(filter-out $(DEBUG_EXCLUSIVE_OBJECTS),$(OBJECTS))

INSTALL_PATH=/usr/local/bin/ 

$(NAME): $(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS) $(LDFLAGS)

$(ODIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@


$(ODIR)/%.o: error.h

$(ODIR)/main.o: interface.h grid.h
$(ODIR)/debug.o: interface.h
#$(ODIR)/error.o:
$(ODIR)/utilities.o: utilities.h
$(ODIR)/interface.o: interface.h utilities.h highscore.h
$(ODIR)/grid.o: grid.h
$(ODIR)/highscore.o: highscore.h misc.h utilities.h
$(ODIR)/misc.o: misc.h
$(ODIR)/timer.o: timer.h

.PHONY: install
install: $(NAME)
	cp $(NAME) $(INSTALL_PATH) 

.PHONY: clean
clean:
	@-rm -f $(OBJECTS) $(DEBUG_NAME) $(DEBUG_NAME) $(DEBUG_EXCLUSIVE_OBJECTS)

.PHONY: debug
debug: $(DEBUG_OBJECTS)
	$(CC) -o $(DEBUG_NAME) $(DEBUG_OBJECTS) $(LDFLAGS)
