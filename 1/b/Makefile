# Author: Tobias Gruber, 11912367
# Program: supervisor?

CC = gcc # c compiler
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L # definitions
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS) # compiler flags
LDFLAGS = -lrt -pthread # linker flags

.PHONY: all clean valg
all: supervisor generator

supervisor: supervisor.o
	$(CC) -o $@ $^ $(LDFLAGS)

generator: generator.o graph.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

generator.o: generator.c graph.h
graph.o: graph.c graph.h

clean:
	rm -rf *.o supervisor generator