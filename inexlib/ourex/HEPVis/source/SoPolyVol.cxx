//#include "stdafx.h"
//remove the above line to run with compilers other than Visual C++
#include <HEPVis/nodes/SoPolyVol.h>

#ifdef __KCC
#include <cassert>
#include <cmath>
#else
#include <assert.h>
#include <math.h>
#endif

#include <vector>

#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoShapeHints.h>


//////////////////////////////////////////////////////////////////////////////////
//
// G. Alverson, CMS/Northeastern University Software Group
// 16 December 1998
//
//
// SoPolyVol creates a PolyVol.
//
//
//
//////////////////////////////////////////////////////////////////////////////////



SO_NODE_SOURCE(SoPolyVol)

SoPolyVol::SoPolyVol() {
	SO_NODE_CONSTRUCTOR(SoPolyVol);


	SO_NODE_ADD_FIELD(vtxPts, (SbVec3f(0,0,0)));
	SO_NODE_ADD_FIELD(center, (SbVec3f(0,0,0)));
	SO_NODE_ADD_FIELD(dZ, (1.0));
	SO_NODE_ADD_FIELD(alternateRep, (NULL));
	children = new SoChildList(this);

	const SbVec3f defPolyVol[10] = {
		SbVec3f(0,0,0),SbVec3f(1,0,0),SbVec3f(1.5F,1,0),
                SbVec3f(0.2F,1.2F,0),SbVec3f(-0.2F,1,0),
		SbVec3f(0,0,0.5F),SbVec3f(1,0,0.5F),SbVec3f(1.5F,1,0.5F),
		SbVec3f(0.2F,1.2F,0.5F),SbVec3f(-0.2F,1,0.5F)	
	};
	vtxPts.setValues(0,10,defPolyVol);
}

SoPolyVol::~SoPolyVol(){
	delete children;
}

void SoPolyVol::initClass(){
	SO_NODE_INIT_CLASS(SoPolyVol, SoShape, "Shape");
}

void SoPolyVol::generateChildren(){
	assert(children->getLength() ==0);
//  Create a separator upon which to hang the co-ords and the surface
	SoSeparator	*PolyVolSeparator	= new SoSeparator();
	SoTranslation	*PolyVolTranslation	= new SoTranslation();


//	Add a ShapeHint
	SoShapeHints *PolyVolShapeHints = new SoShapeHints;
//  Create the co-ordinates... 
    SoVertexProperty *PolyVolVertexProperty	= new SoVertexProperty;

//  Create the face set 
	SoIndexedFaceSet *PolyVolFaceSet = new SoIndexedFaceSet;

//  Tie them all together
	PolyVolSeparator->addChild(PolyVolTranslation);
	PolyVolSeparator->addChild(PolyVolShapeHints);
	PolyVolSeparator->addChild(PolyVolVertexProperty);
	PolyVolSeparator->addChild(PolyVolFaceSet);

	children->append(PolyVolSeparator);
	updateChildren();

//	Connect up to the outside world	
	PolyVolTranslation->translation.connectFrom(&center);


}
void SoPolyVol::generateAlternateRep(SoAction*) {
	if (children->getLength() == 0) generateChildren();
	updateChildren();
	alternateRep.setValue(( *children)[0]);
}
void SoPolyVol::clearAlternateRep() {
	alternateRep.setValue(NULL);
}

