#include "fecpp.h"
#include <fstream>
#include <stdexcept>
#include <vector>
#include <boost/algorithm/string.hpp>

std::string decode_hex(const std::string& in)
   {
   const unsigned char HEX_TO_BIN[256] = {
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x01,
      0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
      0x0F, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x0A, 0x0B, 0x0C,
      0x0D, 0x0E, 0x0F, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };

   if(in.size() % 2 != 0)
      throw std::invalid_argument("Odd sized hex string: "+ in);

   std::string out;

   for(size_t i = 0; i != in.size(); i += 2)
      {
      unsigned char c1 = in[i];
      unsigned char c2 = in[i+1];

      c1 = HEX_TO_BIN[c1];
      c2 = HEX_TO_BIN[c2];

      if(c1 == 0x80 || c2 == 0x80)
         throw std::invalid_argument("Invalid hex: " + in);

      unsigned char c = (c1 << 4) | c2;

      out.push_back(c);
      }

   return out;
   }

// this is really, stupendouly dumb, but it works for current purposes
class chooser_of_k_of_n
   {
   public:
      int choose();

      chooser_of_k_of_n(int k_arg, int n_arg) :
         k(k_arg), n(n_arg), chosen(n) {}

   private:
      int k, n;
      std::vector<bool> chosen;
   };

int chooser_of_k_of_n::choose()
   {
   while(true)
      {
      for(size_t i = 0; i != chosen.size(); ++i)
         {
         if(std::rand() % 16 == 0)
            if(chosen[i] == false)
               {
               chosen[i] = true;
               return i;
               }
         }
      }
   }

bool check_recovery(byte k, byte n, const std::string& hex_input,
                    const std::vector<std::string>& hex_packets)
   {
   std::string input = decode_hex(hex_input);

   std::vector<std::string> packets;
   for(size_t i = 0; i != hex_packets.size(); ++i)
      packets.push_back(decode_hex(hex_packets[i]));

   fec_code code(k, n);

   byte** pkts = new byte*[k];
   int* indexes = new int[k];

   chooser_of_k_of_n chooser(k,n);

   for(int i = 0; i != k; ++i)
      {
      pkts[i] = new byte[input.length() / k];

      int ind = chooser.choose();

      indexes[i] = ind;

      const void* src_packet = packets[ind].c_str();
      int src_len = packets[ind].length();

      memcpy(pkts[i], src_packet, src_len);
      }

   code.decode(pkts, indexes, input.length() / k);

   //printf("%s\n", hex_input.c_str());
   for(int i = 0; i != k; ++i)
      {
      int stride = input.length() / k;

      for(size_t j = 0; j != input.length() / k; ++j)
         {
         //printf("%02X", pkts[i][j]);

         if(pkts[i][j] != input[stride*i+j])
            printf("Bad: pkts[%d][%d] = %02X != input[%d*%d+%d] = %02X\n",
                   i, j, pkts[i][j], stride, i, j, (byte)input[stride*i+j]);
         }
      }
   //printf("\n");

   for(int i = 0; i != k; ++i)
      delete[] pkts[i];

   delete[] pkts;
   delete[] indexes;

   return false;
   }

int main()
   {
   std::ifstream testfile("tests.txt");

   std::srand(0);

   while(testfile.good())
      {
      std::string line;
      std::getline(testfile, line);

      if(line == "")
         continue;

      std::vector<std::string> inputs;
      boost::split(inputs, line, boost::is_any_of(", "));

      int k = 0, n = 0;
      std::string input;
      std::vector<std::string> blocks;

      for(size_t i = 0; i != inputs.size(); ++i)
         {
         if(inputs[i] == "")
            continue;

         std::vector<std::string> name_val;
         boost::split(name_val, inputs[i], boost::is_any_of("="));

         if(name_val.size() != 2)
            throw std::invalid_argument("Bad test line " + inputs[i]);

         std::string name = name_val[0];
         std::string val = name_val[1];

         if(name == "k")
            k = atoi(val.c_str());
         else if(name == "n")
            n = atoi(val.c_str());
         else if(name == "input")
            input = val;

         // assuming stored in text file in order
         else if(name.find_first_of("block_") != std::string::npos)
            blocks.push_back(val);
         }

      if((int)blocks.size() != n)
         throw std::logic_error("Bad block count");

      check_recovery(k, n, input, blocks);
      }
   }
