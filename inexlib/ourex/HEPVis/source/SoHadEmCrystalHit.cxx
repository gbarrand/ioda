/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoHadEmCrystalHit                                       */
/* Description:      Draws a hit in an individual crystal                    */
/* Author:           J. Boudreau based on SoCrystal Hit by L. Taylor         */
/*                                                                           */
/* 02/02/2001 : G.Barrand : delete children in destructor.                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
//
// Synopsis:
//   #include<HEPVis/nodes/SoHadEmCrystalHit.h>
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
//    SoSFFloat Energy; 	     // Energy                  (GeV)
//    SoSFFloat MinimumEnergyTot;    // Minimum Energy to draw  (GeV)
//    SoSFFloat Scale;  	     // scale for energy (GeV / unit distance)
//    SoSFFloat RelativeWidth;       // Width of energy bump relative to crystal [0,1]
//
//    SoSFVec3f	Front1; 	     // corner 1 of front face
//    SoSFVec3f	Front2; 	     // corner 2 of front face
//    SoSFVec3f	Front3; 	     // corner 3 of front face
//    SoSFVec3f	Front4; 	     // corner 4 of front face
//      	            

#include <assert.h>
#include <math.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SbBasic.h>
#include <Inventor/SbBox.h>

#include <Inventor/actions/SoGLRenderAction.h>

#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFBool.h>

#include <Inventor/misc/SoChildList.h>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoNormal.h>

#include <HEPVis/nodes/SoHadEmCrystalHit.h>
const int32_t SoHadEmCrystalHit::shapeIndicesHad[5*6]   = {0,    1,    2,    3,    SO_END_FACE_INDEX,
						       7,    6,    5,    4,	 SO_END_FACE_INDEX,

						       1,    5,    6,    2,	 SO_END_FACE_INDEX,
						       2,    6,    7,    3,	 SO_END_FACE_INDEX,
						       3,    7,    4,    0,	 SO_END_FACE_INDEX,
						       0,    4,    5,    1,	 SO_END_FACE_INDEX};

const int32_t SoHadEmCrystalHit::shapeIndicesEm[5*5]   = {  15,   14,   13,   12,    SO_END_FACE_INDEX,

							9,   13,   14,   10,    SO_END_FACE_INDEX,
							10,   14,   15,   11,   SO_END_FACE_INDEX,
							11,   15,   12,    8,   SO_END_FACE_INDEX,
							8,   12,   13,    9,    SO_END_FACE_INDEX};

const int32_t SoHadEmCrystalHit::normalIndicesHad[6]= {0,1,2,3,4,5};
const int32_t SoHadEmCrystalHit::normalIndicesEm [5]= {1,2,3,4,5  };



SO_NODE_SOURCE(SoHadEmCrystalHit)
    
    //-------------//
    // Constructor //
    //-------------//

SoHadEmCrystalHit::SoHadEmCrystalHit() {

  SO_NODE_CONSTRUCTOR(SoHadEmCrystalHit);

  // Initialize data fields
  // ----------------------

  SO_NODE_ADD_FIELD(EnergyHad,	        (0));
  SO_NODE_ADD_FIELD(EnergyEm,	        (0));
  SO_NODE_ADD_FIELD(MinimumEnergyTot,   (0.001F));  // 1 MeV 
  SO_NODE_ADD_FIELD(Scale ,	        (1));
  SO_NODE_ADD_FIELD(RelativeWidth,      (0.75F));
 
 
  SO_NODE_ADD_FIELD(Front1,	   ( 0.5F,  0.5F, 0) );   // initialise to unit box in +z
  SO_NODE_ADD_FIELD(Front2,	   (-0.5F,  0.5F, 0) );
  SO_NODE_ADD_FIELD(Front3,	   (-0.5F, -0.5F, 0) );
  SO_NODE_ADD_FIELD(Front4,	   ( 0.5F, -0.5F, 0) );

  SO_NODE_ADD_FIELD(alternateRep,  (NULL));

  children = new SoChildList(this);

  myEnergyHad        = EnergyHad.getValue();
  myEnergyEm         = EnergyEm.getValue();
  myMinimumEnergyTot = MinimumEnergyTot.getValue();
  myScale            = Scale.getValue();
  myRelativeWidth    = RelativeWidth.getValue();
  
  myFront1        = Front1.getValue();
  myFront2        = Front2.getValue();
  myFront3        = Front3.getValue();
  myFront4        = Front4.getValue();

}


//-------------//
// Destructor  //
//-------------//

SoHadEmCrystalHit::~SoHadEmCrystalHit() {
  delete children;
}

//-----------//
// initClass //
//-----------//

void SoHadEmCrystalHit::initClass(){
  // This statement is required.
  SO_NODE_INIT_CLASS(SoHadEmCrystalHit,SoShape,"Shape");
}

//-----------//
// GLRrender //
//-----------//

void SoHadEmCrystalHit::GLRender(SoGLRenderAction *action) {
  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();
  
   if ( 
        (EnergyHad.getValue()          != myEnergyHad)        ||
        (EnergyEm.getValue()           != myEnergyEm)         ||
        (MinimumEnergyTot.getValue()   != myMinimumEnergyTot) ||
	(Scale.getValue()              != myScale)            ||
	(RelativeWidth.getValue()      != myRelativeWidth)    ||
	(Front1.getValue()             != myFront1)           ||
	(Front2.getValue()             != myFront2)           ||
	(Front3.getValue()             != myFront3)           ||
	(Front4.getValue()             != myFront4)
	) { 
     
       updateChildren();
   }

  children->traverse(action);  

  myEnergyHad          = EnergyHad.getValue();

  myEnergyEm           = EnergyEm.getValue();
  myMinimumEnergyTot   = MinimumEnergyTot.getValue();
  myScale              = Scale.getValue() ;
  myRelativeWidth      = RelativeWidth.getValue() ;
  myFront1             = Front1.getValue();
  myFront2             = Front2.getValue();
  myFront3             = Front3.getValue();
  myFront4             = Front4.getValue();

}

//--------------------//
// generatePrimitives //
//--------------------//

void SoHadEmCrystalHit::generatePrimitives(SoAction *action) {
  if (children->getLength()==0) generateChildren();
  if ( 
      (EnergyHad.getValue()          != myEnergyHad)        ||
      (EnergyEm.getValue()           != myEnergyEm)         ||
      (MinimumEnergyTot.getValue()   != myMinimumEnergyTot) ||
      (Scale.getValue()              != myScale)            ||
      (RelativeWidth.getValue()      != myRelativeWidth)    ||
      (Front1.getValue()             != myFront1)           ||
      (Front2.getValue()             != myFront2)           ||
      (Front3.getValue()             != myFront3)           ||
      (Front4.getValue()             != myFront4)
      ) { 
      
      updateChildren();
   }
  children->traverse(action);
  myEnergyHad          = EnergyHad.getValue();
  myEnergyEm           = EnergyEm.getValue();
  myMinimumEnergyTot   = MinimumEnergyTot.getValue();
  myScale              = Scale.getValue() ;
  myRelativeWidth      = RelativeWidth.getValue() ;
  myFront1             = Front1.getValue();
  myFront2             = Front2.getValue();
  myFront3             = Front3.getValue();
  myFront4             = Front4.getValue();
}

//-------------//
// getChildren //
//-------------//

SoChildList *SoHadEmCrystalHit::getChildren() const {
  return children;
}

//-------------//
// computeBBox //
//-------------//

 void SoHadEmCrystalHit::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){

   // TO BE DONE
   //float depth = (Scale.getValue())*(Energy.getValue());
   float length=Scale.getValue()*(EnergyHad.getValue()+EnergyEm.getValue())/2;
   float depth = (Front1.getValue()-Front3.getValue()).length()/2;
   float dim  = length > depth ? length : depth;

   SbVec3f v = (Front1.getValue()+Front3.getValue())/2;
   SbVec3f vdir = v/v.length();
   SbVec3f deltaV = vdir*length;
   center = v + deltaV;
  
   SbVec3f u = SbVec3f(dim,dim,dim);
   box.setBounds(center-u, center+u);
   

 }

//----------------//
// updateChildren //
//----------------//

void SoHadEmCrystalHit::updateChildren() {

  assert(children->getLength()==1);
  
  SoSeparator       *theSoSeparator       = (SoSeparator *)       ( *children)[0];
  SoVertexProperty  *vertexProperty       = (SoVertexProperty *)  (theSoSeparator->getChild(1));
  //SoIndexedFaceSet  *hadSoIndexedFaceSet  = (SoIndexedFaceSet *)  (theSoSeparator->getChild(2));
  //SoIndexedFaceSet  *emSoIndexedFaceSet   = (SoIndexedFaceSet *)  (theSoSeparator->getChild(4));
  
  assert (EnergyHad.getValue()        >= 0.0);
  assert (EnergyEm.getValue()         >= 0.0);
  assert (Scale.getValue()            >= 0.0);
  assert (RelativeWidth.getValue()    >= 0.0);
  assert (RelativeWidth.getValue()    <= 1.0);

  // HadEmCrystal part of shape

  shapeVertices[0] = Front1.getValue();
  shapeVertices[1] = Front2.getValue();
  shapeVertices[2] = Front3.getValue();
  shapeVertices[3] = Front4.getValue();

  SbVec3f u1=Front1.getValue()/Front1.getValue().length();
  SbVec3f u2=Front2.getValue()/Front2.getValue().length();
  SbVec3f u3=Front3.getValue()/Front3.getValue().length();
  SbVec3f u4=Front4.getValue()/Front4.getValue().length();

  shapeVertices[4] = Front1.getValue()+ Scale.getValue()*EnergyHad.getValue()*u1;
  shapeVertices[5] = Front2.getValue()+ Scale.getValue()*EnergyHad.getValue()*u2;
  shapeVertices[6] = Front3.getValue()+ Scale.getValue()*EnergyHad.getValue()*u3;
  shapeVertices[7] = Front4.getValue()+ Scale.getValue()*EnergyHad.getValue()*u4;
  // Energy part  

  // First find shape of crystal shrunk laterally according to RelativeWidth

  //float delta = (1.0 - 0.5 * (1.0 - RelativeWidth.getValue()));  // take half off each side 

  // shrunken front face coplanar with original plane

  SbVec3f myShrunkenFront1 = shapeVertices[4];// + delta * (Back3.getValue() - Back1.getValue());
  SbVec3f myShrunkenFront2 = shapeVertices[5];// + delta * (Back4.getValue() - Back2.getValue());
  SbVec3f myShrunkenFront3 = shapeVertices[6];// + delta * (Back1.getValue() - Back3.getValue());
  SbVec3f myShrunkenFront4 = shapeVertices[7];// + delta * (Back2.getValue() - Back4.getValue());

  SbVec3f v1=myShrunkenFront1/myShrunkenFront1.length();
  SbVec3f v2=myShrunkenFront2/myShrunkenFront2.length();
  SbVec3f v3=myShrunkenFront3/myShrunkenFront3.length();
  SbVec3f v4=myShrunkenFront4/myShrunkenFront4.length();

  // shrunken back face coplanar with original plane

  SbVec3f myShrunkenBack1  = myShrunkenFront1+v1*Scale.getValue() * EnergyEm.getValue();
  SbVec3f myShrunkenBack2  = myShrunkenFront2+v2*Scale.getValue() * EnergyEm.getValue();
  SbVec3f myShrunkenBack3  = myShrunkenFront3+v3*Scale.getValue() * EnergyEm.getValue();
  SbVec3f myShrunkenBack4  = myShrunkenFront4+v4*Scale.getValue() * EnergyEm.getValue();

  // Carry on shape of (laterally shrunken) crystal starting from back face 


  shapeVertices[12]  = myShrunkenBack1;
  shapeVertices[13]  = myShrunkenBack2;
  shapeVertices[14]  = myShrunkenBack3;
  shapeVertices[15]  = myShrunkenBack4;

  // Make the length proportional to the energy 

  shapeVertices[8]  = myShrunkenFront1;
  shapeVertices[9]  = myShrunkenFront2;
  shapeVertices[10] = myShrunkenFront3;
  shapeVertices[11] = myShrunkenFront4;


  // The normal facing out:
  normalVertices[0] = (Front2.getValue()-Front3.getValue()).cross(Front2.getValue()-Front1.getValue());
  normalVertices[1] = -normalVertices[0];
  normalVertices[2] = (Front3.getValue()).cross(Front3.getValue()-Front2.getValue());
  normalVertices[3] = (Front4.getValue()).cross(Front4.getValue()-Front3.getValue());
  normalVertices[4] = (Front1.getValue()).cross(Front1.getValue()-Front4.getValue());
  normalVertices[5] = (Front2.getValue()).cross(Front2.getValue()-Front1.getValue());
  for (int v=0;v<6;v++) {
    normalVertices[v] *= 1.0F/normalVertices[v].length();
  }


  // order vertice in each face so that right-handed normal points outwards

  // Order indices for composite (crystal+bump) face set.  11 faces = (2 trapezoids * 6 faces) - (1 hidden one where they join)

  // Order indices for single trapezoid (either crystal or bump but not both) face set. 


// Decide whether hit should be drawn based on user's desire and energy threshold

  SbBool DrawTheHit;
  
  if (EnergyHad.getValue()+EnergyEm.getValue() >= myMinimumEnergyTot) {        // draw the hit
    DrawTheHit = TRUE;
    }
  else{
    DrawTheHit = FALSE;
    }
   
   
   
// Draw crystal and/or hit if requested
// ------------------------------------   
   
   
//  cout << "DrawHadEmCrystal.getValue(), DrawTheHit " << DrawHadEmCrystal.getValue() << "," << DrawTheHit << endl; 
   
  if (DrawTheHit) {                                                                // draw crystal and hit


    vertexProperty->vertex.setValues(0,16,shapeVertices); 
    vertexProperty->normal.setValues(0,6,normalVertices);
  }
  
}

//------------------//
// generateChildren //
//------------------//

void SoHadEmCrystalHit::generateChildren() {

  assert(children->getLength() ==0);
  SoSeparator      *theSoSeparator       = new SoSeparator(); 
  SoVertexProperty *vertexProperty       = new SoVertexProperty;
  SoIndexedFaceSet *hadSoIndexedFaceSet  = new SoIndexedFaceSet;
  SoIndexedFaceSet *emSoIndexedFaceSet   = new SoIndexedFaceSet;

  SoMaterial *redMaterial = new SoMaterial;
  redMaterial->diffuseColor.setValue(1,0,0);

  SoMaterial *blueMaterial = new SoMaterial;
  blueMaterial->diffuseColor.setValue(0,0,1);

  vertexProperty->normalBinding=SoVertexProperty::PER_FACE_INDEXED;

  //
  theSoSeparator->addChild(redMaterial);           // 0
  theSoSeparator->addChild(vertexProperty);        // 1
  theSoSeparator->addChild(hadSoIndexedFaceSet);   // 2
  theSoSeparator->addChild(blueMaterial);          // 3
  theSoSeparator->addChild(emSoIndexedFaceSet);    // 4


  hadSoIndexedFaceSet->coordIndex.setValues(0,30, shapeIndicesHad);
  emSoIndexedFaceSet->coordIndex.setValues(0,25, shapeIndicesEm);
  hadSoIndexedFaceSet->normalIndex.setValues(0,6,normalIndicesHad);
  emSoIndexedFaceSet->normalIndex.setValues(0,5,normalIndicesEm);
  children->append(theSoSeparator);
}

//----------------------//
// generateAlternateRep //
//----------------------//

void SoHadEmCrystalHit::generateAlternateRep(SoAction*) {

  // This sets the alternate representation to the child list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *children)[0]);
}

//-------------------//
// clearAlternateRep //
//-------------------//

void SoHadEmCrystalHit::clearAlternateRep() {
  alternateRep.setValue(NULL);

}



