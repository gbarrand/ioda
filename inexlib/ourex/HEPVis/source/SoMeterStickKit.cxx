/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoMeterStickKit                                         */
/* Description:      Represents an MeterStick, you can pick it up & move it  */
/* Author:           Joe Boudreau September 1997                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <assert.h>
#include <math.h>
#include <Inventor/SbBox.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/manips/SoJackManip.h>
#include <Inventor/nodekits/SoAppearanceKit.h>
#include <HEPVis/SbMath.h>
#include <HEPVis/nodekits/SoMeterStickKit.h>
#include <iostream>
#include <vector>
#include <algorithm>

// Here is a class that helps us to sort through a list
// of rotations and find the closest one:
class SortByRotationalProximity{

public:  

  // Construct given a reference rotation
  SortByRotationalProximity(SbRotation & reference):
    _reference(reference){_reference.invert();}

  // Compare:
  bool operator () (const SbRotation &A, const SbRotation &B) const {

    SbRotation 
      AR = (A)*_reference,
      BR = (B)*_reference;
    SbVec3f aA, aB;
    float   rA, rB;
    AR.getValue(aA, rA);
    BR.getValue(aB, rB);
    return rA<rB;
    
  }

private:

  SbRotation _reference;

};

std::vector<SbRotation>  SoMeterStickKit::_validRotations;
const int                SoMeterStickKit::_NVALID=12;

// This statement is required
SO_KIT_SOURCE(SoMeterStickKit)

// initClass
void SoMeterStickKit::initClass(){
  // This statement is required.
  SO_KIT_INIT_CLASS(SoMeterStickKit,SoBaseKit,"BaseKit");
  //
  // Valid rotations.  When Rotation snap is enabled we will snap
  // to one of the rotations in this list.
  // 
  for (int ip=0;ip<_NVALID;ip++) {
    for (int it=0;it<_NVALID;it++) {
      for (int is=0;is<_NVALID;is++) {
	double phiAngle   = ip*2.0*M_PI/_NVALID;
	double thetaAngle = it*2.0*M_PI/_NVALID;
	double psiAngle   = is*2.0*M_PI/_NVALID;
	SbRotation phiRotation   =SbRotation(SbVec3f(0,0,1),(float)phiAngle);
	SbRotation thetaRotation =SbRotation(SbVec3f(0,1,0),(float)thetaAngle);
	SbRotation psiRotation   =SbRotation(SbVec3f(1,0,0),(float)psiAngle);
	_validRotations.push_back(phiRotation*thetaRotation*psiRotation);
      }
    }
  }
}

// Constructor
SoMeterStickKit::SoMeterStickKit() {
  // This statement is required
  SO_KIT_CONSTRUCTOR(SoMeterStickKit);

  // Add fields
  SO_NODE_ADD_FIELD(type,                 (METER_STICK));
  SO_NODE_ADD_FIELD(rotationSnap,               (FALSE));
  SO_NODE_ADD_FIELD(alternateRep,                (NULL));
  
  // This initializes appearance kits:
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,             SoSeparator,         FALSE,  this,         \0, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(meterStickAppearance,     SoAppearanceKit,     FALSE,  topSeparator, \0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(rulingAppearance,         SoAppearanceKit,     FALSE,  topSeparator, \0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(jackManip,                SoJackManip,         FALSE,  topSeparator, \0, TRUE); 
  // Data fields are initialized like this:
  SO_KIT_INIT_INSTANCE();
  createInitialMeterStick();
}

// Destructor
SoMeterStickKit::~SoMeterStickKit() {
  delete children;
}



// generateChildren
void SoMeterStickKit::createInitialMeterStick() {


  //
  // Appearances first:
  //
  SoAppearanceKit * msAppearance = (SoAppearanceKit *) meterStickAppearance.getValue();
  SoAppearanceKit * ruAppearance = (SoAppearanceKit *) rulingAppearance.getValue();

  msAppearance->set("material", "ambientColor  1.00 1.00 1.00");
  msAppearance->set("material", "diffuseColor  0.71 0.71 1.00");
  msAppearance->set("material", "specularColor 0.80 0.80 1.00");
  msAppearance->set("material", "emissiveColor 0.00 0.00 0.00");
  msAppearance->set("material", "shininess 0.75");
  msAppearance->set("material", "transparency 0");

  ruAppearance->set("drawStyle", "lineWidth 4");
  ruAppearance->set("lightModel" "model SoLightModel::BASE_COLOR");
  ruAppearance->set("material",  "diffuseColor 1 0 0");

  

  // Then, construct the parts:
  //

  SoJackManip *jkManip = (SoJackManip *) jackManip.getValue();
  // Make the manipulator.
  SoDragger *dragger = jkManip->getDragger();
  dragger->setPart(SbName("scaler.scaler"), new SoSeparator);
  dragger->setPart(SbName("surroundScale"), new SoSeparator);
  dragger->addFinishCallback(motionFinished, this);
  
  double baseLength=10.0;
  double width     = 0.10 *pow(baseLength,type.getValue()+1);
  double height    = 0.025*pow(baseLength,type.getValue()+1);
  double depth     = 1.0  *pow(baseLength,type.getValue()+1);
  // Replace the X, Y, and Z translators with the meter stick itself

  // These translations and spheres will be used below.  Twice.  First, they will be drawn (Unpickable, Visible)
  // together with the meter stick and second they will be drawn (Pickable, invisible) on the rotator.

  SoTranslation *t0=new SoTranslation; 
  t0->translation=SbVec3f(float((depth+width)/2.0), 0,                     0);
  SoTranslation *t1=new SoTranslation; 
  t1->translation=SbVec3f(float(-(depth+width)/2.0),0,                     0);
  SoTranslation *t2=new SoTranslation; 
  t2->translation=SbVec3f(0,             float((depth+width)/2.0),         0);
  SoTranslation *t3=new SoTranslation; 
  t3->translation=SbVec3f(0,             float(-(depth+width)/2.0),        0);
  SoTranslation *t4=new SoTranslation; 
  t4->translation=SbVec3f(0,             0,        float((depth+width)/2.0));
  SoTranslation *t5=new SoTranslation; 
  t5->translation=SbVec3f(0,             0,        float(-(depth+width)/2.0));
  
  SoSphere *sp=new SoSphere; 
  sp->radius.setValue((float)(width/2));
  
  for (int axis=0;axis<3;axis++) {
    
    SoSeparator *stickSeparator = new SoSeparator;
    SoSeparator *handleSeparator = new SoSeparator;
    
    // Not added..  Will be used to substitute for the part.
    
    
    // Make the meter stick bar.
    SoCube *cube=new SoCube;
    cube->width.setValue((float)width);
    cube->height.setValue((float)height);
    cube->depth.setValue((float)depth);

    SoCube *translateCube = new SoCube;
    translateCube->width.setValue((float)height);
    translateCube->height.setValue((float)height);
    translateCube->depth.setValue((float)depth);
    
    if (axis==0) {
      dragger->setPart(SbName("translator.xTranslator.translator"),       stickSeparator);
      dragger->setPart(SbName("translator.xTranslator.translatorActive"), stickSeparator->copy());
      dragger->setPart(SbName("translator.yzTranslator.translator"),      handleSeparator);
      dragger->setPart(SbName("translator.yzTranslator.translatorActive"),handleSeparator->copy());
    }
    else if (axis==1) {
      dragger->setPart(SbName("translator.yTranslator.translator"),       stickSeparator);
      dragger->setPart(SbName("translator.yTranslator.translatorActive"), stickSeparator->copy());
      dragger->setPart(SbName("translator.xzTranslator.translator"),      handleSeparator);
      dragger->setPart(SbName("translator.xzTranslator.translatorActive"),handleSeparator->copy());
    }
    else if (axis==2) {
      dragger->setPart(SbName("translator.zTranslator.translator"),       stickSeparator);
      dragger->setPart(SbName("translator.zTranslator.translatorActive"), stickSeparator->copy());
      dragger->setPart(SbName("translator.xyTranslator.translator"),      handleSeparator);
      dragger->setPart(SbName("translator.xyTranslator.translatorActive"),handleSeparator->copy());
    }
    
    
    SoLineSet *frontLineSet= new SoLineSet;
    SoCoordinate3 *frontCoordinates  = new SoCoordinate3;
    SoLineSet *backLineSet= new SoLineSet;
    SoCoordinate3 *backCoordinates  = new SoCoordinate3;
    
    // Major tick marks:
    int pcounter=0, vcounter=0;
    for (int I=0;I<=1000;I++) {
      
      frontLineSet->numVertices.set1Value(vcounter,2);
      backLineSet->numVertices.set1Value(vcounter,2);
      vcounter++;
      
      frontCoordinates->point.set1Value(pcounter,SbVec3f(-cube->width.getValue()/2, 
						      -cube->height.getValue()/2,
						      -cube->depth.getValue()/2 +
						      cube->depth.getValue()*I/1000));
      backCoordinates->point.set1Value(pcounter,SbVec3f(-cube->width.getValue()/2, 
						      +cube->height.getValue()/2,
						      -cube->depth.getValue()/2 +
						      cube->depth.getValue()*I/1000));
      pcounter++;
      if (I%100==0) {
	frontCoordinates->point.set1Value(pcounter,SbVec3f(+cube->width.getValue()/2, 
							-cube->height.getValue()/2,
							-cube->depth.getValue()/2 +
							cube->depth.getValue()*I/1000));
	backCoordinates->point.set1Value(pcounter,SbVec3f(+cube->width.getValue()/2, 
							+cube->height.getValue()/2,
							-cube->depth.getValue()/2 +
							cube->depth.getValue()*I/1000));
	pcounter++;
      }
      else if (I%10==0) {
	frontCoordinates->point.set1Value(pcounter,SbVec3f(0, 
							-cube->height.getValue()/2,
							-cube->depth.getValue()/2 +
							cube->depth.getValue()*I/1000));
	backCoordinates->point.set1Value(pcounter,SbVec3f(0, 
							+cube->height.getValue()/2,
							-cube->depth.getValue()/2 +
							cube->depth.getValue()*I/1000));
	pcounter++;
      }
      else {
	frontCoordinates->point.set1Value(pcounter,SbVec3f(-cube->width.getValue()/4, 
							-cube->height.getValue()/2,
							-cube->depth.getValue()/2 +
							cube->depth.getValue()*I/1000));
	backCoordinates->point.set1Value(pcounter,SbVec3f(-cube->width.getValue()/4, 
							+cube->height.getValue()/2,
							-cube->depth.getValue()/2 +
							cube->depth.getValue()*I/1000));
	pcounter++;
      }
    }
    
    // Make the marking:
    SoRotationXYZ  *R0 = new SoRotationXYZ;
    SoRotationXYZ  *R1 = new SoRotationXYZ;
    SoSeparator *ssSeparator=new SoSeparator;
    SoPickStyle *ssPickStyle=new SoPickStyle;
    ssPickStyle->style=SoPickStyle::UNPICKABLE;
    stickSeparator->addChild(ssSeparator);
    ssSeparator->addChild(ssPickStyle);
    if (axis==0) {
      ssSeparator->addChild(t0);
      ssSeparator->addChild(sp);
      ssSeparator->addChild(t1);
      ssSeparator->addChild(t1);
      ssSeparator->addChild(sp);
      ssSeparator->addChild(t0);
      R0->axis.setValue(SoRotationXYZ::Z);
      R0->angle.setValue((float)(M_PI/2));
      R1->axis.setValue(SoRotationXYZ::X);
      R1->angle.setValue((float)(-M_PI/2));
    }
    else if (axis==1) {
      ssSeparator->addChild(t0);
      ssSeparator->addChild(sp);
      ssSeparator->addChild(t1);
      ssSeparator->addChild(t1);
      ssSeparator->addChild(sp);
      ssSeparator->addChild(t0);
      R0->axis.setValue(SoRotationXYZ::Z);
      R0->angle.setValue((float)(M_PI/2));
      R1->axis.setValue(SoRotationXYZ::X);
      R1->angle.setValue((float)(-M_PI/2));
    }
    else if (axis==2) {
      ssSeparator->addChild(t0);
      ssSeparator->addChild(sp);
      ssSeparator->addChild(t1);
      ssSeparator->addChild(t1);
      ssSeparator->addChild(sp);
      ssSeparator->addChild(t0);
      R0->axis.setValue(SoRotationXYZ::X);
      R0->angle.setValue((float)M_PI);
      R1->axis.setValue(SoRotationXYZ::Y);
      R1->angle.setValue((float)(M_PI/2));
    }
    handleSeparator->addChild(R0);
    handleSeparator->addChild(R1);
    handleSeparator->addChild(meterStickAppearance.getValue());
    handleSeparator->addChild(translateCube);

    stickSeparator->addChild(R0);
    stickSeparator->addChild(R1);
    stickSeparator->addChild(meterStickAppearance.getValue());
    stickSeparator->addChild(cube);
    stickSeparator->addChild(rulingAppearance.getValue());
    stickSeparator->addChild(frontCoordinates);
    stickSeparator->addChild(frontLineSet);
    stickSeparator->addChild(backCoordinates);
    stickSeparator->addChild(backLineSet);
  }

  SoSeparator *rotorBallSeparator = new SoSeparator;
  SoDrawStyle *drawStyle = new SoDrawStyle();
  drawStyle->style.setValue(SoDrawStyle::INVISIBLE);
  rotorBallSeparator->addChild(drawStyle);
  rotorBallSeparator->addChild(t0); rotorBallSeparator->addChild(sp); rotorBallSeparator->addChild(t1);
  rotorBallSeparator->addChild(t1); rotorBallSeparator->addChild(sp); rotorBallSeparator->addChild(t0);
  rotorBallSeparator->addChild(t2); rotorBallSeparator->addChild(sp); rotorBallSeparator->addChild(t3);
  rotorBallSeparator->addChild(t3); rotorBallSeparator->addChild(sp); rotorBallSeparator->addChild(t2);
  rotorBallSeparator->addChild(t4); rotorBallSeparator->addChild(sp); rotorBallSeparator->addChild(t5);
  rotorBallSeparator->addChild(t5); rotorBallSeparator->addChild(sp); rotorBallSeparator->addChild(t4);
  dragger->setPart(SbName("rotator.rotator"),       rotorBallSeparator);
  dragger->setPart(SbName("rotator.rotatorActive"), rotorBallSeparator->copy());
}

// generateAlternateRep
void SoMeterStickKit::generateAlternateRep(SoAction*) {

  // This routine sets the alternate representation to the child
  // list of this mode.  

  alternateRep.setValue(jackManip.getValue());
}

// clearAlternateRep
void SoMeterStickKit::clearAlternateRep() {
  alternateRep.setValue(NULL);
}

SbBool SoMeterStickKit::affectsState() const {
  return FALSE;
}

void SoMeterStickKit::motionFinished(void *stick, SoDragger* /*dragger*/) {
  //
  // Resolve this.
  //
  SoMeterStickKit *This = (SoMeterStickKit *) stick;
  //
  // Return if there is no rotation snap set:
  //
  if (!This->rotationSnap.getValue()) return;
  //
  // Get the dragger
  //  
  SoJackManip *jkManip = (SoJackManip *) This->jackManip.getValue();
  //
  // Get the rotation out of the manip:
  // 
  SbRotation rotation = jkManip->rotation.getValue();
  //
  // Now choose the closet rotation to the user's  
  //
  SortByRotationalProximity sorter(rotation);
  std::vector<SbRotation>::const_iterator newRotationIter = std::min_element(_validRotations.begin(),
									     _validRotations.end(),
									     sorter);
  //
  // And then put the new, valid rotation back into the manip.
  //
  if (newRotationIter!=_validRotations.end()) {
    SbRotation newRotation = *newRotationIter;
    jkManip->rotation.setValue(newRotation);
  }
  //
  // Bye..
  //      
  return;
}



