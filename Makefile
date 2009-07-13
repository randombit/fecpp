
CC=g++
WARNINGS= -Wall -Wextra
OPTFLAGS=-O2 -march=native
DEBUGFLAGS=-g

CFLAGS=$(OPTFLAGS) $(DEBUGFLAGS) $(WARNINGS)

PROGS = benchmark zfec test_recovery gen_test_vec

all: fecpp.so $(PROGS)

libfecpp.a: fecpp.o
	ar crs $@ $<

fecpp.o: fecpp.cpp fecpp.h
	$(CC) $(CFLAGS) -I. -c $< -o $@

test/%.o: test/%.cpp fecpp.h
	$(CC) $(CFLAGS) -I. -c $< -o $@

zfec: test/zfec.o libfecpp.a
	$(CC) $(CFLAGS) $<  -L. -lfecpp -o $@

benchmark: test/benchmark.o libfecpp.a
	$(CC) $(CFLAGS) $<  -L. -lfecpp -o $@

test_fec: test/test_fec.o libfecpp.a
	$(CC) $(CFLAGS) $<  -L. -lfecpp -o $@

test_recovery: test/test_recovery.o libfecpp.a
	$(CC) $(CFLAGS) $< -L. -lfecpp -o $@

gen_test_vec: test/gen_test_vec.o libfecpp.a
	$(CC) $(CFLAGS) $< -L. -lfecpp -o $@

fecpp.so: fecpp.cpp fecpp_python.cpp fecpp.h
	$(CC) -shared -fPIC $(CFLAGS) -I/usr/include/python2.5 fecpp.cpp fecpp_python.cpp -lboost_python -o fecpp.so

clean:
	rm -f *.a *.o test/*.o
	rm -f $(PROGS)
