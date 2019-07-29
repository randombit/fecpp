/*
 * (C) 2009-2010 Jack Lloyd (jack@randombit.net)
 *
 * Distributed under the terms given in license.txt (Simplified BSD)
 */

#include "fecpp.h"
#include <emmintrin.h>

namespace fecpp {

size_t addmul_sse2(uint8_t z[], const uint8_t x[], uint8_t y, size_t size)
   {
   const __m128i polynomial = _mm_set1_epi8(0x1D);

   const size_t y_bits = 32 - __builtin_clz(y);

   const size_t consumed = size - (size % 64);

   // unrolled out to cache line size
   while(size >= 64)
      {
      __m128i x_1 = _mm_loadu_si128((const __m128i*)(x));
      __m128i x_2 = _mm_loadu_si128((const __m128i*)(x + 16));
      __m128i x_3 = _mm_loadu_si128((const __m128i*)(x + 32));
      __m128i x_4 = _mm_loadu_si128((const __m128i*)(x + 48));

      __m128i z_1 = _mm_load_si128((const __m128i*)(z));
      __m128i z_2 = _mm_load_si128((const __m128i*)(z + 16));
      __m128i z_3 = _mm_load_si128((const __m128i*)(z + 32));
      __m128i z_4 = _mm_load_si128((const __m128i*)(z + 48));

      // prefetch next two x and z blocks
      _mm_prefetch(x + 64, _MM_HINT_T0);
      _mm_prefetch(z + 64, _MM_HINT_T0);
      _mm_prefetch(x + 128, _MM_HINT_T1);
      _mm_prefetch(z + 128, _MM_HINT_T1);

      if(y & 0x01)
         {
         z_1 = _mm_xor_si128(z_1, x_1);
         z_2 = _mm_xor_si128(z_2, x_2);
         z_3 = _mm_xor_si128(z_3, x_3);
         z_4 = _mm_xor_si128(z_4, x_4);
         }

      for(size_t j = 1; j != y_bits; ++j)
         {
         /*
         * Each byte of each mask is either 0 or the polynomial 0x1D,
         * depending on if the high bit of x_i is set or not.
         */

         __m128i mask_1 = _mm_setzero_si128();
         __m128i mask_2 = _mm_setzero_si128();
         __m128i mask_3 = _mm_setzero_si128();
         __m128i mask_4 = _mm_setzero_si128();

         // flip operation?
         mask_1 = _mm_cmpgt_epi8(mask_1, x_1);
         mask_2 = _mm_cmpgt_epi8(mask_2, x_2);
         mask_3 = _mm_cmpgt_epi8(mask_3, x_3);
         mask_4 = _mm_cmpgt_epi8(mask_4, x_4);

         x_1 = _mm_add_epi8(x_1, x_1);
         x_2 = _mm_add_epi8(x_2, x_2);
         x_3 = _mm_add_epi8(x_3, x_3);
         x_4 = _mm_add_epi8(x_4, x_4);

         mask_1 = _mm_and_si128(mask_1, polynomial);
         mask_2 = _mm_and_si128(mask_2, polynomial);
         mask_3 = _mm_and_si128(mask_3, polynomial);
         mask_4 = _mm_and_si128(mask_4, polynomial);

         x_1 = _mm_xor_si128(x_1, mask_1);
         x_2 = _mm_xor_si128(x_2, mask_2);
         x_3 = _mm_xor_si128(x_3, mask_3);
         x_4 = _mm_xor_si128(x_4, mask_4);

         if((y >> j) & 1)
            {
            z_1 = _mm_xor_si128(z_1, x_1);
            z_2 = _mm_xor_si128(z_2, x_2);
            z_3 = _mm_xor_si128(z_3, x_3);
            z_4 = _mm_xor_si128(z_4, x_4);
            }
         }

      _mm_store_si128((__m128i*)(z     ), z_1);
      _mm_store_si128((__m128i*)(z + 16), z_2);
      _mm_store_si128((__m128i*)(z + 32), z_3);
      _mm_store_si128((__m128i*)(z + 48), z_4);

      x += 64;
      z += 64;
      size -= 64;
      }

   return consumed;
   }

}
