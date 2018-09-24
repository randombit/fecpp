#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include "fecpp.h"

using namespace fecpp;

class save_results
   {
   public:
      save_results(size_t n) : results(n) {}

      void operator()(size_t i, size_t, const byte fec[], size_t fec_len)
         {
         results[i].append(reinterpret_cast<const char*>(fec), fec_len);
         }

      boost::python::list get_results() const
         {
         boost::python::list list;
         for(size_t i = 0; i != results.size(); ++i)
            list.append(boost::python::str(results[i].c_str(),
                                           results[i].length()));
         return list;
         }

   private:
      std::vector<std::string> results;
   };

boost::python::list fec_encode(fec_code* code,
                               const std::string& input)
   {
   save_results fec_saver(code->get_N());

   code->encode(reinterpret_cast<const byte*>(input.c_str()),
                input.size(),
                std::ref(fec_saver));

   return fec_saver.get_results();
   }

boost::python::list fec_decode(fec_code* code,
                               boost::python::dict dict)
   {
   size_t share_size = 0;
   std::map<size_t, const byte*> shares;

   std::vector<std::string> share_data(code->get_N());

   for(size_t i = 0; i != code->get_N(); ++i)
      {
      if(!dict.has_key(i))
         continue;

      share_data[i] = boost::python::extract<std::string>(dict.get(i));

      if(share_size == 0)
         share_size = share_data[i].length();
      else if(share_size != share_data[i].length())
         throw std::invalid_argument("FEC shares of unusual size");

      shares[i] = reinterpret_cast<const byte*>(share_data[i].c_str());
      }

   if(shares.size() < code->get_K())
      throw std::invalid_argument("Could not decode, insufficient shares");

   save_results fec_saver(code->get_K());

   code->decode(shares, share_size, std::ref(fec_saver));

   return fec_saver.get_results();
   }

BOOST_PYTHON_MODULE(fecpp)
   {
   boost::python::class_<fec_code>
      ("fec_code", boost::python::init<size_t, size_t>())
      .def("encode", fec_encode)
      .def("decode", fec_decode)
      .add_property("K", &fec_code::get_K)
      .add_property("N", &fec_code::get_N);
   }
