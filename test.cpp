#include "fecpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gen_test_vector(int k, int n)
   {
   fec_parms* code = fec_new(k, n);



   unsigned int buf_size = k * 4;
   unsigned char* buf = new byte[buf_size];

   for(int i = 0; i != buf_size; ++i)
      buf[i] = rand();

   printf("k=%d, n=%d, input=", k, n);
   for(int i = 0; i != buf_size; ++i)
      printf("%02X", buf[i]);
   printf(", ");

   unsigned char* buf_ptrs[k];

   for(int i = 0; i != k; ++i)
      {
      buf_ptrs[i] = buf + i * (buf_size / k);
      /*
      printf("BUF %d: ", i);
      for(int l = 0; l != buf_size / k; ++l)
         printf("%02X", buf_ptrs[i][l]);
      printf("\n");
      */
      }

   unsigned char* fec = new byte[buf_size / k];

   for(int i = 0; i != n; ++i)
      {
      fec_encode(code, buf_ptrs, fec, i, buf_size / k);

      printf("block_%d=", i);
      for(int j = 0; j != buf_size / k; ++j)
         printf("%02X", fec[j]);
      if(i != n -1)
         printf(", ");
      }

   printf("\n");

   fec_free(code);

   }

int main()
   {
   srand(0);

   for(int n = 1; n != 10; ++n)
      {
      for(int k = 1; k != 10; ++k)
         {
         if(k >= n)
            continue;

         gen_test_vector(k, n);
         }
      }

   return 0;
   }
