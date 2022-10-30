# Author: Tobias Gruber, 11912367
# Program: supervisor?

CC = gcc # c compiler
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L # definitions
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS) # compiler flags
LDFLAGS = -lrt -pthread # linker flags

.PHONY: all clean
all: supervisor generator

supervisor: supervisor.o shm.o graph.o misc.o
	$(CC) -o $@ $^ $(LDFLAGS)

generator: generator.o shm.o graph.o misc.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

supervisor.o: supervisor.c shm.h
generator.o: generator.c shm.h
shm.o: shm.c shm.h graph.h
graph.o: graph.c graph.h misc.h
misc.o: misc.c misc.h

clean:
	rm -rf *.o supervisor generator