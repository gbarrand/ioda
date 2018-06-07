//  20/08/1998 : G.Barrand : creation.

#ifndef HEPVis_SoGrid_h
#define HEPVis_SoGrid_h

// Inheritance :
#include <Inventor/nodes/SoShape.h>

#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>
#include <Inventor/fields/SoSFNode.h>

class SoGLRenderAction;

class SoGrid  : public SoShape {
  SO_NODE_HEADER(SoGrid);
 public:
  SoSFVec3f position;
  SoSFVec3f X;
  SoSFVec3f Y;
  SoSFInt32 rows;
  SoSFInt32 columns;
  SoSFFloat width;
  SoSFFloat height;
  SoSFNode  alternateRep;
 public:
  SoGrid ();
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
 protected:
  virtual  ~SoGrid();
 protected:
  virtual void computeBBox(SoAction*,SbBox3f&,SbVec3f&);
  virtual void doAction(SoAction*);
 public: /*SoINTERNAL*/
  static void initClass();
 protected:
  virtual void        generatePrimitives(SoAction*);
};

#endif
