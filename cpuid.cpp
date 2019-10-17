
#include "fecpp.h"

namespace fecpp {

bool has_sse2() {
#ifdef HAVE_SSE2
  return true;
#else
  return false;
#endif
}

bool has_ssse3() {
#ifdef HAVE_SSSE3
  return true;
#else
  return false;
#endif
}

}
