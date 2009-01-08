
CC=g++
WARNINGS= -Wall -Wextra
CFLAGS=-O2 -g $(WARNINGS)

all: libfecpp.a test_fec test_recovery gen_test_vec

libfecpp.a: fecpp.o
	ar crs libfecpp.a fecpp.o

fecpp.o: fecpp.cpp fecpp.h
	$(CC) $(CFLAGS) -c $< -o $@

test_fec: test/test_fec.cpp libfecpp.a
	$(CC) $(CFLAGS) $< -I. -L. -lfecpp -o $@

test_recovery: test/test_recovery.cpp libfecpp.a
	$(CC) $(CFLAGS) $< -I. -L. -lfecpp -o $@

gen_test_vec: test/gen_test_vec.cpp libfecpp.a
	$(CC) $(CFLAGS) $< -I. -L. -lfecpp -o $@

clean:
	rm -f *.a *.o test_fec test_recovery gen_test_vec
