#ifndef HEPVis_SoEllipticalPrism_h
#define HEPVis_SoEllipticalPrism_h

#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/fields/SoSFVec2f.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodes/SoShape.h>

class SoSFNode;
//! SoEllipticalPrism - Inventor version of an elliptical prism
/*!
 * Node:            SoEllipticalPrism
 * 
 * Description:     Inventor version of an elliptical prism
 * 
 * Author:            Joe Boudreau Nov 11 1996
 * 
 * Always use Inventor Fields. This allows Inventor to detect a change to
 * the data field and take the appropriate action; e.g., redraw the scene.
 *
 */
class SoEllipticalPrism:public SoShape {

  //
  //! This is required
  //
  SO_NODE_HEADER(SoEllipticalPrism);
 
public:

  //
  //! Shape parameters
  //
  SoSFVec2f    eigenvalues;   
  SoSFFloat    extent;

  //
  //! The rotation
  //
  SoSFRotation rotation;

  //
  //! Center of the prism
  //
  SoSFVec3f    center;

  //
  //! Alternate rep - required
  //
  SoSFNode     alternateRep;          // the alternate representation, required

  //
  //! Constructor, required
  //
  SoEllipticalPrism();

  //
  //! Class Initializer, required
  //
  static void initClass();

  //
  //! Generate AlternateRep, required.  Generating an alternate representation
  //! must be done upon users request.  It allows an Inventor program to read
  //! back the file without requiring *this* code to be dynamically linked. 
  //! If the users expects that *this* code will be dynamically linked, he
  //! need not invoke this method.  
  //
  virtual void generateAlternateRep(SoAction*);

  //
  //! We better be able to clear it, too!
  //
  virtual void clearAlternateRep();

protected:

  //
  //! compute bounding Box, required
  //
  virtual void computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center );

  //
  //! Generate Primitives, required
  //
  virtual void generatePrimitives(SoAction *action);

  //
  //! GLRender, required
  //
  virtual void GLRender(SoGLRenderAction *action); 

  //
  //! GetChildList, required whenever the class has hidden children
  //
  virtual SoChildList *getChildren() const;


protected:

  //
  //! Destructor, required
  //
  virtual ~SoEllipticalPrism();

private: 

  //
  //! Generate Children. Used to create the hidden children. Required whenever
  //! the node has hidden children.  
  //
  void generateChildren();  

  //
  //! Used to modify hidden children when a data field is changed. Required 
  //! whenever the class has hidden children. 
  //
  void updateChildren();

  //
  //! ChildList. Required whenever the class has hidden children.  
  //
  SoChildList *children;
};
#endif
