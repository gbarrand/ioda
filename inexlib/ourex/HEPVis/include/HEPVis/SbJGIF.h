#ifndef HEPVis_SbJGIF_h
#define HEPVis_SbJGIF_h

class SbJGIF {
public:
  static void beginGIF_Stream(void*,long,long);
  static void putColormapInGIF_Stream(int,unsigned char**);
  static void putImageInGIF_Stream(long,int,unsigned char***);
  static void endGIF_Stream();
};

#endif


