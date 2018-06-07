/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoCoordinateAxis                                        */
/* Description:      draws a coordinate axis                                 */
/* Author:           T. Paul                                                 */
/*---------------------------------------------------------------------------*/
//
// Synopsis:
//   #include<SoCoordinateAxis.h>
//
//   Draws a coordinate axis with the specified number of divisions of
//   the specified length.  Writes the length of a division someplace
//   on the picture.
//
//  Fields:
//   
//    SoSFFloat fDivisionLength            The length of one division
//    SoSFInt32 fNDivision                 The number of divisions
//
//
//

// this :
#include <HEPVis/nodes/SoCoordinateAxis.h>

#include <Inventor/SbBox.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/actions/SoGLRenderAction.h>

#include <HEPVis/SbMath.h>

#include <assert.h>

SO_NODE_SOURCE(SoCoordinateAxis)

//-----------//
// initClass //
//----------//

void SoCoordinateAxis::initClass(){

  // This statement is required.
  SO_NODE_INIT_CLASS(SoCoordinateAxis,SoShape,"Shape");
}

//-------------//
// Constructor //
//-------------//
SoCoordinateAxis::SoCoordinateAxis() {

  SO_NODE_CONSTRUCTOR(SoCoordinateAxis);

  // Initialize data fields
  // ----------------------
  SO_NODE_ADD_FIELD(fNDivision,           (3));
  SO_NODE_ADD_FIELD(fDivisionLength,      (0.5));
  SO_NODE_ADD_FIELD(alternateRep,       (NULL));
  children = new SoChildList(this);
  
  // initialize private data members
  //--------------------------------  
  myNDivision      = fNDivision.getValue();
  myDivisionLength = fDivisionLength.getValue();

}

//-------------//
// Destructor  //
//-------------//

SoCoordinateAxis::~SoCoordinateAxis() {
 delete children;
}

//-----------//
// GLRrender //
//-----------//

void SoCoordinateAxis::GLRender(SoGLRenderAction *action) {

  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();

  //printf("debug : SoCoord : %g\n",fDivisionLength.getValue());

  // Update the picture (ie render it) if a field has changed
  // --------------------------------------------------------
  if ( (fNDivision.getValue()      != myNDivision) ||
       (fDivisionLength.getValue() != myDivisionLength) ) {
    updateChildren();
  }

  children->traverse(action);  

  myNDivision      = fNDivision.getValue();
  myDivisionLength = fDivisionLength.getValue();

}

//--------------------//
// generatePrimitives //
//--------------------//

void SoCoordinateAxis::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

//-------------//
// getChildren //
//-------------//

SoChildList *SoCoordinateAxis::getChildren() const {
  return children;
}

//-------------//
// computeBBox //
//-------------//

void SoCoordinateAxis::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){

  // slop of 100% in size of corner to allow arrowheads to fit in bounding box
  float corner = 1.5F * fDivisionLength.getValue()*
                 (float)(fNDivision.getValue());
  SbVec3f mn(0, 0, 0);
  SbVec3f mx( corner, corner, corner);
  center.setValue(corner/2, corner/2, corner/2);
  box.setBounds(mn,mx);

}

//----------------//
// updateChildren //
//----------------//

