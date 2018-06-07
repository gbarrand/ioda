/* G.Barrand */

#if defined(_MSC_VER)

  #include "H5Tinit_visual.ic"

#elif defined(__APPLE__)

  #include <TargetConditionals.h>

  #if TARGET_OS_IPHONE
  #include "H5Tinit_Linux.ic"
  #elif defined(__i386__)
  #include "H5Tinit_macosx_intel.ic"
  #elif defined(__x86_64__)
  #include "H5Tinit_macosx_x86_64.ic"
  #else
  #include "H5Tinit_macosx.ic"
  #endif

/*
#elif defined(__CYGWIN__)

  #if defined(__GNUC__)
  #include "H5Tinit_CYGWIN.ic"
  #else
  #error "CYGWIN with another compiler than gcc is not yet supported."
  #endif
*/

#elif defined(__linux__)

  #if defined(__x86_64__)
  #include "H5Tinit_Linux_x86_64.ic"
  #else
  #include "H5Tinit_Linux.ic"
  #endif

#elif defined(ANDROID) /*G.Barrand*/

  #include "H5Tinit_Linux.ic"

#else

  #error "Unknown platform."

#endif
