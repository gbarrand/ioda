/* G.Barrand */
/* src/H5Tinit.c has to be customized in the same way. */

#if defined(WIN32)

  #include "H5pubconf_win32.h"

#elif defined(__APPLE__) 

  #include <TargetConditionals.h>

  #if TARGET_OS_IPHONE /*G.Barrand*/
  #include "H5pubconf_Linux.h"    /*iPad-1 is 32 bits and little endian.*/
  #undef H5_HAVE_FEATURES_H
  #undef H5_HAVE_LSEEK64

  #elif defined(__i386__)
  #include "H5pubconf_macosx_intel.h"

  #elif defined(__x86_64__)
  #include "H5pubconf_macosx_x86_64.h"

  #else
  #include "H5pubconf_macosx.h"
  #endif

#elif defined(__CYGWIN__)

  #if defined(__GNUC__)
  #include "H5pubconf_CYGWIN.h"
  #else
  #error "CYGWIN with another compiler than gcc is not yet supported."
  #endif

#elif defined(ANDROID) /*G.Barrand*/

  /*Samsung Galaxy S with Android 2.1 is 32 bits and little endian.*/

  #include "H5pubconf_Linux.h"

  #undef H5_HAVE_LSEEK64

#elif defined(__linux__)

  #if defined(__x86_64__)
  #include "H5pubconf_Linux_x86_64.h"
  #else
  #include "H5pubconf_Linux.h"
  #endif

#else

  #error "Unknown platform."

#endif
