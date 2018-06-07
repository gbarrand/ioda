/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoFormRegion                                            */
/* Description:      A nodekit to glue shapes within a viewport region.      */
/* Author            Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoFormRegion_h
#define HEPVis_SoFormRegion_h

/*
 * SoFormRegion is a basekit to handle shapes within a viewport region.
 */

#include <Inventor/fields/SoMFNode.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFNode.h>

// Inheritance :
#include <HEPVis/nodekits/SoRegion.h>

class SoFormRegion : public SoRegion {
  SO_KIT_HEADER(SoFormRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(light);
  SO_KIT_CATALOG_ENTRY_HEADER(lightModel);
  SO_KIT_CATALOG_ENTRY_HEADER(camera);
  SO_KIT_CATALOG_ENTRY_HEADER(boxes);
public:
  SoMFNode shapes;
  SoSFInt32 columns;
  SoSFInt32 rows;
  SoSFNode alternateRep;
public: /*SoINTERNAL*/
  static void initClass();
public:
  SoFormRegion();
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
public: //SoRegion
  virtual void updateChildren();
  virtual SoSeparator* getTopSeparator() const;
  virtual SoCamera* getCamera() const;
  virtual void resetCamera();
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
  virtual ~SoFormRegion();
private:
  void matchViewport(float,float);
private:
  short fOldWidth;
  short fOldHeight;
};


#endif
