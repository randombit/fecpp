
CC=g++
WARNINGS= -Wall -Wextra
CFLAGS=-O2 -march=pentium4 -g $(WARNINGS)

PROGS = benchmark zfec test_recovery gen_test_vec

all: $(PROGS)

libfecpp.so: fecpp.o
	$(CC) -shared -o $@ $<

fecpp.o: fecpp.cpp fecpp.h
	$(CC) $(CFLAGS) -fPIC -I. -c $< -o $@

test/%.o: test/%.cpp fecpp.h
	$(CC) $(CFLAGS) -I. -c $< -o $@

zfec: test/zfec.o libfecpp.so
	$(CC) $(CFLAGS) $<  -L. -lfecpp -o $@

benchmark: test/benchmark.o libfecpp.so
	$(CC) $(CFLAGS) $<  -L. -lfecpp -o $@

test_fec: test/test_fec.o libfecpp.so
	$(CC) $(CFLAGS) $<  -L. -lfecpp -o $@

test_recovery: test/test_recovery.o libfecpp.so
	$(CC) $(CFLAGS) $< -L. -lfecpp -o $@

gen_test_vec: test/gen_test_vec.o libfecpp.so
	$(CC) $(CFLAGS) $< -L. -lfecpp -o $@

clean:
	rm -f *.so *.o test/*.o
	rm -f $(PROGS)
