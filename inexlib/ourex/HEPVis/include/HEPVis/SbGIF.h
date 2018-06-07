#ifndef HEPVis_SbGIF_h
#define HEPVis_SbGIF_h

#include <stdio.h>   

class SbGIF {
public:
  typedef unsigned long(*GetPixelFunction)(int,int);
  static unsigned char* getImageFromStream(FILE*,unsigned int&,unsigned int&,
                                           int&,double*&,double*&,double*&,int&);
  static void putBytesInStream(FILE*,unsigned char*,unsigned int,unsigned int,
                               int,double*,double*,double*);
  static void putBytesInStream(FILE*,unsigned char*,unsigned int,unsigned int,
                               int,unsigned char*,unsigned char*,unsigned char*);
  static void putImageInStream(FILE*,GetPixelFunction,
                               unsigned int,unsigned int,
                               int,double*,double*,double*);
};

#endif
