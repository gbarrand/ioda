/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoDisplayRegion                                         */
/* Description:      A nodekit to handle 3D display within a viewport region.*/
/* Author            Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoDisplayRegion_h
#define HEPVis_SoDisplayRegion_h

/*
 *  See the applications/Plotter example.
 */

#include <HEPVis/nodekits/SoRegion.h>

#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFNode.h>

#include <vector>

class SoGroup;
class SoDirectionalLight;
class SoLightModel;
class SoTransform;

class SoSeparator;

class SoDisplayRegion : public SoRegion {
  SO_KIT_HEADER(SoDisplayRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(light);
  SO_KIT_CATALOG_ENTRY_HEADER(lightModel);
  SO_KIT_CATALOG_ENTRY_HEADER(cameraGroup);
  SO_KIT_CATALOG_ENTRY_HEADER(sceneSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(transform);
  SO_KIT_CATALOG_ENTRY_HEADER(dynamicScene);
  SO_KIT_CATALOG_ENTRY_HEADER(staticScene);
public:
  SoSFString projection; 
  SoSFNode alternateRep;
public: /*SoINTERNAL*/
  static void initClass();
public: /*SoEXTENDER*/
  virtual void doAction(SoAction*);
  virtual void write(SoWriteAction*);
/*virtual void callback(SoCallbackAction*);
  virtual void GLRender(SoGLRenderAction*);
  virtual void rayPick(SoRayPickAction*);
  virtual void pick(SoPickAction*);
  virtual void getBoundingBox(SoGetBoundingBoxAction*);*/
public:
  SoDisplayRegion();
  void setCameraType(SoType);
  void setCamera(SoCamera*);
  SoSeparator* getSceneSeparator() const;
  SoSeparator* getStaticScene() const;
  SoSeparator* getDynamicScene() const;
  SoDirectionalLight* getLight() const;
  SoLightModel* getLightModel() const;
  SoTransform* getTransform() const;
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
public: //SoRegion
  virtual void updateChildren();
  virtual SoSeparator* getTopSeparator() const;
  virtual SoCamera* getCamera() const;
  virtual void clear(const SbString& = "");
  virtual void resetCamera();
  virtual SbBool doIt(const SbDoIt&);
  virtual void undo();
  virtual void resetUndo();
  virtual SbBool connectFrom(SoRegion*);
  virtual SoNode* getNodeOfInterest(const SbString& = "") const;
  virtual SbBool readScene(const SbString&,
                           const SbString&,
                           const SbString& = "");
protected:
  virtual ~SoDisplayRegion();
private:
  std::vector< std::pair<SoGroup*,SoNode*> > fDoList;
};


#endif
