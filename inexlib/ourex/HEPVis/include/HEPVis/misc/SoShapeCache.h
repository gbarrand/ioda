#ifndef HEPVis_SoShapeCache_h
#define HEPVis_SoShapeCache_h

// Inheritance :
#include <Inventor/nodes/SoGroup.h>

class SbPolyhedron;
class SoPolyhedron;

class SoShapeCache : public SoGroup {
public:
  SoShapeCache();
protected:
  virtual ~SoShapeCache();
public:
  SoPolyhedron* getPolyhedron(const SbPolyhedron&,SbBool,SbBool);
private:
  SoGroup* fPolyhedrons;
};

#endif