void SoCoordinateAxis::updateChildren() {

  // Draw (or redraw) the Axis and all associated crap
  // -------------------------------------------------

  assert(children->getLength()==1);
  SoSeparator   *sep                = (SoSeparator *)  ( *children)[0];
  SoCoordinate3 *myCoords           = (SoCoordinate3 *)(sep->getChild(0));
  SoLineSet     *myLines            = (SoLineSet  *)   (sep->getChild(1));

  SoSeparator   *coneX              = (SoSeparator *)  (sep->getChild(2));
  SoTransform   *myXTransform       = (SoTransform *)  (coneX->getChild(0));
  SoCone        *myCone             = (SoCone *)       (coneX->getChild(1));

  SoSeparator   *coneY              = (SoSeparator *)  (sep->getChild(3));
  SoTransform   *myYTransform       = (SoTransform *)  (coneY->getChild(0));
  
  SoSeparator   *coneZ              = (SoSeparator *)  (sep->getChild(4));
  SoTransform   *myZTransform       = (SoTransform *)  (coneZ->getChild(0));
 
  SoSeparator   *divs               = (SoSeparator *)  (sep->getChild(5));

  SoSeparator   *letterX            = (SoSeparator *)  (sep->getChild(6));
  SoTranslation *letterXTranslate   = (SoTranslation*) (letterX->getChild(0));
  SoFont        *myAxisFont         = (SoFont *)       (letterX->getChild(1));
  SoText2       *letterXText        = (SoText2*)       (letterX->getChild(2));

  SoSeparator   *letterY            = (SoSeparator *)  (sep->getChild(7));
  SoTranslation *letterYTranslate   = (SoTranslation*) (letterY->getChild(0));
  SoText2       *letterYText        = (SoText2*)       (letterY->getChild(2));
  
  SoSeparator   *letterZ            = (SoSeparator *)  (sep->getChild(8));
  SoTranslation *letterZTranslate   = (SoTranslation*) (letterZ->getChild(0));
  SoText2       *letterZText        = (SoText2*)       (letterZ->getChild(2));


  float axisLength = fDivisionLength.getValue()*(float)(fNDivision.getValue());

  //
  // Make the 3 axis lines
  //-----------------------

  SbVec3f vertices[6] = {SbVec3f(0, 0, 0),
                         SbVec3f(0, 0, 0),
                         SbVec3f(0, 0, 0),
                         SbVec3f(0, 0, 0),   
                         SbVec3f(0, 0, 0),
                         SbVec3f(0, 0, 0)};
  vertices[1][0] = vertices[3][1] = vertices[5][2] = axisLength;

  // Set the coordinates to draw lines between
  myCoords->point.setValues(0,6,vertices);
  
  // Set which vertices to use for each line
  static int32_t myIndices[3] = {2,2,2};

  myLines->startIndex.setValue(0);
  myLines->numVertices.setValues(0,3,myIndices);

  //
  // Make arrowheads for the ends of the lines
  // -----------------------------------------

  float coneRadius = fDivisionLength.getValue()/10;  
  float coneHeight = axisLength/4;

  myCone->bottomRadius.setValue(coneRadius);
  myCone->height.setValue(coneHeight);
  
  myXTransform->
    translation.setValue(axisLength + coneHeight/2, 0, 0);
  myXTransform->rotation.setValue(SbVec3f(0,0,1),-FM_PI_2);

  myYTransform->
    translation.setValue(0,axisLength + coneHeight/2, 0);
  myYTransform->rotation.setValue(SbVec3f(0,0,1),0);

  myZTransform->
    translation.setValue(0, 0, axisLength + coneHeight/2);
  myZTransform->rotation.setValue(SbVec3f(1,0,0),FM_PI_2);

  //
  // Remake the division markers, after removing old ones 
  // ----------------------------------------------------
  SoSeparator *divX;
  divX = (SoSeparator*)SoNode::getByName("divX");
  divs->removeChild(divX);

  SoSeparator *divY;
  divY = (SoSeparator*)SoNode::getByName("divY");
  divs->removeChild(divY);

  SoSeparator *divZ;
  divZ = (SoSeparator*)SoNode::getByName("divZ");
  divs->removeChild(divZ);

  makeDivision(divs);  

  //
  // Put letters (X,Y,Z) at the ends of the axes
  // -------------------------------------------

  const float letterDistance = 1.05F*(axisLength + coneHeight);
  myAxisFont->size.setValue(16);
  myAxisFont->name.setValue("Times-Roman");

  letterXTranslate->translation.setValue(letterDistance,0,0);
  letterXText->string = "X";
  letterXText->justification = SoText2::CENTER;

  letterYTranslate->translation.setValue(0,letterDistance,0);
  letterYText->string = "Y";
  letterYText->justification = SoText2::CENTER;

  letterZTranslate->translation.setValue(0,0,letterDistance);
  letterZText->string = "Z";
  letterZText->justification = SoText2::CENTER;

}

//------------------//
// generateChildren //
//------------------//

void SoCoordinateAxis::generateChildren() {

  //
  // Define all the nodes to be used
  // --------------------------------

  assert(children->getLength() ==0);
  SoSeparator      *sep              = new SoSeparator; 
  SoCoordinate3    *myCoords         = new SoCoordinate3;
  SoLineSet        *myLines          = new SoLineSet;

  //  SoMaterial       *myConeMaterial   = new SoMaterial;
  SoCone           *myCone           = new SoCone;

  SoSeparator      *coneX            = new SoSeparator;
  SoTransform      *myXTransform     = new SoTransform;

  SoSeparator      *coneY            = new SoSeparator;
  SoTransform      *myYTransform     = new SoTransform;

  SoSeparator      *coneZ            = new SoSeparator;
  SoTransform      *myZTransform     = new SoTransform;

  SoFont           *myAxisFont       = new SoFont;
  SoSeparator      *letterX          = new SoSeparator;
  SoTranslation    *letterXTranslate = new SoTranslation;
  SoText2          *letterXText      = new SoText2;

  SoSeparator      *letterY          = new SoSeparator;
  SoTranslation    *letterYTranslate = new SoTranslation;
  SoText2          *letterYText      = new SoText2;

  SoSeparator      *letterZ          = new SoSeparator;
  SoTranslation    *letterZTranslate = new SoTranslation;
  SoText2          *letterZText      = new SoText2;

  //
  // Add children to separators 
  // --------------------------
  
  coneX->addChild(myXTransform);
  coneX->addChild(myCone);

  coneY->addChild(myYTransform);
  coneY->addChild(myCone);

  coneZ->addChild(myZTransform);
  coneZ->addChild(myCone);

  letterX->addChild(letterXTranslate);
  letterX->addChild(myAxisFont);
  letterX->addChild(letterXText);
  
  letterY->addChild(letterYTranslate);
  letterY->addChild(myAxisFont);
  letterY->addChild(letterYText);

  letterZ->addChild(letterZTranslate);
  letterZ->addChild(myAxisFont);
  letterZ->addChild(letterZText);

  sep->addChild(myCoords);   // child 0 of sep  
  sep->addChild(myLines);    // child 1
  sep->addChild(coneX);      // child 2
  sep->addChild(coneY);      // child 3
  sep->addChild(coneZ);      // child 4

  // Add the division markers using makeDivision function
  // ----------------------------------------------------
  SoSeparator* divs = new SoSeparator;
  makeDivision(divs);

  sep->addChild(divs);       // child 5

  sep->addChild(letterX);    // child 6
  sep->addChild(letterY);    // child 7
  sep->addChild(letterZ);    // child 8

  children->append(sep);

}

