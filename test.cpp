#include "fecpp.h"
#include <stdio.h>

int main()
   {
   fec_parms* code = fec_new(4, 8);

   unsigned char buf[64] = { 0 };

   for(int i = 0; i != sizeof(buf); ++i)
      buf[i] = i * 13;

   unsigned char* buf_ptrs[4];

   for(int i = 0; i != 4; ++i)
      buf_ptrs[i] = buf + i * (sizeof(buf) / 4);

   unsigned char fec[sizeof(buf) / 4];

   for(int i = 0; i != 8; ++i)
      {
      fec_encode(code, buf_ptrs, fec, i, sizeof(buf) / 4);

      for(int j = 0; j != sizeof(fec); ++j)
         printf("%02X", fec[j]);
      printf("\n");
      }

   fec_free(code);

   return 0;
   }
