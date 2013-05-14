CC=gcc
CFLAGS=--std=c99 -Wextra -pedantic-errors -Wformat=2 -g

.PHONY: test testclean
test: ppmio_test testclean
	./ppmio_test

testclean:
	-rm test.ppm

ppmio_test: ppmio_test.c ppmio.o ppmio.h
	$(CC) $(CFLAGS) -o ppmio_test ppmio_test.c ppmio.o

ppmio.o: ppmio.c ppmio.h
	$(CC) $(CFLAGS) -c ppmio.c

.PHONY: cleanall cleanobj cleanbin
cleanall: cleanobj cleanbin

cleanobj:
	-rm *.o

cleanbin:
	-rm ppmio_test

