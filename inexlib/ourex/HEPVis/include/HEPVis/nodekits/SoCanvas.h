/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoCanvas                                                */
/* Description:      To move, resize SoViewportRegion with the mous.         */
/* Author:           Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoCanvas_h
#define HEPVis_SoCanvas_h

/*
 * SoCanvas permits to move and resize SoViewportRegion
 * found under its topSeparator node.
 *  See the applications/Plotter example.
 */

#include <Inventor/fields/SoSFBool.h>

class SbVec2s;

class SoCamera;
class SbSetRegionCallback {
public:
  virtual ~SbSetRegionCallback(){}
public:
  virtual void setCamera(SoCamera*) = 0;
};

class SoCanvas;
class SoNodeList;
class SbCollectCallback {
public:
  virtual ~SbCollectCallback(){}
public:
  virtual void execute(const SoCanvas&,const SoNodeList&) = 0;
};

// Inheritance :
#include <Inventor/nodekits/SoBaseKit.h>

class SoEventCallback;
class SoEvent;
class SoViewportRegion;
class SoRegion;
class SoPathList;
class SoSearchAction;
class SbViewportRegion;

class SoCanvas : public SoBaseKit {
  SO_KIT_HEADER(SoCanvas);
  SO_KIT_CATALOG_ENTRY_HEADER(topSeparator);
public:
  SoSFBool verbose;
public: /*SoINTERNAL*/
  static void initClass();
public:
  SoCanvas();
protected:
  virtual ~SoCanvas();
public:
  SoSeparator* getTopSeparator() const;
  void setHandlePickedPoint(SbBool);
  SbBool handlePickedPoint() const;
  void setCollectCallback(SbCollectCallback*);
  SbCollectCallback* getCollectCallback() const;
  void collect(int,int,unsigned int,unsigned int,const SbViewportRegion&);
  // Region factories :
  SoRegion* createRegion(const SbString&,float,float,float,float);
  void deleteRegion(SoRegion*);
  void clearRegions();
  void deleteRegions();
  void raiseUpdateFlagOfRegions();
  void refreshRegions();
  int getNumberOfRegions() const;
  void setRootRegionsHighlightable(SbBool);
  SoRegion* getRegion(int) const;
  int getRegionIndex(SoRegion*) const;
  SoRegion* getNextRegion(SoRegion*) const;
  SoRegion* getPreviousRegion(SoRegion*) const;
  void setRegionOnTop(SoRegion*);
  SoRegion* getPathRegion(const SoFullPath&) const;
  SoRegion* getPickedRegion(int,int) const;
  void setSetRegionCallback(SbSetRegionCallback*);
  void disableRegionCamera();
  // Highlighted region :
  SoRegion* getHighlightedRegion() const;
  void highlightRegion(SoRegion*);
  void highlightNextRegion();
  void highlightPreviousRegion();
  void deleteHighlightedRegion();
  void setRegionsCanvasVerbosity();
  // Root regions :
  int getNumberOfRootRegions() const;
  SbBool isRootRegion(SoRegion*) const;
  SoRegion* getRootRegion(int) const;
  int getRootRegionIndex(SoRegion*) const;
  SoRegion* getNextRootRegion(SoRegion*) const;
  SoRegion* getPreviousRootRegion(SoRegion*) const;
  // Current region (if any, it is a highlighted region) :
  SoRegion* currentRegion() const;
  SbBool setCurrentRegion(int);
  SbBool setCurrentRegion(SoRegion*);
  SbBool connectCurrentRegion(int);
protected:
  SbBool dispatchEvent(const SoEvent*);
  //void enforceWinSize(const SbVec2s&);
  SoPathList& findRegions(SoSearchAction&) const;
private:
  static void eventCB(void*,SoEventCallback*);
  static void keyboardEventCB(void*,SoEventCallback*);
private:
  SbSetRegionCallback* fSetRegionCallback;
  SbCollectCallback* fCollectCallback;
  SbBool fHandlePickedPoint;
};

#endif
