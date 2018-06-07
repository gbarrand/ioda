//  25/05/2005 : G.Barrand : creation.


#ifndef HEPVis_SbTextTTF2_h
#define HEPVis_SbTextTTF2_h

// Inheritance :
#include <HEPVis/SbTTF.h>

/*G.Barrand #include <ft2build.h>*/
#include <ourex_ft2build.h>

#include FT_FREETYPE_H

class SbTextTTF2Face;

class SbTextTTF2 : public virtual SbTTF {
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
  SbTextTTF2(const SbString&);
  virtual ~SbTextTTF2();
private:
  SbBool getStringBounds(const SbString&,const SbString&,FT_BBox&) const;
private:
  SbTextTTF2Face* fTTF_Face;
  SbBool fIsValid;
  unsigned int fResolution;
  int fPointSize;
  int fAscent;
  int fDescent;
};


#endif



