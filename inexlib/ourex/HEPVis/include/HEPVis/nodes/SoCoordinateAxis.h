/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoCoordinateAxis                                        */
/* Description:      draws a coordinate axis                                 */
/* Author:           T. Paul                                                 */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoCoordinateAxis_h
#define HEPVis_SoCoordinateAxis_h

// Inheritance :
#include <Inventor/nodes/SoShape.h>

#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>

class SoSeparator;

class SoCoordinateAxis : public SoShape {

  SO_NODE_HEADER(SoCoordinateAxis);
 
public:

  // Always use Inventor Fields. This allows Inventor to detect a change to
  // the data field and take the appropriate action; e.g., redraw the scene.

  SoSFInt32  fNDivision;            // number of divisions
  SoSFFloat fDivisionLength;       // length of a division in meters
  SoSFNode  alternateRep;          // the alternate representation, required

  SoCoordinateAxis();

  static void initClass();

  virtual void generateAlternateRep(SoAction*);

  virtual void clearAlternateRep();

  const char* getInfo();

protected:

  virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center );

  virtual void generatePrimitives(SoAction *action);

  virtual void GLRender(SoGLRenderAction *action); 

  virtual SoChildList *getChildren() const;

  virtual ~SoCoordinateAxis();

private: 

  void generateChildren();  

  void updateChildren();

  void makeDivision(SoSeparator*);

  SoChildList *children;
  
  //
  // Private data members
  // --------------------

  long  myNDivision;
  float myDivisionLength;

  SbString fInfo;
};

#endif
