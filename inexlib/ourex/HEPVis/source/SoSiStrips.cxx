/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoSiStrips                                              */
/* Description:      Represents a single sided silicon strip detector        */
/* Author:           Joe Boudreau Nov 11 1996                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <assert.h>
#include <math.h>
#include <Inventor/SbBox.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTranslation.h>
#include "HEPVis/nodes/SoSiStrips.h"

// This statement is required
SO_NODE_SOURCE(SoSiStrips)

// Constructor
SoSiStrips::SoSiStrips():
myLength(0.0),
myWidth(0.0),
myThickness(0.0),
myPitch(0.0),
myOffset(0.0),
myGain(0.0){
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoSiStrips);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(thickness,                (0.03F));
  SO_NODE_ADD_FIELD(width,                    (1.28F));
  SO_NODE_ADD_FIELD(length,                    (8));
  SO_NODE_ADD_FIELD(pitch,                   (0.005F));
  SO_NODE_ADD_FIELD(offset,                  (0));
  SO_NODE_ADD_FIELD(strip,                    (0));
  SO_NODE_ADD_FIELD(adc,                      (0));
  SO_NODE_ADD_FIELD(gain,                    (0.001F));
  SO_NODE_ADD_FIELD(alternateRep,             (NULL));
  children = new SoChildList(this);
}

// Destructor
SoSiStrips::~SoSiStrips() {
  delete children;
}


// initClass
void SoSiStrips::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoSiStrips,SoShape,"Shape");
}

// GLRrender
void SoSiStrips::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();
  if ((myLength    !=length.getValue()) ||
      (myWidth     !=width.getValue()) ||
      (myThickness !=thickness.getValue()) ||
      (myPitch     !=pitch.getValue()) ||
      (myOffset    !=offset.getValue()) ||
      (myGain      !=gain.getValue() )  ||
      (!(myStripEntries==strip))  ||
      (!(myADCEntries==adc))){ 

    updateChildren();
    myLength=length.getValue();
    myWidth=width.getValue();
    myThickness=thickness.getValue();
    myPitch=pitch.getValue();
    myOffset=offset.getValue();
    myStripEntries.setValues(0,strip.getNum(),strip.getValues(0));
    myADCEntries.setValues(0,adc.getNum(),adc.getValues(0));
    myGain=gain.getValue();
  }
  children->traverse(action);  
}
// generatePrimitives
void SoSiStrips::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  if ((myLength    !=length.getValue()) ||
      (myWidth     !=width.getValue()) ||
      (myThickness !=thickness.getValue()) ||
      (myPitch     !=pitch.getValue()) ||
      (myOffset    !=offset.getValue()) ||
      (myGain      !=gain.getValue() )  ||
      (!(myStripEntries==strip))  ||
      (!(myADCEntries==adc))){ 


    updateChildren();
    myLength=length.getValue();
    myWidth=width.getValue();
    myThickness=thickness.getValue();
    myPitch=pitch.getValue();
    myOffset=offset.getValue();
    myStripEntries.setValues(0,strip.getNum(),strip.getValues(0));
    myADCEntries.setValues(0,adc.getNum(),adc.getValues(0));
    myGain=gain.getValue();
  }
  children->traverse(action);  
}


// getChildren
SoChildList *SoSiStrips::getChildren() const {
  return children;
}


// computeBBox
void SoSiStrips::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){
  SbVec3f vmin(-length.getValue()/2,
	       -length.getValue()/2,
	       -length.getValue()/2), 
          vmax( length.getValue()/2, 
		length.getValue()/2, 
		length.getValue()/2);
  center.setValue(0,0,0);
  box.setBounds(vmin,vmax);
}




// updateChildren
void SoSiStrips::updateChildren() {


  // Redraw the G4Box....

  assert(children->getLength()==1);
  SoGroup* lod = (SoGroup*)(*children)[0];

  // Wipe out all of the hits. If data and address space aren't equal:
  // its either because A) Someone is changing them and the data isn't
  // ready yet, or B) Someone made a mistake. In either case don't render
  // any hits.

  lod->removeAllChildren();
  if (strip.getNum()!=adc.getNum()) return;
  SoSeparator *sep = new SoSeparator();
  lod->addChild(sep);


  SoTranslation *TheTranslation = new SoTranslation();
  TheTranslation->translation.setValue(0, -width.getValue()/2 + offset.getValue(),0);
  sep->addChild(TheTranslation);

  for (int i=0; i<strip.getNum();i++) {
    SoSeparator   *pulseSep = new SoSeparator();
    sep->addChild(pulseSep);
    SoTranslation *inc = new SoTranslation();
    inc->translation.setValue(0,pitch.getValue()*strip[i],0);
    pulseSep->addChild(inc);
    SoCube  *hitCube  = new SoCube();
    hitCube->width.setValue(thickness.getValue()+adc[i]*gain.getValue());
    hitCube->height.setValue(pitch.getValue());
    hitCube->depth.setValue(length.getValue()-pitch.getValue());
    pulseSep->addChild(hitCube);

  }

}

// generateChildren
void SoSiStrips::generateChildren() {

  // A box consists of a set of scale factors and a 
  // cube.

  assert(children->getLength() ==0);
  SoSeparator      *sep              = new SoSeparator;
  children->append(sep);
}

// generateAlternateRep
void SoSiStrips::generateAlternateRep(SoAction*) {

  // This routine sets the alternate representation to the child
  // list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoNode*)(*children)[0]);
}

// clearAlternateRep
void SoSiStrips::clearAlternateRep() {
  alternateRep.setValue(NULL);
}

