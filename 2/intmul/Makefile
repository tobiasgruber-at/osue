# author: Tobias Gruber, 11912367
# program: supervisor, generator

CC = gcc # c compiler
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L # definitions
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS) # compiler flags
LDFLAGS = -lm # linker flags

.PHONY: all clean
all: intmul

intmul: intmul.o hex.o misc.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

intmul.o: intmul.c hex.h
hex.o: hex.c hex.h misc.h
misc.o: misc.c misc.h

clean:
	rm -rf *.o intmul