void SoPolyVol::updateChildren() {



	assert(children->getLength()==1);
//  Check to see if we actually need to change anything
	if( dZ.getValue() == dZLocal &&
		center.getValue() == centerLocal &&
		vtxPts == vtxPtsLocal)
		return;

	centerLocal = center.getValue();
	vtxPtsLocal = vtxPts;

	SoSeparator	*PolyVolSeparator = static_cast<SoSeparator *>( ( *children)[0]);
	SoShapeHints *PolyVolShapeHints = static_cast<SoShapeHints *>( PolyVolSeparator->getChild(1));
	SoVertexProperty *PolyVolVertexProperty = static_cast<SoVertexProperty *>( PolyVolSeparator->getChild(2));
	SoIndexedFaceSet *PolyVolFaceSet = static_cast<SoIndexedFaceSet *>( PolyVolSeparator->getChild(3));

	assert(vtxPts.getNum()>2);

	PolyVolShapeHints->vertexOrdering.setValue(SoShapeHints::COUNTERCLOCKWISE);
	PolyVolShapeHints->faceType.setValue(SoShapeHints::UNKNOWN_FACE_TYPE);
	PolyVolShapeHints->creaseAngle.setValue(0);

	PolyVolVertexProperty->vertex = vtxPts;

	int nsides = vtxPts.getNum()/2; // number of sides of front polygon

	PolyVolFaceSet->coordIndex.setNum(7*nsides+2); //5*nsides for side polys+ 2*(nsides+1) for front and back
	int32_t *indx = PolyVolFaceSet->coordIndex.startEditing();
//  sides
	int i; 
	for (i=0; i< nsides-1; i++) {
		*indx++ = i;
		*indx++ = i+nsides;
		*indx++ = i+nsides+1;
		*indx++ = i+1;
		*indx++ = SO_END_FACE_INDEX;
	} 
	*indx++ = i; *indx++ = i+nsides; *indx++ = nsides; *indx++ = 0;
	*indx++ = SO_END_FACE_INDEX;

//  front & back faces; back face counts down
	for (i=0; i< nsides; i++) {
		*(indx+nsides+1) = 2*nsides-1-i; //bump past n vertices+end_face
		*indx++ = i;
		}
		*indx++ = SO_END_FACE_INDEX;
		*(indx+nsides) = SO_END_FACE_INDEX;
	PolyVolFaceSet->coordIndex.finishEditing();


}

void SoPolyVol::computeBBox(SoAction *, SbBox3f &box, SbVec3f &TheCenter ){
// replacement for pure virtual function in SoShape
	float xExtentMin = 0, xExtentMax = 0, yExtentMin = 0, yExtentMax=0;
	float zExtentMax = dZ.getValue(), zExtentMin = 0;
	const SbVec3f *vtxPtsCopy = vtxPts.getValues(0);
	float x, y, z;
	for (int i=0; i<vtxPts.getNum(); i++, vtxPtsCopy++) {
		vtxPtsCopy->getValue(x,y,z);
		if (x > xExtentMax) xExtentMax = x;
		if (x < xExtentMin) xExtentMin = x;
		if (y > yExtentMax) yExtentMax = y;
		if (y < yExtentMin) yExtentMin = y;
		if (z > zExtentMax) zExtentMax = z;
		if (z < zExtentMin) zExtentMin = z;
	}
	box.setBounds(center.getValue()+SbVec3f(xExtentMin,yExtentMin,zExtentMin),
		          center.getValue()+SbVec3f(xExtentMax,yExtentMax,zExtentMax));
	TheCenter = center.getValue();
}
void SoPolyVol::generatePrimitives(SoAction *action){
//replacement for pure virtual function in SoShape
	if (children->getLength()==0) generateChildren();
	updateChildren();
	children->traverse(action);
}
SoChildList *SoPolyVol::getChildren() const {
//replacement for SoShape virtual function
	return children;
}
void SoPolyVol::setPolyVolPts(const int nSides, const float dZhalf, 
		const float *angles, const float *sideHalf)
{

	assert(nSides > 2);


//  Stuff the points into the SoMFVec3f
	if (children->getLength() == 0) generateChildren();
	vtxPts.setNum(2*nSides);
	SbVec3f *sbPt = vtxPts.startEditing();
	float x=0, y=0, z=0;
	for (int i=0;i< nSides;i++,sbPt++){		
		*sbPt = SbVec3f(x,y,z);
		*(sbPt+nSides) = SbVec3f(x,y,z+2*dZhalf);
		x += 2 * (*sideHalf) * (float)cos(*angles);
		y += 2 * (*sideHalf++) * (float)sin(*angles++);
	}
	vtxPts.finishEditing();
}

