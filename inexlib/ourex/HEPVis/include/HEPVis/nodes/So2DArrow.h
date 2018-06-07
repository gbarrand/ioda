#ifndef HEPVis_So2DArrow_h
#define HEPVis_So2DArrow_h

#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoSFFloat.h>

class SoAction;
class SbBox3f;
class SbVec3f;

/*
 * an arrow in the (x,y) plane along the x axis, with total length length. 
 * The arrowhead heigth is headHeight and its length lengthRatio*length
 *
 * Laurent Duflot   April 2002
*/

class So2DArrow : public SoShape
{
  SO_NODE_HEADER(So2DArrow);

public:
  SoSFFloat length;        // total length including head
  SoSFFloat lengthRatio;   // fraction of the length for the head
  SoSFFloat headHeight;    // width of the head

  // initialize the class
  static void initClass();

  // constructor
  So2DArrow();

protected:

  // generate arrow from primitives 
  virtual void generatePrimitives(SoAction* action);

  // computes bounding box
  virtual void computeBBox(SoAction* action, SbBox3f& box, SbVec3f& center);

  virtual ~So2DArrow() {}; 

private:
  static SbVec3f normal;
};

#endif
