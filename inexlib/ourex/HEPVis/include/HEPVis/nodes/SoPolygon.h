#ifndef HEPVis_SoPolygon_h
#define HEPVis_SoPolygon_h

#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoMFVec2f.h>

class SoAction;
class SbBox3f;
class SbVec3f;

/*
 * Polygon in the (x,y) plane defined by the 2D vectors pointing
 * to the corners in counterclock order. By default, creates an hexagon with 
 * corners on a circle of radius 1 and centered on (0,0)
 *
 * Laurent Duflot   April 2002
*/

class SoPolygon : public SoShape
{
  SO_NODE_HEADER(SoPolygon);

public:
  // list of offsets for corners, in correct order for normals along +z
  SoMFVec2f corners; 

  // initialize the class
  static void initClass();

  // constructor
  SoPolygon();

protected:

  // generate quad
  virtual void generatePrimitives(SoAction* action);

  // computes bounding box
  virtual void computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center);

  virtual ~SoPolygon() {}; 

private:
  static SbVec3f normal;

};

#endif
