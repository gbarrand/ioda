#ifndef HEPVis_SoDisk_h
#define HEPVis_SoDisk_h

#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFVec2f.h>

class SoAction;
class SoRayPickAction;
class SbBox3f;
class SbVec3f;

/*
 * Disk in the (x,y) plane centered at center with radius radius, that spans
 * an angle sweepAngle in degrees starting along the x axis
 *
 * Laurent Duflot   April 2002
*/

class SoDisk : public SoShape
{
  SO_NODE_HEADER(SoDisk);

public:
  SoSFVec2f center;     // disk center
  SoSFFloat radius;     // disk radius
  SoSFFloat sweepAngle; // angle that the disk spans, starting along the x axis

  // initialize the class
  static void initClass();

  // constructor
  SoDisk();

protected:

  // generate disk from primitives
  virtual void generatePrimitives(SoAction* action);

  // computes bounding box
  virtual void computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center);

  // picking
  virtual void rayPick(SoRayPickAction * action);

  virtual ~SoDisk() {}; 

private:
  static SbVec3f normal;

  static int NbTrianglesPerDisk;

};

#endif
