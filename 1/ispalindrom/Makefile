CC = gcc -std=c99 -g
DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

.PHONY: all clean
all: ispalindrom

ispalindrom: ispalindrom.o strfun.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

ispalindrom.o: ispalindrom.c ispalindrom.h strfun.h
strfun.o: strfun.c strfun.h

clean:
	rm -rf *.o ispalindrom