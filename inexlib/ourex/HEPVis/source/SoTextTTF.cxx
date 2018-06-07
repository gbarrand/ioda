//////////////////////////////////////////////////////////////////////////////
//
// Code extracted from freetype-1.3.1/test/ftview.c, ftstring.c.
//
// rayPick inspired by Coin/SoText2 code.
//
//////////////////////////////////////////////////////////////////////////////

// this :
#include <HEPVis/nodes/SoTextTTF.h>

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/bundles/SoMaterialBundle.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/SbText.h>
#include <HEPVis/SbGL.h>
#include <HEPVis/SbTTF.h>
#include <HEPVis/SbPixmap.h>
#include <HEPVis/actions/SoGL2PSAction.h>

//#include <string.h> //memset

#ifdef HEPVIS_HAS_TTF1
#include <HEPVis/SbTextTTF1.h>
#endif

#ifdef HEPVIS_HAS_TTF2
#include <HEPVis/SbTextTTF2.h>
#endif

SO_NODE_SOURCE(SoTextTTF)

void SoTextTTF::initClass(){
  SO_NODE_INIT_CLASS(SoTextTTF,SoShape,"Shape");
}

SoTextTTF::SoTextTTF()
:fTTF(0)
,fTTFGreek(0)
,fLineWidth(0)
,fTextUP(0)
,fTextWidth(0)
,fTextHeight(0)
,fXJustifyTranslation(0)
,fYJustifyTranslation(0)
,fSmoothing(FALSE)
,fHinting(FALSE)
,fColor(0,0,0)
,fOld_fontName("")
,fOld_size(0)
,fOld_win(0,0)
{
  SO_NODE_CONSTRUCTOR(SoTextTTF);
  
  SO_NODE_ADD_FIELD(fontName,("times"));
  SO_NODE_ADD_FIELD(encoding,("none")); //PAW
  SO_NODE_ADD_FIELD(string,(""));
#define DEF_POINT_SIZE 10
  SO_NODE_ADD_FIELD(size,(DEF_POINT_SIZE));
  SO_NODE_ADD_FIELD(horizontalJustification,(LEFT));
  SO_NODE_ADD_FIELD(verticalJustification,(BOTTOM));
  SO_NODE_ADD_FIELD(align,(LEFT));
  SO_NODE_ADD_FIELD(rotated,(FALSE));
  SO_NODE_ADD_FIELD(smoothing,(FALSE));
  SO_NODE_ADD_FIELD(hinting,(FALSE));

  SO_NODE_ADD_FIELD(rescale,(0));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,LEFT);
  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,CENTER);
  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,RIGHT);

  SO_NODE_DEFINE_ENUM_VALUE(VerticalJustification,BOTTOM);
  SO_NODE_DEFINE_ENUM_VALUE(VerticalJustification,MIDDLE);
  SO_NODE_DEFINE_ENUM_VALUE(VerticalJustification,TOP);

  SO_NODE_SET_SF_ENUM_TYPE(horizontalJustification,HorizontalJustification);
  SO_NODE_SET_SF_ENUM_TYPE(verticalJustification,VerticalJustification);
  SO_NODE_SET_SF_ENUM_TYPE(align,HorizontalJustification);
}

SoTextTTF::~SoTextTTF(){
  delete fTTFGreek;
  delete fTTF;
}

void SoTextTTF::generatePrimitives(SoAction*) {}

