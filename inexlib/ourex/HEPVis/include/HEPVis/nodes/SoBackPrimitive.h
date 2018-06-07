#ifndef HEPVis_SoBackPrimitive_h
#define HEPVis_SoBackPrimitive_h

#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoSFFloat.h>

/**
 * SoBackPrimitive is used by SoPrimitive to represent 
 * the background of GUI node.
 */
class SoBackPrimitive : public SoShape {
  SO_NODE_HEADER(SoBackPrimitive);
public:
  /** half-length of x, at -dz */
  SoSFFloat dx1;                  
  /** half-length of x, at +dz */
  SoSFFloat dx2;
  /** half-length of y, at -dz */
  SoSFFloat dy1;
  /** half-length of y, at +dz */
  SoSFFloat dy2;
  /** half-length along Z */
  SoSFFloat dz;
public:
  SoBackPrimitive();
  static void initClass();
protected:
  virtual ~SoBackPrimitive();
  virtual void computeBBox(SoAction*,SbBox3f&,SbVec3f&);
  virtual void generatePrimitives(SoAction*);
};

#endif
