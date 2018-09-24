#include "fecpp.h"
#include <fstream>
#include <stdexcept>
#include <vector>
#include <boost/algorithm/string.hpp>

using fecpp::byte;

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

class output_checker
   {
   public:
      output_checker(const std::string& in_arg) : in(in_arg) {}

      void operator()(size_t block, size_t max_blocks,
                      const byte buf[], size_t size)
         {
         if(out.size() == 0)
            out.resize(max_blocks * size);

         for(size_t i = 0; i != size; ++i)
            {
            byte inb = in.at(block*size+i);
            if(inb != buf[i])
               {
               printf("Bad: block=%d/%d i=%d in=%02X buf=%02X\n",
                      (int)block, (int)max_blocks, (int)i, inb, buf[i]);
               }
            out[block*size+i] = buf[i];
            }
         }

      void confirm()
         {
         if(in != out)
            printf("Mismatch in final check!\n");
         }
   private:
      std::string in;
      std::string out;
   };

bool check_recovery(byte k, byte n, const std::string& hex_input,
                    const std::vector<std::string>& hex_packets)
   {
   std::string input = decode_hex(hex_input);

   std::vector<std::string> packets;
   for(size_t i = 0; i != hex_packets.size(); ++i)
      packets.push_back(decode_hex(hex_packets[i]));

   //printf("%d, %d\n", k, n);
   fecpp::fec_code code(k, n);

   std::map<size_t, const byte*> packets_map;
   for(size_t i = 0; i != packets.size(); ++i)
      packets_map[i] = (const byte*)packets[i].c_str();

   output_checker check_output(input);

   chooser_of_k_of_n chooser(k,n);

   while(packets_map.size() > k)
      packets_map.erase(chooser.choose());

   // assumes all same size
   code.decode(packets_map, packets[0].size(), std::ref(check_output));

   check_output.confirm();

   return true;
   }

int main()
   {
   std::ifstream testfile("tests.txt");

   if(!testfile)
      {
      printf("Could not open input file\n");
      return 1;
      }

   int seed = time(0);

   //printf("Using %d as random seed\n", seed);

   std::srand(seed);

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

      if(check_recovery(k, n, input, blocks))
         printf("Good recovery %d %d\n", k, n);
      else
         printf("Bad recovery %d %d\n", k, n);
      }
   }
