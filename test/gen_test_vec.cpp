#include "fecpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using fecpp::byte;

void print_block(size_t block_no, size_t n,
                 const byte share[], size_t share_len)
   {
   printf("block_%d=", block_no);
   for(size_t j = 0; j != share_len; ++j)
      printf("%02X", share[j]);
   if(block_no != n - 1)
      printf(", ");
   }

void gen_test_vector(int k, int n)
   {
   fecpp::fec_code fec(k, n);

   std::vector<byte> buf(k * (64 + rand() % 512));
   //std::vector<byte> buf(k * (3 + rand() % 5));

   for(size_t i = 0; i != buf.size(); ++i)
      buf[i] = rand();

   printf("k=%d, n=%d, input=", k, n);
   for(size_t i = 0; i != buf.size(); ++i)
      printf("%02X", buf[i]);
   printf(", ");

   fec.encode(&buf[0], buf.size(), print_block);

   printf("\n");
   }

int main()
   {
   srand(0);

   const int Ms[] = {1, 2, 3, 4, 5, 7, 8, 9, 11, 16, 17, 19, 32, 33, 35, 64,65, 66, 67,
                     128, 129, 130, 131, 192, 254, 255, 0 };
   const int Ks[] = {1,2,3,4,5,6,7,8,9,10,15,16,17,20,31,32,33,63,64,65,127,128,129,192,255 ,0};

   for(int i = 0; Ms[i]; ++i)
      {
      for(int j = 0; Ks[j]; ++j)
         {
         int k = Ks[j];
         int m = Ms[i];

         if(k >= m)
            continue;

         gen_test_vector(k, m);
         }
      }

   return 0;
   }
