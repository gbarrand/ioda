#ifndef HEPVis_SbString_h
#define HEPVis_SbString_h

#include <Inventor/SbBasic.h>

#include <stdarg.h>

class SbString;
class SbPList;

char* SbSTRDUP(const char*);
void SbSTRDEL(char*&);
SbBool SbSTREMPTY(const char*);

SbPList SbStringGetWords(const SbString&,const char*);
SbPList SbStringGetLines(const SbString&);
SbBool SbStringPrintF(SbString&,int,const char*,...);
SbString SbStringPrintF(int,const char*,...);
int SbStringFind(const SbString&,const SbString&);
int SbStringReverseFind(const SbString&,const SbString&);
void SbStringDelete(SbPList&);
SbString* SbStringCreateEmpty(int);
SbBool SbStringIsTTF(const SbString&);
SbBool SbString_isenv(const SbString&);
SbBool SbString_getenv(const SbString&,SbString&);
SbBool SbStringGetenv(SbString&);
SbBool SbStringReplace(SbString&,const SbString&,const SbString&);
SbString SbStringSuffix(const SbString&);

SbBool SbStringConvertToInt(const SbString&,int&);

#define SbStringNotFound (-1)

#endif
