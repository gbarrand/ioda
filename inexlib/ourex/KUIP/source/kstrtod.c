
#include <kuip/kstrtod.h>

#include <locale.h>
#include <stdlib.h> // malloc,free
#include <string.h> // strcpy

#define k_strdup(a_str) \
  strcpy((char*)malloc(strlen(a_str)+1),(a_str))

/* G.Barrand : strtod by enforcing LC_NUMERIC to C. */

double kstrtod(const char* a_str,char** a_end) {
  char* old;
  double d;
  old = setlocale(LC_NUMERIC,0);
  old = k_strdup(old);
  setlocale(LC_NUMERIC,"C");
  d = strtod(a_str,a_end);
  setlocale(LC_NUMERIC,old);
  free(old);
  return d;
}

