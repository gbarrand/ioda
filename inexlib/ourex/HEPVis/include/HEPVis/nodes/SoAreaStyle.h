/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoAreaStyle                                             */
/* Author:           Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoAreaStyle_h
#define HEPVis_SoAreaStyle_h

// Inheritance :
#include <Inventor/nodes/SoNode.h>

#include <Inventor/nodes/SoSubNode.h>

#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFBool.h>

#include <HEPVis/SbStyles.h>
#include <HEPVis/SbStyle.h>

class SoAreaStyle : public SoNode {
  SO_NODE_HEADER(SoAreaStyle);
public:
  SoSFColor color;
  SoSFFloat transparency;
  enum AreaStyle {
    SOLID = SbAreaSolid,
    HATCHED = SbAreaHatched,
    CHECKER = SbAreaChecker,
    EDGED = SbAreaEdged
  };
  SoSFEnum areaStyle;
  SoSFBool visible;
public:
  SoAreaStyle();
  void reset();
  SbBool setFromString(const SbString&);
  SbStyle getSbStyle() const;
public:
  static void initClass();
protected:
  virtual ~SoAreaStyle();
};

#endif
