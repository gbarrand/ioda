/*
  GB : To control kuip output. 
  GB : It is used when GUI is not a terminal.
*/
#ifndef kout_h
#define kout_h

#include <stdarg.h>

#ifdef __cplusplus
extern "C"{
#endif
typedef void(*koutFunc)(void*,char*,va_list);

void k_set_out_func(koutFunc,void*);
void k_get_out_func(koutFunc*,void**);
void kprintf(char*,...);
#ifdef __cplusplus
}
#endif

#endif







