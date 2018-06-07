/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoImageRegion                                           */
/* Description:      A nodekit to handle an image within a viewport region.  */
/* Author            Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoImageRegion_h
#define HEPVis_SoImageRegion_h

/*
 * SoImageRegion is a basekit to handle an image within a viewport region.
 *  See the applications/ImageRegion example.
 */

#include <HEPVis/nodekits/SoRegion.h>

#include <Inventor/fields/SoSFNode.h>

class SoViewportRegion;
class HEPVis_SoImage;

class SoImageRegion : public SoRegion {
  SO_KIT_HEADER(SoImageRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(camera);
  SO_KIT_CATALOG_ENTRY_HEADER(translation);
  SO_KIT_CATALOG_ENTRY_HEADER(image);
public:
  SoSFNode alternateRep;
public: /*SoINTERNAL*/
  static void initClass();
public:
  SoImageRegion();
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
public: //SoRegion
  HEPVis_SoImage* getImage() const;
  virtual void updateChildren();
  virtual SoSeparator* getTopSeparator() const;
  virtual SoCamera* getCamera() const;
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
protected:
  virtual ~SoImageRegion();
private:
  short fOldWidth;
  short fOldHeight;
  SbString fOldFileName;
};


#endif
