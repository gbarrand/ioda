#ifndef HEPVis_SoPainterAction_h
#define HEPVis_SoPainterAction_h 

#include <Inventor/SbColor.h>
#include <Inventor/SbPList.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/actions/SoSubAction.h>

#include <HEPVis/SbPainter.h>

/**
 *  SoPainterAction inherits Inventor/SoCallbackAction and is aimed to render
 * a scene graph by using some native windowing graphic libraries like
 * X11 or the Win32/GDI drawing libraries.
 *  SoPainterAction traverses a scene graph and gives points, line segments,
 * triangles to abstract class SbPainter.
 *
 *  Three concrete classes of SbPainter exists today :
 * - SbPainterX11 : that uses the X11 drawing library.
 * - SbPainterW32 : that draws with the Win32/GDI.
 * - SbPainterPS : that produces PostScript.
 */

///////////////////////////////////////////////////////////////////////
//  With TGS Inventor on WIN32, some fields of SoCallbackAction
// like methods,  enabledElements are put in inv240.lib library with 
//   __declspec(dllimport)
// storage class attribute. In SoPainterAction that inherit
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

class SoPainterAction : public SoCallbackAction {
  SO_ACTION_HEADER(SoPainterAction);
public:
  SoPainterAction(const SbViewportRegion&,SbPainter*,SbBool=FALSE);
  void setPainter(SbPainter*);
  void setViewportRegion(const SbViewportRegion&);
  const SbViewportRegion& getViewportRegion() const;
  void setBackgroundColor(const SbColor&);
  void enableSort(SbBool);
public: /*SoINTERNAL*/
  static void initClass();
  void drawPrimitive(SbPrimitiveType,int,float*,float*,float*);
protected:
  virtual void beginTraversal(SoNode*);
private:
  SbViewportRegion fViewportRegion;
private:
  SbPainter* fPainter;
  SbColor fBackgroundColor;
  SbBool fSort;
  SbBool fCollect;
  int fNumber;
  void** fList;
  void accumulatePrimitive(SbPrimitiveType,
                           int,float*,float*,float*,
                           const SbPainterContext&);
  static void viewportRegionAction(SoAction*,SoNode*);
  void sortPrimitives();
};

#endif

