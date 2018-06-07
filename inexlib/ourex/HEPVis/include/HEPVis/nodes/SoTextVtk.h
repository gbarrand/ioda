#ifndef HEPVis_SoTextVtk_h
#define HEPVis_SoTextVtk_h

#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoShape.h>

class SoState;

class SoTextVtk : public SoShape {
  SO_NODE_HEADER(SoTextVtk);
public:
  // Fields
  SoMFString string;
  SoTextVtk();
public: /*SoEXTENDER*/ 
  virtual void GLRender(SoGLRenderAction*);
  virtual void rayPick(SoRayPickAction*);
  virtual void pick(SoPickAction*);
public:/*SoINTERNAL*/ 
  static void initClass();
protected:
  virtual void computeBBox(SoAction*,SbBox3f&,SbVec3f&);
  virtual void generatePrimitives(SoAction*);
  virtual ~SoTextVtk();
private:
  const char* fLetters[127];
  void renderString(SoAction*,const char*);
  void renderCharacter(SoAction*,char);
};

#endif
