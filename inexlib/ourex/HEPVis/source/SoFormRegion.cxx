// this :
#include <HEPVis/nodekits/SoFormRegion.h>

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoTranslation.h>
//#include <Inventor/nodes/SoScale.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/elements/SoViewportRegionElement.h>

// HEPVis :
#include <HEPVis/SbString.h>
#include <HEPVis/SbMath.h>
#include <HEPVis/nodes/SoViewportRegion.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

#define CHILD_TRANS 0
//#define CHILD_SCALE 1
#define CHILD_SHAPE 1

//#define YOFFSET 2 //FIXME : have an SoField.

SO_KIT_SOURCE(SoFormRegion) 
//////////////////////////////////////////////////////////////////////////////
void SoFormRegion::initClass (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoFormRegion,SoRegion,"SoRegion");
}
//////////////////////////////////////////////////////////////////////////////
SoFormRegion::SoFormRegion(
)
:fOldWidth(0)
,fOldHeight(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoFormRegion);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0,FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(light,SoDirectionalLight,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lightModel,SoLightModel,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(camera,SoOrthographicCamera,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(boxes,SoSeparator,FALSE,topSeparator,\0,TRUE);

  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(columns,(1));
  SO_NODE_ADD_FIELD(rows,(1));

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  //FIXME : Empty shapes ?

  SoDirectionalLight* soLight = (SoDirectionalLight*)light.getValue();
  soLight->color.setValue(1,1,1);
  // Same as Coin<Gui>/So<Gui>Viewer :
  soLight->direction.setValue(SbVec3f(1,-1,-10));

  SoLightModel* soLightModel = (SoLightModel*)lightModel.getValue();
  soLightModel->model.setValue(SoLightModel::PHONG);

  SoFormRegion::resetCamera();
}
//////////////////////////////////////////////////////////////////////////////
SoFormRegion::~SoFormRegion(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoFormRegion::doAction(
 SoAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ALTERNATEREP_DO_ACTION(aAction)

  //WARNING : SoViewportRegion can't be within a SoSeparator in
  //          the nodekit, else state will pop at end of SoBaseKit::doAction
  //          and we will have a bad SbViewportRegion in the below.
  // Execute SoBaseKit in order to execute SoViewportRegion doAction.
  //FIXME : how to traverse SoRegion children only ?
  //SoRegion::doAction(aAction);
  SoViewportRegion* soViewportRegion = SoRegion::getViewportRegion();
  soViewportRegion->doAction(aAction);

  const SbViewportRegion& vp = SoViewportRegionElement::get(aAction->getState());
  short wvp = vp.getViewportSizePixels()[0];
  short hvp = vp.getViewportSizePixels()[1];

  if((wvp!=fOldWidth)||(hvp!=fOldHeight)) {
    fOldWidth = wvp;
    fOldHeight = hvp;
    fUpdate = TRUE;
  }

  updateChildren();
}

#define ACTION(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  if(visible.getValue()==FALSE) return;\
  aClass::doAction(action);\
  aSuper::aMethod(action);\
}
ACTION(SoFormRegion,SoRegion,SoCallbackAction,callback)
ACTION(SoFormRegion,SoRegion,SoGLRenderAction,GLRender)
ACTION(SoFormRegion,SoRegion,SoGetBoundingBoxAction,getBoundingBox)
//ACTION(SoFormRegion,SoRegion,SoGetMatrixAction,getMatrix)
ACTION(SoFormRegion,SoRegion,SoHandleEventAction,handleEvent)
ACTION(SoFormRegion,SoRegion,SoRayPickAction,rayPick)
ACTION(SoFormRegion,SoRegion,SoPickAction,pick)

#define ACTION1(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::updateChildren();\
  aSuper::aMethod(action);\
}
ACTION1(SoFormRegion,SoRegion,SoGetMatrixAction,getMatrix)

