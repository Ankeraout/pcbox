CC := gcc -c
CFLAGS := -W -Wall -Wextra -pedantic -std=c11 -g -O0 -Isrc $(shell sdl2-config --cflags)
LD := gcc
LDFLAGS := $(shell sdl2-config --libs)

SRCDIR := src
BINDIR := bin

SOURCES := 	$(SRCDIR)/main.c \
			$(SRCDIR)/core/bios.c \
			$(SRCDIR)/core/cpu.c \
			$(SRCDIR)/core/mmu.c \
			$(SRCDIR)/core/vga.c \
			$(SRCDIR)/frontend/sdl2.c

OBJECTS := $(SOURCES:%.c=%.o)

EXEC := $(BINDIR)/pcbox

all: $(EXEC)

$(EXEC): $(OBJECTS) bin
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

bin:
	mkdir bin

clean:
	rm -rf $(OBJECTS) bin

.PHONY: all clean
