/*
 * Forward error correction based on Vandermonde matrices
 *
 * (C) 1997-1998 Luigi Rizzo (luigi@iet.unipi.it)
 * (C) 2009 Jack Lloyd (lloyd@randombit.net)
 *
 * Distributed under the terms given in license.txt
 */

#ifndef FECPP_H__
#define FECPP_H__

#include <map>
#include <vector>
#include <functional>
#include <cstdint>

namespace fecpp {

using std::uint8_t;
using std::size_t;

using byte = std::uint8_t;

/**
* Forward error correction code
*/
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
         const uint8_t input[], size_t size,
         std::function<void (size_t, size_t, const uint8_t[], size_t)> out)
         const;

      /**
      * @param shares map of share id to share contents
      * @param share_size size in bytes of each share
      * @param out the output callback
      */
      void decode(
         const std::map<size_t, const uint8_t*>& shares, size_t share_size,
         std::function<void (size_t, size_t, const uint8_t[], size_t)> out)
         const;

   private:
      size_t K, N;
      std::vector<uint8_t> enc_matrix;
   };

}

#endif
