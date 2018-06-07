/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoTextStyle                                             */
/* Author:           Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoTextStyle_h
#define HEPVis_SoTextStyle_h

// Inheritance :
#include <Inventor/nodes/SoNode.h>

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFUShort.h>

#include <HEPVis/SbStyles.h>
#include <HEPVis/SbStyle.h>

class SoTextStyle : public SoNode {
  SO_NODE_HEADER(SoTextStyle);
public:
  enum HorizontalJustification {
    LEFT   = 0x00,
    CENTER = 0x01,
    RIGHT  = 0x02
  };
  enum VerticalJustification {
    BOTTOM = 0x00,
    MIDDLE = 0x01,
    TOP    = 0x02
  };
  SoSFColor color;
  SoSFFloat transparency;
  SoSFString fontName;
  SoSFInt32 fontSize;
  //SoSFFloat angle;
  SoSFBool visible;
  SoSFString encoding; 
  SoSFBool smoothing;
  SoSFBool hinting;
  SoSFEnum horizontalJustification;
  SoSFEnum verticalJustification;
  SoSFFloat scale;
  // For 3D text :
  SoSFVec3f orientationX;
  SoSFVec3f orientationY;
  // For bitmap text :
  SoSFBool rotated;
  // Case font is Hershey :
  SoSFInt32 lineWidth;
  SoSFUShort linePattern;
  SoSFBool enforced;
  SoSFVec3f translation;
public:
  SoTextStyle();
  void reset();
  SbBool setFromString(const SbString&);
  SbStyle getSbStyle() const;
public:
  static void initClass();
protected:
  virtual ~SoTextStyle();
};

#endif
