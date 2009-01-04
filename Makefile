
CC=g++
CFLAGS=-O2 -DGF_BITS=8 -Wall -Wextra

fec: fec.o test.o
	$(CC) $(CFLAGS) -o fec fec.o test.o

%.o: %.c fec.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o fec
