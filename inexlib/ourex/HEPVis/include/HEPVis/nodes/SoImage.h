// History :
//  16/11/1998 : G.Barrand : creation.
////////////////////////////////////////////////////////////
//
//  This node is able to visualize 'fits' files. 
// 'fits' is a format used in astronomy.
//
////////////////////////////////////////////////////////////


#ifndef HEPVis_SoImage_h
#define HEPVis_SoImage_h 

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFEnum.h>

#include <HEPVis/SbLUT.h>

class HEPVis_SoImage  : public SoNode {
  SO_NODE_HEADER(HEPVis_SoImage);
public:
  enum Format {
    FITS,
    GIF,
    JPEG,
    DVI,
    TEX
  };
  SoSFEnum format;
  SoSFString fileName;
  HEPVis_SoImage();
  HEPVis_SoImage(int,int,short*);
  void readImage();
  void getSize(unsigned int&,unsigned int&) const;
  void setPixmap(unsigned int,unsigned int,unsigned char*,int,
                 double*,double*,double*,int);
  void setPixmap(unsigned int,unsigned int,unsigned char*);
  void setPixmap(int,int,short*);

protected:
  virtual  ~HEPVis_SoImage();
public: /*SoEXTENDER*/
  virtual void GLRender(SoGLRenderAction*);
public: /*SoINTERNAL*/
  static void initClass();
private:
  void reset();
  void buildPixels();
private:
  // FITS :
  int fWidth;
  int fHeight;
  short* fDataShort;
  SbLUT* fLUT;
  // GIF :
  unsigned int fUnsignedWidth;
  unsigned int fUnsignedHeight;
  unsigned char* fDataUnsignedChar;
  int fColorNumber;
  double* fReds;
  double* fGreens;
  double* fBlues;
  int fTransparent;
  // Common :
  float* fPixels;
};

#endif

