/*G.Barrand*/
#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE
#include "config_ios.h"
#elif defined(ANDROID)
#include "config_android.h"
#elif defined(_WIN32)
#include "config_win32.h"
#elif defined(__APPLE__)
#include "config_macosx.h"
#else
#include "config_linux.h"
#endif
