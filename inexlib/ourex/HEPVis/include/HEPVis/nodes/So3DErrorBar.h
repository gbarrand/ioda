#ifndef __So3DErrorBar_hh_
#define __So3DErrorBar_hh_

#include <Inventor/SbBasic.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/nodes/SoShape.h>

class SoSFNode;
//! So3DErrorBar - 3D point with error bars in 3 orthogonal directions
/*!
 * Node:   So3DErrorBar         
 * 
 * Description: 3D point with error bars in 3 orthogonal directions    
 * 
 * Author: Lucas Taylor            
 * 
 * Always use Inventor Fields. This allows Inventor to detect a change to
 * the data field and take the appropriate action; e.g., redraw the scene.
 *
 */
class So3DErrorBar:public SoShape {

  //
  //! This is required
  //
  SO_NODE_HEADER(So3DErrorBar);
 
public:

  SoSFFloat lineWidth;   //!< line thickness in pixels
  SoSFVec3f u1;          //!< first  end of error bar in u dimension
  SoSFVec3f u2;          //!< second end of error bar in u dimension
  SoSFVec3f v1;          //!< first  end of error bar in v dimension
  SoSFVec3f v2;          //!< second end of error bar in v dimension
  SoSFVec3f w1;          //!< first  end of error bar in w dimension
  SoSFVec3f w2;          //!< second end of error bar in w dimension
 
  //
  //! Alternate rep - required
  //
  SoSFNode  alternateRep;
 
  //
  //! Constructor, required
  //
  So3DErrorBar();

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
  virtual ~So3DErrorBar();

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
  // Private data members
  // --------------------
 
  float   mylineWidth;
  SbVec3f myu1;
  SbVec3f myu2;
  SbVec3f myv1;
  SbVec3f myv2;
  SbVec3f myw1;
  SbVec3f myw2;
 
};

#endif
