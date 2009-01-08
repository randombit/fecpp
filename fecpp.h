/*
 * Forward error correction based on Vandermonde matrices
 *
 * (C) 1997-98 Luigi Rizzo (luigi@iet.unipi.it)
 * (C) 2009 Jack Lloyd (lloyd@randombit.net)
 *
 * Distributed under the terms given in license.txt
 */

#ifndef FECPP_H__
#define FECPP_H__

typedef unsigned char byte;

struct fec_parms {
   unsigned int magic;
   int k, n;  /* parameters of the code */
   byte* enc_matrix;
} ;

void fec_free(struct fec_parms *p);
struct fec_parms* fec_new(int k, int n);

void fec_encode(struct fec_parms* code,
                byte* src[], byte* fec, int index, int sz);

int fec_decode(struct fec_parms* code, byte* pkt[], int index[], int sz);

#endif
