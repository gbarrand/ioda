#ifndef HEPVis_SoTextTTF_h
#define HEPVis_SoTextTTF_h

#include <Inventor/nodes/SoShape.h>

#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFNode.h>

#include <Inventor/SbColor.h>

class SbTTF;
class SbChar;
class SoState;
class SbBox2s;

/**
 *  SoTextTTF permits to render text in the pixmap buffer (like SoText2)
 * but by using the freetype fonts. It permits to have portable (X11, Win32)
 * rendering for scalable fonts.
 *  See the applications/TextTTF example.
 */

class SoTextTTF : public SoShape { //FIXME : inherit SoNode only.
  SO_NODE_HEADER(SoTextTTF);
public:
  enum HorizontalJustification {
    LEFT   = 0x00,
    CENTER = 0x01,
    RIGHT  = 0x02
  };
  enum VerticalJustification {
    BOTTOM = 0x00,
    MIDDLE = 0x01,
    TOP    = 0x02
  };
  // Fields
  SoSFString fontName;
  SoSFString encoding;
  SoMFString string;
  SoSFInt32 size;
  SoSFBool smoothing;
  SoSFBool hinting;
  SoSFEnum horizontalJustification;
  SoSFEnum verticalJustification;
  SoSFEnum align;
  SoSFBool rotated;

  SoSFInt32 rescale;
  SoSFNode alternateRep;
public:
  SoTextTTF();
  SbBool getTextBounds(SbBox2s&);
  int getAscent();
  int getDescent();
  SbTTF* getLatin() const;
  SbTTF* getGreek() const;
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
public: /*SoEXTENDER*/ 
  virtual void GLRender(SoGLRenderAction*);
  virtual void rayPick(SoRayPickAction*);
  virtual void pick(SoPickAction*);
  virtual void doAction(SoAction*);
  virtual void write(SoWriteAction*);
public:/*SoINTERNAL*/ 
  static void initClass();
protected:
  virtual ~SoTextTTF();
  virtual void computeBBox(SoAction*,SbBox3f&,SbVec3f&);
  virtual void generatePrimitives(SoAction*);
private:
  void update(SoAction*); 
  SbBool getQuad(SoState*,SbVec3f&,SbVec3f&,SbVec3f&,SbVec3f&);
  void renderString(SoAction*,const SbString&);
  void renderCharacter(SoAction*,const SbChar&,SbTTF&);
  void renderCharacterSmoothing(SoAction*,const SbChar&,SbTTF&);
private:
  static SbBool IsTTF(const SbString& aString) {
    if(aString.getLength()<4) return FALSE;
    if(aString[0]!='T') return FALSE;
    if(aString[1]!='T') return FALSE;
    if(aString[2]!='F') return FALSE;
    if(aString[3]!='/') return FALSE;
    return TRUE;
  }
private:
  SbTTF* fTTF;
  SbTTF* fTTFGreek;
  float fLineWidth;
  float fRasterPos[4];
  unsigned short fTextUP;
  unsigned short fTextWidth;
  unsigned short fTextHeight;
  float fXJustifyTranslation;
  float fYJustifyTranslation;
  SbBool fSmoothing;
  SbBool fHinting;
  SbColor fColor;
  SbString fOld_fontName;
  int fOld_size;
  SbVec2s fOld_win;
};

#endif