#define ACTION2(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  /*if(visible.getValue()==FALSE) return;*/\
  /* There is no SoViewportRegionElement on search and write action.*/\
  /*aClass::updateChildren();*/\
  aSuper::aMethod(action);\
}
ACTION2(SoFormRegion,SoRegion,SoSearchAction,search)
//ACTION2(SoFormRegion,SoRegion,SoWriteAction,write)
void SoFormRegion::write(SoWriteAction* aAction) {
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    altRep->write(aAction);
  } else {
    SoBaseKit::write(aAction);
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SoSeparator* SoFormRegion::getTopSeparator(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoSeparator*)topSeparator.getValue();
}
//////////////////////////////////////////////////////////////////////////////
SoCamera* SoFormRegion::getCamera(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoCamera*)camera.getValue();
}
//////////////////////////////////////////////////////////////////////////////
void SoFormRegion::resetCamera(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoCamera* soCamera = (SoCamera*)camera.getValue();
  //WARNING : ADJUST_CAMERA does not work properly
  soCamera->viewportMapping.setValue(SoCamera::LEAVE_ALONE);
  soCamera->aspectRatio.setValue(1);
  soCamera->position.setValue(0,0,4);
  soCamera->orientation.setValue(SbRotation(SbVec3f(0,1,0),0));
  soCamera->nearDistance.setValue(1);
  soCamera->farDistance.setValue(100);
  soCamera->focalDistance.setValue(4);
  if(soCamera->isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
    SoOrthographicCamera* soOrthoCamera = (SoOrthographicCamera*)soCamera;
    soOrthoCamera->height.setValue(200);
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoFormRegion::updateChildren(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fUpdate==FALSE) return;

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoFormRegion::updateChildren","%lu : update",(unsigned long)this);
  }

  SoSeparator* soBoxes = (SoSeparator*)boxes.getValue();
  //SbBool flag = soLines->enableNotify(FALSE);
  soBoxes->removeAllChildren();
  int number = shapes.getNum();
  for(int index=0;index<number;index++) {
    SoSeparator* box = new SoSeparator; 
    soBoxes->addChild(box);

    //CHILD_TRANS
    box->addChild(new SoTranslation);

    //CHILD_SCALE
    //box->addChild(new SoScale);

    //CHILD_SHAPE
    box->addChild(shapes[index]);
  }
  //soLines->enableNotify(flag);


  float wvp = float(fOldWidth);
  float hvp =  float(fOldHeight);

  matchViewport(wvp,hvp);

  fUpdate = FALSE;
}
//////////////////////////////////////////////////////////////////////////////
void SoFormRegion::matchViewport(
 float aWidth
,float aHeight
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if((aWidth<=0)||(aHeight<=0)) return;

  short marginx = horizontalMargin.getValue();
  short marginy = verticalMargin.getValue();
  if(2*marginy>=aHeight) return; //Empty area.
  if(2*marginx>=aWidth) return; //Empty area.

  SoCamera* soCamera = (SoCamera*)camera.getValue();
  //SbBool flag = soCamera->enableNotify(FALSE);
  soCamera->aspectRatio.setValue(aWidth/aHeight);
  soCamera->position.setValue(0,0,4);
  if(soCamera->isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
    SoOrthographicCamera* soOrthoCamera = (SoOrthographicCamera*)soCamera;
    soOrthoCamera->height.setValue(aHeight);
  }
  //soCamera->enableNotify(flag);

  SoSeparator* soBoxes = (SoSeparator*)boxes.getValue();
  int number = soBoxes->getNumChildren();
  //flag = soLines->enableNotify(FALSE);

  int coln = columns.getValue();
  if(coln<=0) return;
  int rown = rows.getValue();
  if(rown<=0) return;

  float width = (aWidth - 2 * marginx)/coln;
  float height = (aHeight - 2 * marginy)/rown;

  // Reposition texts :
  float ulx = -aWidth*0.5F+marginx;
  float uly =  aHeight*0.5F-marginy;
  int boxi = 0;
  for(int row=0;row<rown;row++) {
    for(int col=0;col<coln;col++) {
      if(boxi>=number) return;
      SoSeparator* box = (SoSeparator*)soBoxes->getChild(boxi);

      SoTranslation* trans = (SoTranslation*)box->getChild(CHILD_TRANS);
      float tx = ulx + col * width + width * 0.5F;
      float ty = uly - row * height - height * 0.5F;
      trans->translation.setValue(SbVec3f(tx,ty,0));

      /*
      SoScale* scale = (SoScale*)box->getChild(CHILD_SCALE);
      float sx = width/2.0F;
      float sy = height/2.0F;
      float sz = 1;
      scale->scaleFactor.setValue(SbVec3f(sx,sy,sz));
      */

      SoNode* node = box->getChild(CHILD_SHAPE);
     {SoField* soField = node->getField("width");
      if(soField && 
         (soField->isOfType(SoSFFloat::getClassTypeId())==TRUE) ) {
        ((SoSFFloat*)soField)->setValue(width);
      } else {
        SoDebugError::postInfo("SoFormRegion::matchViewport",
          "for child %d, no SoSFFloat width field found.",boxi);
      }}

     {SoField* soField = node->getField("height");
      if(soField && 
         (soField->isOfType(SoSFFloat::getClassTypeId())==TRUE) ) {
        ((SoSFFloat*)soField)->setValue(height);
      } else {
        SoDebugError::postInfo("SoFormRegion::matchViewport",
          "for child %d, no SoSFFloat height field found.",boxi);
      }}

      boxi++;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoFormRegion::generateAlternateRep(SoAction*) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(getAlternateRep());
}
//////////////////////////////////////////////////////////////////////////////
void SoFormRegion::clearAlternateRep(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(NULL);
}
