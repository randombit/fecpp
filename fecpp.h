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

#include <tr1/functional>
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

      void encode(
         const byte input[], size_t size,
         std::tr1::function<void (size_t, size_t, const byte[], size_t)> out)
         const;

#if 0
      void decode(
         const std::map<size_t, const byte*>&
         std::tr1::function<void (size_t, size_t, const byte[], size_t)> out)
         const;
#else
      void decode(byte* pkt[], size_t index[], size_t sz) const;
#endif
   private:
      size_t k, n;
      std::vector<byte> enc_matrix;
   };

#endif
