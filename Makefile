# automatically generated makefile by ./scripts/create-make

CFLAGS=-Wall -g -O2 -std=gnu99 -I./include -L./lib

LIBS=-larithmetic


all: test/investigate test/test-arithmetic

clean:
	rm -f lib/libarithmetic.a  lib/arithmetic.o test/investigate test/test-arithmetic

lib/arithmetic.o: lib/arithmetic.c
	gcc -c $(CFLAGS) lib/arithmetic.c -o lib/arithmetic.o

test/investigate: test/investigate.c lib/libarithmetic.a include/arithmetic.h
	gcc $(CFLAGS) test/investigate.c $(LIBS) -L/usr/local/lib -o test/investigate

test/test-arithmetic: test/test-arithmetic.c lib/libarithmetic.a include/arithmetic.h
	gcc $(CFLAGS) test/test-arithmetic.c $(LIBS) -L/usr/local/lib -o test/test-arithmetic



lib/libarithmetic.a: lib/arithmetic.o
	ar crs lib/libarithmetic.a lib/arithmetic.o
