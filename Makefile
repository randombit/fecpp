
CC=g++
WARNINGS= -Wall -Wextra
CFLAGS=-O2 -g $(WARNINGS)

all: libfecpp.a zfec test_recovery gen_test_vec

libfecpp.a: fecpp.o
	ar crs libfecpp.a fecpp.o

%.o: %.cpp fecpp.h
	$(CC) $(CFLAGS) -I. -c $< -o $@

zfec: zfec.o libfecpp.a
	$(CC) $(CFLAGS) $<  -L. -lfecpp -o $@

test_fec: test/test_fec.o libfecpp.a
	$(CC) $(CFLAGS) $<  -L. -lfecpp -o $@

test_recovery: test/test_recovery.o libfecpp.a
	$(CC) $(CFLAGS) $< -L. -lfecpp -o $@

gen_test_vec: test/gen_test_vec.o libfecpp.a
	$(CC) $(CFLAGS) $< -L. -lfecpp -o $@

clean:
	rm -f *.a *.o test/*.o
	rm -f zfec test_fec test_recovery gen_test_vec
