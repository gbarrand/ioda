/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoRulerRegion                                           */
/* Description:      A nodekit to handle a ruler within a viewport region.   */
/* Author            Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoRulerRegion_h
#define HEPVis_SoRulerRegion_h

// Inheritance :
#include <HEPVis/nodekits/SoRegion.h>

#include <Inventor/fields/SoSFNode.h>

class SoAxis;
class SoSensor;
class SoFieldSensor;

class SoRulerRegion : public SoRegion {
  SO_KIT_HEADER(SoRulerRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(camera);
  SO_KIT_CATALOG_ENTRY_HEADER(transform);
  SO_KIT_CATALOG_ENTRY_HEADER(axis);
public:
  SoSFNode alternateRep;
public: /*SoINTERNAL*/
  static void initClass();
public:
  SoRulerRegion();
  void setMeasuredRegion(SoRegion*);
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
public: //SoRegion
  virtual void updateChildren();
  virtual SoSeparator* getTopSeparator() const;
  virtual SoCamera* getCamera() const;
  virtual SoAxis* getAxis() const;
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
protected:
  virtual ~SoRulerRegion();
private:
  void matchViewport(short,short);
  void setHorizontalLayout(float,float);
  void setVerticalLayout(float,float); 
  static void sensorCallback(SoRulerRegion*,SoSensor*);
private:
  short fOldWidth;
  short fOldHeight;
  SoRegion* fMeasuredRegion;
  SoFieldSensor* fCameraHeightSensor;
  SoFieldSensor* fCameraAspectRatioSensor;
  SoFieldSensor* fRegionSizeSensor;
};


#endif
