// This may look like C code, but it is really -*- C++ -*-
//
// $Id: utils.h,v 1.1 2000/04/12 17:49:54 ansari Exp $
//

#ifndef UTILS_EA
#define UTILS_EA

#include "machdefs.h"
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>


// MemCpy marche meme en cas de recouvrement.
// #define memcpy MemCpy

void* MemCpy(void* dest, const void* source, size_t n);

char const* PeidaWorkPath();
void buildPath(char* dst, const char* dir, const char* file);
void buildPath(string& dst, const string& dir, const string& file);
void changeSuffix(char* file, const char* suffix);
void changeSuffix(string& file, const string& suffix);
string itos(int);
string ftos(double);

inline void GetIntEnv(char const* s, int& v)
{
  char* pdeb = getenv(s);
  if (pdeb) v = atoi(pdeb);
}

/* if defined(__DECCXX) || defined(__xlC) */
#if defined(__xlC) || defined(__DECCXX)
#define STRUCTCOMP(s)                        \
  bool operator==(s const& b) const          \
{ return memcmp(this, &b, sizeof(s)) == 0; } \
  bool operator<(s const& b) const           \
{ return memcmp(this, &b, sizeof(s)) < 0; } 
  
#define STRUCTCOMPF(s,f)                        \
  bool operator==(s const& b) const          \
{ return f == b.f; }                          \
  bool operator<(s const& b) const           \
{ return f < b.f; }
#else
#define STRUCTCOMP(s) 
#define STRUCTCOMPF(s,f)
#endif

#endif // UTILS_EA