//--------------//
// makeDivision //
//--------------//

//
// Make the axis division marks.  If a field
// is updated, all the old dividers are removed
// and new ones are made by this routine.
// This is probably inefficient, but its the only 
// way to do it if fNDivison is changed.
//

void SoCoordinateAxis::makeDivision(SoSeparator* divs){

  float axisLength = fDivisionLength.getValue()*(float)(fNDivision.getValue());
  
  // Following advice of "Inventor Mentor" p. 72, do not allocate
  // nodes in array (this causes problems when unreferencing them)
  
  //
  // set the Y dividers
  // ------------------
  SoSeparator* divY = new SoSeparator;
  long nDiv = fNDivision.getValue() - 1;
  if(nDiv>=1) {
    // define the basic divider
    // ------------------------
    SoCylinder* myCylinder = new SoCylinder;
    myCylinder->radius.setValue(fDivisionLength.getValue()/10);
    myCylinder->height.setValue(axisLength/200);
    float tempTrans;
    for (int iDiv = 0 ; iDiv < nDiv ; iDiv++){
      tempTrans = fDivisionLength.getValue();
      SoTranslation* myDivYTransform = new SoTranslation;  
      myDivYTransform->translation.setValue(0, tempTrans,  0);
      divY->addChild(myDivYTransform);
      divY->addChild(myCylinder);
    }
  }

  //
  // Set the X dividers 
  // ------------------
  SoSeparator* divX = new SoSeparator;
  SoRotation* subRotX = new SoRotation;
  subRotX->rotation.setValue(SbVec3f(0,0,1),-FM_PI_2);
  divX->addChild(subRotX);
  divX->addChild(divY);
  
  //
  // set the Z dividers
  // ------------------
  SoSeparator* divZ = new SoSeparator;
  SoRotation* subRotZ = new SoRotation;
  subRotZ->rotation.setValue(SbVec3f(1,0,0),FM_PI_2);
  divZ->addChild(subRotZ);
  divZ->addChild(divY);

  //
  // Set names for these children, so they can be deleted (by name) later
  // --------------------------------------------------------------------

  divX->setName("divX");
  divY->setName("divY");
  divZ->setName("divZ");

  divs->addChild(divX);
  divs->addChild(divY);
  divs->addChild(divZ);   
}


//--------------------------------//
// getInfo                        //
//   Dump info if                 //
//   graphics object is selected  //
//--------------------------------//

const char* SoCoordinateAxis::getInfo() {
  /*
  ostrstream cat;
  cat.precision(3);
  cat.setf(ios::fixed, ios::floatfield);

  cat << "You have picked the Coordinate Axes.\n";
  cat << "------------------------------------\n";
  cat << "division length     = " << fDivisionLength.getValue() <<" m." << endl;
  cat << "number of divisions = " << fNDivision.getValue() << endl;

  cat.put('\0');               // now it is a real string
  char* dump = cat.str();
  return dump; 
  */

  char s[32];
  fInfo = "";
  fInfo += "You have picked the Coordinate Axes.\n";
  fInfo += "------------------------------------\n";
  fInfo += "division length     = ";
  sprintf(s,"%g",fDivisionLength.getValue()); fInfo += s;
  fInfo += " m.\n";
  fInfo += "number of divisions = ";
  sprintf(s,"%d",(int)fNDivision.getValue()); fInfo += s;
  fInfo += "\n";

  return fInfo.getString();
}



//----------------------//
// generateAlternateRep //
//----------------------//
//

void SoCoordinateAxis::generateAlternateRep(SoAction*) {
  // This routine sets the alternate representation to the child
  // list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *children)[0]);
}

// clearAlternateRep
void SoCoordinateAxis::clearAlternateRep() {
  alternateRep.setValue(NULL);
}













