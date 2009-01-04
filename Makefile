
CC=g++
CFLAGS=-O2 -DGF_BITS=8 -Wall -Wextra

test_fec: test_fec.o libfecpp.a
	$(CC) $(CFLAGS) test_fec.o -L. -lfecpp -o test_fec

test_fec.o: test_fec.cpp fecpp.h
	$(CC) $(CFLAGS) -c $< -o $@

libfecpp.a: fecpp.o
	ar crs libfecpp.a fecpp.o

fecpp.o: fecpp.cpp fecpp.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.a *.o test_fec
