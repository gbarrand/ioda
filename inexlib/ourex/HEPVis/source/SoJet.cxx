/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoJet                                                   */
/* Description:      Draws a Jet object using energy, theta, phi, thrust     */
/* Author:           T. Paul                                                 */
/*---------------------------------------------------------------------------*/
//
// Synopsis:
//   #include<SoJet.h>
//
//   Draws an object represting a jet using theta, phi, (energy, thrust).
//   Optionally displays information on energy, angles, etc. next to the
//   object.
//
// Fields:
//
//   SoSFFloat fEnergy            jet energy
//   SoSFFloat fTheta             jet axis theta
//   SoSFFloat fPhi               jet axis phi
//   SoSFFloat fThrust            jet thrust
//   SoSFFloat fMaxZ              Half length of cylinder outside of which jet does not extend
//   SoSFFloat fMaxR              Radius      of cylinder outside of which jet does not extendw
//
//
#include <assert.h>
#include <math.h>
#include <Inventor/SbBox.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText2.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/nodes/SoJet.h>

SO_NODE_SOURCE(SoJet)

//-----------//
// initClass //
//-----------//

void SoJet::initClass(){

  SO_NODE_INIT_CLASS(SoJet,SoShape,"Shape");
}

//-------------//
// Constructor //
//-------------//

SoJet::SoJet() {

  SO_NODE_CONSTRUCTOR(SoJet);

  // Initialize data fields
  // ----------------------

  SO_NODE_ADD_FIELD(fEnergy,   (0));
  SO_NODE_ADD_FIELD(fTheta,    (0));
  SO_NODE_ADD_FIELD(fPhi,      (0));
  SO_NODE_ADD_FIELD(fThrust,   (1));
  SO_NODE_ADD_FIELD(fMaxZ,     (4)); // set these to something more sensible ...
  SO_NODE_ADD_FIELD(fMaxR,     (2)) ;
  SO_NODE_ADD_FIELD(alternateRep,       (NULL));
  children = new SoChildList(this);

  myEnergy      = fEnergy.getValue();
  myTheta       = fTheta.getValue();
  myPhi         = fPhi.getValue();
  myThrust      = fThrust.getValue();

  myMaxZ        = fMaxZ.getValue();
  myMaxR        = fMaxR.getValue();
}

//-------------//
// Destructor  //
//-------------//

SoJet::~SoJet() {
 delete children;
}

//-----------//
// GLRrender //
//-----------//

void SoJet::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();
  
   if ( (fEnergy.getValue()        != myEnergy) ||
        (fTheta.getValue()         != myTheta)  ||
        (fPhi.getValue()           != myPhi)    ||
        (fThrust.getValue()        != myThrust) ||
        //        (fLabelOptions.getValue()  != myLabelOption) ||
        (fMaxZ.getValue()          != myMaxZ  ) ||
        (fMaxR.getValue()          != myMaxR  )  ) { 
   updateChildren();
   }

  children->traverse(action);  

  myEnergy      = fEnergy.getValue();
  myTheta       = fTheta.getValue();
  myPhi         = fPhi.getValue();
  myThrust      = fThrust.getValue();

  myMaxZ        = fMaxZ.getValue();
  myMaxR        = fMaxR.getValue();
}

//--------------------//
// generatePrimitives //
//--------------------//

void SoJet::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

//-------------//
// getChildren //
//-------------//

SoChildList *SoJet::getChildren() const {
  return children;
}

//-------------//
// computeBBox //
//-------------//

void SoJet::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){

  setParameters();

  SbVec3f jetMin(-FFABS(jetX)-hatRadius, 
                 -FFABS(jetY)-hatRadius, 
                 -FFABS(jetZ)-hatRadius);
  SbVec3f jetMax( FFABS(jetX)+hatRadius,  
                  FFABS(jetY)+hatRadius,  
                  FFABS(jetZ)+hatRadius);
  center.setValue(0, 0, 0);

  box.setBounds(jetMin,jetMax);
}

//----------------//
// updateChildren //
//----------------//

void SoJet::updateChildren() {

  assert(children->getLength()==1);
  SoSeparator* sep                = (SoSeparator *)  ( *children)[0];

  SoSeparator* jetBody            = (SoSeparator *)  (sep->getChild(0));
  SoTransform* bottomConeTransform= (SoTransform *)  (jetBody->getChild(0));
  SoCone*      bottomCone         = (SoCone *)       (jetBody->getChild(1));

  SoSeparator* jetHat             = (SoSeparator *)  (sep->getChild(1));
  SoTransform* topConeTransform   = (SoTransform *)  (jetHat->getChild(0));
  SoCone*      topCone            = (SoCone *)       (jetHat->getChild(1));

  //SoSeparator* label              = (SoSeparator *)  (sep->getChild(2));

  //
  // Set the dimensions and angles for everything
  // --------------------------------------------
  
  setParameters();

  bottomCone->bottomRadius.setValue(bodyRadius);  
  bottomCone->height.setValue(bodyHeight)      ;  

  //
  // Define cone rotations and translations
  //---------------------------------------

  // vector normal to thrust-y plane    
  SbVec3f rotVec(FCOS(theta), 0, -FSIN(theta)*FCOS(phi));   
  // angle between y axis and thrust axis
  float alpha = FACOS(FSIN(theta)*FSIN(phi));
  // the M_PI turns the cone on its head
  bottomConeTransform->rotation.setValue(rotVec,alpha + (float)M_PI);
  float tempShiftX = bodyHeight*FSIN(theta)*FCOS(phi)/2;   
  float tempShiftY = bodyHeight*FSIN(theta)*FSIN(phi)/2;
  float tempShiftZ = bodyHeight*FCOS(theta)/2;
  // Move cone vertex to the origin
  bottomConeTransform->translation.setValue(tempShiftX,tempShiftY,tempShiftZ);

  //
  // The pointy "hat" on top of the jet
  // ----------------------------------

  topCone->bottomRadius.setValue(hatRadius);
  topCone->height.setValue(hatHeight);         

  topConeTransform->rotation.setValue(rotVec,alpha);
  tempShiftX = (bodyHeight+hatHeight/2)*FSIN(theta)*FCOS(phi);   
  tempShiftY = (bodyHeight+hatHeight/2)*FSIN(theta)*FSIN(phi);
  tempShiftZ = (bodyHeight+hatHeight/2)*FCOS(theta);
  topConeTransform->translation.setValue(tempShiftX, tempShiftY, tempShiftZ);  

}

