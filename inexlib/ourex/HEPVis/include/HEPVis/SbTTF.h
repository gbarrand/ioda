//  26/05/2005 : G.Barrand : creation.

/*
 * Abstract interface for SbTextTTF and SbTextTTF2.
 * It is used in SoTextTTF.
 */

#ifndef HEPVis_SbTTF_h
#define HEPVis_SbTTF_h

#include <Inventor/SbString.h>

typedef struct {
  int xMin;
  int yMin;
  int xMax;
  int yMax;
} SbTTF_BBox;

typedef struct {
  int rows;
  int cols;
  int width;
  void* buffer;
  int size;
  int grays;
} SbTTF_Raster_Map;

class SbBox2s;

class SbTTF {
public:
  virtual ~SbTTF() {}
public:
  virtual SbBool isValid() const = 0;
  virtual SbBool getBounds(int,const SbString*,const SbString&,SbBox2s&) const = 0;
  virtual SbBool setPointSize(int) = 0;
  virtual int getPointSize() const = 0;
  virtual SbBool getCharacter(char,SbBool,SbBool,SbTTF_BBox&,int&,SbTTF_Raster_Map&) const = 0;
  virtual int getAscent() const = 0;
  virtual int getDescent() const = 0;
  virtual SbString getFontName() const = 0;
};


#endif



