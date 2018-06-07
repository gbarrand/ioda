//  20/08/1998 : G.Barrand : creation.

#ifndef HEPVis_SoTextHershey_h
#define HEPVis_SoTextHershey_h

// Inheritance :
#include <Inventor/nodes/SoShape.h>

#include <Inventor/SbBox.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFBitMask.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFNode.h>

#include <vector>

//#define SOTEXT_HERSHEY_CHILD_LIST

#ifdef SOTEXT_HERSHEY_CHILD_LIST
class SoChildList;
#endif

class SoTextHershey : public SoShape {
  SO_NODE_HEADER(SoTextHershey);
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
  enum Font {
    LATIN,
    GREEK,
    SPECIAL
  };
  SoMFString string;
  SoSFString encoding;
  SoSFEnum font;
  SoSFEnum horizontalJustification;
  SoSFEnum verticalJustification;
  SoSFFloat height;
  SoSFNode alternateRep;
public:
  SoTextHershey();
  void getTextBounds(SbBox3f&);
  void getTextBounds(int,const SbString*,SbBox3f&);
  float getAscent() const;
  float getDescent() const;
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
public: /*SoEXTENDER*/ 
  virtual void GLRender(SoGLRenderAction*);
#ifdef SOTEXT_HERSHEY_CHILD_LIST
  virtual void write(SoWriteAction*);
  virtual void callback(SoCallbackAction*);
#else
  virtual void rayPick(SoRayPickAction*);
  virtual void pick(SoPickAction*);
  virtual void write(SoWriteAction*);
  virtual void callback(SoCallbackAction*);
#endif
  virtual void doAction(SoAction*);
public:/*SoINTERNAL*/ 
  static void initClass();
protected:
  virtual void computeBBox(SoAction*,SbBox3f&,SbVec3f&);
  virtual void generatePrimitives(SoAction*);
  virtual ~SoTextHershey();
private:
  void generateSegments(std::vector<SbVec3f>&);
  void renderString(SbBool,const char*,float&,float&,std::vector<SbVec3f>&);
  float renderCharacter(SbBool,char,SoTextHershey::Font,float,SbBool,float,float,std::vector<SbVec3f>&);
  void getCharBound(char,SoTextHershey::Font,float,SbBool,SbBox3f&) const;
#ifdef SOTEXT_HERSHEY_CHILD_LIST
  void updateChildren();
private:
  SoChildList* fChildren;

  SoMFString fOld_string;
  SbString fOld_encoding;
  int fOld_font;
  int fOld_horizontalJustification;
  int fOld_verticalJustification;
  float fOld_height;
#endif
};

#endif
