/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoVtxReconTrack                                         */
/* Description:      Represents a VtxRecon Track                             */
/* Author:           Joe Boudreau Nov 7 1996                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoVtxReconTrack_h
#define HEPVis_SoVtxReconTrack_h

#include <Inventor/fields/SoSFNode.h>
#include <Inventor/nodes/SoShape.h>

class SbBox;
class SoSFNode;
class SoVtxReconTrack:public SoShape {

  // The following is required:
  SO_NODE_HEADER(SoVtxReconTrack);
 
public:


  SoSFFloat cotTheta;              // theta is scattering angle
  SoSFFloat phi0;                  // particle direction at closest approach
  SoSFNode  ellipsoid;             // the ellipsoid from which the track takes it errors
  SoSFFloat s0;                    // inner arclength
  SoSFFloat s1;                    // outer arclength
  SoSFNode alternateRep;           // the alternate representation, required

  // Constructor, required
  SoVtxReconTrack();

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

  // Compute Bounding Box, required
  virtual void computeBBox(SoAction *action, SbBox3f & box, SbVec3f &center);

  // Generate Primitives, required
  virtual void generatePrimitives(SoAction *action);

  // GLRender, required
  virtual void GLRender(SoGLRenderAction *action); 

  // GetChildList, required whenever the class has hidden children
  virtual SoChildList *getChildren() const;


protected:
  virtual ~SoVtxReconTrack();

private: 

  // Generate Children. Used to create the hidden children. Required whenever
  // the node has hidden children.  
  void generateChildren();  

  // Used to modify hidden children when a data field is changed. Required 
  // whenever the class has hidden children. 
  void updateChildren();

  // ChildList. Required whenever the class has hidden children.  
  SoChildList *children;

  // THE METHODS BELOW ARE PARTICULAR TO THE TRACK CLASS.  IN GENERAL
  // ONE CAN PUT ANY SORT OF HELPER FUNCTIONS IN PLACE OF THIS:

protected:

  // Retrieves to track position as a function of distance
  virtual SbVec3f getTrackPosition(float distance);

  // Retrieves to tangent
  virtual SbVec3f getTrackDirection();

  virtual void getAngle(float sig0, float sig1, float corr, float &angle, float &sig0prime, float &sig1prime);

};

#endif
