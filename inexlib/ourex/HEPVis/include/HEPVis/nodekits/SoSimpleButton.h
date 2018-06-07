#ifndef HEPVis_SoSimpleButton_h
#define HEPVis_SoSimpleButton_h

/*
 *  SoSimpleButton.h
 *  
 *
 *  Created by Guy Barrand on Tue Apr 11 2006.
 *
 */

// Inheritance :
#include <Inventor/nodekits/SoBaseKit.h>

#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFString.h>

#include <Inventor/misc/SoCallbackList.h>

//#include <HEPVis/SbStyles.h>
#include <HEPVis/SbStyle.h>

class SoAreaStyle;
class SoTextStyle;
class SoStyleCache;
class SoEventCallback;

typedef void SoSimpleButtonCB(void*,SoNode*);

class SoSimpleButton : public SoBaseKit {
  SO_KIT_HEADER(SoSimpleButton);
  
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(backSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(labelSeparator);
  // Styles :
  SO_KIT_CATALOG_ENTRY_HEADER(backStyle);
  SO_KIT_CATALOG_ENTRY_HEADER(labelStyle);
public :
  SoSFNode alternateRep;
  SoSFBool verbose;
  SoSFFloat width;
  SoSFFloat height;
  SoSFString label;
  //SoSFFloat labelToBorder;
  //SoSFFloat labelHeight;
public:
  static void initClass();
public: /*SoEXTENDER*/
  virtual void doAction(SoAction*);
  virtual void callback(SoCallbackAction*);
  virtual void GLRender(SoGLRenderAction*);
  virtual void getBoundingBox(SoGetBoundingBoxAction*);
  virtual void getMatrix(SoGetMatrixAction*);
  virtual void handleEvent(SoHandleEventAction*);
  virtual void rayPick(SoRayPickAction*);
  virtual void search(SoSearchAction*);
  virtual void write(SoWriteAction*);
  virtual void pick(SoPickAction*);
public:
  SoSimpleButton();
  void updateChildren();
  SoAreaStyle* getBackStyle(); 
  SoTextStyle* getLabelStyle(); 

  void addCallback(SoSimpleButtonCB*,void* = NULL);
  void removeCallback(SoSimpleButtonCB*,void* = NULL);

  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();

  void invokeCallbacks();
protected:
  virtual ~SoSimpleButton();
private:
  static void eventCB(void*,SoEventCallback*);
private:
  float fOld_width;
  float fOld_height;
  SbString fOld_label;
  //float fOld_labelToBorder;
  //float fOld_labelHeight;

  SbStyle fOld_backStyle;
  SbStyle fOld_labelStyle;

  SoStyleCache* fStyleCache;
  SoCallbackList fCallbackList;
};

#endif
