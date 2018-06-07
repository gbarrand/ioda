#include <stdlib.h>
#include <string.h>

#include <kuip/ksys.h>
#include <kuip/kfor.h>
#include <kuip/kuser.h>

extern void Kuinit(int);
extern void Kuwhat();
extern void testd_();

extern void kgl_setwidth();
extern void kgl_config(const char*,int);
extern char* kgl_getline(const char*);
extern void kgl_histadd(const char*);

static int sSeed = 333;

int main() {
  int NWORDS=15000;
  Kuinit(NWORDS);

  // Getline :
  ku_xgl_setwidth(kgl_setwidth);
  ku_xgl_config(kgl_config);
  ku_xgl_getline(kgl_getline);
  ku_xgl_histadd(kgl_histadd);

/*
* Create user command structure from definition file
* (the definition routine name TESTD is defined in the CDF with '>N TESTD').
*/
  testd_();
  srand(sSeed);
#ifdef _WIN32
  ku_exec("/KUIP/SET_SHOW/RECALL_STYLE 'NONE'");
#endif

  ku_exec("SEED");
  ku_exec("SEED S 2");
  ku_exec("NUMBER");
  ku_exec("SET/PROMPT 'RANDOM>'");

  Kuwhat();

  return 0;
}
void tseed_() {
  /* ku_get...s sould come in the right order. */
  char* option = ku_getc();
  int iseed = ku_geti();
  /*printf("debug : tseed %s %d\n",option,iseed);*/
  if( strcmp(option, "G" ) == 0 ) {
    printf("SEED : %d\n",sSeed);
  } else if( strcmp(option, "S" ) == 0 ) {
    sSeed = iseed;
    srand(sSeed);
  } else {
    printf("SEED : Illegal option\n");
  }
}
void numlet_() {
  /* ku_get...s sould come in the right order. */
  int n = ku_geti();
  char* format = ku_getc();
  /* CALL KUPATL(CMD,NPAR)*/
  int count;
  for(count=0;count<n;count++) {
    double value  = (double)rand();
    value /= (double)RAND_MAX;
    printf("%g\n",value);
  }
}

