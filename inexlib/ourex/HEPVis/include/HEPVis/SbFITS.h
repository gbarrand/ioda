// History :
//  18/11/1998 : G.Barrand : creation.

#ifndef HEPVis_SbFITS_h
#define HEPVis_SbFITS_h

#include <stdio.h> 

class SbFITS {
public:
  static short* getImageFromStream(FILE*,int,int,int,int,int);
  static void getHeaderFromStream(FILE*,int&,int&);
  static void dumpHeaderInStream(FILE*);
  static void putImageInStream(FILE*,short*,int,int,int);
};

#endif
