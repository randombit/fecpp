#!/usr/bin/python

import fecpp as fec
import md5

k = 3
n = 10

code = fec.fec_code(k,n)

f = open('testinput')

hash_fns = [md5.new() for i in xrange(0, n)]

chunk = 4 * 1024

while True:
    block = f.read(chunk * k)

    if len(block) != chunk*k:
        break

    code_blocks = code.encode(block)

    for i in xrange(0, len(code_blocks)):
        hash_fns[i].update(code_blocks[i])

top_hash = md5.new()

for hash_fn in hash_fns:
    print hash_fn.hexdigest()
    top_hash.update(hash_fn.digest())

print top_hash.hexdigest()

