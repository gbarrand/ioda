#ifndef HEPVis_SbJPEG_h
#define HEPVis_SbJPEG_h

#include <stdio.h>

class SbJPEG {
public:
 static int writeFile(FILE*,int,unsigned char*,unsigned int,unsigned int);
 static unsigned char* readFile(FILE*,unsigned int&,unsigned int&);
};

#endif


