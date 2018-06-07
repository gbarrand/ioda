/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoLineStyle                                             */
/* Author:           Guy Barrand                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoLineStyle_h
#define HEPVis_SoLineStyle_h

// Inheritance :
#include <Inventor/nodes/SoNode.h>

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFUShort.h>
#include <Inventor/fields/SoSFBool.h>

#include <HEPVis/SbStyles.h>
#include <HEPVis/SbStyle.h>

class SoLineStyle : public SoNode {
  SO_NODE_HEADER(SoLineStyle);
public:
  SoSFColor color;
  SoSFFloat transparency;
  SoSFInt32 width;
  SoSFUShort pattern;
  SoSFBool visible;
public:
  SoLineStyle();
  void reset();
  SbBool setFromString(const SbString&);
  SbStyle getSbStyle() const;
public:
  static void initClass();
protected:
  virtual ~SoLineStyle();
};

#endif
