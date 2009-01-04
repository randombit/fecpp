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
# COPT= -O9 -funroll-loops -DGF_BITS=8
COPT= -O1 -DGF_BITS=8
CFLAGS=$(COPT) -Wall # -DTEST
SRCS= fec.c Makefile test.c fec.s.980621e \
	fec.S.980624a \
	fec.S16.980624a
DOCS= README fec.3
ALLSRCS= $(SRCS) $(DOCS) fec.h

fec: fec.o test.o
	$(CC) $(CFLAGS) -o fec fec.o test.o

fec.o: fec.h fec.S
	$(CC) $(CFLAGS) -c -o fec.o fec.S

fec.S: fec.c Makefile
	$(CC) $(CFLAGS) -S -o fec.S fec.c

clean:
	- rm *.core *.o fec.s fec

tgz: $(ALLSRCS)
	tar cvzf vdm`date +%y%m%d`.tgz $(ALLSRCS)
