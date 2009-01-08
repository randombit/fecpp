#include "fecpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gen_test_vector(int k, int n)
   {
   fec_code code(k, n);

   unsigned int buf_size = k * (3 + rand() % 32);
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
      code.encode(buf_ptrs, fec, i, buf_size / k);

      printf("block_%d=", i);
      for(int j = 0; j != buf_size / k; ++j)
         printf("%02X", fec[j]);
      if(i != n -1)
         printf(", ");
      }

   printf("\n");

   delete[] fec;
   delete[] buf;
   }

int main()
   {
   srand(0);

   const int Ms[] = {1, 2, 3, 5, 7, 9, 11, 17, 19, 33, 35, 65, 66, 67,
                     128, 129, 130, 131, 254, 255, 0 };
   const int Ks[] = {1,2,3,4,5,6,7,8,9,17,31,32,33,63,64,65,128,129,255 ,0};

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
