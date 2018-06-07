#ifndef HEPVis_SbSystem_h
#define HEPVis_SbSystem_h

#include <Inventor/SbBasic.h>
#include <Inventor/SbString.h>

SbBool SbSystemIsMacApp(const SbString&,SbString&);
SbBool SbSystemPutenv(const SbString&,const SbString&);
SbString SbSystemPathSeparator();
SbString SbSystemFileSeparator();

#endif
