CC=gcc
CFLAGS=--std=gnu99 -Wextra -pedantic-errors -Wformat=2 -g

blitemoth: blitemoth_main.c blitemoth_cli.o blitemoth_engine.o ppmio.o
	$(CC) $(CFLAGS) -o blitemoth \
		blitemoth_main.c blitemoth_cli.o blitemoth_engine.o ppmio.o

blitemoth_cli.o: blitemoth_cli.c blitemoth_cli.h blitemoth_info.h blitemoth_engine.h
	$(CC) $(CFLAGS) -Wno-format-nonliteral -c blitemoth_cli.c

blitemoth_engine.o: blitemoth_engine.c blitemoth_engine.h
	$(CC) $(CFLAGS) -c blitemoth_engine.c

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
.PHONY: cleanall cleanobj cleanbin
cleanall: cleanobj cleanbin

cleanobj:
	-rm *.o

cleanbin:
	-rm ppmio_test
	-rm blitemoth

