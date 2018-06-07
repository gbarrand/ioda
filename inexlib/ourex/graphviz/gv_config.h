
#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#else
#define HAVE_STRCASECMP  1
#define HAVE_STRNCASECMP 1
#define gv_strcasecmp strcasecmp
#define gv_strncasecmp strncasecmp
#define HAVE_DRAND48
#endif

#define HAVE_ERRNO_H     1
#define HAVE_STRING_H    1

#define VERSION "2.28.0"

#define DEFAULT_DPI 96
