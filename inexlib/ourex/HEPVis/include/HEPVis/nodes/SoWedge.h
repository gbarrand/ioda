/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoWedge                                                 */
/* Description:      Just a plain, ugly wedge                                */
/* Author:           Peter Hamel                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef __SoWedge_hh__
#define __SoWedge_hh__
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoMFShort.h>
#include <Inventor/nodes/SoShape.h>

class SoSFNode;
class SoWedge : public SoShape {

  // The following is required:
  SO_NODE_HEADER(SoWedge);

public:

  // Always use Inventor Fields. This allows Inventor to detect a change to
  // the data field and take the appropriate action; e.g., redraw the scene.

  SoSFFloat halfWidth1;
  SoSFFloat halfWidth2;
  SoSFFloat halfHeight;
  SoSFFloat thickness;
  SoSFNode  alternateRep;          // the alternate representation, required

  // Constructor, required
  SoWedge();

  // Class Initializer, required
  static void initClass();

  // Generate AlternateRep, required.  Generating an alternate representation
  // must be done upon users request.  It allows an Inventor program to read
  // back the file without requiring *this* code to be dynamically linked. 
  // If the users expects that *this* code will be dynamically linked, he
  // need not invoke this method.  
  virtual void generateAlternateRep(SoAction*);

  // We better be able to clear it, too!
  virtual void clearAlternateRep();

protected:

  // computeBBox, required
  virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center );

  // Generate Primitives, required
  virtual void generatePrimitives(SoAction *action);

  // GLRender, required
  virtual void GLRender(SoGLRenderAction *action); 

  // GetChildList, required whenever the class has hidden children
  virtual SoChildList *getChildren() const;


protected:
  // Destructor.  Required.  Always Private.
  virtual ~SoWedge();

private: 
  // Generate Children. Used to create the hidden children. Required whenever
  // the node has hidden children.  
  void generateChildren();  

  // Used to modify hidden children when a data field is changed. Required 
  // whenever the class has hidden children. 
  void updateChildren();

  // ChildList. Required whenever the class has hidden children.  
  SoChildList *children;

  float myHalfHeight;   
  float myHalfWidth1;
  float myHalfWidth2;
  float myThickness;

};

#endif
