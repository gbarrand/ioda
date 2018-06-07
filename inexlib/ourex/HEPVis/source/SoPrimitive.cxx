// this :
#include <HEPVis/nodekits/SoPrimitive.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/sensors/SoFieldSensor.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

#include <HEPVis/nodes/SoBackPrimitive.h>

#include <math.h>

SO_KIT_SOURCE(SoPrimitive) 
//////////////////////////////////////////////////////////////////////////////
void SoPrimitive::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoPrimitive,SoBaseKit,"BaseKit");
}
//////////////////////////////////////////////////////////////////////////////
SoPrimitive::SoPrimitive(
)
:fObjectWidth(1.)
,fObjectHeight(1.)
,fUpdateSceneGraphSensor(NULL)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoPrimitive);
  SO_KIT_ADD_CATALOG_ENTRY(backSeparator,SoSeparator,FALSE,this,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(backTransform,SoTransform,
                           FALSE,backSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(backMaterial,SoMaterial,
                           FALSE,backSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(backTrd,SoBackPrimitive,FALSE,backSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(frontSeparator,SoSeparator,FALSE,this,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(frontTransform,SoTransform,
                           FALSE,frontSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(frontMaterial,SoMaterial,
                           FALSE,frontSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(frontGroup,SoGroup,FALSE,frontSeparator,\0,TRUE);
  SO_KIT_INIT_INSTANCE();
  
  SO_NODE_ADD_FIELD(requireUpdateSceneGraph,(TRUE));
  
#define OFFSET 0.01F
#define SIZE   1.08F  //1.+2.*0.02+2.*0.02
  SO_NODE_ADD_FIELD(foreground,(SbColor(0,0,0)));
  SO_NODE_ADD_FIELD(background,(SbColor(1,1,1)));
  SO_NODE_ADD_FIELD(shadowThickness,(0.02F));
  SO_NODE_ADD_FIELD(marginWidth,(0.02F));
  SO_NODE_ADD_FIELD(marginHeight,(0.02F));
  // The below is updated by updateSceneGraph.
  SO_NODE_ADD_FIELD(width,(SIZE)); 
  SO_NODE_ADD_FIELD(height,(SIZE)); 
  
  SO_NODE_ADD_FIELD(leftAttachment,(ATTACH_NONE));
  SO_NODE_ADD_FIELD(rightAttachment,(ATTACH_NONE));
  SO_NODE_ADD_FIELD(topAttachment,(ATTACH_NONE));
  SO_NODE_ADD_FIELD(bottomAttachment,(ATTACH_NONE));
  
  SO_NODE_ADD_FIELD(leftNode,(NULL));
  SO_NODE_ADD_FIELD(rightNode,(NULL));
  SO_NODE_ADD_FIELD(topNode,(NULL));
  SO_NODE_ADD_FIELD(bottomNode,(NULL));
  
  SO_NODE_DEFINE_ENUM_VALUE(Attachment,ATTACH_NONE);
  SO_NODE_DEFINE_ENUM_VALUE(Attachment,ATTACH_NODE);
  SO_NODE_DEFINE_ENUM_VALUE(Attachment,ATTACH_OPPOSITE_NODE);
  
  SO_NODE_SET_SF_ENUM_TYPE(leftAttachment,Attachment);
  SO_NODE_SET_SF_ENUM_TYPE(rightAttachment,Attachment);
  SO_NODE_SET_SF_ENUM_TYPE(topAttachment,Attachment);
  SO_NODE_SET_SF_ENUM_TYPE(bottomAttachment,Attachment);
  
  initialize();
}
//////////////////////////////////////////////////////////////////////////////
SoPrimitive::~SoPrimitive(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoMaterial* material;
  material = (SoMaterial*)backMaterial.getValue();
  if(material!=NULL) material->diffuseColor.disconnect();
  material = (SoMaterial*)frontMaterial.getValue();
  if(material!=NULL) material->diffuseColor.disconnect();
  //
  shadowThickness.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  marginWidth.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  marginHeight.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  //
  if(fUpdateSceneGraphSensor!=NULL) {
    fUpdateSceneGraphSensor->detach();
    delete fUpdateSceneGraphSensor;
  }
  leftNode.setValue(NULL);
  rightNode.setValue(NULL);
  topNode.setValue(NULL);
  bottomNode.setValue(NULL);
}
//////////////////////////////////////////////////////////////////////////////
void SoPrimitive::initialize(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fUpdateSceneGraphSensor = new SoFieldSensor(SoPrimitive::sensorCB,this);
  fUpdateSceneGraphSensor->attach(&requireUpdateSceneGraph);
  //
  shadowThickness.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  marginWidth.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  marginHeight.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  //
  SoMaterial* material;
  material = (SoMaterial*)getPart("backMaterial",TRUE);
  if(material!=NULL) material->diffuseColor.connectFrom(&background);
  material = (SoMaterial*)getPart("frontMaterial",TRUE);
  if(material!=NULL) material->diffuseColor.connectFrom(&foreground);
  //
  SoPrimitive::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoPrimitive::updateSceneGraph(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  //printf("debug : SoPrimitive::updatePrim : %s : begin\n",
  // getTypeId().getName().getString());
  SoTransform* transform = SO_GET_PART(this,"frontTransform",SoTransform);
  if(transform==NULL) return;
  SbBox3f bb = getFrontBBox();
  if(bb.isEmpty()==FALSE) {
    const SbVec3f& bmin = bb.getMin();
    const SbVec3f& bmax = bb.getMax();
    /*
    printf("debug : SoPrimitive::updatePrim : %s : %g %g %g : %g %g %g\n",
           getTypeId().getName().getString(),
           bmin[0],bmin[1],bmin[2],bmax[0],bmax[1],bmax[2]);
    */
    fObjectWidth = bmax[0] - bmin[0];
    fObjectHeight = bmax[1] - bmin[1];
    SoPrimitive::updateBack(TRUE);
    // Recenter front object,
    // Bring back of front things at OFFSET (0.01) :
    transform->translation.setValue(-bmin[0] - fObjectWidth/2,
                                    -bmin[1] - fObjectHeight/2,
                                    -bmin[2] + OFFSET);
  } else {
    fObjectWidth = 1.;
    fObjectHeight = 1.;
    SoPrimitive::updateBack(TRUE);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoPrimitive::compellSize(
 float aWidth
,float aHeight
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoTransform* transform = SO_GET_PART(this,"frontTransform",SoTransform);
  if(transform==NULL) return;
  SbBox3f bb = getFrontBBox();
  if(bb.isEmpty()==FALSE) {
    const SbVec3f& bmin = bb.getMin();
    //const SbVec3f& bmax = bb.getMax();
    /*
    printf("debug : SoPrimitive::updatePrim : %s : %g %g %g : %g %g %g\n",
           getTypeId().getName().getString(),
           bmin[0],bmin[1],bmin[2],bmax[0],bmax[1],bmax[2]);
    */
    fObjectWidth = aWidth;
    fObjectHeight = aHeight;
    SoPrimitive::updateBack(FALSE);
    // Recenter front object,
    // Bring back of front things at OFFSET (0.01) :
    transform->translation.setValue(-bmin[0] - fObjectWidth/2,
                                    -bmin[1] - fObjectHeight/2,
                                    -bmin[2] + OFFSET);
  } else {
    fObjectWidth = aWidth;
    fObjectHeight = aHeight;
    SoPrimitive::updateBack(FALSE);
  }
}
//////////////////////////////////////////////////////////////////////////////
SbBox3f SoPrimitive::getFrontBBox(
)
//////////////////////////////////////////////////////////////////////////////
// Default method to retreive size of front things.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSeparator* separator = (SoSeparator*)frontSeparator.getValue();
  SoTransform* transform = SO_GET_PART(this,"frontTransform",SoTransform);
  // Reset front Transform before applying get bounding box :
  transform->translation.setValue(0.,0.,0.);
  // Retreive front object size :
  // Get bounding box :
  SbViewportRegion vp; 
  SoGetBoundingBoxAction* getBB = new SoGetBoundingBoxAction(vp);
  getBB->apply(separator);
  SbBox3f bb = getBB->getBoundingBox();
  delete getBB;
  return bb;
}
//////////////////////////////////////////////////////////////////////////////
void SoPrimitive::updateBack(
 SbBool aNotify
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoTransform* transform = SO_GET_PART(this,"backTransform",SoTransform);
  if(transform==NULL) return;
  SoBackPrimitive* trd = SO_GET_PART(this,"backTrd",SoBackPrimitive);
  if(trd==NULL) return;

  // Adapt SoBackPrimitive size to fObjectWidth, fObjectHeight : 
  float borderSize = shadowThickness.getValue();
  float margWidth = marginWidth.getValue();
  float frontWidth = fObjectWidth + 2 * margWidth;
  if(frontWidth>0.) {
      trd->dx1.setValue(frontWidth/2 + borderSize);
      trd->dx2.setValue(frontWidth/2);
    }
  float margHeight = marginHeight.getValue();
  float frontHeight = fObjectHeight + 2 * margHeight;
  if(frontHeight>0.) {
    trd->dy1.setValue(frontHeight/2 + borderSize);
    trd->dy2.setValue(frontHeight/2);
  }
  float fDz = (borderSize == 0. ? 0.01F : borderSize/2);
  trd->dz.setValue(fDz);

  // Front plane of background at z = -OFFSET
  transform->translation.setValue(0.,0.,-fDz - OFFSET); 

  float totalWidth = frontWidth + 2 * borderSize;
  float totalHeight = frontHeight + 2 * borderSize;

  SbBool flag;
  flag = width.enableNotify(aNotify);
  width.setValue(totalWidth);
  width.enableNotify(flag);

  flag = height.enableNotify(aNotify);
  height.setValue(totalHeight);
  height.enableNotify(flag);

  //printf("debug : SoPrimitive::updateBack : %s : %g %g\n",
  //         getTypeId().getName().getString(),totalWidth,totalHeight);
}
//////////////////////////////////////////////////////////////////////////////
void SoPrimitive::setBackground(
 const SbColor& aColor
)
//////////////////////////////////////////////////////////////////////////////
// Fastest way to set background
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoMaterial* material = (SoMaterial*)backMaterial.getValue();
  if(material==NULL) return;
  material->diffuseColor.setValue(aColor);
}
//////////////////////////////////////////////////////////////////////////////
void SoPrimitive::setForeground(
 const SbColor& aColor
)
//////////////////////////////////////////////////////////////////////////////
// Fastest way to set foreground
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoMaterial* material = (SoMaterial*)frontMaterial.getValue();
  if(material==NULL) return;
  material->diffuseColor.setValue(aColor);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoPrimitive::sensorCB(
 void* aData
,SoSensor*
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoPrimitive* This = (SoPrimitive*)aData;
  This->updateSceneGraph();
}
