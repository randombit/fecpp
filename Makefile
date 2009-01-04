#
# makefile for vdm.
#
# fec.S.980624a is an optimized version for use on 486 and old pentium
# machines. It is only for GF_BITS=8 and generally does not work
# fast on systems with multiple instruction pipelines (PentiumPro,
# Pentium2)
# same for fec.S16.980624a , for use with GF_BITS=16
#
# gcc does something strange, so check the various opt. levels for
# best performance (or write addmul1 in assembly code).
#
# Standard compilation with -O9 works well for PentiumPro and Pentium2
# machines.
#

CC=gcc
CFLAGS=-O2 -DGF_BITS=8 -Wall -Wextra

fec: fec.o test.o
	$(CC) $(CFLAGS) -o fec fec.o test.o

%.o: %.c fec.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o fec
