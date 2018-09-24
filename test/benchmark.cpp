#include "fecpp.h"
#include <stdio.h>
#include <string.h>

using fecpp::byte;

namespace {

class output_checker
   {
   public:
      void operator()(size_t block, size_t /*max_blocks*/,
                      const byte buf[], size_t size)
         {
         for(size_t i = 0; i != size; ++i)
            {
            byte expected = block*size + i;
            if(buf[i] != expected)
               printf("block=%d i=%d got=%02X expected=%02X\n",
                      (int)block, (int)i, buf[i], expected);

            }
         }
   };

class save_to_map
   {
   public:
      save_to_map(size_t& share_len_arg,
                  std::map<size_t, const byte*>& m_arg) :
         share_len(share_len_arg), m(m_arg) {}

      void operator()(size_t block_no, size_t,
                      const byte share[], size_t len)
         {
         share_len = len;

         // Contents of share[] are only valid in this scope, must copy
         byte* share_copy = new byte[share_len];
         memcpy(share_copy, share, share_len);
         m[block_no] = share_copy;
         }
   private:
      size_t& share_len;
      std::map<size_t, const byte*>& m;
   };

void benchmark_fec(size_t k, size_t n)
   {

   fecpp::fec_code fec(k, n);

   std::vector<byte> input(k * 1024);
   for(size_t i = 0; i != input.size(); ++i)
      input[i] = i;

   std::map<size_t, const byte*> shares;
   size_t share_len;

   save_to_map saver(share_len, shares);

   fec.encode(&input[0], input.size(), std::ref(saver));

   while(shares.size() > k)
      shares.erase(shares.begin());

   output_checker check_output;
   fec.decode(shares, share_len, check_output);
   }

}

int main()
   {
   const int Ms[] = {1, 2, 3, 4, 5, 7, 8, 16, 32, 64, 128, 192, 254, 255, 0 };
   const int Ks[] = {1,2,3,4,5,6,7,8,9,10,15,16,17,20,31,32,33,63,64,65,127,128,129,192,255 ,0};

   for(int i = 0; Ms[i]; ++i)
      {
      for(int j = 0; Ks[j]; ++j)
         {
         int k = Ks[j];
         int m = Ms[i];

         if(k >= m)
            continue;

         printf("%d %d\n", k, m);
         benchmark_fec(k, m);
         }
      }
   }
