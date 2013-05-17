CC=gcc
CFLAGS=--std=gnu99 -Wextra -pedantic-errors -Wformat=2 -g

blitemoth: blitemoth_main.c blitemoth_parser.o ppmio.o
	$(CC) $(CFLAGS) -o blitemoth \
		blitemoth_main.c blitemoth_parser.o ppmio.o

blitemoth_parser.o: blitemoth_parser.c blitemoth_parser.h blitemoth_info.h
	$(CC) $(CFLAGS) -Wno-format-nonliteral -c blitemoth_parser.c

# TESTING STUFF
#
.PHONY: test testclean
test: ppmio_test testclean
	./ppmio_test

testclean:
	-rm test.ppm

ppmio_test: ppmio_test.c ppmio.o ppmio.h
	$(CC) $(CFLAGS) -o ppmio_test ppmio_test.c ppmio.o

ppmio.o: ppmio.c ppmio.h
	$(CC) $(CFLAGS) -c ppmio.c

# CLEANING STUFF
#
.PHONY: clean cleanobj cleanbin
clean: cleanobj cleanbin

cleanobj:
	-rm *.o

cleanbin:
	-rm ppmio_test
	-rm blitemoth

