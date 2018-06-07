#ifndef HEPVis_SoQuad_h
#define HEPVis_SoQuad_h

#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoSFVec2f.h>

class SoAction;
class SbBox3f;
class SbVec3f;

/*
 * Quadrilateral in the (x,y) plane defined by the 2D vectors pointing
 * to the corners
 *
 * Laurent Duflot   April 2002
*/

class SoQuad : public SoShape
{
  SO_NODE_HEADER(SoQuad);

public:
  SoSFVec2f LLCornerOffset; // lower left corner
  SoSFVec2f LRCornerOffset; // lower right corner
  SoSFVec2f URCornerOffset; // upper right corner
  SoSFVec2f ULCornerOffset; // upper left corner

  // initialize the class
  static void initClass();

  // constructor
  SoQuad();


protected:

  // generate quad
  virtual void generatePrimitives(SoAction* action);

  // computes bounding box
  virtual void computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center);

  virtual ~SoQuad() {}; 

private:
  static SbVec3f normal;

};

#endif
