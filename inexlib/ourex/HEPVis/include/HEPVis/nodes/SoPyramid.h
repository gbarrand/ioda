#ifndef HEPVis_SoPyramid_h
#define HEPVis_SoPyramid_h

#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoSFFloat.h>

class SoPyramid : public SoShape {
  SO_NODE_HEADER(SoPyramid);
public:
  SoSFFloat backWidth;
  SoSFFloat backHeight;

  SoSFFloat frontWidth;
  SoSFFloat frontHeight;

  SoSFFloat depth;
public:
  SoPyramid();
  static void initClass();
protected:
  virtual ~SoPyramid();
  virtual void computeBBox(SoAction*,SbBox3f&,SbVec3f&);
  virtual void generatePrimitives(SoAction*);
};

#endif
