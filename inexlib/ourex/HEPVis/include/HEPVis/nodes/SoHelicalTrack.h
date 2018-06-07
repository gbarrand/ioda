#ifndef HEPVis_SoHelicalTrack_h
#define HEPVis_SoHelicalTrack_h

#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoShape.h>

class SbBox;
class SoSFNode;
//! SoHelicalTrack - Generates a helical track, with axis along Z
/*!
 * Node:            SoHelicalTrack
 * 
 * Description:     Generates a helical track, with axis along Z
 * 
 * Author:            Joe Boudreau Nov 7 1996
 * 
 * Always use Inventor Fields. This allows Inventor to detect a change to
 * the data field and take the appropriate action; e.g., redraw the scene.
 *
 */
class SoHelicalTrack:public SoShape {

  //
  //! This is required
  //
  SO_NODE_HEADER(SoHelicalTrack);
 
public:

  //
  //! Signed - positive if counterclockwise
  //
  SoSFFloat inverseRadius;

  //
  //! Theta is scattering angle
  //
  SoSFFloat cotTheta;

  //
  //! Particle direction at closest approach
  //
  SoSFFloat phi0;

  //
  //! Signed - positive if Lz>0
  //
  SoSFFloat d0;

  //
  //! Z position at closest approach
  //
  SoSFFloat z0;

  //
  //! Inner arc length
  //
  SoSFFloat s0;

  //
  //! Outer arclength
  //
  SoSFFloat s1;

  //
  //! Alternate rep - required
  //
  SoSFNode alternateRep;

  //
  //! Constructor, required
  //
  SoHelicalTrack();

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
  //! GetChildList, required whenever the class has hidden children
  //
  virtual SoChildList *getChildren() const;


protected:

  //
  //! Destructor, required
  //
  virtual ~SoHelicalTrack();

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

  //
  //! Retrieves to track position as a function of distance
  //
  virtual void getTrackPosition(int i, float s0, float deltaS, SbVec3f & pos);

  //! THE METHODS BELOW ARE PARTICULAR TO THE TRACK CLASS.  IN GENERAL
  //! ONE CAN PUT ANY SORT OF HELPER FUNCTIONS IN PLACE OF THIS:

private:

  double _cosTheta;
  double _sinTheta;
  double _cosPhi0;
  double _sinPhi0;
  double _cosPhi1;
  double _sinPhi1;
  double _cosDeltaPhi;
  double _sinDeltaPhi;
  double _w;
  double _D0;
  double _Z0;

  //
  //! help with trigonometry.  increments sines an cosines by an angle.
  //
  void inc(double & sinPhi, double & cosPhi, double sinDeltaPhi, double cosDeltaPhi) const {
    double oldSin=sinPhi,oldCos=cosPhi;
    sinPhi = oldSin*cosDeltaPhi+oldCos*sinDeltaPhi;
    cosPhi = oldCos*cosDeltaPhi-oldSin*sinDeltaPhi;    
  }
    
};

#endif