void SoTextTTF::GLRender(SoGLRenderAction* aAction) {
  if(!shouldGLRender(aAction)) return;
  update(aAction);
  if(!fTTF) return;

  SoState* state = aAction->getState();

  state->push();

  SoLazyElement::setLightModel(state,SoLazyElement::BASE_COLOR);

  SbViewportRegion vpr = SoViewportRegionElement::get(state);
  short x,y,w,h;
  vpr.getViewportOriginPixels().getValue(x,y);
  vpr.getViewportSizePixels().getValue(w,h);
  
  SoMaterialBundle mb(aAction);
  mb.sendFirst();

  fColor = SoLazyElement::getDiffuse(state,0);
  
  fSmoothing = smoothing.getValue();
  fHinting = hinting.getValue();

  glPushAttrib(GL_ENABLE_BIT);

  //FIXME : arrange to use SoViewportRegion::GLRender scissoring ?
  //FIXME : OpenPAW : should do glScissor only if SoViewportRegion visible.
  glEnable(GL_SCISSOR_TEST);
  glScissor(x,y,w,h);

  glPushAttrib(GL_CURRENT_BIT);
  glPushAttrib(GL_PIXEL_MODE_BIT);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  glColor3f(fColor[0],fColor[1],fColor[2]);

  if(fSmoothing==TRUE) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }

  //  glRasterPos3f fixes also 
  // the color for rasterisation. Then
  // glColor3f should be done before it.
  glRasterPos3f(0,0,0);

  GLint swapbytes, lsbfirst, rowlength;
  GLint skiprows, skippixels, alignment;
  glGetIntegerv( GL_UNPACK_SWAP_BYTES, &swapbytes );
  glGetIntegerv( GL_UNPACK_LSB_FIRST, &lsbfirst );
  glGetIntegerv( GL_UNPACK_ROW_LENGTH, &rowlength );
  glGetIntegerv( GL_UNPACK_SKIP_ROWS, &skiprows );
  glGetIntegerv( GL_UNPACK_SKIP_PIXELS, &skippixels );
  glGetIntegerv( GL_UNPACK_ALIGNMENT, &alignment );

  glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_FALSE );
  glPixelStorei( GL_UNPACK_LSB_FIRST, GL_FALSE );
  glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
  glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );

  if(fSmoothing==TRUE) {
    glPixelZoom(1.,1.);
    glPixelTransferf( GL_RED_SCALE,   1. );
    glPixelTransferf( GL_GREEN_SCALE, 1. );
    glPixelTransferf( GL_BLUE_SCALE,  1. );
    glPixelTransferf( GL_ALPHA_SCALE, 1. );
    glPixelTransferf( GL_RED_BIAS,    0. );
    glPixelTransferf( GL_GREEN_BIAS,  0. );
    glPixelTransferf( GL_BLUE_BIAS,   0. );
    glPixelTransferf( GL_ALPHA_BIAS,  0. );
  }

  // Get text bound once if needed :
  SbBox2s box;
  short szx = 0;
  short szy = 0;
  if( (horizontalJustification.getValue()==CENTER) ||
      (horizontalJustification.getValue()==RIGHT) ||
      (verticalJustification.getValue()==MIDDLE) ||
      (verticalJustification.getValue()==TOP) ||
      (align.getValue()==RIGHT) || 
      (align.getValue()==CENTER)
   ){
    getTextBounds(box);
    box.getSize(szx,szy);
  }

  // Global justification for the whole text.
  fXJustifyTranslation = 0;
  if(horizontalJustification.getValue()==LEFT) {
  } else if(horizontalJustification.getValue()==CENTER) {
    fXJustifyTranslation = 0.5F * szx;
  } else if(horizontalJustification.getValue()==RIGHT) {
    fXJustifyTranslation = szx;
  }

  fYJustifyTranslation = 0;
  if(verticalJustification.getValue()==BOTTOM) {
  } else if(verticalJustification.getValue()==MIDDLE) {
    fYJustifyTranslation = 0.5F * szy;
  } else if(verticalJustification.getValue()==TOP) {
    fYJustifyTranslation = szy;
  }

  int linen = string.getNum();
  const SbString* lines = string.getValues(0);
  for(int count=0;count<linen;count++) {
    float old = fXJustifyTranslation;
    if( (align.getValue()==RIGHT) || (align.getValue()==CENTER) ) {
      SbBox2s box;
      fTTF->getBounds(1,&(lines[count]),encoding.getValue(),box);
      short lszx,lszy;
      box.getSize(lszx,lszy);
      if(align.getValue()==RIGHT) {
        fXJustifyTranslation -= (szx - lszx);
      } else { //CENTER
        fXJustifyTranslation -= (szx - lszx)/2.0F;
      }
    }
    renderString(aAction,lines[count]);
    fXJustifyTranslation = old;
  }

  // Restore saved packing modes.
  glPixelStorei( GL_UNPACK_SWAP_BYTES, swapbytes );
  glPixelStorei( GL_UNPACK_LSB_FIRST, lsbfirst );
  glPixelStorei( GL_UNPACK_ROW_LENGTH, rowlength );
  glPixelStorei( GL_UNPACK_SKIP_ROWS, skiprows );
  glPixelStorei( GL_UNPACK_SKIP_PIXELS, skippixels );
  glPixelStorei( GL_UNPACK_ALIGNMENT, alignment );

  glPopAttrib(); //GL_PIXEL_MODE_BIT
  glPopAttrib(); //GL_CURRENT_BIT

  glPopAttrib(); //GL_ENABLE_BIT

  state->pop();
}

