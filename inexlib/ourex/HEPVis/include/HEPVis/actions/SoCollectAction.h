#ifndef HEPVis_SoCollectAction_h
#define HEPVis_SoCollectAction_h 

/**
 *  SoCollectAction inherits Inventor/SoCallbackAction and is aimed to 
 * collect shapes for which projection on the viewing plane is within
 * a given rectangle. SoCollectAction permits to do enlarged picking 
 * with an "elastic rubber band" rectangle.
 */

///////////////////////////////////////////////////////////////////////
//  With TGS Inventor on WIN32, some fields of SoCallbackAction
// like methods,  enabledElements are put in inv240.lib library with 
//   __declspec(dllimport)
// storage class attribute. In SoCollectAction that inherit
// SoCallbackAction they must be referenced with the same attribute.
//
// I found that TGS Inventor version 3.0 does not have this problem - JCK
///////////////////////////////////////////////////////////////////////
#if defined(SoFreePackage) || defined(__COIN__)
#include <Inventor/actions/SoCallbackAction.h>
#else // TGS
#ifdef INVENTOR2_1
#undef __IvStaticImport
#define __IvStaticImport __declspec(dllimport)
#include <Inventor/actions/SoCallbackAction.h>
#undef __IvStaticImport
#define __IvStaticImport
#else
#include <Inventor/actions/SoCallbackAction.h>
#endif
#endif

#include <Inventor/SbViewportRegion.h>
#include <Inventor/actions/SoSubAction.h>

#include <vector>

class SoCollectAction : public SoCallbackAction {
  SO_ACTION_HEADER(SoCollectAction);
public:
  SoCollectAction(const SbViewportRegion&);
  virtual ~SoCollectAction();
  void setSearchArea(int,int,unsigned int,unsigned int);
  SoPathList& paths();
public: /*SoINTERNAL*/
  static void initClass();
protected:
  virtual void beginTraversal(SoNode*);
private:
  static void viewportRegionAction(SoAction*,SoNode*);
  static void shapeTriangleCB(void*,SoCallbackAction*,
                              const SoPrimitiveVertex*,
                              const SoPrimitiveVertex*,
                              const SoPrimitiveVertex*);
  static void shapeLineSegmentCB(void*,SoCallbackAction*,
                                 const SoPrimitiveVertex*,
                                 const SoPrimitiveVertex*);
  static void shapePointCB(void*,SoCallbackAction*,
                           const SoPrimitiveVertex*);
  SbBool clip(const std::vector<SbVec3f>&);
  static unsigned long clipPoint(float,float);
  static SbBool clipLine(float,float,float,float);
private:
  SbViewportRegion fViewportRegion;
  SoPathList fPaths;
  SbBool fStopNode;
  SoNode* fCurrentNode;
  float fSearchViewport[4];
};

#endif

