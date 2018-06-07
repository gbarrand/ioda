/*
 *  SoTestKit.cxx
 *  
 *  Created by Guy Barrand to test / debug VRML2 convertion of nodekits.
 *    12 Oct 2006.
 *
 */ 

// this :  
#include <HEPVis/nodekits/SoTestKit.h>

// Inventor :
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>

// HEPVis :
#include <HEPVis/SbStyle.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

SO_KIT_SOURCE(SoTestKit)

//////////////////////////////////////////////////////////////////////////////
void SoTestKit::initClass(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoTestKit,SoBaseKit,"BaseKit");
}
//////////////////////////////////////////////////////////////////////////////
SoTestKit::SoTestKit(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoTestKit);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0, FALSE);
  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  initialize();
}
//////////////////////////////////////////////////////////////////////////////
SoTestKit::~SoTestKit(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
void SoTestKit::initialize(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSeparator* soTop = (SoSeparator*)topSeparator.getValue();
  soTop->removeAllChildren();

  SoMaterial* material = new SoMaterial;
  //material->diffuseColor.setValue(1, 1, 1);
  material->emissiveColor.setValue(1, 0, 0);
  soTop->addChild(material);

  SoDrawStyle* drawStyle = new SoDrawStyle;
  drawStyle->style.setValue(SoDrawStyle::LINES);
  //drawStyle->style.setValue(SoDrawStyle::FILLED);
  drawStyle->lineWidth.setValue(1);
  drawStyle->linePattern.setValue(SbLinePattern_solid);
  soTop->addChild(drawStyle);

  SbVec3f points[2];
  points[0].setValue(0,0,0);
  points[1].setValue(1,0,0);

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t pointn = 2;
  coordinate3->point.setValues(0,pointn,points);
  soTop->addChild(coordinate3);

  SoLineSet* lineSet = new SoLineSet;
  lineSet->numVertices.setValues(0,1,&pointn);
  soTop->addChild(lineSet);

}
//////////////////////////////////////////////////////////////////////////////
void SoTestKit::generateAlternateRep(SoAction*) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  printf("debug : SoTestKit::generateAlternateRep : +++\n");
  //alternateRep.setValue(topSeparator.getValue());

  //SoSeparator* separator = new SoSeparator;
  //separator->addChild(topSeparator.getValue().copy());

  alternateRep.setValue(topSeparator.getValue()->copy());
}
//////////////////////////////////////////////////////////////////////////////
void SoTestKit::clearAlternateRep(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(NULL);
}
