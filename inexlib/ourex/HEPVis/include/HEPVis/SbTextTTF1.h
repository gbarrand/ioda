//  01/04/2005 : G.Barrand : creation.


#ifndef HEPVis_SbTextTTF1_h
#define HEPVis_SbTextTTF1_h

// Inheritance :
#include <HEPVis/SbTTF.h>

#include <freetype.h>

class SbTextTTF1Face;

class SbTextTTF1 : public virtual SbTTF {
public: //SbTTF:
  virtual SbBool isValid() const;
  virtual SbBool getBounds(int,const SbString*,const SbString&,SbBox2s&) const;
  virtual SbBool setPointSize(int);
  virtual int getPointSize() const;
  virtual SbBool getCharacter(char,SbBool,SbBool,SbTTF_BBox&,int&,SbTTF_Raster_Map&) const;
  virtual int getAscent() const;
  virtual int getDescent() const;
  virtual SbString getFontName() const;
public:
  SbTextTTF1(const SbString&);
  virtual ~SbTextTTF1();
private:
  SbBool getStringBounds(const SbString&,TT_BBox&,SbTTF_Skip = 0) const;
private:
  SbTextTTF1Face* fTTF_Face;
  SbBool fIsValid;
  TT_Instance fInstance;
  TT_Instance_Metrics fInstanceMetrics;
  int fAscent;
  int fDescent;
};


#endif



