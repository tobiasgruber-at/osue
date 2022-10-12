all: ispalindrom

ispalindrom: ispalindrom.o strfun.o
	gcc -std=c99 -g -o ispalindrom ispalindrom.o strfun.o

ispalindrom.o: ispalindrom.c
	gcc -std=c99 -g -c -o ispalindrom.o ispalindrom.c

strfun.o: strfun.c
	gcc -std=c99 -g -c -o strfun.o strfun.c

clean:
	rm -f ispalindrom ispalindrom.o strfun.o