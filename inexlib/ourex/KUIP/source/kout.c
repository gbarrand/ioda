/*
  GB : To control kuip output. 
  GB : It is used when GUI is not a terminal.
*/

#include <stdio.h>
#include <stdarg.h>

#include <kuip/kout.h>

static void _vprintf(void* a_tag,char* a_format,va_list a_args) {vprintf(a_format,a_args);}

static koutFunc s_print_va = _vprintf; /* not thread safe.*/
static void* s_tag = NULL;             /* not thread safe.*/

void k_set_out_func(koutFunc a_proc,void* a_tag) {
  s_print_va = a_proc;
  s_tag = a_tag;
}

void k_get_out_func(koutFunc* a_old_proc,void** a_old_tag) {
  *a_old_proc = s_print_va;
  *a_old_tag = s_tag;
}

void kprintf(char* a_format,...) {
  va_list args;
  if(a_format==NULL) return;
  if(s_print_va==NULL) return;
  va_start(args,a_format);
  s_print_va(s_tag,a_format,args);
  va_end(args);
}

