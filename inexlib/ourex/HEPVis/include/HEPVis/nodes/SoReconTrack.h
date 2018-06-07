#ifndef HEPVis_SoReconTrack_h
#define HEPVis_SoReconTrack_h

#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/nodes/SoShape.h>

class SbBox;
class SoSFNode;
//! SoReconTrack - Represents a Recon trajectory with axis along z
/*!
 *
 * Node:             SoReconTrack
 *
 * Description:      Represents a Recon trajectory with axis along z
 *
 * Author:             Joe Boudreau Nov 7 1996
 *
 * Always use Inventor Fields. This allows Inventor to detect a change to
 * the data field and take the appropriate action; e.g., redraw the scene.
 *
 */
class SoReconTrack:public SoShape {

  //
  //! This is required:
  //
  SO_NODE_HEADER(SoReconTrack);
 
public:

  //
  //!theta is scattering angle
  //
  SoSFFloat cotTheta;              // theta is scattering angle

  //
  //! particle direction at closest approach
  //
  SoSFFloat phi0;                  // particle direction at closest approach

  //
  //! signed! positive if Lz>0
  //
  SoSFFloat d0;                    // signed! positive if Lz>0

  //
  //! z postition at closest approach
  //
  SoSFFloat z0;                    // z postition at closest approach

  //
  //! error in d0
  //
  SoSFFloat sigD0;                 // error in d0

  //
  //! error in z0
  //
  SoSFFloat sigZ0;                 // error in z0

  //
  //! d0-z0 correlation
  //
  SoSFFloat correlation;           // d0-z0 correlation;

  //
  //! inner arclength
  //
  SoSFFloat s0;                    // inner arclength

  //
  //! outer arclength
  //
  SoSFFloat s1;                    // outer arclength
 
  //
  //! Alternate rep - required
  //
  SoSFNode alternateRep;           // the alternate representation, required

  //
  //! Constructor, required
  //
  SoReconTrack();

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
  virtual void computeBBox(SoAction *action, SbBox3f & box, SbVec3f &center);

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
  virtual ~SoReconTrack();

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

protected:

  //! THE METHODS BELOW ARE PARTICULAR TO THE TRACK CLASS.  IN GENERAL
  //! ONE CAN PUT ANY SORT OF HELPER FUNCTIONS IN PLACE OF THIS:

  //
  //! Retrieves to track position as a function of distance
  //
  virtual SbVec3f getTrackPosition(float distance);

  //
  //! Gets rotation angle and eigenvalues from sigmas and correlations
  //
  virtual void getAngle(float &angle, float &sig0prime, float &sig1prime);

};

#endif
