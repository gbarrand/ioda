/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoViewportRegion                                        */
/* Description:      To encapsulate glViewport.                              */
/* Author:           Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#ifndef HEPVis_SoViewportRegion_h
#define HEPVis_SoViewportRegion_h

#include <Inventor/nodes/SoSubNode.h>

#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFUShort.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFNode.h>

#include <Inventor/SbLinear.h>
#include <Inventor/SbViewportRegion.h>

class SoEvent;

class SoViewportRegion:public SoNode {
  SO_NODE_HEADER(SoViewportRegion);
public:
  enum Unit {
    PERCENT,
    PIXELS
  };
  SoSFBool visible;
  SoSFEnum originUnit;
  SoSFEnum sizeUnit;
  SoSFVec2f origin; // In percentage of window size.
  SoSFVec2f size; // In percentage of window size.
  SoSFShort horizontalBorder; // In pixels.
  SoSFShort verticalBorder; // In pixels.
  SoSFBool background;
  SoSFColor backgroundColor;
  SoSFColor borderColor;
  SoSFBool highlighted;
  SoSFColor highlightColor;
  SoSFShort horizontalHighlightBorder; // In pixels.
  SoSFShort verticalHighlightBorder; // In pixels.
  SoSFShort horizontalShadow; // In pixels.
  SoSFShort verticalShadow; // In pixels.
  SoSFBool sensitiveBorder;
  SoSFShort horizontalOriginOffset; // In pixels.
  SoSFShort verticalOriginOffset; // In pixels.
  SoSFNode alternateRep;
  SoSFBool showHighlight;
public:
  SoViewportRegion();
protected:
  virtual ~SoViewportRegion();
public:
  void reset();
  void setPositionPixels(int,int);
  void setSizePixels(unsigned int,unsigned int);
  void setPositionPercent(float,float);
  void setSizePercent(float,float);
  void getPositionPercent(float&,float&) const;
  void getSizePercent(float&,float&) const;
  void getPositionPixels(int&,int&) const;
  void getSizePixels(unsigned int&,unsigned int&) const;
  void setOriginUnit(Unit);
  void setSizeUnit(Unit);
  SbVec2s getBorderSize() const;
  SbBool isInside(int,int) const;
  SbBool isResizing() const;
  SbBool isMoving() const;
  SbBool dispatchEvent(const SoEvent*);
  //void enforceWinSize(const SbVec2s&);
public:
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
public: /*SoEXTENDER*/
  virtual void doAction(SoAction*);
  virtual void GLRender(SoGLRenderAction*);
  virtual void rayPick(SoRayPickAction*);
  virtual void pick(SoPickAction*);
  virtual void write(SoWriteAction*);
public: /*SoINTERNAL*/
  static void initClass();
private:
  SbBool fIsDragging;
  SbVec2s fDragPos;
  int fDragCorner;  
  Unit fDragOriginUnit;
  Unit fDragSizeUnit;
  SbVec2f fDragOldOrigin;
  SbVec2f fDragOldSize;
  //
  SbBool fShow;
  SbBool fShowOldVisible;
  SbVec2s fShowOldHighlightBorder;
  SbVec2s fShowOldBorder;
  //
  //SbVec2s fEnforcedWinSize;
  //
  SbVec2f f_renderSizePercent; //output. In percent of window size.
  SbVec2s f_renderSizePixels; //output. In pixels
  SbVec2f f_renderOriginPercent; //output. In percent of window size.
  SbVec2s f_renderOriginPixels; //output. In pixels
};

#endif
