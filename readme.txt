
FECpp: Erasure codes based on Vandermonde matrices

FECpp contains an implementation of an encoder/decoder for an erasure
code based on Vandermonde matrices computed over GF(2^8). It is based
on fec, by Luigi Rizzo, which is available at
  http://info.iet.unipi.it/~luigi/fec.html

FECpp should be compatible with zfec (http://allmydata.org/trac/zfec),
producing bitwise identical results in all cases.

Principle of Operation
========================================

The encoded data is computed as

	y = E x

where x is a k-vector with source data, y is an n-vector with the
redundant info, and E is an n*k matrix derived from a Vandermonde
matrix. The code is systematic, meaning the first k rows of E are the
identity matrix. This causes the first k blocks of output to be equal
to the input, split into k pieces.

At the receiver, any subset y' of k elements from y allows the
reconstruction of the whole x by solving the system

	y' = E' x

where E' is made of rows from E corresponding to the received
elements.

The complexity of matrix inversion is O(k*l^2) where l is the number
of elements not in x available at the receiver. This might seem
large, but data elements are in fact be packets of large size, so
the inversion cost can be amortized over the size of the packet.
For practical applications (k and l as large as 30, packet sizes
of 1KB) the cost can be neglected.

API Usage
========================================

fecpp provides a single class, fec_code, which is declared in the
header file fecpp.h

fec_code's constructor takes two integers, k and n. The encoder will
generate n shares, any k of which will be sufficient to recover the
original input.

To encode, call fec_code's encode operation with a pointer to a
buffer, a length, and a std::function which will be called for each
output block:

void encode(const byte input[], size_t size,
   std::function<void (size_t, size_t, const byte[], size_t)> out) const

The length of the input must be a multiple of k bytes.

The arguments to the callback are, in order, the share identifier, the
maximum share that will be generated, and the share contents and
length. The buffer that contains the share data may be reused once the
callback function returns, so if the data must be retained, make a
copy. However in many applications the share will be immediately
written to a file or socket, in which case no copy is necessary.
Each share will be of equal length, specifically size / k bytes.

To decode a set of shares into the original input, call decode:

void decode(const std::map<size_t, const byte*>& shares,
            size_t share_size,
   std::function<void (size_t, size_t, const byte[], size_t)> out) const

The map of shares is a mapping from share identifier (the first
parameter to the encoding callback) to the contents of the share. It
is essential that the share identifiers and shares are associated
properly: otherwise the decoding will fail. As described above, each
share is of equal length, which is specified using the share_size
parameter.

The output callback for decoding has the same interface, and much the
same semantics, as for encoding. The second parameter, which sets the
maximum number of blocks, will be k instead of n, since there are k
original input blocks. Since each block is the same size, you can
compute the full size of the output (which will be k, the second
parameter, multiplied by the size of each subpart, which is the fourth
parameter). For example to reconstruct the input into a file, you
could seek back and forth writing each block as it becomes available.

For both encoding and decoding, you should not assume that the output
blocks will be provided to the callback in order. Currently this is
the case for encoding, but not for decoding, and later if
multithreaded operations or OpenMP is used to parellize the encoding
it is quite likely that shares will be provided out of order.

Future Work / Todos / Send Patches
========================================

 * Use threads or OpenMP
 * Investigate loop tiling and other matrix multiplication optimizations
 * Use a sliding window for the SSE2 multiplication
 * Add support for NEON, AVX2, AVX-512, ...
 * Streaming interface
 * Progressive decoding (is that even possible?)
 * Allow use of different polynomials
