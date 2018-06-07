/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoPlotterRegion                                         */
/* Description:      A nodekit to handle a plotter within a viewport region. */
/* Author            Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoPlotterRegion_h
#define HEPVis_SoPlotterRegion_h

/*
 *  See the applications/Plotter example.
 */

// Inheritance :
#include <HEPVis/nodekits/SoRegion.h>

#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFNode.h>

#include <HEPVis/SbPlottableThings.h>

#include <vector>
#include <cstring> //::strcmp

class SbPlottableObject;
class SoPlotter;

class SbPlotterDoIt : public virtual SbDoIt {
public: //SbDoIt
  virtual void* cast(const SbString& aClass) const{
    if(!::strcmp(aClass.getString(),"SbPlotterDoIt")) 
      return (SbPlotterDoIt*)this;
    else return 0;
  }
public:
  SbPlotterDoIt(SbPlottableObject* aPlottableObject,SoPlotter* aPlotter = 0)
  :fPlottableObject(aPlottableObject),fPlotter(aPlotter)
  {}
  virtual ~SbPlotterDoIt(){delete fPlottableObject;}
private:
  SbPlotterDoIt(const SbPlotterDoIt&){}
  SbPlotterDoIt& operator=(const SbPlotterDoIt&){return *this;}
public:
  SbPlottableObject* getPlottableObject() const{return fPlottableObject;}
  SoPlotter* getPlotter() const {return fPlotter;}
  void reset(){fPlottableObject = 0;}
private:
  SbPlottableObject* fPlottableObject;
  SoPlotter* fPlotter;
};

class SoPlotter;
class SoViewportRegion;
class SoOrthographicCamera;
class SoTextRegion;
class SoInfosRegion;
class SoLegendRegion;
class SoSensor;
class SoFieldSensor;
class SoNodeSensor;
class SoMFString;
class SoCamera;

class SoPlotterRegion : public SoRegion {
  SO_KIT_HEADER(SoPlotterRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(plotterSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(light);
  SO_KIT_CATALOG_ENTRY_HEADER(lightModel);
  SO_KIT_CATALOG_ENTRY_HEADER(cameraGroup);
  SO_KIT_CATALOG_ENTRY_HEADER(plotter);
  SO_KIT_CATALOG_ENTRY_HEADER(overlays);
  SO_KIT_CATALOG_ENTRY_HEADER(titleRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(titleDummy);
  SO_KIT_CATALOG_ENTRY_HEADER(infosRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(infosDummy);
  SO_KIT_CATALOG_ENTRY_HEADER(legendRegion);
  SO_KIT_CATALOG_ENTRY_HEADER(legendDummy);
  SO_KIT_CATALOG_ENTRY_HEADER(etcSeparator);

  SO_KIT_CATALOG_ENTRY_HEADER(regionsSeparator);
  SO_KIT_CATALOG_ENTRY_HEADER(legendRegions);
public:
  SoSFNode alternateRep;
  SoSFBool titleRegionVisible;
  SoSFBool infosRegionVisible;
  SoSFBool legendRegionVisible;
  SoSFVec2f titleRegionOrigin;
  SoSFVec2f titleRegionSize;
  SoSFBool infosRegionAttachedToDataFrame;
  SoSFBool legendRegionAttachedToInfosRegion;
  SoSFVec2f infosRegionOrigin;
  SoSFVec2f infosRegionSize;
  SoSFVec2f legendRegionOrigin;
  enum Unit {
    PERCENT,
    AXIS
  };
  SoSFEnum legendRegionOriginUnit;
  SoSFVec2f legendRegionSize;
  SoSFString infosRegionStyle;
  enum ViewportMapping {
    ADJUST_VIEWPORT,
    LEAVE_ALONE
  };
  SoSFEnum plotterViewportMapping;
  enum ResizePolicy {
    RESIZE_HEIGHT,
    ASPECT_RATIO
  };
  SoSFEnum infosRegionResizePolicy;
  SoSFBool legendAutomated;
public: /*SoINTERNAL*/
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
public: //SoRegion
  virtual void updateChildren();
  virtual SoSeparator* getTopSeparator() const;
  virtual SoCamera* getCamera() const;
  virtual void clear(const SbString& = "");
  virtual void resetCamera();
  virtual void setCamera(SoCamera*);
  virtual void refresh();
  virtual SbBool doIt(const SbDoIt&);
  virtual void undo();
  virtual void resetUndo();
  virtual SoNode* getNodeOfInterest(const SbString& = "") const;
  virtual void placeInPage(float,float,float,float,float,float,float,float);
public:
  void setStyle(const SbString&);
  void setStyleLab_Default();
  void setStyleHippo_Default();
  void setStylePAW_Default();
  void setStyleROOT_Default();
  void setStyleROOT_Plain();
  void setStyleROOT_Bold();
  void setStyleROOT_Video();
  void setStyleROOT_Pub();
  void setStyleROOT_LHCb();
public:
  SoPlotterRegion();
protected:
  virtual ~SoPlotterRegion();
public:
  SoPlotter* getPlotter() const;
  SoTextRegion* getTitleRegion() const;
  SoInfosRegion* getInfosRegion() const;
  SoLegendRegion* getLegendRegion() const;
  SoSeparator* getEtcSeparator() const;
  //void addPlottableObject(SbPlottableObject*);
  int getNumberOfLegendRegions();
  int getNumberOfOverlays();
  SoPlotter* getOverlay(int);
  SoLegendRegion* getLegendRegion(int);
public:
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
private:
  void updateOneSoPlotter(SoPlotter*,float,float);
  void placeOneSoPlotterInPage(SoPlotter*,float,float,float,float,
                               float,float,float,float);
  void updateTitleRegionContent();
  void updateInfosContent(); 
  void updateLegendContent(); 
  void updateTitleRegionGeometry(); 
  void updateInfosRegionGeometry(const SbVec2f&); 
  //void updateLegendRegionGeometry(const SbVec2f&); 
  void updateLegendRegionGeometry(SoLegendRegion&,
                                  const SbVec2f& pos,
                                  const SbVec2f& size,SbBool = FALSE);
private:
  std::vector<SbPlottableObject*> fDoList;
  int fOldWidth;
  int fOldHeight;
  // SoPlotterRegion fields :
  SbBool fOld_titleRegionVisible;
  SbBool fOld_infosRegionVisible;
  SbBool fOld_legendRegionVisible;
  SbVec2f fOld_titleRegionOrigin;
  SbVec2f fOld_titleRegionSize;
  SbVec2f fOld_infosRegionOrigin;
  SbVec2f fOld_infosRegionSize;
  SbVec2f fOld_legendRegionOrigin;
  SbBool fOld_legendRegionOriginUnit;
  SbVec2f fOld_legendRegionSize;
  SbString fOld_infosRegionStyle;
  int fOld_plotterViewportMapping;
  int fOld_infosRegionResizePolicy;
  SbBool fOld_legendAutomated;
  // SoPlotter fieds :
  float fOldPlotter_height;
  float fOldPlotter_depth;
  int fOldPlotter_shape;
  SbString fOldPlotter_title;
  SoMFString* fOldPlotter_infos;
  SoMFString* fOldPlotter_legend;
  int fOld_numberOfLegendRegions;
};

#endif