void SoTextTTF::rayPick(SoRayPickAction* aAction){
  if(!shouldRayPick(aAction)) return;
  update(aAction);
  if(!fTTF) return;

  aAction->setObjectSpace();

  SbVec3f v0, v1, v2, v3;
  if(!getQuad(aAction->getState(), v0, v1, v2, v3)) return;
  if (v0 == v1 || v0 == v3) return; // empty

  SbVec3f isect;
  SbVec3f bary;
  SbBool front;
  SbBool hit = aAction->intersect(v0, v1, v2, isect, bary, front);
  if(!hit) hit = aAction->intersect(v0, v2, v3, isect, bary, front);

  if(hit && aAction->isBetweenPlanes(isect)) {
    SoPickedPoint* pp = aAction->addIntersection(isect);
    if(pp) {
      pp->setMaterialIndex(0);
      pp->setObjectNormal(SbVec3f(0,0,1));
    }
  }

}

void SoTextTTF::pick(SoPickAction* aAction){SoShape::pick(aAction);}

void SoTextTTF::computeBBox(SoAction* aAction,SbBox3f& aBox,SbVec3f& aCenter){
  update(aAction);
  if(!fTTF) {
    aBox.makeEmpty();
    aCenter = aBox.getCenter();
    return;
  }
  //setFont(aAction);
  SbVec3f v0, v1, v2, v3;
  // this will cause a cache dependency on the view volume,
  // model matrix and viewport.
  if(getQuad(aAction->getState(), v0, v1, v2, v3)) {
    aBox.extendBy(v0);
    aBox.extendBy(v1);
    aBox.extendBy(v2);
    aBox.extendBy(v3);
  } else {
    aBox.makeEmpty();
  }
  aCenter = aBox.getCenter();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoTextTTF::update(SoAction* aAction){
  if(fOld_fontName!=fontName.getValue()) {
    fOld_fontName = fontName.getValue();

    const SbString& fname = fontName.getValue();
    SbString name;
    if(fname=="defaultFont") {
      name = "times";
    } else if(IsTTF(fname)==TRUE) {
      name = fname.getSubString(4);
    } else {
      name = fname;
    }
    if(fTTF && (fTTF->getFontName()==name)) {
    } else {
      delete fTTF;
      fTTF = 0;
#ifdef HEPVIS_HAS_TTF1
      fTTF = new SbTextTTF1(name);
#endif
#ifdef HEPVIS_HAS_TTF2
      fTTF = new SbTextTTF2(name);
#endif
      if(!fTTF || (fTTF->isValid()==FALSE) ) {
        delete fTTF;
        fTTF = 0;
      }
    }
    if(encoding.getValue()=="PAW") {
      if(!fTTFGreek) {
#ifdef HEPVIS_HAS_TTF1
        fTTFGreek = new SbTextTTF1("symbol");
#endif
#ifdef HEPVIS_HAS_TTF2
        fTTFGreek = new SbTextTTF2("symbol");
#endif
        if(!fTTFGreek || (fTTFGreek->isValid()==FALSE)) {
          delete fTTFGreek;
          fTTFGreek = 0;
        }
      }
    }

    fOld_size = 0;
  }

  int pointSize = size.getValue();
  if(pointSize<=0) pointSize = DEF_POINT_SIZE;

  SbBool update_pointSize = FALSE;
  if(fOld_size!=pointSize) {
    fOld_size = pointSize;
    update_pointSize = TRUE;
  }

  int renderSize = pointSize;
  if( (rescale.getValue()!=0) && (aAction!=NULL) ) {
    SoState* state = aAction->getState();
    SbViewportRegion vpr = SoViewportRegionElement::get(state);
    const SbVec2s& win = vpr.getViewportSizePixels();
    if(fOld_win!=win) {
      fOld_win = win;
      update_pointSize = TRUE;
      int owin = rescale.getValue(); //For exa 200
      if(win[0]<=win[1]) {
        renderSize = (int)(pointSize * ((float)win[0] / (float)owin));
      } else {
        renderSize = (int)(pointSize * ((float)win[1] / (float)owin));
      }
    }
  }

  if(update_pointSize==TRUE) {
    if(fTTF && (fTTF->setPointSize(renderSize)==FALSE)) {
      SoDebugError::post("SoTextTTF::update",
                         "can't set point size %d",renderSize);
    }
    if(fTTFGreek && (fTTFGreek->setPointSize(renderSize)==FALSE)) {
      SoDebugError::post("SoTextTTF::update",
                         "can't set point size %d for greek font.",renderSize);
    }
  }
}

void SoTextTTF::renderString(SoAction* aAction,const SbString& aString){
  if(aString=="") return;
  fLineWidth = 0;
  glGetFloatv(GL_CURRENT_RASTER_POSITION,fRasterPos);

  SbBool encod_PAW = (encoding.getValue()=="PAW"?TRUE:FALSE);

  SbEncodedString sbs = encod_PAW ? 
    SbText::decodePAW(aString) : SbText::decodePlain(aString);

  unsigned int n = sbs.size();
  for(unsigned int index=0;index<n;index++) {
    SbChar sbChar = sbs[index];

    if(encod_PAW) {
      if(sbChar.fFont==SbChar::GREEK) {
        //WARNING : we assume that in PAW encoding ki is on h
        //          as for Hershey/GREEK.
        //          For example [h]^2 being for ki 2.
        //          See OnXLab/PlottableThings/getInfos.
        //          But with TTF/symbol ki is on c.
        if(sbChar.fChar=='h') sbChar.fChar ='c';
      }
    }

    SbTTF* sbFont = fTTF;
    if( (sbChar.fFont==SbChar::GREEK) && fTTFGreek) sbFont = fTTFGreek;

    int osize = sbFont->getPointSize();
    if( (sbChar.fYMove==SbChar::UP) || (sbChar.fYMove==SbChar::DOWN) ) {
      sbFont->setPointSize(int(float(osize)*2.0F/3.0F));
    }
    if(fSmoothing==TRUE) {
      renderCharacterSmoothing(aAction,sbChar,*sbFont);
    } else {
      renderCharacter(aAction,sbChar,*sbFont);
    }
    if( (sbChar.fYMove==SbChar::UP) || (sbChar.fYMove==SbChar::DOWN) ) {
      sbFont->setPointSize(osize);
    }
  }
}

void SoTextTTF::renderCharacter(SoAction* aAction,const SbChar& aChar,SbTTF& aTTF){
  SbTTF_BBox box;
  int advance;
  SbTTF_Raster_Map rasterMap;
  if(aTTF.getCharacter
    (aChar.fChar,FALSE,FALSE,box,advance,rasterMap)==FALSE) return;

  float orp[4];
  orp[0] = fRasterPos[0];
  orp[1] = fRasterPos[1];
  orp[2] = fRasterPos[2];
  orp[3] = fRasterPos[3];
  glGetFloatv(GL_CURRENT_RASTER_POSITION,fRasterPos);

  float HEIGHT = float(aTTF.getAscent());
  float Y_ADVANCE = 2 * HEIGHT; // Same as SoTextHershey :

  float xmove,ymove;
  if(aChar.fCR==TRUE) {
    xmove = -fLineWidth; //FIXME : if aChar.fBack ?
    ymove = - Y_ADVANCE;
  } else {
    if(aChar.fBack==TRUE) {
      xmove = 0;
    } else {
      xmove = float(advance);
    }
    ymove = 0;
    fLineWidth += xmove;
  }

  if(rotated.getValue()==TRUE) {
    unsigned char* bitmap = SbPixmap::rotateBitmap
      (rasterMap.width,rasterMap.rows,rasterMap.cols,
       (unsigned char*)rasterMap.buffer);
    GLsizei bheight = rasterMap.width;    
    GLsizei bwidth = rasterMap.rows;    
    float yorig = fXJustifyTranslation; //FIXME : box.xMin ???
    float xorig = bwidth + box.yMin + fYJustifyTranslation;
    if(aChar.fYMove==SbChar::UP) {
      xorig -= - HEIGHT*0.6F; //Same as SoTextHershey
    } else if(aChar.fYMove==SbChar::DOWN) {
      xorig -=   HEIGHT*0.6F; //Same as SoTextHershey
    }
    if(aChar.fBack==TRUE) {
      yorig += fRasterPos[1]-orp[1];
    }
    glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glBitmap(bwidth,bheight,xorig,yorig,ymove,xmove,(GLubyte*)bitmap);
    if(aAction->isOfType(SoGL2PSAction::getClassTypeId())) {
      ((SoGL2PSAction*)aAction)->addBitmap
        (bwidth,bheight,xorig,yorig,ymove,xmove);
    }
    delete [] bitmap;
  } else { //Not rotated.
    float xorig = - box.xMin + fXJustifyTranslation;
    float yorig = - box.yMin + fYJustifyTranslation;
    if(aChar.fYMove==SbChar::UP) {
      yorig += - HEIGHT*0.6F; //Same as SoTextHershey
    } else if(aChar.fYMove==SbChar::DOWN) {
      yorig +=   HEIGHT*0.6F; //Same as SoTextHershey
    }
    if(aChar.fBack==TRUE) {
      xorig += fRasterPos[0]-orp[0];
    }
    glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glBitmap(rasterMap.width,rasterMap.rows,
             xorig,yorig,xmove,ymove,
             (GLubyte*)rasterMap.buffer);
    if(aAction->isOfType(SoGL2PSAction::getClassTypeId())) {
      ((SoGL2PSAction*)aAction)->addBitmap
        (rasterMap.width,rasterMap.rows,xorig,yorig,xmove,ymove);
    }
  }

  delete [] ((char*)rasterMap.buffer);

  if(aChar.fBar==TRUE) {
    unsigned int W = rasterMap.width;
    unsigned int H = box.yMax - box.yMin;
    H = int(float(H)*1.2);
    GLubyte* bitmap = (GLubyte*)SbPixmap::createBarBitmap(W,H);
    if(rotated.getValue()==TRUE) {
      unsigned int bytes = (W+7)/8; //bytes per line.  
      unsigned char* rbitmap = SbPixmap::rotateBitmap
        (W,H,bytes,(unsigned char*)bitmap);
      GLsizei bheight = W;    
      GLsizei bwidth = H;    
      float yorig = fXJustifyTranslation; //FIXME : box.xMin ???
      float xorig = bwidth + box.yMin + fYJustifyTranslation;
      if(aChar.fYMove==SbChar::UP) {
        xorig -= - HEIGHT*0.6F; //Same as SoTextHershey
      } else if(aChar.fYMove==SbChar::DOWN) {
        xorig -=   HEIGHT*0.6F; //Same as SoTextHershey
      }
      if(aChar.fBack==TRUE) {
        yorig += fRasterPos[1]-orp[1];
      }
      xorig += ymove;
      yorig += xmove;
      glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
      glPixelStorei(GL_UNPACK_ALIGNMENT,1);
      glBitmap(bwidth,bheight,xorig,yorig,0,0,(GLubyte*)rbitmap);
      if(aAction->isOfType(SoGL2PSAction::getClassTypeId())) {
        ((SoGL2PSAction*)aAction)->addBitmap(bwidth,bheight,xorig,yorig,0,0);
      }
      delete [] rbitmap;
    } else { //Not rotated.
      float xorig = - box.xMin + fXJustifyTranslation;
      float yorig = - box.yMin + fYJustifyTranslation;
      if(aChar.fYMove==SbChar::UP) {
        yorig += - HEIGHT*0.6F; //Same as SoTextHershey
      } else if(aChar.fYMove==SbChar::DOWN) {
        yorig +=   HEIGHT*0.6F; //Same as SoTextHershey
      }
      if(aChar.fBack==TRUE) {
        xorig += fRasterPos[0]-orp[0];
      }
      xorig += xmove;
      yorig += ymove;
      glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
      glPixelStorei(GL_UNPACK_ALIGNMENT,1);
      glBitmap(W,H,xorig,yorig,0,0,(GLubyte*)bitmap);
      if(aAction->isOfType(SoGL2PSAction::getClassTypeId())) {
        ((SoGL2PSAction*)aAction)->addBitmap(W,H,xorig,yorig,0,0);
      }
    }
    delete [] bitmap;
  }
}

static float func(float x) {
  return ::sqrt(x); //more black.
  //return x*x; //more white.
  //return x;
}

unsigned char* SoTextTTF_getPixmap( 
 char aChar
,SbTTF_Raster_Map& aRasterMap
,float aRGBA[4]
)
{
  int size = aRasterMap.rows * aRasterMap.cols * 4 ;
  if(aRasterMap.width>aRasterMap.cols) {
    SoDebugError::post("SoTextTTF_getPixmap",
      "pb with %c : rows %d : width %d : cols %d",
      aChar,aRasterMap.rows,aRasterMap.width,aRasterMap.cols);
    size = aRasterMap.rows * aRasterMap.width * 4 ;
  }
  if(size<=0) return 0;
  if(!aRasterMap.buffer) return 0;
  int grays = aRasterMap.grays;
  if(grays<=1) return 0;
  unsigned char* pixmap = new unsigned char [size];

  //NOTE : In ROOT see : asimage/src/TASImage.cxx/DrawGlyph.
  //       As background they arrange to get the bitmap
  //       behind the character and get the mean value color of it.
  //       They build a palette of five colors by interpolation between
  //       this "mean background" and the foreground.
  //       (4 = foreground, 0 = background)

  typedef unsigned char RGBA[4];
  RGBA* palette = new RGBA[grays];
 {float gn = float(grays-1);
  for(int j=0;j<grays;++j) {
    for(int k=0;k<4;++k) {
      palette[j][k] = (unsigned char)(255 * aRGBA[k] * func(float(j)/gn));
    }
  }}

  unsigned char* bline = (unsigned char*)aRasterMap.buffer;
  unsigned char* pline = pixmap;
  for(int j=0;j<aRasterMap.rows;++j) {
    for(int i=0;i<aRasterMap.width;++i) {
      unsigned char k = bline[i]; //[0,grays-1]
      // assuming sizeof(int) == 4 ...        
      *((int*)(pline + i*4)) = *((int*) &(palette[k][0]));
    }
    bline += aRasterMap.cols;
    pline += aRasterMap.cols * 4;
  }

  delete [] palette;

  return pixmap;
}

void SoTextTTF::renderCharacterSmoothing(SoAction* aAction,const SbChar& aChar,SbTTF& aTTF){
  SbTTF_BBox box;
  int advance;
  SbTTF_Raster_Map rasterMap;
  if(aTTF.getCharacter
    (aChar.fChar,TRUE,fHinting,box,advance,rasterMap)==FALSE) return;

  float orp[4];
  orp[0] = fRasterPos[0];
  orp[1] = fRasterPos[1];
  orp[2] = fRasterPos[2];
  orp[3] = fRasterPos[3];
  glGetFloatv(GL_CURRENT_RASTER_POSITION,fRasterPos);

  float HEIGHT = float(aTTF.getAscent());
  float Y_ADVANCE = 2 * HEIGHT; // Same as SoTextHershey :

  float xmove,ymove;
  if(aChar.fCR==TRUE) {
    xmove = -fLineWidth; //FIXME : if aChar.fBack ?
    ymove = - Y_ADVANCE;
  } else {
    if(aChar.fBack==TRUE) {
      xmove = 0;
    } else {
      xmove = float(advance);
    }
    ymove = 0;
    fLineWidth += xmove;
  }

  float rgba[4];
  //NOTE : some time the below does not get 
  //         SoLazyElement::getDiffuse(state,0);
  //       but some grey.
  //glGetFloatv(GL_CURRENT_COLOR,rgba);
  rgba[0] = fColor[0];
  rgba[1] = fColor[1];
  rgba[2] = fColor[2];
  rgba[3] = 1;
  //printf("debug : fore %g %g %g\n",rgb[0],rgb[1],rgb[2]);

  // Create a RGBA pixmap for OpenGL :
  unsigned char* xxx_pixmap = SoTextTTF_getPixmap(aChar.fChar,rasterMap,rgba);
  GLsizei xxx_width = rasterMap.width;
  GLsizei xxx_rows = rasterMap.rows;

  if(rotated.getValue()==TRUE) {
    unsigned char* pixmap = SbPixmap::rotatePixmap
      (xxx_width,xxx_rows,xxx_pixmap);
    GLsizei bheight = xxx_width;    
    GLsizei bwidth = xxx_rows;    
    float yorig = fXJustifyTranslation; //FIXME : box.xMin ???
    float xorig = bwidth + box.yMin + fYJustifyTranslation;
    if(aChar.fYMove==SbChar::UP) {
      xorig -= - HEIGHT*0.6F; //Same as SoTextHershey
    } else if(aChar.fYMove==SbChar::DOWN) {
      xorig -=   HEIGHT*0.6F; //Same as SoTextHershey
    }
    if(aChar.fBack==TRUE) {
      yorig += fRasterPos[1]-orp[1];
    }
    glBitmap(0,0,0,0,-xorig,-yorig,(const GLubyte*)0);
    if(pixmap) {
      glPixelStorei(GL_UNPACK_ROW_LENGTH,bwidth);
      glPixelStorei(GL_UNPACK_ALIGNMENT,4);
      glDrawPixels((GLsizei)bwidth,(GLsizei)bheight,
                   GL_RGBA,GL_UNSIGNED_BYTE,pixmap);
    }
    glBitmap(0,0,0,0,xorig+ymove,yorig+xmove,(const GLubyte*)0);

    if(aAction->isOfType(SoGL2PSAction::getClassTypeId())) {
      ((SoGL2PSAction*)aAction)->addBitmap
        (bwidth,bheight,xorig,yorig,ymove,xmove);
    }
    delete [] pixmap;

  } else {
    float xorig = - box.xMin + fXJustifyTranslation;
    float yorig = - box.yMin + fYJustifyTranslation;
    if(aChar.fYMove==SbChar::UP) {
      yorig += - HEIGHT*0.6F; //Same as SoTextHershey
    } else if(aChar.fYMove==SbChar::DOWN) {
      yorig +=   HEIGHT*0.6F; //Same as SoTextHershey
    }
    if(aChar.fBack==TRUE) {
      xorig += fRasterPos[0]-orp[0];
    }
    glBitmap(0,0,0,0,-xorig,-yorig,(const GLubyte*)0);
    if(xxx_pixmap) {
      glPixelStorei(GL_UNPACK_ROW_LENGTH,rasterMap.cols);
      glPixelStorei(GL_UNPACK_ALIGNMENT,4);
      glDrawPixels((GLsizei)xxx_width,(GLsizei)xxx_rows,
                   GL_RGBA,GL_UNSIGNED_BYTE,xxx_pixmap);
    }
    glBitmap(0,0,0,0,xorig+xmove,yorig+ymove,(const GLubyte*)0);
  
    if(aAction->isOfType(SoGL2PSAction::getClassTypeId())) {
      ((SoGL2PSAction*)aAction)->addBitmap(
        xxx_width,xxx_rows,xorig,yorig,xmove,ymove);
    }
  }

  delete [] xxx_pixmap;
  delete [] ((char*)rasterMap.buffer);

  //FIXME : same code as renderCharacter. Put it in common.
  //FIXME : smoothed ?
  if(aChar.fBar==TRUE) {
    unsigned int W = rasterMap.width;
    unsigned int H = box.yMax - box.yMin;
    H = int(float(H)*1.2);
    GLubyte* bitmap = (GLubyte*)SbPixmap::createBarBitmap(W,H);
    if(rotated.getValue()==TRUE) {
      unsigned int bytes = (W+7)/8; //bytes per line.  
      unsigned char* rbitmap = SbPixmap::rotateBitmap
        (W,H,bytes,(unsigned char*)bitmap);
      GLsizei bheight = W;    
      GLsizei bwidth = H;    
      float yorig = fXJustifyTranslation; //FIXME : box.xMin ???
      float xorig = bwidth + box.yMin + fYJustifyTranslation;
      if(aChar.fYMove==SbChar::UP) {
        xorig -= - HEIGHT*0.6F; //Same as SoTextHershey
      } else if(aChar.fYMove==SbChar::DOWN) {
        xorig -=   HEIGHT*0.6F; //Same as SoTextHershey
      }
      if(aChar.fBack==TRUE) {
        yorig += fRasterPos[1]-orp[1];
      }
      xorig += ymove;
      yorig += xmove;
      glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
      glPixelStorei(GL_UNPACK_ALIGNMENT,1);
      glBitmap(bwidth,bheight,xorig,yorig,0,0,(GLubyte*)rbitmap);
      if(aAction->isOfType(SoGL2PSAction::getClassTypeId())) {
        ((SoGL2PSAction*)aAction)->addBitmap(bwidth,bheight,xorig,yorig,0,0);
      }
      delete [] rbitmap;
    } else { //Not rotated.
      float xorig = - box.xMin + fXJustifyTranslation;
      float yorig = - box.yMin + fYJustifyTranslation;
      if(aChar.fYMove==SbChar::UP) {
        yorig += - HEIGHT*0.6F; //Same as SoTextHershey
      } else if(aChar.fYMove==SbChar::DOWN) {
        yorig +=   HEIGHT*0.6F; //Same as SoTextHershey
      }
      if(aChar.fBack==TRUE) {
        xorig += fRasterPos[0]-orp[0];
      }
      xorig += xmove;
      yorig += ymove;
      glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
      glPixelStorei(GL_UNPACK_ALIGNMENT,1);
      glBitmap(W,H,xorig,yorig,0,0,(GLubyte*)bitmap);
      if(aAction->isOfType(SoGL2PSAction::getClassTypeId())) {
        ((SoGL2PSAction*)aAction)->addBitmap(W,H,xorig,yorig,0,0);
      }
    }
    delete [] bitmap;
  }
}

SbBool SoTextTTF::getTextBounds(SbBox2s& aBox){
  update(NULL);
  if(!fTTF) {
    aBox.makeEmpty();
    return FALSE;
  }
  int linen = string.getNum();
  const SbString* lines = string.getValues(0);
  return fTTF->getBounds(linen,lines,encoding.getValue(),aBox);
}

int SoTextTTF::getAscent(){
  update(NULL);
  if(!fTTF) return 0;
  return fTTF->getAscent();
}

int SoTextTTF::getDescent(){
  update(NULL);
  if(!fTTF) return 0;
  return fTTF->getDescent();
}

SbTTF* SoTextTTF::getLatin() const {return fTTF;}

SbTTF* SoTextTTF::getGreek() const {return fTTFGreek;}

SbBool SoTextTTF::getQuad(
 SoState* aState
,SbVec3f& aV0
,SbVec3f& aV1
,SbVec3f& aV2
,SbVec3f& aV3
){
  SbBox2s box;
  if(!getTextBounds(box)) {
    aV0.setValue(0,0,0);
    aV1.setValue(0,0,0);
    aV2.setValue(0,0,0);
    aV3.setValue(0,0,0);
    return FALSE;
  }

  short szx,szy;
  box.getSize(szx,szy);
  if( (szx<=0) || (szy<=0) ) {
    aV0.setValue(0,0,0);
    aV1.setValue(0,0,0);
    aV2.setValue(0,0,0);
    aV3.setValue(0,0,0);
    return FALSE;
  }

  SbVec3f nilpoint(0,0,0);
  const SbMatrix& mat = SoModelMatrixElement::get(aState);
  mat.multVecMatrix(nilpoint, nilpoint);

  const SbViewVolume& vv = SoViewVolumeElement::get(aState);

  SbVec3f screenpoint;
  vv.projectToScreen(nilpoint, screenpoint);

  const SbViewportRegion& vp = SoViewportRegionElement::get(aState);
  SbVec2s vpsize = vp.getViewportSizePixels();
  
  // find normalized width and height of text
  float halfw = szx/2.F;
  float halfh = szy/2.F;

  halfw /= vpsize[0];
  halfh /= vpsize[1];

  SbVec2f n0 = SbVec2f(screenpoint[0]-halfw, screenpoint[1]-halfh);
  SbVec2f n1 = SbVec2f(screenpoint[0]+halfw, screenpoint[1]-halfh);
  SbVec2f n2 = SbVec2f(screenpoint[0]+halfw, screenpoint[1]+halfh);
  SbVec2f n3 = SbVec2f(screenpoint[0]-halfw, screenpoint[1]+halfh);

  // get distance from nilpoint to camera plane
  const SbVec3f & nm = vv.getPlane(0).getNormal();
  float dz = vv.getPlane(0).getDistanceFromOrigin();
  float dist = -(nilpoint.dot(nm)-dz);
  //float dist = -vv.getPlane(0).getDistance(nilpoint);
  
  // find the four image points in the plane
  aV0 = vv.getPlanePoint(dist, n0);
  aV1 = vv.getPlanePoint(dist, n1);
  aV2 = vv.getPlanePoint(dist, n2);
  aV3 = vv.getPlanePoint(dist, n3);

  // transform back to object space
  SbMatrix inv = mat.inverse();
  inv.multVecMatrix(aV0, aV0);
  inv.multVecMatrix(aV1, aV1);
  inv.multVecMatrix(aV2, aV2);
  inv.multVecMatrix(aV3, aV3);

  return TRUE;
}

/*FIXME : have a dynamic rescaling vs viewport size here ?
void SoTextTTF::setFont(SoAction* aAction){
  if(rescale.getValue()==FALSE) {
    setPointSize(size.getValue());
  } else {
    if(!aAction) return;
    SoState* state = aAction->getState();
    SbViewportRegion vpr = SoViewportRegionElement::get(state);
    const SbVec2s& win = vpr.getViewportSizePixels();
    int pointSize = 0; 
    int owin = viewportSize.getValue(); //pixels
    if(win[0]<=win[1]) {
      pointSize = (int)(size.getValue() * ((float)win[0] / (float)owin));
    } else {
      pointSize = (int)(size.getValue() * ((float)win[1] / (float)owin));
    }
    //WARNING : 
    //  due to (int) rounding, we may fall on 
    //  an oscilating situation oldPointSize <--> newPointSize
    //  inducing recursive call to GLRender.
    //  Then we disable notification on renderSize.
    SbBool flag = renderSize.enableNotify(FALSE);
    setPointSize(pointSize);
    renderSize.enableNotify(flag);
  }
}
*/

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText2.h>

void SoTextTTF::generateAlternateRep(SoAction*) {
  SoSeparator* sep = new SoSeparator;

  SoFont* soFont = new SoFont;
  soFont->name.setValue(fontName.getValue());
  int sz = size.getValue(); //in points
  soFont->size.setValue(sz);

  sep->addChild(soFont);
  SoText2* soText2 = new SoText2;
  sep->addChild(soText2);

  soText2->string = this->string;
  if(horizontalJustification.getValue()==CENTER) {
    soText2->justification.setValue(SoText2::CENTER);
  } else if(horizontalJustification.getValue()==LEFT) {
    soText2->justification.setValue(SoText2::LEFT);
  } else if(horizontalJustification.getValue()==RIGHT) {
    soText2->justification.setValue(SoText2::RIGHT);
  }

  //FIXME handle encoding.

  alternateRep.setValue(sep);
}
void SoTextTTF::clearAlternateRep() {
  alternateRep.setValue(NULL);
}

#include <HEPVis/actions/SoAlternateRepAction.h>

void SoTextTTF::doAction(SoAction* action) {
  SO_ALTERNATEREP_DO_ACTION(action)
  SoShape::doAction(action);
}

#include <HEPVis/nodes/SoSubNode.h>

void SoTextTTF::write(SoWriteAction* aAction){
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    // because of SoSFEnum.
    altRep->write(aAction);
  } else {
    SoShape::write(aAction);
  }
}
