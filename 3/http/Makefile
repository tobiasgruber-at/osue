# author: Tobias Gruber, 11912367
# program: client

CC = gcc # c compiler
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L # definitions
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS) # compiler flags
LDFLAGS = # linker flags

.PHONY: all clean
all: client

client: client.o misc.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

client.o: client.c misc.h
misc.o: misc.c misc.h

clean:
	rm -rf *.o client