//------------------//
// generateChildren //
//------------------//

void SoJet::generateChildren() {

  assert(children->getLength() ==0);
  SoSeparator* sep = new SoSeparator(); 
  
  //
  // The body of the jet
  // -------------------

  SoCone* bottomCone = new SoCone;
  SoTransform* bottomConeTransform = new SoTransform;
  
  //
  // The pointy "hat" on top of the jet
  // ----------------------------------

  SoCone* topCone = new SoCone;
  SoTransform* topConeTransform = new SoTransform;

  //
  // Make separators for the jet body and hat
  // and organize cone stuf underneath them
  // -----------------------------------------

  SoSeparator* jetHat = new SoSeparator;
  jetHat->addChild(topConeTransform);
  jetHat->addChild(topCone);  
  
  SoSeparator* jetBody = new SoSeparator;
  jetBody->addChild(bottomConeTransform); 
  jetBody->addChild(bottomCone);

  SoSeparator* label = new SoSeparator;
      
  sep->addChild(jetBody);        // child 0
  sep->addChild(jetHat);         // child 1
  sep->addChild(label);          // child 2

  children->append(sep);
}

// Set data members giving dimensions and angles 
// according to the fields.  (this is used by computeBBox
// and generateChildren).
//
void SoJet::setParameters() {

  theta = fTheta.getValue();
  phi   = fPhi.getValue();

  float length1,length2;

  if ( FCOS(theta) != 0){
    length1 = fMaxZ.getValue()/FFABS(FCOS(theta));
  }
  else {
    length1 = fMaxZ.getValue();
  }

  if ( FSIN(theta) != 0){
    length2 = fMaxR.getValue()/FFABS(FSIN(theta));
  }
  else {
    length2 = fMaxR.getValue();
  }

  if (length1 < length2) {
    bodyHeight = length1;
  }
  else {
    bodyHeight = length2;
  }

  // TO BE DONE .... SET RADIUS TO SOMETHING MEANINGFUL
  // first attempt to make bodyRadius somehow related to thrust
  bodyRadius = 0.3F * ( 1 / (fThrust.getValue() + 0.001F) );

  hatRadius = 1.4F*bodyRadius;

  // TO BE DONE ...SET HEIGHT TO SOMETHING MEANINGFUL
  // first attempt to relate hat height to energy. 
  // for now, hat height varies from 0. up to fMaxR,
  // depending on what max energy is set to
  //
  float highestEnergy = 100;        // should be set elsewhere
  hatHeight = fMaxR.getValue()*fEnergy.getValue()/highestEnergy;

  // coordinates of tip of SoJet object
  //
  float totalLength = bodyHeight + hatHeight;
  jetX = totalLength*FSIN(theta)*FCOS(phi);
  jetY = totalLength*FSIN(theta)*FSIN(phi);
  jetZ = totalLength*FCOS(theta);

}



 //----------------------//
 // generateAlternateRep //
 //----------------------//

 void SoJet::generateAlternateRep(SoAction*) {

   // This routine sets the alternate representation to the child
   // list of this mode.  

   if (children->getLength() == 0) generateChildren();
   updateChildren();
   alternateRep.setValue((SoSeparator *)  ( *children)[0]);
 }

 //-------------------//
 // clearAlternateRep //
 //-------------------//

 void SoJet::clearAlternateRep() {
   alternateRep.setValue(NULL);

 }

 //--------------------------------//
 // getInfo                        //
 //   Dump of physics info if      //
 //   graphics object is selected  //
 //--------------------------------//

const char* SoJet::getInfo() {
   /*
   ostrstream cat;
   cat.precision(3);
   cat.setf(ios::fixed, ios::floatfield);

   cat << "You have picked a JET.\n";
   cat << "----------------------\n";
   cat << "   energy    " << fEnergy.getValue() << " GeV\n";
   cat << "   phi       " << fPhi.getValue() << "\n";
   cat << "   theta     " << fTheta.getValue() << "\n";
   cat << "   thrust    " << fThrust.getValue() << "\n";


   cat.put('\0');               // now it is a real string
   char* dump = cat.str();
   return dump; 
   */
  char s[32];
  fInfo = "";
  fInfo += "You have picked a JET.\n";
  fInfo += "----------------------\n";
  fInfo += "   energy    ";
  sprintf(s,"%g",fEnergy.getValue()); fInfo += s;
  fInfo += " GeV\n";
  fInfo += "   phi       ";
  sprintf(s,"%g",fPhi.getValue()); fInfo += s;
  fInfo += "\n";
  fInfo += "   theta     ";
  sprintf(s,"%g",fTheta.getValue()); fInfo += s;
  fInfo += "\n";
  fInfo += "   thrust    ";
  sprintf(s,"%g",fThrust.getValue()); fInfo += s;
  fInfo += "\n";

  return fInfo.getString();
 }






