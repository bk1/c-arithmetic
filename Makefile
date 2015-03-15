# automatically generated makefile by ./scripts/create-make

CFLAGS=-Wall -g -O2 -std=gnu99 -I./include -L./lib

LIBS=-larithmetic


all: test/test-arithmetic-cunit test/test-arithmetic-ruby test/investigate

clean:
	rm -f lib/libarithmetic.a  lib/arithmetic.o test/test-arithmetic-cunit test/test-arithmetic-ruby test/investigate

lib/arithmetic.o: lib/arithmetic.c
	gcc -c $(CFLAGS) lib/arithmetic.c -o lib/arithmetic.o

test/test-arithmetic-cunit: test/test-arithmetic-cunit.c lib/libarithmetic.a include/arithmetic.h
	gcc $(CFLAGS) test/test-arithmetic-cunit.c $(LIBS) -L/usr/local/lib -lcunit -o test/test-arithmetic-cunit

test/test-arithmetic-ruby: test/test-arithmetic-ruby.c lib/libarithmetic.a include/arithmetic.h
	gcc $(CFLAGS) test/test-arithmetic-ruby.c $(LIBS) -L/usr/local/lib -lcunit -o test/test-arithmetic-ruby

test/investigate: test/investigate.c lib/libarithmetic.a include/arithmetic.h
	gcc $(CFLAGS) test/investigate.c $(LIBS) -L/usr/local/lib -lcunit -o test/investigate



lib/libarithmetic.a: lib/arithmetic.o
	ar crs lib/libarithmetic.a lib/arithmetic.o
