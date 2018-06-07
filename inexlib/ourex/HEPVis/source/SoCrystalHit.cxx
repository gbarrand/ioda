/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoCrystalHit                                            */
/* Description:      Draws a hit in an individual crystal                    */
/* Author:           L. Taylor                                               */
/*                                                                           */
/* 02/02/2001 : G.Barrand : delete children in destructor.                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
//
// Synopsis:
//   #include<HEPVis/nodes/SoCrystalHit.h>
//
//    Draws a hit in an individual crystal specified by:
// 
//    The crystal is drawn as a trapezoid and the energy is represented by 
//    a 3D trapezoidal "histogram bar" extending outwards from the back face  
//    of the crystal a distance which is proportional to the energy
//
//
//    To do:  -- should perhaps add SoTrap as a valid constructor  
//            -- add code to check order of input vertices is correct 
//
// Fields:
//
//    SoSFFloat Energy; 	// Energy                  (GeV)
//    SoSFFloat MinimumEnergy; 	// Minimum Energy to draw  (GeV)
//    SoSFFloat Scale;  	// scale for energy (GeV / unit distance)
//    SoSFFloat RelativeWidth;  // Width of energy bump relative to crystal [0,1]
//
//    SoSFBool  DrawCrystal;    // Draw crystal (if TRUE)
//    SoSFBool  DrawHit;        // Draw hit     (if TRUE)
//
//    SoSFVec3f	Front1; 	// corner 1 of front face
//    SoSFVec3f	Front2; 	// corner 2 of front face
//    SoSFVec3f	Front3; 	// corner 3 of front face
//    SoSFVec3f	Front4; 	// corner 4 of front face
//      	            
//    SoSFVec3f	Back1;  	// corner 1 of back face
//    SoSFVec3f	Back2;  	// corner 2 of back face
//    SoSFVec3f	Back3;  	// corner 3 of back face
//    SoSFVec3f	Back4;  	// corner 4 of back face
 
//

#include <assert.h>
#include <math.h>
#include <algorithm>
#include <Inventor/SbLinear.h>
#include <Inventor/SbBasic.h>
#include <Inventor/SbBox.h>

#include <Inventor/actions/SoGLRenderAction.h>

#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFBool.h>

#include <Inventor/misc/SoChildList.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include <HEPVis/nodes/SoCrystalHit.h>

SO_NODE_SOURCE(SoCrystalHit)

//-------------//
// Constructor //
//-------------//

SoCrystalHit::SoCrystalHit() {

  SO_NODE_CONSTRUCTOR(SoCrystalHit);

  // Initialize data fields
  // ----------------------

  SO_NODE_ADD_FIELD(Energy,	   (0));
  SO_NODE_ADD_FIELD(MinimumEnergy, (0.001F));  // 1 MeV 
  SO_NODE_ADD_FIELD(Scale ,	   (1));
  SO_NODE_ADD_FIELD(RelativeWidth, (0.75F));
 
  SO_NODE_ADD_FIELD(DrawCrystal,   (TRUE));
  SO_NODE_ADD_FIELD(DrawHit,       (TRUE));
 
  SO_NODE_ADD_FIELD(Front1,	   ( 0.5F,  0.5F, 0) );   // initialise to unit box in +z
  SO_NODE_ADD_FIELD(Front2,	   (-0.5F,  0.5F, 0) );
  SO_NODE_ADD_FIELD(Front3,	   (-0.5F, -0.5F, 0) );
  SO_NODE_ADD_FIELD(Front4,	   ( 0.5F, -0.5F, 0) );

  SO_NODE_ADD_FIELD(Back1,	   ( 0.5F,  0.5F, 1) );
  SO_NODE_ADD_FIELD(Back2,	   (-0.5F,  0.5F, 1) );
  SO_NODE_ADD_FIELD(Back3,	   (-0.5F, -0.5F, 1) );
  SO_NODE_ADD_FIELD(Back4,	   ( 0.5F, -0.5F, 1) );

  SO_NODE_ADD_FIELD(alternateRep,  (NULL));

  children = new SoChildList(this);

  myEnergy        = Energy.getValue();
  myMinimumEnergy = MinimumEnergy.getValue();
  myScale         = Scale.getValue();
  myRelativeWidth = RelativeWidth.getValue();
  
  myDrawCrystal   = DrawCrystal.getValue();
  myDrawHit       = DrawHit.getValue();
  
  myFront1        = Front1.getValue();
  myFront2        = Front2.getValue();
  myFront3        = Front3.getValue();
  myFront4        = Front4.getValue();
 
  myBack1         = Back1.getValue() ;
  myBack2         = Back2.getValue() ;
  myBack3         = Back3.getValue() ;
  myBack4         = Back4.getValue() ;

}


//-------------//
// Destructor  //
//-------------//

SoCrystalHit::~SoCrystalHit() {
  delete children;
}

//-----------//
// initClass //
//-----------//

void SoCrystalHit::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoCrystalHit,SoShape,"Shape");
}

//-----------//
// GLRrender //
//-----------//

void SoCrystalHit::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();
  
   if ( 
        (Energy.getValue()        != myEnergy)        ||
        (MinimumEnergy.getValue() != myMinimumEnergy) ||
	(Scale.getValue()         != myScale)         ||
	(RelativeWidth.getValue() != myRelativeWidth) ||
	(DrawCrystal.getValue()   != myDrawCrystal)   ||
	(DrawHit.getValue()       != myDrawHit)       ||
	(Front1.getValue()        != myFront1)        ||
	(Front2.getValue()        != myFront2)        ||
	(Front3.getValue()        != myFront3)        ||
	(Front4.getValue()        != myFront4)        ||
	(Back1.getValue()         != myBack1 )        ||
	(Back2.getValue()         != myBack2 )        ||
	(Back3.getValue()         != myBack3 )        ||
	(Back4.getValue()         != myBack4 )    
      ) { 
     
   updateChildren();
   }

  children->traverse(action);  

  myEnergy        = Energy.getValue();
  myMinimumEnergy = MinimumEnergy.getValue();
  myScale         = Scale.getValue() ;
  myRelativeWidth = RelativeWidth.getValue() ;
  myDrawCrystal   = DrawCrystal.getValue();
  myDrawHit       = DrawHit.getValue();
  myFront1        = Front1.getValue();
  myFront2        = Front2.getValue();
  myFront3        = Front3.getValue();
  myFront4        = Front4.getValue();
  myBack1         = Back1.getValue() ;
  myBack2         = Back2.getValue() ;
  myBack3         = Back3.getValue() ;
  myBack4         = Back4.getValue() ;

}

//--------------------//
// generatePrimitives //
//--------------------//

void SoCrystalHit::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

//-------------//
// getChildren //
//-------------//

SoChildList *SoCrystalHit::getChildren() const {
  return children;
}

//-------------//
// computeBBox //
//-------------//

 void SoCrystalHit::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){

   setParameters();

   // TO BE DONE
   //float depth = (Scale.getValue())*(Energy.getValue());

   //SbVec3f vmin(-100,-100,-100);       
   //SbVec3f vmax(100,100,100);
   //box.setBounds(vmin,vmax);
   
   double x[]= {
     Front1.getValue()[0], 
     Front2.getValue()[0], 
     Front3.getValue()[0],
     Front4.getValue()[0], 
     Back1.getValue()[0], 
     Back2.getValue()[0],
     Back3.getValue()[0], 
     Back4.getValue()[0],
     (Back1.getValue()[0]-Front1.getValue()[0])*Scale.getValue()*Energy.getValue()+Back1.getValue()[0],
     (Back2.getValue()[0]-Front2.getValue()[0])*Scale.getValue()*Energy.getValue()+Back2.getValue()[0],
     (Back3.getValue()[0]-Front3.getValue()[0])*Scale.getValue()*Energy.getValue()+Back3.getValue()[0],
     (Back4.getValue()[0]-Front4.getValue()[0])*Scale.getValue()*Energy.getValue()+Back4.getValue()[0]
   };

   double y[]= {
     Front1.getValue()[1], 
     Front2.getValue()[1], 
     Front3.getValue()[1],
     Front4.getValue()[1], 
     Back1.getValue()[1], 
     Back2.getValue()[1],
     Back3.getValue()[1], 
     Back4.getValue()[1],
     (Back1.getValue()[1]-Front1.getValue()[1])*Scale.getValue()*Energy.getValue()+Back1.getValue()[1],
     (Back2.getValue()[1]-Front2.getValue()[1])*Scale.getValue()*Energy.getValue()+Back2.getValue()[1],
     (Back3.getValue()[1]-Front3.getValue()[1])*Scale.getValue()*Energy.getValue()+Back3.getValue()[1],
     (Back4.getValue()[1]-Front4.getValue()[1])*Scale.getValue()*Energy.getValue()+Back4.getValue()[1]
   };

   double z[]= {
     Front1.getValue()[2], 
     Front2.getValue()[2], 
     Front3.getValue()[2],
     Front4.getValue()[2], 
     Back1.getValue()[2], 
     Back2.getValue()[2],
     Back3.getValue()[2], 
     Back4.getValue()[2],
     (Back1.getValue()[2]-Front1.getValue()[2])*Scale.getValue()*Energy.getValue()+Back1.getValue()[2],
     (Back2.getValue()[2]-Front2.getValue()[2])*Scale.getValue()*Energy.getValue()+Back2.getValue()[2],
     (Back3.getValue()[2]-Front3.getValue()[2])*Scale.getValue()*Energy.getValue()+Back3.getValue()[2],
     (Back4.getValue()[2]-Front4.getValue()[2])*Scale.getValue()*Energy.getValue()+Back4.getValue()[2]

   };
     
   int mn=0,mx=12;
   if (DrawCrystal.getValue()) {
     mn=0;
   }
   else {
     mn=4;
   }

   if (DrawHit.getValue()) {
     mx=12;
   }
   else {
     mx=8;
   }
     
   

   double xmin =*std::min_element(x+mn,x+mx);
   double xmax =*std::max_element(x+mn,x+mx);
   double ymin =*std::min_element(y+mn,y+mx);
   double ymax =*std::max_element(y+mn,y+mx);
   double zmin =*std::min_element(z+mn,z+mx);
   double zmax =*std::max_element(z+mn,z+mx);
   
   SbVec3f vmin((float)xmin, (float)ymin, (float)zmin);
   SbVec3f vmax((float)xmax, (float)ymax, (float)zmax);

   center = (vmin+vmax)/2.0F;
   box.setBounds(vmin,vmax);

   

 }

//----------------//
// updateChildren //
//----------------//

void SoCrystalHit::updateChildren() {

  assert(children->getLength()==1);
  
  SoSeparator       *theSoSeparator       = (SoSeparator *)       ( *children)[0];
  SoCoordinate3     *shapeSoCoordinate3   = (SoCoordinate3 *)     (theSoSeparator->getChild(0));
  SoIndexedFaceSet  *theSoIndexedFaceSet  = (SoIndexedFaceSet *)  (theSoSeparator->getChild(1));
  SoDrawStyle       *theSoDrawStyle       = (SoDrawStyle *)       (theSoSeparator->getChild(2));
  SoCoordinate3     *linesSoCoordinate3   = (SoCoordinate3 *)     (theSoSeparator->getChild(3));
  SoIndexedLineSet  *theSoIndexedLineSet  = (SoIndexedLineSet *)  (theSoSeparator->getChild(4));
 
  setParameters();

  assert (Energy.getValue()        >= 0);
  assert (Scale.getValue()         >= 0);
  assert (RelativeWidth.getValue() >= 0);
  assert (RelativeWidth.getValue() <= 1);

  const int nvert = 16;
  SbVec3f shapeVertices[nvert];

  // Crystal part of shape

  shapeVertices[0] = Front1.getValue();
  shapeVertices[1] = Front2.getValue();
  shapeVertices[2] = Front3.getValue();
  shapeVertices[3] = Front4.getValue();

  shapeVertices[4] = Back1.getValue();
  shapeVertices[5] = Back2.getValue();
  shapeVertices[6] = Back3.getValue();
  shapeVertices[7] = Back4.getValue();

  // Energy part  

  // First find shape of crystal shrunk laterally according to RelativeWidth

  float delta = (1 - 0.5F * (1 - RelativeWidth.getValue()));  // take half off each side 

  // shrunken front face coplanar with original plane

  SbVec3f myShrunkenFront1 = Front1.getValue() + delta * (Front3.getValue() - Front1.getValue());
  SbVec3f myShrunkenFront2 = Front2.getValue() + delta * (Front4.getValue() - Front2.getValue());
  SbVec3f myShrunkenFront3 = Front3.getValue() + delta * (Front1.getValue() - Front3.getValue());
  SbVec3f myShrunkenFront4 = Front4.getValue() + delta * (Front2.getValue() - Front4.getValue());
	
  // shrunken back face coplanar with original plane

  SbVec3f myShrunkenBack1  = Back1.getValue()  + delta * (Back3.getValue()  - Back1.getValue());
  SbVec3f myShrunkenBack2  = Back2.getValue()  + delta * (Back4.getValue()  - Back2.getValue());
  SbVec3f myShrunkenBack3  = Back3.getValue()  + delta * (Back1.getValue()  - Back3.getValue());
  SbVec3f myShrunkenBack4  = Back4.getValue()  + delta * (Back2.getValue()  - Back4.getValue());

  // Carry on shape of (laterally shrunken) crystal starting from back face 
  // Move a tiny fraction of the crystal length away, along crystal axis, 
  // from the back face of crystal so line around base is not lost

  float epsilon = 0.0001F;             

  shapeVertices[8]  = myShrunkenBack1 	+ (myShrunkenBack1 - myShrunkenFront1) * epsilon;
  shapeVertices[9]  = myShrunkenBack2 	+ (myShrunkenBack2 - myShrunkenFront2) * epsilon;
  shapeVertices[10] = myShrunkenBack3 	+ (myShrunkenBack3 - myShrunkenFront3) * epsilon;
  shapeVertices[11] = myShrunkenBack4 	+ (myShrunkenBack4 - myShrunkenFront4) * epsilon;

  // Make the length proportional to the energy 

  shapeVertices[12] = shapeVertices[8]  + (myShrunkenBack1 - myShrunkenFront1) * Scale.getValue() * Energy.getValue();
  shapeVertices[13] = shapeVertices[9]  + (myShrunkenBack2 - myShrunkenFront2) * Scale.getValue() * Energy.getValue();
  shapeVertices[14] = shapeVertices[10] + (myShrunkenBack3 - myShrunkenFront3) * Scale.getValue() * Energy.getValue();
  shapeVertices[15] = shapeVertices[11] + (myShrunkenBack4 - myShrunkenFront4) * Scale.getValue() * Energy.getValue();

  // order vertice in each face so that right-handed normal points outwards

  // Order indices for composite (crystal+bump) face set.  11 faces = (2 trapezoids * 6 faces) - (1 hidden one where they join)

  int32_t shapeIndicesFull[5*11]  = {0,    1,    2,    3,    SO_END_FACE_INDEX,
		   	         7,    6,    5,    4,	 SO_END_FACE_INDEX,
		   	         1,    5,    6,    2,	 SO_END_FACE_INDEX,
		   	         2,    6,    7,    3,	 SO_END_FACE_INDEX,
		   	         3,    7,    4,    0,	 SO_END_FACE_INDEX,
		   	         0,    4,    5,    1,	 SO_END_FACE_INDEX, // end of crystal vertices
 			        15,   14,   13,   12,    SO_END_FACE_INDEX,
 			         9,   13,   14,   10,    SO_END_FACE_INDEX,
 			        10,   14,   15,   11,    SO_END_FACE_INDEX,
 			        11,   15,   12,    8,    SO_END_FACE_INDEX,
 			         8,   12,   13,    9,    SO_END_FACE_INDEX};
 			         
  int32_t linesIndicesFull[5*11]  = {0,    1,    2,    3,    SO_END_LINE_INDEX,  
		   	    	 7,    6,    5,    4,	 SO_END_LINE_INDEX,
		   	    	 1,    5,    6,    2,	 SO_END_LINE_INDEX,
		   	    	 2,    6,    7,    3,	 SO_END_LINE_INDEX,
		   	    	 3,    7,    4,    0,	 SO_END_LINE_INDEX,
		   	    	 0,    4,    5,    1,	 SO_END_LINE_INDEX,
 			        15,   14,   13,   12,    SO_END_LINE_INDEX,
 			         9,   13,   14,   10,    SO_END_LINE_INDEX,
 			        10,   14,   15,   11,    SO_END_LINE_INDEX,
 			        11,   15,   12,    8,    SO_END_LINE_INDEX,
 			         8,   12,   13,    9,    SO_END_LINE_INDEX};
 
  // Order indices for single trapezoid (either crystal or bump but not both) face set. 

  int32_t shapeIndicesTrap[5*6]   = {0,    1,    2,    3,    SO_END_FACE_INDEX,
		   	         7,    6,    5,    4,	 SO_END_FACE_INDEX,
		   	         1,    5,    6,    2,	 SO_END_FACE_INDEX,
		   	         2,    6,    7,    3,	 SO_END_FACE_INDEX,
		   	         3,    7,    4,    0,	 SO_END_FACE_INDEX,
		   	         0,    4,    5,    1,	 SO_END_FACE_INDEX};
  
  int32_t linesIndicesTrap[5*6]   = {0,    1,    2,    3,    SO_END_LINE_INDEX,  
		   	    	 7,    6,    5,    4,	 SO_END_LINE_INDEX,
		   	    	 1,    5,    6,    2,	 SO_END_LINE_INDEX,
		   	    	 2,    6,    7,    3,	 SO_END_LINE_INDEX,
		   	    	 3,    7,    4,    0,	 SO_END_LINE_INDEX,
		   	    	 0,    4,    5,    1,	 SO_END_LINE_INDEX};
  


// Decide whether hit should be drawn based on user's desire and energy threshold

  SbBool DrawTheHit;
  
  if (DrawHit.getValue() && Energy.getValue() >= myMinimumEnergy) {        // draw the hit
    DrawTheHit = TRUE;
    }
  else{
    DrawTheHit = FALSE;
    }
   
   
   
// Draw crystal and/or hit if requested
// ------------------------------------   
   
   
//  cout << "DrawCrystal.getValue(), DrawTheHit " << DrawCrystal.getValue() << "," << DrawTheHit << endl; 
   
  if (DrawCrystal.getValue() && DrawTheHit) {                                     // draw crystal and hit
   
    shapeSoCoordinate3->point.setValues(0,16,shapeVertices); 
    linesSoCoordinate3->point.setValues(0,16,shapeVertices); 
       
    theSoIndexedFaceSet->coordIndex.setValues(0,55, shapeIndicesFull);
    theSoIndexedLineSet->coordIndex.setValues(0,55, linesIndicesFull);
 
  }
  else if (DrawCrystal.getValue() && !DrawTheHit)  {                              // draw crystal only
   
    shapeSoCoordinate3->point.setValues(0,8,shapeVertices);                       // start at 0 and use 8 points
    linesSoCoordinate3->point.setValues(0,8,shapeVertices); 
     
    theSoIndexedFaceSet->coordIndex.setValues(0,30, shapeIndicesTrap);
    theSoIndexedLineSet->coordIndex.setValues(0,30, linesIndicesTrap);
 
  }

  else if (!DrawCrystal.getValue() && DrawTheHit)  {                              // draw hit only

  // fill up first 8 entries with shape of crystal

  for (int ivert = 0 ; ivert < 8 ; ivert++){
    shapeVertices[ivert] = shapeVertices[ivert+8]; 
    shapeVertices[ivert] = shapeVertices[ivert+8]; 
    }   

    shapeSoCoordinate3->point.setValues(0,8,shapeVertices);                       // start at 8 and use 8 points
    linesSoCoordinate3->point.setValues(0,8,shapeVertices); 
     
    theSoIndexedFaceSet->coordIndex.setValues(0,30, shapeIndicesTrap);     
    theSoIndexedLineSet->coordIndex.setValues(0,30, linesIndicesTrap);   
 
  }
  
  theSoDrawStyle->lineWidth.setValue(2);
  theSoDrawStyle->linePattern.setValue(0xffff);    // 0xffff = solid


}

//------------------//
// generateChildren //
//------------------//

void SoCrystalHit::generateChildren() {

  assert(children->getLength() ==0);
  SoSeparator      *theSoSeparator       = new SoSeparator(); 
  SoCoordinate3    *shapeSoCoordinate3   = new SoCoordinate3;
  SoIndexedFaceSet *theSoIndexedFaceSet  = new SoIndexedFaceSet;
  SoDrawStyle      *theSoDrawStyle       = new SoDrawStyle;
  SoCoordinate3    *linesSoCoordinate3   = new SoCoordinate3;
  SoIndexedLineSet *theSoIndexedLineSet  = new SoIndexedLineSet;
  
  theSoSeparator->addChild(shapeSoCoordinate3);
  theSoSeparator->addChild(theSoIndexedFaceSet);
  theSoSeparator->addChild(theSoDrawStyle);
  theSoSeparator->addChild(linesSoCoordinate3);
  theSoSeparator->addChild(theSoIndexedLineSet);

  children->append(theSoSeparator);
}

//----------------------//
// generateAlternateRep //
//----------------------//

void SoCrystalHit::generateAlternateRep(SoAction*) {

  // This sets the alternate representation to the child list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *children)[0]);
}

//-------------------//
// clearAlternateRep //
//-------------------//

void SoCrystalHit::clearAlternateRep() {
  alternateRep.setValue(NULL);

}

//---------------------//
// setParameters       //
//---------------------//

void SoCrystalHit::setParameters(){

//   phi    = fPhi.getValue();
//   theta  = fTheta.getValue();
//   radius = fRadius.getValue();

}


//--------------------------------//
// getInfo                        //
//   Dump of physics info if      //
//   graphics object is selected  //
//--------------------------------//

const char* SoCrystalHit::getInfo() {
  /* 
  ostrstream cat;
  cat.precision(3);
  cat.setf(ios::fixed, ios::floatfield);

  cat << "You have picked an SoCrystalHit.\n";
  cat << "-----------------------------\n";
  cat << " Energy = " << Energy.getValue() << " GeV\n";

  cat.put('\0');               // now it is a real string
  char* dump = cat.str();
  return dump; 
  */

  char s[32];
  fInfo = "";
  fInfo += "You have picked an SoCrystalHit.\n";
  fInfo += "-----------------------------\n";
  fInfo += " Energy = ";
  sprintf(s,"%g",Energy.getValue()); fInfo += s;
  fInfo += "\n";

  return fInfo.getString();
}



