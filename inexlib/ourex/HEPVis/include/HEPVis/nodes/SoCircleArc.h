#ifndef HEPVis_SoCircleArc_h
#define HEPVis_SoCircleArc_h

#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFVec3f.h>

class SoAction;
class SoRayPickAction;
class SbBox3f;

/*
 * Circle arc in the (x,y) plane centered at center with radius radius, 
 * that spans an angle sweepAngle in degrees starting along the x axis
 *
 * Laurent Duflot   April 2002
*/

class SoCircleArc : public SoShape
{
  SO_NODE_HEADER(SoCircleArc);

public:
  SoSFVec3f center;     // disk center
  SoSFFloat radius;     // circle radius
  SoSFFloat sweepAngle; // angle that the disk spans, starting along the x axis

  // initialize the class
  static void initClass();

  // constructor
  SoCircleArc();

protected:

  // generate disk from primitives
  virtual void generatePrimitives(SoAction* action);

  // computes bounding box
  virtual void computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center);

  virtual ~SoCircleArc() {}; 

private:
  static SbVec3f normal;

  static int NbTrianglesPerCircleArc;

};

#endif
