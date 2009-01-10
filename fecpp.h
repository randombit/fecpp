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

#include <stddef.h>
#include <vector>

typedef unsigned char byte;

class fec_code
   {
   public:
      /**
      * fec_code constructor
      * @param k the number of shares needed for recovery
      * @param n the number of shares generated
      */
      fec_code(size_t k, size_t n);

      size_t get_k() const { return k; }
      size_t get_n() const { return n; }

      /**
      * @param src The list of src packets, each size k
      * @param fec The output buffer
      */
      void encode(byte* src[], byte* fec, size_t index, size_t sz) const;

      void decode(byte* pkt[], size_t index[], size_t sz) const;
   private:
      size_t k, n;
      std::vector<byte> enc_matrix;
   };

#endif
