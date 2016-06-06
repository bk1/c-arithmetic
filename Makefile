# automatically generated makefile by ./scripts/create-make

CFLAGS=-Wall -g -O2 -std=gnu99 -I./include -L./lib

LIBS=-larithmetic


all: test/test-arithmetic-cunit test/test-sqrt-cunit test/test-arithmetic-ruby test/investigate test/test-sqrt-ffff

clean:
	rm -f lib/libarithmetic.a  lib/sqrt.o lib/arithmetic.o lib/cbrt.o test/test-arithmetic-cunit test/test-sqrt-cunit test/test-arithmetic-ruby test/investigate test/test-sqrt-ffff

lib/sqrt.o: lib/sqrt.c
	gcc -c $(CFLAGS) lib/sqrt.c -o lib/sqrt.o

lib/arithmetic.o: lib/arithmetic.c
	gcc -c $(CFLAGS) lib/arithmetic.c -o lib/arithmetic.o

lib/cbrt.o: lib/cbrt.c
	gcc -c $(CFLAGS) lib/cbrt.c -o lib/cbrt.o

test/test-arithmetic-cunit: test/test-arithmetic-cunit.c lib/libarithmetic.a include/arithmetic.h
	gcc $(CFLAGS) test/test-arithmetic-cunit.c $(LIBS) -L/usr/local/lib -lcunit -o test/test-arithmetic-cunit

test/test-sqrt-cunit: test/test-sqrt-cunit.c lib/libarithmetic.a include/arithmetic.h
	gcc $(CFLAGS) test/test-sqrt-cunit.c $(LIBS) -L/usr/local/lib -lcunit -o test/test-sqrt-cunit

test/test-arithmetic-ruby: test/test-arithmetic-ruby.c lib/libarithmetic.a include/arithmetic.h
	gcc $(CFLAGS) test/test-arithmetic-ruby.c $(LIBS) -L/usr/local/lib -lcunit -o test/test-arithmetic-ruby

test/investigate: test/investigate.c lib/libarithmetic.a include/arithmetic.h
	gcc $(CFLAGS) test/investigate.c $(LIBS) -L/usr/local/lib -lcunit -o test/investigate

test/test-sqrt-ffff: test/test-sqrt-ffff.c lib/libarithmetic.a include/arithmetic.h
	gcc $(CFLAGS) test/test-sqrt-ffff.c $(LIBS) -L/usr/local/lib -lcunit -o test/test-sqrt-ffff



lib/libarithmetic.a: lib/sqrt.o lib/arithmetic.o lib/cbrt.o
	ar crs lib/libarithmetic.a lib/sqrt.o lib/arithmetic.o lib/cbrt.o
