#ifndef HEPVis_SoArrow_h
#define HEPVis_SoArrow_h

#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoShape.h>

class SoSFNode;
//! SoArrow - Inventor version of the Arrow Geant Geometry entity
/*!
 *                                                                            
 * Node:             SoArrow                                                    
 *                                                                            
 * Description:     Represents the Arrow Geant Geometry entity              
 *                                                                            
 * Author:            Joe Boudreau Nov 11 1996                                    
 *                                                                            
 * An arrow has a cylinder and also a conical tip.  It is suitable for
 * representing vectors and the like.
 *
 * Always use Inventor Fields. This allows Inventor to detect a change to
 * the data field and take the appropriate action; e.g., redraw the scene.
*/

class SoArrow:public SoShape {

  //
  //! This is required
  //
  SO_NODE_HEADER(SoArrow);
 
public:

  //
  //! The tip
  //
  SoSFVec3f tip;

  //
  //! The tail
  //
  SoSFVec3f tail;

  //
  //! size of the shaft
  //
  SoSFFloat size;

  //
  //! if true the shaft will be a cone
  //
  SoSFBool  conicalShaft;

  //
  //! the alternate representation, required
  //
  SoSFNode  alternateRep;

  //
  //! Constructor, required
  //
  SoArrow();

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

  //
  //! Destructor, required
  //
  virtual ~SoArrow();

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

  //
  //! Cache this locally; if it doesn't change, don't regenerate.
  //
  SbVec3f cachedTip;

  //
  //! Cache these locally; if it doesn't change, don't regenerate.
  //
  SbVec3f cachedTail;

  //
  //! Cache these locally; if it doesn't change, don't regenerate.
  //
  SoSFFloat cachedSize;

  //
  //! Cache these locally; if it doesn't change, don't regenerate.
  //
  SoSFBool  cachedConicalShaft;
};

#endif
