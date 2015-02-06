CFLAGS=-ljack -lpthread -ldl -lrt -lm -O2
DFLAGS=-ljack -lpthread -ldl -lrt -lm -g
PREFIX=bin
OBJ=$(PREFIX)/obj
SRC=src
#LIB=$(SRC)/lib

$(PREFIX)/filter: $(SRC)/filter.c Makefile
	mkdir -p $(PREFIX)
	gcc $(SRC)/filter.c -o $(PREFIX)/filter $(CFLAGS)

clean:
	rm -fr $(OBJ)

clean-all:
	rm -fr $(PREFIX)
