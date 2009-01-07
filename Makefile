
CC=g++
CFLAGS=-O2 -g -DGF_BITS=8 -Wall -Wextra

all: test_fec test_recovery gen_test_vec

test_fec: test_fec.o libfecpp.a
	$(CC) $(CFLAGS) $< -L. -lfecpp -o $@

test_recovery: test_recovery.o fecpp.h
	$(CC) $(CFLAGS) $< -L. -lfecpp -o $@

gen_test_vec: gen_test_vec.o fecpp.h
	$(CC) $(CFLAGS) $< -L. -lfecpp -o $@

libfecpp.a: fecpp.o
	ar crs libfecpp.a fecpp.o

%.o: %.cpp fecpp.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.a *.o test_fec test_recovery gen_test_vec

