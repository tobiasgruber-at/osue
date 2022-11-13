# author: Tobias Gruber, 11912367
# program: ispalindrom

CC = gcc # c compiler
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L # definitions
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS) # compiler flags
LDFLAGS = # linker flags

.PHONY: all clean
all: ispalindrom

ispalindrom: ispalindrom.o strfun.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

ispalindrom.o: ispalindrom.c strfun.h
strfun.o: strfun.c strfun.h

clean:
	rm -rf *.o ispalindrom