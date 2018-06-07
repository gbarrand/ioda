/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoStyle                                                 */
/* Description:      A container to gather modeling parameters.              */
/* Author:           Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoStyle_h
#define HEPVis_SoStyle_h

// Inheritance :
#include <Inventor/nodes/SoNode.h>

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFUShort.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFVec3f.h>

#include <HEPVis/SbStyles.h>

class SbStyle;

class SoStyle : public SoNode {
  SO_NODE_HEADER(SoStyle);
public:
  SoSFColor color;
  SoSFFloat transparency;

  SoSFInt32 lineWidth;
  SoSFUShort linePattern;

  SoSFInt32 markerSize;
  enum MarkerStyle {
    DOT = SbMarkerDot,
    PLUS = SbMarkerPlus,
    ASTERISK = SbMarkerAsterisk,
    CROSS = SbMarkerCross,
    STAR = SbMarkerStar,
    CIRCLE_LINE = SbMarkerCircleLine,
    CIRCLE_FILLED = SbMarkerCircleFilled,
    TRIANGLE_UP_LINE = SbMarkerTriangleUpLine,
    TRIANGLE_UP_FILLED = SbMarkerTriangleUpFilled,
    TRIANGLE_DOWN_LINE = SbMarkerTriangleDownLine,
    TRIANGLE_DOWN_FILLED = SbMarkerTriangleDownFilled,
    SWISS_CROSS_LINE = SbMarkerSwissCrossLine,
    SWISS_CROSS_FILLED = SbMarkerSwissCrossFilled,
    DAVID_STAR_LINE = SbMarkerDavidStarLine,
    DAVID_STAR_FILLED = SbMarkerDavidStarFilled,
    DIAMOND_LINE = SbMarkerDiamondLine,
    DIAMOND_FILLED = SbMarkerDiamondFilled,
    SQUARE_LINE = SbMarkerSquareLine,
    SQUARE_FILLED = SbMarkerSquareFilled,
    PENTA_STAR_LINE = SbMarkerPentaStarLine,
    PENTA_STAR_FILLED = SbMarkerPentaStarFilled
  };
  SoSFEnum markerStyle;

  enum AreaStyle {
    SOLID = SbAreaSolid,
    HATCHED = SbAreaHatched,
    CHECKER = SbAreaChecker,
    EDGED = SbAreaEdged
  };
  SoSFEnum areaStyle;

  SoSFString fontName;
  SoSFInt32 fontSize;
  SoSFFloat scale;

  enum HatchingPolicy {
    NONE = SbHatchingNone,
    RIGHT = SbHatchingRight,
    LEFT = SbHatchingLeft,
    LEFT_AND_RIGHT = SbHatchingLeftAndRight
  };
  SoSFEnum hatching;

  SoSFFloat spacing;
  SoSFFloat angle;
  SoSFFloat offset;
  SoSFFloat stripWidth;

  SoSFBool visible;

  SoSFString modeling; 

  SoSFString lightModel; 

  SoSFString tickModeling; 

  SoSFString encoding; 

  SoSFBool smoothing;

  SoSFBool hinting;

  enum PaintingPolicy {
    UNIFORM = SbPaintingUniform,
    BY_VALUE = SbPaintingByValue,
    BY_LEVEL = SbPaintingByLevel,
    GREY_SCALE = SbPaintingGreyScale,
    VIOLET_TO_RED = SbPaintingVioletToRed,
    GREY_SCALE_INVERSE = SbPaintingGreyScaleInverse
  };
  SoSFEnum painting;

  SoSFInt32 multiNodeLimit;
  SoSFInt32 divisions;
  SoSFInt32 rotationSteps;

  SoSFFloat barOffset;
  SoSFFloat barWidth;

  SoSFBool editable;
  SoSFBool automated;

  SoSFString options;
  SoSFString colorMapping;
  SoSFBool enforced;

  SoSFVec3f translation;
public:
  SoStyle();
  void set(const SbStyle&);
  SbBool setFromString(const SbString&);
  SbStyle getSbStyle() const;
public:
  static void initClass();
protected:
  virtual ~SoStyle();
};

#endif
