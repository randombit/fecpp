
#include "fecpp.h"

#if defined(__i386__) || defined(__x86_64__) || defined(

#if defined(_MSC_VER)
  #include <intrin.h>
#elif defined(__GNUC__)
  //GCC and Clang
  #include <cpuid.h>
#endif

#if defined(_MSC_VER)
  #define X86_CPUID(type, out) do { __cpuid((int*)out, type); } while(0)
#elif defined(__GNUC__)
  #define X86_CPUID(type, out) do { __get_cpuid(type, out, out+1, out+2, out+3); } while(0)
#else
  #warning "Don't know how to invoke CPUID"
#endif

namespace fecpp {

static uint64_t cpuid_bits;

enum class CPUID_BITS {
   INITIALIZED = 0,
   SSE2 = 1,
   SSSE3 = 2,
   AVX2 = 3,
   AVX512 = 4,
   NEON = 5,
   VMX = 6
};

namespace {

uint64_t init_cpuid()
   {
   uint64_t bits = 0;

   bits |= (1 << CPUID_BITS::INITIALIZED);
   }

}

bool has_sse2()
   {
   if(cpuid_bits == 0)
      cpuid_bits = init_cpuid();

   return cpuid_bits & (1 << CPUID_BITS::SSE2);
   }

bool has_ssse3()
   {
   if(cpuid_bits == 0)
      cpuid_bits = init_cpuid();

   return cpuid_bits & (1 << CPUID_BITS::SSE2);
   }

}
