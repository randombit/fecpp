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

#include <map>
#include <vector>
#include <tr1/functional>

using std::size_t;

typedef unsigned char byte;

class fec_code
   {
   public:
      /**
      * fec_code constructor
      * @param K the number of shares needed for recovery
      * @param N the number of shares generated
      */
      fec_code(size_t K, size_t n);

      size_t get_K() const { return K; }
      size_t get_N() const { return N; }

      /**
      * @param input the data to FEC
      * @param size the length in bytes of input
      * @param out the output callback
      */
      void encode(
         const byte input[], size_t size,
         std::tr1::function<void (size_t, size_t, const byte[], size_t)> out)
         const;

      /**
      * @param shares map of share id to share contents
      * @param share_size size in bytes of each share
      * @param out the output callback
      */
      void decode(
         const std::map<size_t, const byte*>& shares, size_t share_size,
         std::tr1::function<void (size_t, size_t, const byte[], size_t)> out)
         const;

   private:
      size_t K, N;
      std::vector<byte> enc_matrix;
   };

#endif
