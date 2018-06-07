/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             So3DErrorBar                                            */
/* Description:      3D point with error bars in 3 orthogonal directions     */
/* Author:           Lucas Taylor                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
//
// Synopsis:
//   #include<HEPVis/nodes/So3DErrorBar.h>
//
//  Draws a 3D point with error bars in 3 orthogonal directions.  
//  Bars are aligned arbitrarily in space (i.e. not necessarily
//  parallel to x,y,z axes.
//  Used for e.g. a space point from a tracker.
//
//  Fields:
//   
//    SoSFFloat lineWidth;   // line thickness in pixels
//    SoSFVec3f	u1; 	     // first  end of error bar in u dimension
//    SoSFVec3f	u2; 	     // second end of error bar in u dimension
//    SoSFVec3f	v1; 	     // first  end of error bar in v dimension
//    SoSFVec3f	v2; 	     // second end of error bar in v dimension
//    SoSFVec3f	w1; 	     // first  end of error bar in w dimension
//    SoSFVec3f	w2; 	     // second end of error bar in w dimension
//
//
#include <assert.h>
#include <math.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include <HEPVis/nodes/So3DErrorBar.h>

SO_NODE_SOURCE(So3DErrorBar)

//-------------//
// Constructor //
//-------------//
So3DErrorBar::So3DErrorBar() {

  SO_NODE_CONSTRUCTOR(So3DErrorBar);

  // Initialize data fields
  // ----------------------
 
  SO_NODE_ADD_FIELD(lineWidth, ( 1.0) );
  SO_NODE_ADD_FIELD(u1,	       ( 0.5,  0.0,  0.0) );
  SO_NODE_ADD_FIELD(u2,	       (-0.5,  0.0,  0.0) );
  SO_NODE_ADD_FIELD(v1,	       ( 0.0,  0.5,  0.0) );
  SO_NODE_ADD_FIELD(v2,	       ( 0.0, -0.5,  0.0) );
  SO_NODE_ADD_FIELD(w1,	       ( 0.0,  0.0,  0.5) );
  SO_NODE_ADD_FIELD(w2,	       ( 0.0,  0.0, -0.5) );

  SO_NODE_ADD_FIELD(alternateRep,  (NULL));

  children = new SoChildList(this);
  
  // initialize private data members
  //--------------------------------  

  mylineWidth = lineWidth.getValue();
  myu1        = u1.getValue();
  myu2        = u2.getValue();
  myv1        = v1.getValue();
  myv2        = v2.getValue();
  myw1        = w1.getValue();
  myw2        = w2.getValue();

}

//-------------//
// Destructor  //
//-------------//

So3DErrorBar::~So3DErrorBar() {
}

//-----------//
// initClass //
//----------//

void So3DErrorBar::initClass(){

  // This statement is required.
  SO_NODE_INIT_CLASS(So3DErrorBar,SoShape,"Shape");
}

//-----------//
// GLRrender //
//-----------//

void So3DErrorBar::GLRender(SoGLRenderAction *action) {

  if (!shouldGLRender(action)) return;
  if (children->getLength() == 0) generateChildren();

  // Update the picture (ie render it) if a field has changed
  // --------------------------------------------------------

   if ( (lineWidth.getValue() != mylineWidth) ||
        (u1.getValue()	      != myu1)	      || 
        (u2.getValue()	      != myu2)	      ||
        (v1.getValue()	      != myv1)	      || 
        (v2.getValue()	      != myv2)	      ||
        (w1.getValue()	      != myw1)	      ||
        (w2.getValue()	      != myw2)	      )
       { 
   updateChildren();
   }
   
  children->traverse(action);  

  mylineWidth = lineWidth.getValue(); 
  myu1	      = u1.getValue();
  myu2	      = u2.getValue();
  myv1	      = v1.getValue();
  myv2	      = v2.getValue();
  myw1	      = w1.getValue();
  myw2	      = w2.getValue();
  
}

//--------------------//
// generatePrimitives //
//--------------------//

void So3DErrorBar::generatePrimitives(SoAction *action) {

  if (children->getLength()==0) generateChildren();
  updateChildren();
  children->traverse(action);
}

//-------------//
// getChildren //
//-------------//

SoChildList *So3DErrorBar::getChildren() const {
  return children;
}

//-------------//
// computeBBox //
//-------------//

void So3DErrorBar::computeBBox(SoAction *, SbBox3f & /*box*/, SbVec3f & /*center*/ ){

//
//  Put something in here...
//
//   // slop of 100% in size of corner to allow arrowheads to fit in bounding box
//   float corner = 1.5*fDivisionLength.getValue()*
//                  (float)(fNDivision.getValue());
//   SbVec3f min(0.0, 0.0, 0.0);
//   SbVec3f max( corner, corner, corner);
//   center.setValue(corner/2.0, corner/2.0, corner/2.0);
//   box.setBounds(min,max);

}

//----------------//
// updateChildren //
//----------------//

void So3DErrorBar::updateChildren() {

  assert(children->getLength()==1);
 
  SoSeparator       *theSoSeparator       = (SoSeparator *)       ( *children)[0];
  SoDrawStyle       *theSoDrawStyle       = (SoDrawStyle *)       (theSoSeparator->getChild(0));
  SoCoordinate3     *linesSoCoordinate3   = (SoCoordinate3 *)     (theSoSeparator->getChild(1));
  SoIndexedLineSet  *theSoIndexedLineSet  = (SoIndexedLineSet *)  (theSoSeparator->getChild(2));

  const int nvert = 6;
  SbVec3f lineVertices[nvert];

  lineVertices[0] = u1.getValue();
  lineVertices[1] = u2.getValue();
  
  lineVertices[2] = v1.getValue();
  lineVertices[3] = v2.getValue();
  
  lineVertices[4] = w1.getValue();
  lineVertices[5] = w2.getValue();

  int32_t linesIndices[3*3] = {0, 1, SO_END_LINE_INDEX,  
		       	   2, 3, SO_END_LINE_INDEX,
		   	   4, 5, SO_END_LINE_INDEX};

  linesSoCoordinate3->point.setValues(0,6,lineVertices); 
  theSoIndexedLineSet->coordIndex.setValues(0,9,linesIndices);

  theSoDrawStyle->lineWidth.setValue(lineWidth.getValue());
  theSoDrawStyle->linePattern.setValue(0xffff);    // 0xffff = solid

}

//------------------//
// generateChildren //
//------------------//

void So3DErrorBar::generateChildren() {

  assert(children->getLength() ==0);
  
  SoSeparator      *theSoSeparator       = new SoSeparator(); 
  SoDrawStyle      *theSoDrawStyle       = new SoDrawStyle;
  SoCoordinate3    *linesSoCoordinate3   = new SoCoordinate3;
  SoIndexedLineSet *theSoIndexedLineSet  = new SoIndexedLineSet;
  
  theSoSeparator->addChild(theSoDrawStyle);
  theSoSeparator->addChild(linesSoCoordinate3);
  theSoSeparator->addChild(theSoIndexedLineSet);

  children->append(theSoSeparator);

}


//----------------------//
// generateAlternateRep //
//----------------------//

void So3DErrorBar::generateAlternateRep(SoAction*) {

  // This sets the alternate representation to the child list of this mode.  

  if (children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *children)[0]);
}

//-------------------//
// clearAlternateRep //
//-------------------//

void So3DErrorBar::clearAlternateRep() {
  alternateRep.setValue(NULL);

}












