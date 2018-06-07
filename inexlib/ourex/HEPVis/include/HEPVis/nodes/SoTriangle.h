#ifndef HEPVis_SoTriangle_h
#define HEPVis_SoTriangle_h

#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoSFVec2f.h>

class SoAction;
class SbBox3f;
class SbVec3f;

/*
 * Triangle in the (x,y) plane defined by the 2D vectors pointing
 * to the corners
 *
 * Laurent Duflot   April 2002
*/

class SoTriangle : public SoShape
{
  SO_NODE_HEADER(SoTriangle);

public:
  // corners
  SoSFVec2f Corner1Offset;
  SoSFVec2f Corner2Offset;
  SoSFVec2f Corner3Offset;

  // initialize the class
  static void initClass();

  // constructor
  SoTriangle();

protected:

  // generate triangle
  virtual void generatePrimitives(SoAction* action);

  // computes bounding box
  virtual void computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center);

  virtual ~SoTriangle() {}; 

private:
  static SbVec3f normal;
};

#endif
