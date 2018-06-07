/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoTextRegion                                            */
/* Description:      A nodekit to handle text within a viewport region.      */
/* Author            Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoTextRegion_h
#define HEPVis_SoTextRegion_h

/*
 * SoTextRegion is a basekit to handle text within a viewport region.
 */

// Inheritance :
#include <HEPVis/nodekits/SoRegion.h>

#include <Inventor/fields/SoMFString.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFNode.h>

#include <vector>

class SoTextStyle;

class SoTextRegion : public SoRegion {
  SO_KIT_HEADER(SoTextRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(light);
  SO_KIT_CATALOG_ENTRY_HEADER(lightModel);
  SO_KIT_CATALOG_ENTRY_HEADER(camera);
  SO_KIT_CATALOG_ENTRY_HEADER(styleSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(lines);
  SO_KIT_CATALOG_ENTRY_HEADER(styles);
public:
  SoMFString text;
  enum HorizontalJustification {
    LEFT   = 0x00,
    RIGHT  = 0x01,
    CENTER = 0x02
  };
  SoSFEnum horizontalJustification;
  SoSFNode alternateRep;
public: /*SoINTERNAL*/
  static void initClass();
public:
  SoTextRegion();
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
public: //SoRegion
  virtual void updateChildren();
  virtual SoSeparator* getTopSeparator() const;
  virtual SoCamera* getCamera() const;
  virtual void resetCamera();
  virtual void clear(const SbString& = "");
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
  SoTextStyle* getStyle(int);
protected:
  virtual ~SoTextRegion();
private:
  void matchViewport(float,float);
  void setCharSize(float);
  void getInsideSize(float&,float&,float&,float&,float&,
                     std::vector<float>&);
private:
  short fOldWidth;
  short fOldHeight;
  SoTextStyle* fDefaultStyle;
  SoMFString* fOldText;
};


#endif
