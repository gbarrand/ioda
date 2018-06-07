// this :
#include <HEPVis/nodes/SoImage.h>

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/errors/SoDebugError.h>
//#include <Inventor/elements/SoViewportRegionElement.h>

#include <HEPVis/SbGL.h>
#include <HEPVis/SbFITS.h>
#include <HEPVis/SbGIF.h>
#include <HEPVis/SbJPEG.h>
#include <HEPVis/SbLatex.h>
#include <HEPVis/SbDVI.h>
#include <HEPVis/SbPixmap.h>
#include <HEPVis/actions/SoGL2PSAction.h>

static void getImageLUT(short*,int,int,int&,int&,int&);
static SbBool convoluteImage(short*,int,int,int,unsigned char*);

#define SHORT_MAX 32767

SO_NODE_SOURCE(HEPVis_SoImage)
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoImage::initClass (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_INIT_CLASS(HEPVis_SoImage,SoNode,"Node");
}
//////////////////////////////////////////////////////////////////////////////
HEPVis_SoImage::HEPVis_SoImage (
)
:fWidth(0)
,fHeight(0)
,fDataShort(0)
,fLUT(0)
,fUnsignedWidth(0)
,fUnsignedHeight(0)
,fDataUnsignedChar(0)
,fColorNumber(0)
,fReds(0),fGreens(0),fBlues(0)
,fPixels(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(HEPVis_SoImage);
  SO_NODE_ADD_FIELD(format,(GIF));
  SO_NODE_ADD_FIELD(fileName,(""));

  SO_NODE_DEFINE_ENUM_VALUE(Format,FITS);
  SO_NODE_DEFINE_ENUM_VALUE(Format,GIF);
  SO_NODE_DEFINE_ENUM_VALUE(Format,JPEG);
  SO_NODE_DEFINE_ENUM_VALUE(Format,DVI);
  SO_NODE_DEFINE_ENUM_VALUE(Format,TEX);

  SO_NODE_SET_SF_ENUM_TYPE(format,Format);
}
//////////////////////////////////////////////////////////////////////////////
HEPVis_SoImage::HEPVis_SoImage (
 int aWidth
,int aHeight
,short* aData                    
)
:fWidth(0)
,fHeight(0)
,fDataShort(0)
,fLUT(0)
,fUnsignedWidth(0)
,fUnsignedHeight(0)
,fDataUnsignedChar(0)
,fColorNumber(0)
,fReds(0),fGreens(0),fBlues(0)
,fPixels(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(HEPVis_SoImage);
  SO_NODE_ADD_FIELD(format,(GIF));
  SO_NODE_ADD_FIELD(fileName,(""));

  SO_NODE_DEFINE_ENUM_VALUE(Format,FITS);
  SO_NODE_DEFINE_ENUM_VALUE(Format,GIF);
  SO_NODE_DEFINE_ENUM_VALUE(Format,JPEG);
  SO_NODE_DEFINE_ENUM_VALUE(Format,TEX);
  SO_NODE_DEFINE_ENUM_VALUE(Format,DVI);

  SO_NODE_SET_SF_ENUM_TYPE(format,Format);

  Format fm = (Format)format.getValue();
  if(fm==FITS) {  
    fWidth = aWidth;
    fHeight = aHeight;
    fDataShort = aData;
    int mn,mx,nbin;
    mn = -SHORT_MAX;
    mx = 0;
    nbin = 14;
    fLUT = new SbLUT(65536);
    if(fLUT) {
      fLUT->initialize(SbLUT::LINEAR,mn+SHORT_MAX,mx+SHORT_MAX,nbin);
      convoluteImage(fDataShort,fWidth,fHeight,
                     fLUT->getCellNumber(),
                     fLUT->getCells());
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
HEPVis_SoImage::~HEPVis_SoImage (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  delete [] fDataShort;
  delete fLUT;

  delete [] fDataUnsignedChar;
  delete [] fReds;
  delete [] fGreens;
  delete [] fBlues;

  delete [] fPixels;
}
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoImage::setPixmap(
 unsigned int aUnsignedWidth
,unsigned int aUnsignedHeight
,unsigned char* aDataUnsignedChar
,int aColorNumber
,double* aReds
,double* aGreens
,double* aBlues
,int aTransparent
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  reset();

  fUnsignedWidth = aUnsignedWidth;
  fUnsignedHeight = aUnsignedHeight;
  fDataUnsignedChar = aDataUnsignedChar;
  fColorNumber = aColorNumber;
  fReds = aReds;
  fGreens = aGreens;
  fBlues = aBlues;
  fTransparent = aTransparent;
  format = GIF;

  buildPixels();
}

//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoImage::setPixmap(
 unsigned int aUnsignedWidth
,unsigned int aUnsignedHeight
,unsigned char* aDataUnsignedChar
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  reset();

  fUnsignedWidth = aUnsignedWidth;
  fUnsignedHeight = aUnsignedHeight;
  fDataUnsignedChar = aDataUnsignedChar;
  fColorNumber = 0;
  fReds = 0;
  fGreens = 0;
  fBlues = 0;
  format = JPEG;

  buildPixels();
}

//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoImage::setPixmap(
 int aWidth
,int aHeight
,short* aDataShort
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  reset();

  fWidth = aWidth;
  fHeight = aHeight;
  fDataShort = aDataShort;
  fColorNumber = 0;
  fReds = 0;
  fGreens = 0;
  fBlues = 0;
  format = FITS;

  buildPixels();
}

//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoImage::getSize(
 unsigned int& aWidth
,unsigned int& aHeight
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{ 
  Format fm = (Format)format.getValue();
  if(fm==FITS) {  
    aWidth = (int)fWidth;
    aHeight = (int)fHeight;
  } else {
    aWidth = fUnsignedWidth;
    aHeight = fUnsignedHeight;
  }
}
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoImage::GLRender (
 SoGLRenderAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  Format fm = (Format)format.getValue(); // We should have an element !

  int w = 0;
  int h = 0;
  if(fm==FITS) {  
    if(!fDataShort) readImage();
    if(!fPixels) buildPixels();
    w = fWidth;
    h = fHeight;
  } else { //GIF or JPEG
    if(!fDataUnsignedChar) readImage();
    if(!fPixels) buildPixels();
    w = (int)fUnsignedWidth;
    h = (int)fUnsignedHeight;
  }

  if((w<=0)||(h<=0)) return;

  if(fPixels) {
    if( (fm == GIF) || (fm == DVI) || (fm == TEX)) { //GIF: RGBA mode
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
      
      glRasterPos3f(0,0,0);
      glDrawPixels((GLsizei)w,(GLsizei)h,GL_RGBA,GL_FLOAT,fPixels);
    } else {
      glRasterPos3f(0,0,0);
      glDrawPixels((GLsizei)w,(GLsizei)h,GL_RGB,GL_FLOAT,fPixels);
    }
    
    if(aAction->isOfType(SoGL2PSAction::getClassTypeId())) {
      ((SoGL2PSAction*)aAction)->addBitmap(w,h);
    }
    
  }
  
}
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoImage::reset(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  delete [] fPixels;
  fPixels = 0;
  // Reset FITS :
  delete [] fDataShort;
  fDataShort = 0;
  fWidth = 0;
  fHeight = 0;
  // Reset GIF :
  delete [] fDataUnsignedChar;
  delete [] fReds;
  delete [] fGreens;
  delete [] fBlues;
  fUnsignedWidth = 0;
  fUnsignedHeight = 0;
  fDataUnsignedChar = 0;
  fColorNumber = 0;
  fReds = 0;
  fGreens = 0;
  fBlues = 0;
}
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoImage::readImage(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  reset();

  if(fileName.getValue()=="") return;

  Format fm = (Format)format.getValue();
  if(fm==FITS) {  
    FILE* file = ::fopen(fileName.getValue().getString(),"rb");
    if(!file) {
      SoDebugError::post("SoImage::readImage",
                         "Cannot open file %s",
                         fileName.getValue().getString());
      return;
    }
    // Get size :
    //SbFITS::getHeaderFromStream(file,fWidth,fHeight);
    //rewind(file);
    fWidth  = 200;
    fHeight = 200;
    int X_org = 0;
    int Y_org = 0;
    int swap = 0;  
    fDataShort = SbFITS::getImageFromStream(file,
                                            X_org,Y_org,
                                            fWidth,fHeight,
                                            swap);
    ::fclose(file);
    if(!fDataShort) {
      SoDebugError::post("SoImage::readImage",
                         "Cannot read file %s",
                         fileName.getValue().getString());
      fWidth = 0;
      fHeight = 0;
      return;
    }

    int mn,mx,nbin;
    getImageLUT(fDataShort,fWidth,fHeight,mn,mx,nbin);
    fLUT = new SbLUT(65536);
    if(fLUT) {
      fLUT->initialize(SbLUT::LINEAR,mn+SHORT_MAX,mx+SHORT_MAX,nbin);
      convoluteImage(fDataShort,fWidth,fHeight,
                     fLUT->getCellNumber(),
                     fLUT->getCells());
    }

  } else if(fm==JPEG) {

    FILE* file = ::fopen(fileName.getValue().getString(),"rb");
    if(!file) {
      SoDebugError::post("SoImage::readImage",
                         "Cannot open file %s",
                         fileName.getValue().getString());
      return;
    }
    fDataUnsignedChar = SbJPEG::readFile(file,
                                         fUnsignedHeight,
                                         fUnsignedWidth);
    ::fclose(file);
/*
    SoDebugError::post("SoImage::readImage",
                   " %s : %d %d",
                   fileName.getValue().getString(),
                   fUnsignedWidth,
                   fUnsignedHeight);
*/
    if(!fDataUnsignedChar) {
      SoDebugError::post("SoImage::readImage",
                         "Cannot read file %s",
                         fileName.getValue().getString());
      delete [] fDataUnsignedChar;
      fUnsignedWidth = 0;
      fUnsignedHeight = 0;
      fDataUnsignedChar = 0;
      return;
    }

  } else if(fm==TEX) {
    SbString dviTmpName = "";
    SbLatex latex;
    if (latex.fileToDvi(fileName.getValue(),dviTmpName)==FALSE) return;
    SbDVI dvi;
    fDataUnsignedChar = dvi.getImageFromStream(dviTmpName,
                                                  fUnsignedWidth,
                                                  fUnsignedHeight,
                                                  fColorNumber,
                                                  fReds,
                                                  fGreens,
                                                  fBlues,
                                                  fTransparent);
    if(!fDataUnsignedChar) {
      SoDebugError::post("SoImage::readImage",
                         "Cannot read file %s",
                         fileName.getValue().getString());
      reset();
    }
    // remove dvi tmp file
    ::remove(dviTmpName.getString());

  } else if(fm==DVI) {
    SbDVI dvi;
    fDataUnsignedChar = dvi.getImageFromStream(fileName.getValue(),
                                                  fUnsignedWidth,
                                                  fUnsignedHeight,
                                                  fColorNumber,
                                                  fReds,
                                                  fGreens,
                                                  fBlues,
                                                  fTransparent);
    if(!fDataUnsignedChar) {
      SoDebugError::post("SoImage::readImage",
                         "Cannot read file %s",
                         fileName.getValue().getString());
      reset();
    }

  } else { //GIF

    FILE* file = ::fopen(fileName.getValue().getString(),"rb");
    if(!file) {
      SoDebugError::post("SoImage::readImage",
                         "Cannot open file %s",
                         fileName.getValue().getString());
      return;
    }
    fDataUnsignedChar = SbGIF::getImageFromStream(file,
                                                  fUnsignedWidth,
                                                  fUnsignedHeight,
                                                  fColorNumber,
                                                  fReds,
                                                  fGreens,
                                                  fBlues,
                                                  fTransparent);
    ::fclose(file);
    //SoDebugError::post("SoImage::readImage",
    //               " %s : %d %d",
    //               fileName.getValue().getString(),
    //               fUnsignedWidth,
    //               fUnsignedHeight);
    if(!fDataUnsignedChar) {
      SoDebugError::post("SoImage::readImage",
                         "Cannot read file %s",
                         fileName.getValue().getString());
      delete [] fDataUnsignedChar;
      delete [] fReds;
      delete [] fGreens;
      delete [] fBlues;
      fUnsignedWidth = 0;
      fUnsignedHeight = 0;
      fDataUnsignedChar = 0;
      fColorNumber = 0;
      fReds = 0;
      fGreens = 0;
      fBlues = 0;
      return;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoImage::buildPixels(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  delete [] fPixels;
  fPixels = 0;

  Format fm = (Format)format.getValue(); // We should have an element !
  if(fm==FITS) {  

    if(fDataShort) fPixels = SbPixmap::FITSToGL(fWidth,fHeight,fDataShort);

  } else if(fm==JPEG) {  

    if(fDataUnsignedChar) 
      fPixels = SbPixmap::JPEGToGL
        (fUnsignedWidth,fUnsignedHeight,fDataUnsignedChar);

  } else { //GIF

    if(fDataUnsignedChar)
      fPixels = SbPixmap::GIFToGL(
                  fUnsignedWidth,fUnsignedHeight,fDataUnsignedChar,
                  fColorNumber,fReds,fGreens,fBlues,fTransparent);

  }
}
//////////////////////////////////////////////////////////////////////////////
void getImageLUT (
 short* aData
,int aWidth
,int aHeight
,int& aMin 
,int& aMax 
,int& aNbin 
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aMin = 0;
  aMax = 0;
  aNbin = 0; 
  if(!aData) return;
  if((aWidth==0)||(aHeight==0)) return;  
  int ideb,ifin;
  if(aWidth > 200) {
    ideb = 50;
    ifin = aWidth-50;
  } else {
    ideb = 1;
    ifin = aWidth-1;
  }
  int jdeb,jfin;
  if (aHeight > 200) {
    jdeb = 50;
    jfin = aHeight-50;
  } else {
    jdeb = 1;
    jfin = aHeight-1;
  }

  double sx = 0.;
  double sx2 = 0.;
  int npix  = 0;
  int row,col;
  double val;
  short* pData;
  for(row=jdeb;row<jfin;row++) { 
    pData = aData + row * aWidth + ideb;
    for (col=ideb;col<ifin;col++) { 
      val = (double)(*pData);
      sx += val;
      sx2 += val * val;
      npix++;
      pData++;
    }
  }
  
  sx /= npix;
  sx2 /= npix;
  sx2 -= sx * sx;
  sx2 = sqrt((double) sx2);

  aMin = (int)(sx-sx2/3);
  aMax = (int)(sx+sx2*2);
  aNbin = 14;
}
//////////////////////////////////////////////////////////////////////////////
SbBool convoluteImage (
 short* aData
,int aWidth
,int aHeight
,int aCelln
,unsigned char* aCells
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!aData) return FALSE;
  if( (aWidth==0) || (aHeight==0) ) return FALSE;   
  SbBool ok = TRUE;
  int row,col;
  short* pData;
  unsigned short value;
  for (row=0;row<aHeight;row++) { 
    pData = aData + row * aWidth;
    for (col=0;col<aWidth;col++) { 
      value = (*pData) + SHORT_MAX;  // short -> unsigned short.
      if(value>=aCelln) {
        ok = FALSE;
      } else {
        *pData = (short)aCells[value]; // unsigned short -> color index.
      }
      pData++;
    }
  }
  return ok;
}
