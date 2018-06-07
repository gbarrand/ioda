#ifndef HEPVis_SoEllipsoidSegment_h
#define HEPVis_SoEllipsoidSegment_h

// Inheritance :
#include <Inventor/nodes/SoShape.h>

#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/fields/SoSFFloat.h>

#ifdef INVENTOR2_0
#include <Inventor/fields/SoSFLong.h>
#else
#include <Inventor/fields/SoSFInt32.h>
#endif

class SoSFNode;
//! SoEllipsoidSegment - Inventor version of various ellipsoid segments
/*!
 *  Node: SoEllipsoidSegment
 *
 *  Description:
 *
 *        Create Inventor node for the following entities
 *
 *           1) ellipsoidal segment 
 *
 *           2) spherical segment
 *
 *           3) ellipsoid
 *
 *           4) sphere
 *
 *  This class is based on SoTriangleStripSet in Inventor
 *
 *  Author: Xiaoling Fan, Northwestern University, Nov. 1996
 *  Modified by: Joe Boudreau, University of Pittsburgh...
 *
 *  Always use Inventor Fields. This allows Inventor to detect a change to
 *  the data field and take the appropriate action; e.g., redraw the scene.
 */
class SoEllipsoidSegment:public SoShape {

  //
  //! The following is required:
  //
  SO_NODE_HEADER(SoEllipsoidSegment);
 
public:

  //
  //! Semimajor axis
  //
  SoSFFloat    ax;

  //
  //! Semimajor axis
  //
  SoSFFloat    by;

  //
  //! Semimajor axis
  //
  SoSFFloat    cz;

  //
  //! Starting phi
  //
  SoSFFloat    fSPhi;

  //
  //! Delta phi
  //
  SoSFFloat    fDPhi;

  //
  //! Starting theta
  //
  SoSFFloat    fSTheta;

  //
  //! Delta Theta
  //
  SoSFFloat    fDTheta;

  //
  //! Number of sides
  //
#ifdef INVENTOR2_0
  SoSFLong    nSides;
#else
  SoSFInt32    nSides;
#endif

  //
  //! Alternate rep - required
  //
  SoSFNode     alternateRep;        // the alternate representation, required

  //
  //! Constructor, required
  //
  SoEllipsoidSegment();

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
  //! Generate Primitives, required
  //
  virtual void GLRender(SoGLRenderAction *action); 

  //
  //! GetChildList, required whenever the class has hidden children
  //
  virtual SoChildList* getChildren() const;


protected:

  //
  //! Destructor, required
  //
  virtual ~SoEllipsoidSegment();

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
