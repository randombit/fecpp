#!/usr/bin/python

import pyfecpp
import random

c = pyfecpp.fec_code(3,10)

shares = c.encode('abcdef012345')

print shares

shares = dict([share for share in zip(xrange(0,len(shares)), shares)])

print shares

while len(shares) > c.K:
    del shares[random.choice(shares.keys())]

print shares

dec = c.decode(shares)

print dec

print ''.join(dec)
