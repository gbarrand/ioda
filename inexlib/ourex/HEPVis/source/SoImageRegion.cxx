// this :
#include <HEPVis/nodekits/SoImageRegion.h>

#include <Inventor/SbBox.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoTranslation.h>
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
#include <HEPVis/SbMath.h>
#include <HEPVis/nodes/SoImage.h>
#include <HEPVis/nodes/SoViewportRegion.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

SO_KIT_SOURCE(SoImageRegion) 
//////////////////////////////////////////////////////////////////////////////
void SoImageRegion::initClass (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoImageRegion,SoRegion,"SoRegion");
}
//////////////////////////////////////////////////////////////////////////////
SoImageRegion::SoImageRegion(
)
:fOldWidth(0)
,fOldHeight(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoImageRegion);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0,FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(camera,SoOrthographicCamera,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(translation,SoTranslation,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(image,HEPVis_SoImage,FALSE,topSeparator,\0,TRUE);
  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SoOrthographicCamera* soCamera = (SoOrthographicCamera*)camera.getValue();
  //WARNING : ADJUST_CAMERA does not work properly
  soCamera->viewportMapping.setValue(SoCamera::LEAVE_ALONE);
  soCamera->aspectRatio.setValue(1);
  soCamera->position.setValue(0,0,4);
  soCamera->orientation.setValue(SbRotation(SbVec3f(0,1,0),0));
  soCamera->height.setValue(200);
  soCamera->nearDistance.setValue(1);
  soCamera->farDistance.setValue(100);
  soCamera->focalDistance.setValue(4);
}
//////////////////////////////////////////////////////////////////////////////
SoImageRegion::~SoImageRegion(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
HEPVis_SoImage* SoImageRegion::getImage(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (HEPVis_SoImage*)image.getValue();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoImageRegion::doAction(
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
  //SoRegion::doAction(aAction); // It would traverse also SoImage.
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

void SoImageRegion::updateChildren(){

  HEPVis_SoImage* soImage = (HEPVis_SoImage*)image.getValue();
  if(soImage->fileName.getValue()!=fOldFileName) {
    fOldFileName = soImage->fileName.getValue();
    soImage->readImage();
    fUpdate = TRUE;
  }

  if(fUpdate==FALSE) return;

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoImageRegion::updateChildren","%lu : update",(unsigned long)this);
  }

  unsigned int wim,him;
  soImage->getSize(wim,him);
  if((wim>0)&&(him>0)) {
    short marginx = horizontalMargin.getValue();
    short marginy = verticalMargin.getValue();
    SoViewportRegion* region = (SoViewportRegion*)viewportRegion.getValue();
    SbVec2s border = region->getBorderSize();
    //SbBool flag = region->enableNotify(FALSE);
    int rw = wim + 2 * marginx + 2 * border[0];
    int rh = him + 2 * marginy + 2 * border[1];
    region->setSizePixels(rw,rh);
    //region->enableNotify(flag);

    SoCamera* soCamera = (SoCamera*)camera.getValue();
    //flag = soCamera->enableNotify(FALSE);
    soCamera->aspectRatio.setValue(float(rw)/float(rh));
    soCamera->position.setValue(0,0,4);
    if(soCamera->isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
      SoOrthographicCamera* soOrthoCamera = (SoOrthographicCamera*)soCamera;
      soOrthoCamera->height.setValue(float(rh));
    }

    //soCamera->enableNotify(flag);

    float tx = 0.5F * float(wim);
    float ty = 0.5F * float(him);

    SoTranslation* trans = (SoTranslation*)translation.getValue();
    //flag = trans->enableNotify(FALSE);
    trans->translation.setValue(SbVec3f(-tx,-ty,0));
    //trans->enableNotify(flag);
  }

  fUpdate = FALSE;
}

#define ACTION(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  if(visible.getValue()==FALSE) return;\
  aClass::doAction(action);\
  aSuper::aMethod(action);\
}
ACTION(SoImageRegion,SoRegion,SoCallbackAction,callback)
ACTION(SoImageRegion,SoRegion,SoGLRenderAction,GLRender)
ACTION(SoImageRegion,SoRegion,SoGetBoundingBoxAction,getBoundingBox)
//ACTION(SoImageRegion,SoRegion,SoGetMatrixAction,getMatrix)
ACTION(SoImageRegion,SoRegion,SoHandleEventAction,handleEvent)
ACTION(SoImageRegion,SoRegion,SoRayPickAction,rayPick)
ACTION(SoImageRegion,SoRegion,SoPickAction,pick)

#define ACTION1(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::updateChildren();\
  aSuper::aMethod(action);\
}
ACTION1(SoImageRegion,SoRegion,SoGetMatrixAction,getMatrix)

#define ACTION2(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  /*if(visible.getValue()==FALSE) return;*/\
  /* There is no SoViewportRegionElement on search and write action.*/\
  /*aClass::updateChildren();*/\
  aSuper::aMethod(action);\
}
ACTION2(SoImageRegion,SoRegion,SoSearchAction,search)
//ACTION2(SoImageRegion,SoRegion,SoWriteAction,write)
void SoImageRegion::write(SoWriteAction* aAction) {
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
SoSeparator* SoImageRegion::getTopSeparator(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoSeparator*)topSeparator.getValue();
}
//////////////////////////////////////////////////////////////////////////////
SoCamera* SoImageRegion::getCamera(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoCamera*)camera.getValue();
}
//////////////////////////////////////////////////////////////////////////////
void SoImageRegion::generateAlternateRep(SoAction*) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(getAlternateRep());
}
//////////////////////////////////////////////////////////////////////////////
void SoImageRegion::clearAlternateRep(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(NULL);
}
