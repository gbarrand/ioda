/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoSiWedgeStrips                                         */
/* Description:      Represents Silicon Strips for Wedge Shaped Detectors    */
/* Author:           Peter Hamel                                             */
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
#include <HEPVis/nodes/SoSiWedgeStrips.h>

// This statement is required
SO_NODE_SOURCE(SoSiWedgeStrips)

// Constructor
SoSiWedgeStrips::SoSiWedgeStrips() {
  // This statement is required
  SO_NODE_CONSTRUCTOR(SoSiWedgeStrips);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(thickness,                (0.03F));
  SO_NODE_ADD_FIELD(width,                    (1.28F));
  SO_NODE_ADD_FIELD(length,                    (8));
  SO_NODE_ADD_FIELD(pitch,                   (0.005F));
  SO_NODE_ADD_FIELD(offset,                  (0.000F));
  SO_NODE_ADD_FIELD(strip,                       (0));
  SO_NODE_ADD_FIELD(adc,                         (0));
  SO_NODE_ADD_FIELD(gain,                    (0.001F));
  SO_NODE_ADD_FIELD(cutoff,                      (0));
  SO_NODE_ADD_FIELD(slope,                   (0));
  SO_NODE_ADD_FIELD(tilt,                    (0));
  SO_NODE_ADD_FIELD(alternateRep,             (NULL));
  children = new SoChildList(this);
}

// Destructor
SoSiWedgeStrips::~SoSiWedgeStrips() {
}


// initClass
void SoSiWedgeStrips::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoSiWedgeStrips,SoShape,"Shape");
}

// GLRrender
void SoSiWedgeStrips::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();

  if ((myLength    !=length.getValue()) ||
      (myWidth     !=width.getValue()) ||
      (myThickness !=thickness.getValue()) ||
      (myPitch     !=pitch.getValue()) ||
      (myOffset    !=offset.getValue()) ||
      (myGain      !=gain.getValue() )  ||
      (myCutoff    !=cutoff.getValue() ) ||
      (mySlope     !=slope.getValue() )  ||
      (myTilt      !=tilt.getValue() )  ||
      (!(myStripEntries==strip.getNum()))  ||
      (!(myADCEntries==adc.getNum()))){ 


    updateChildren();
  }
  children->traverse(action);
  myLength=length.getValue();
  myWidth=width.getValue();
  myThickness=thickness.getValue();
  myPitch=pitch.getValue();
  myOffset=offset.getValue();
  myStripEntries=(float)strip.getNum();
  myADCEntries=(float)adc.getNum();
  myGain=gain.getValue();
  myCutoff=cutoff.getValue();
  mySlope=slope.getValue();
  myTilt=tilt.getValue();
}

// generatePrimitives
void SoSiWedgeStrips::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}


// getChildren
SoChildList *SoSiWedgeStrips::getChildren() const {
  return children;
}


// computeBBox
void SoSiWedgeStrips::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){
  SbVec3f mn(-length.getValue()/2,-length.getValue()/2,-length.getValue()/2), 
          mx( length.getValue()/2, length.getValue()/2, length.getValue()/2);
  center.setValue(0,0,0);
  box.setBounds(mn,mx);
}




// updateChildren
void SoSiWedgeStrips::updateChildren() {


  // Redraw the G4Box....

  assert(children->getLength()==1);
  SoSeparator       *lod                = (SoSeparator *)  ( *children)[0];

  // Wipe out all of the hits. If data and address space aren't equal:
  // its either because A) Someone is changing them and the data isn't
  // ready yet, or B) Someone made a mistake. In either case don't render
  // any hits.

  lod->removeAllChildren();
  if (strip.getNum()!=adc.getNum()) return;
  SoSeparator *sep = new SoSeparator();
  lod->addChild(sep);
  SoSeparator *fakeSep= new SoSeparator();
  lod->addChild(fakeSep);


  SoTranslation *TheTranslation = new SoTranslation();
  TheTranslation->translation.setValue(0, -width.getValue()/2,0);
  sep->addChild(TheTranslation);
  short stripnum;
  float neg;
  if (slope.getValue() > 0) neg = -1; else neg = 1;
  for (int i=0; i<strip.getNum();i++) {
    SoSeparator   *pulseSep = new SoSeparator();
    sep->addChild(pulseSep);
    SoTranslation *inc = new SoTranslation();
    stripnum = strip[i]-cutoff.getValue();
    if (stripnum > 0)
      inc->translation.setValue(0,pitch.getValue()*strip[i],.5F*stripnum*slope.getValue()+tilt.getValue()*strip[i]);
    else inc->translation.setValue(0,pitch.getValue()*strip[i],tilt.getValue()*strip[i]);

    pulseSep->addChild(inc);
    SoCube  *hitCube  = new SoCube();
    hitCube->width.setValue(thickness.getValue()+adc[i]*gain.getValue());
    hitCube->height.setValue(pitch.getValue());
    if (stripnum > 0)
      hitCube->depth.setValue(length.getValue()-pitch.getValue()+(neg*stripnum*slope.getValue()));
    else hitCube->depth.setValue(length.getValue()-pitch.getValue());
    pulseSep->addChild(hitCube);

  }

}

// generateChildren
void SoSiWedgeStrips::generateChildren() {

  // A box consists of a set of scale factors and a 
  // cube.

  assert(children->getLength() ==0);
  SoLevelOfDetail      *sep              = new SoLevelOfDetail();
  sep->screenArea.setValue(500); 
  children->append(sep);
}

// generateAlternateRep
void SoSiWedgeStrips::generateAlternateRep(SoAction*) {

  // This routine sets the alternate representation to the child
  // list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *children)[0]);
}

// clearAlternateRep
void SoSiWedgeStrips::clearAlternateRep() {
  alternateRep.setValue(NULL);
}
