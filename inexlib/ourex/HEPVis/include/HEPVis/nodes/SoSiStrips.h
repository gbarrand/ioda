#ifndef HEPVis_SoSiStrips_h
#define HEPVis_SoSiStrips_h

#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoMFShort.h>
#include <Inventor/nodes/SoShape.h>

class SoSFNode;
//! SoSiStrips - Silicon strip detector
/*!
 * Node:            SoSiStrips
 * 
 * Description:     Represents a silicon strip detector
 * 
 * Author:            Joe Boudreau Nov 11 1996
 * 
 * Always use Inventor Fields. This allows Inventor to detect a change to
 * the data field and take the appropriate action; e.g., redraw the scene.
 *
 */
class SoSiStrips:public SoShape {

  //
  //! This is required
  //
  SO_NODE_HEADER(SoSiStrips);
 
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
  //! Alternate rep - required
  //
  SoSFNode  alternateRep;

  //
  //! Constructor, required
  //
  SoSiStrips();

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
  virtual ~SoSiStrips();

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
  float myGain;
  SoMFShort myADCEntries;
  SoMFShort myStripEntries;
};

#endif
