#ifndef HEPVis_SoShell_h
#define HEPVis_SoShell_h

#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/fields/SoSFColor.h>

class SoShell : public SoBaseKit {
  SO_KIT_HEADER(SoShell);
  SO_KIT_CATALOG_ENTRY_HEADER(shellSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(shellMaterial);
  SO_KIT_CATALOG_ENTRY_HEADER(shellGrid);
  SO_KIT_CATALOG_ENTRY_HEADER(shellTransform);
  SO_KIT_CATALOG_ENTRY_HEADER(shellGroup);
public:
  SoSFColor background;
public:
  SoShell();
  void addChild(SoNode*);
public: /*SoEXTENDER*/
  virtual void doAction(SoAction*);
  virtual void callback(SoCallbackAction*);
  virtual void GLRender(SoGLRenderAction*);
  virtual void rayPick(SoRayPickAction*);
  virtual void pick(SoPickAction*);
  virtual void getBoundingBox(SoGetBoundingBoxAction*);
public:
  static void initClass();
protected:
  virtual ~SoShell();
};

#endif
