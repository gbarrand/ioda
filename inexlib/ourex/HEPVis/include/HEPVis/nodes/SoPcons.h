/*-----------------------------Hepvis----------------------------------------*/
/*                                                                           */
/* Node:             SoPcons                                                 */
/* Description:      Represents the G4PCon Geant Geometry entity             */
/*                   Meant as a replacement to SoPcon                        */
/* Author:           Joe Boudreau Nov 11 1996                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifndef HEPVis_SoPcons_h
#define HEPVis_SoPcons_h

// Inheritance :
#include <Inventor/nodes/SoShape.h>

#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFBool.h>

class SoSFNode;
//! SoPcons - Inventor version of the G4Cons Geant Geometry entity
/*!
 * Node:             SoPcons
 *
 * Description:      The Inventor version of the G4Cons Geant Geometry entity
 *
 * Author:             Joe Boudreau Nov 2004
 *

*/

class SoPcons:public SoShape {

  // The following is required:
  SO_NODE_HEADER(SoPcons);
 
public:

  //
  //! Inside radii
  //
  SoMFFloat fRmin;   
  //
  //! Outside radii
  //
  SoMFFloat fRmax;
  //
  //! Z Positions
  //
  SoMFFloat fDz;
  //
  //! Starting angle, in radians
  //
  SoSFFloat fSPhi;
  //
  //! Delta-angle, in radians
  //
  SoSFFloat fDPhi;             
  //
  //! An Inventor option - slightly better render, worse performance
  //
  SoSFBool  smoothDraw;
  //
  //! Alternate rep required - for use by users without HEPVis shared objects
  //
  SoSFNode  alternateRep;

  //
  //! Constructor, required
  //
  SoPcons();

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
  //! GetChildList, required whenever the class has hidden children
  //
  virtual SoChildList *getChildren() const;


protected:
  //
  //! Destructor, required
  //
  virtual ~SoPcons();

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
  //! help with trigonometry.  increments sines an cosines by an angle.
  //
  void inc(double & sinPhi, double & cosPhi, double sinDeltaPhi, double cosDeltaPhi) const {
    double oldSin=sinPhi,oldCos=cosPhi;
    sinPhi = oldSin*cosDeltaPhi+oldCos*sinDeltaPhi;
    cosPhi = oldCos*cosDeltaPhi-oldSin*sinDeltaPhi;    
  }
};

#endif
