CFLAGS=-ljack -lpthread -ldl -lrt -lm -lfftw3 -O2
DFLAGS=-ljack -lpthread -ldl -lrt -lm -lfftw3 -g
PREFIX=bin
OBJ=$(PREFIX)/obj
SRC=src
#LIB=$(SRC)/lib

$(PREFIX)/filter: $(SRC)/filter.c Makefile
	mkdir -p $(PREFIX)
	gcc $(SRC)/filter.c -o $(PREFIX)/filter $(CFLAGS)

debug:
	mkdir -p $(PREFIX)
	gcc $(SRC)/filter.c -o $(PREFIX)/filter_db $(DFLAGS)

clean:
	rm -fr $(OBJ)

clean-all:
	rm -fr $(PREFIX)
