#ifndef __SoSiWedgeStrips_hh__
#define __SoSiWedgeStrips_hh__
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoMFShort.h>
#include <Inventor/nodes/SoShape.h>

class SoSFNode;
//! SoSiWedgeStrips - Silicon strips for wedge-shaped detectors
/*!
 * Node:            SoSiWedgeStrips
 * 
 * Description:     Represents silicon strips for wedge-shaped detector
 * 
 * Author:            Peter Hamel
 * 
 * Always use Inventor Fields. This allows Inventor to detect a change to
 * the data field and take the appropriate action; e.g., redraw the scene.
 *
 */
class SoSiWedgeStrips:public SoShape {

  //
  //! This is required
  //
  SO_NODE_HEADER(SoSiWedgeStrips);
 
public:


  //
  //! Strip length
  //
  SoSFFloat length;   

  //
  //! Strip width
  //
  SoSFFloat width;

  //
  //! Strip thickness
  //
  SoSFFloat thickness;

  //
  //! Strip pitch (angle)
  //
  SoSFFloat pitch;

  //
  //! Strip offset
  //
  SoSFFloat offset;

  //
  //! Strip length
  //
  SoMFShort strip;

  //
  //! Strip adc
  //
  SoMFShort adc;

  //
  //! Strip gain
  //
  SoSFFloat gain;

  //
  //! Strip cutoff - strip number after which length decreases
  //
  SoSFShort cutoff;

  //
  //! Strip slope - length decrease per strip
  //
  SoSFFloat slope;

  //
  //! Strip tilt - offset needed to align tilted strips with 
  //! straight edge of wedge
  //
  SoSFFloat tilt;

  //
  //! Alternate rep - required
  //
  SoSFNode  alternateRep;

  //
  //! Constructor, required
  //
  SoSiWedgeStrips();

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
  virtual ~SoSiWedgeStrips();

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
  //! Private data  
  //
  float myLength;   
  float myWidth;
  float myThickness;
  float myPitch;
  float myOffset;
  float myStripEntries;
  float myADCEntries;
  float myGain;
  short myCutoff;
  float mySlope;
  float myTilt;

};

#endif
