// this :
#include <HEPVis/nodekits/SoRulerRegion.h>

#include <Inventor/SbBox.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/sensors/SoFieldSensor.h>

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
#include <HEPVis/SbStyles.h>
#include <HEPVis/SbMath.h>
#include <HEPVis/nodes/SoTextStyle.h>
#include <HEPVis/nodekits/SoAxis.h>
#include <HEPVis/nodes/SoViewportRegion.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

SO_KIT_SOURCE(SoRulerRegion) 
//////////////////////////////////////////////////////////////////////////////
void SoRulerRegion::initClass (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoRulerRegion,SoRegion,"SoRegion");
}
//////////////////////////////////////////////////////////////////////////////
SoRulerRegion::SoRulerRegion(
)
:fOldWidth(0)
,fOldHeight(0)
,fMeasuredRegion(0)
,fCameraHeightSensor(0)
,fCameraAspectRatioSensor(0)
,fRegionSizeSensor(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoRulerRegion);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0,FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(camera,SoOrthographicCamera,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(transform,SoTransform,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(axis,SoAxis,FALSE,topSeparator,\0,TRUE);

  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SoRulerRegion::resetCamera();
}
//////////////////////////////////////////////////////////////////////////////
SoRulerRegion::~SoRulerRegion(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  setMeasuredRegion(0);
}
//////////////////////////////////////////////////////////////////////////////
void SoRulerRegion::clear(
 const SbString& //aWhat
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxis* soAxis = (SoAxis*)axis.getValue();
  soAxis->reset();

  horizontalMargin.setValue(2); //PIXELS
  verticalMargin.setValue(2); //PIXELS

  //getViewportRegion()->reset();

  //setMeasuredRegion(0);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoRulerRegion::doAction(
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

void SoRulerRegion::updateChildren(){

  if(fUpdate==FALSE) return;

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoRulerRegion::updateChildren","%lu : update",(unsigned long)this);
  }

  matchViewport(fOldWidth,fOldHeight);

  fUpdate = FALSE;
}

#define ACTION(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  if(visible.getValue()==FALSE) return;\
  aClass::doAction(action);\
  aSuper::aMethod(action);\
}
ACTION(SoRulerRegion,SoRegion,SoCallbackAction,callback)
ACTION(SoRulerRegion,SoRegion,SoGLRenderAction,GLRender)
ACTION(SoRulerRegion,SoRegion,SoGetBoundingBoxAction,getBoundingBox)
//ACTION(SoRulerRegion,SoRegion,SoGetMatrixAction,getMatrix)
ACTION(SoRulerRegion,SoRegion,SoHandleEventAction,handleEvent)
ACTION(SoRulerRegion,SoRegion,SoRayPickAction,rayPick)
ACTION(SoRulerRegion,SoRegion,SoPickAction,pick)

#define ACTION1(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::updateChildren();\
  aSuper::aMethod(action);\
}
ACTION1(SoRulerRegion,SoRegion,SoGetMatrixAction,getMatrix)

#define ACTION2(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  /*if(visible.getValue()==FALSE) return;*/\
  /* There is no SoViewportRegionElement on search and write action.*/\
  /*aClass::updateChildren();*/\
  aSuper::aMethod(action);\
}
ACTION2(SoRulerRegion,SoRegion,SoSearchAction,search)
//ACTION2(SoRulerRegion,SoRegion,SoWriteAction,write)
void SoRulerRegion::write(SoWriteAction* aAction) {
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
SoSeparator* SoRulerRegion::getTopSeparator(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoSeparator*)topSeparator.getValue();
}
//////////////////////////////////////////////////////////////////////////////
SoCamera* SoRulerRegion::getCamera(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoCamera*)camera.getValue();
}
//////////////////////////////////////////////////////////////////////////////
SoAxis* SoRulerRegion::getAxis(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoAxis*)axis.getValue();
}
//////////////////////////////////////////////////////////////////////////////
void SoRulerRegion::resetCamera(
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
    soOrthoCamera->height.setValue(2);
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoRulerRegion::matchViewport(
 short aWidth
,short aHeight
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if((aWidth<=0)||(aHeight<=0)) return;

  float w = float(aWidth);
  float h = float(aHeight);

  SoCamera* soCamera = (SoCamera*)camera.getValue();
  soCamera->aspectRatio.setValue(w/h);
  soCamera->position.setValue(0,0,4);
  if(soCamera->isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
    SoOrthographicCamera* soOrthoCamera = (SoOrthographicCamera*)soCamera;
    soOrthoCamera->height.setValue(h);
  }

  SoAxis* soAxis = (SoAxis*)axis.getValue();
  soAxis->modeling.setValue(SbTickModeling_hplot);

  if(w>=h) { //Horizontal ruler.

    setHorizontalLayout(w,h);

    float tl  = h*0.5F; //tickLength
    float t2l = h*0.1F; //tick to label
    float lh  = h*0.3F; //labelHeight
    soAxis->tickLength.setValue(tl);
    soAxis->labelToAxis.setValue(tl+t2l); //Text is top justified.
    soAxis->labelHeight.setValue(lh);

  } else { //Vertical ruler.

    setVerticalLayout(w,h); 

    float tl  = w*0.5F; //tickLength
    float t2l = w*0.1F; //tick to label
    float lh  = w*0.2F; //labelHeight
    soAxis->tickLength.setValue(tl);
    soAxis->labelToAxis.setValue(tl+t2l); //Text is right justified.
    soAxis->labelHeight.setValue(lh);
  }

  //printf("debug : SoRulerRegion::matchViewport : %lu : mregion %lu\n",
  //  this,fMeasuredRegion);

  if(fMeasuredRegion) {
    SoCamera* mcamera = fMeasuredRegion->getCamera();
    if(mcamera && 
       mcamera->viewportMapping.getValue()==SoCamera::ADJUST_CAMERA) {
      if(mcamera->isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
        float mch = ((SoOrthographicCamera*)mcamera)->height.getValue();
        SoViewportRegion* mvpr = fMeasuredRegion->getViewportRegion();
        unsigned int mw;
        unsigned int mh;
        mvpr->getSizePixels(mw,mh);
        //printf("debug : SoRulerRegion::matchViewport : %lu : msize %d %d\n",
	//       this,mw,mh);
        if(mw && mh) {
          float amx = 1;
          if(mw>=mh) {
            float factor = mch/float(mh);
            if(w>=h) { //Horizontal ruler.
              // mch maps mh pixels for the measured region then
              // aWidth pixels maps to : aWidth * mch / mh
              amx = float(aWidth) * factor;
            } else { //Vertical ruler.
              // mch maps mh pixels for the measured region then
              // aHeight pixels maps to : aHeight * mch / mh
              amx = float(aHeight) * factor;
            }
          } else {
            float mcw = mch * mcamera->aspectRatio.getValue();
            float factor = mcw/float(mw);
            if(w>=h) { //Horizontal ruler.
              // mcw maps mw pixels for the measured region then
              // aWidth pixels maps to : aWidth * mcw / mw
              amx = float(aWidth) * factor;
            } else { //Vertical ruler.
              // mcw maps mw pixels for the measured region then
              // aHeight pixels maps to : aHeight * mcw / mw
              amx = float(aHeight) * factor;
            }
          }
          //printf("debug : SoRulerRegion::matchViewport : %lu : amx %g\n",
          //  this,amx);
          soAxis->minimumValue.setValue(0);
          soAxis->maximumValue.setValue(amx);
	} else {
          fOldWidth = 0; //To compell passing again in matchViewport.
        }
      } else {
        SoDebugError::postInfo("SoRulerRegion::matchViewport",
          "SoPerspectiveCamera not yet handled.");
        //FIXME
        soAxis->minimumValue.setValue(0);
        soAxis->maximumValue.setValue(1);
      }
    } else {
      SoDebugError::postInfo("SoRulerRegion::matchViewport",
         "today, handle only camera with an ADJUST_CAMERA viewport mapping.");
      //FIXME
      soAxis->minimumValue.setValue(0);
      soAxis->maximumValue.setValue(1);
    }
  }

  soAxis->updateChildren(); 
}
//////////////////////////////////////////////////////////////////////////////
void SoRulerRegion::setHorizontalLayout(
 float aWidth
,float aHeight
) 
//////////////////////////////////////////////////////////////////////////////
// From SoPlotter::updateXAxis2D(
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxis* xAxisNode = (SoAxis*)axis.getValue();

  xAxisNode->tickUp.setValue(FALSE);
  xAxisNode->width.setValue(aWidth);

 {SoTextStyle* style = xAxisNode->getLabelsStyle(); 
  style->orientationX.setValue(SbVec3f(1,0,0));
  style->orientationY.setValue(SbVec3f(0,1,0));
  style->horizontalJustification.setValue(SoTextStyle::CENTER);
  style->verticalJustification.setValue(SoTextStyle::TOP);}

 {SoTextStyle* style = xAxisNode->getTitleStyle(); 
  style->orientationX.setValue(SbVec3f(1,0,0));
  style->orientationY.setValue(SbVec3f(0,1,0));
  if(xAxisNode->titleToAxisJustification.getValue()==SoAxis::LEFT) {
    style->horizontalJustification.setValue(SoTextStyle::LEFT);
  } else if(xAxisNode->titleToAxisJustification.getValue()==SoAxis::CENTER) {
    style->horizontalJustification.setValue(SoTextStyle::CENTER);
  } else if(xAxisNode->titleToAxisJustification.getValue()==SoAxis::RIGHT) {
    style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  }  
  style->verticalJustification.setValue(SoTextStyle::TOP);}

 {SoTextStyle* style = xAxisNode->getMagStyle(); 
  style->horizontalJustification.setValue(SoTextStyle::LEFT);
  style->verticalJustification.setValue(SoTextStyle::MIDDLE);}

 {SoTransform* transf = (SoTransform*)transform.getValue();
  SbMatrix matrix;
  matrix.makeIdentity();
  transf->setMatrix(matrix);
  // Attach the axis to upper left corner. (Not in SoPlotter method).
  SbMatrix trans;
  trans.setTranslate(SbVec3f(-aWidth*0.5F,aHeight*0.5F,0));
  transf->multLeft(trans);}
}
//////////////////////////////////////////////////////////////////////////////
void SoRulerRegion::setVerticalLayout(
 float aWidth
,float aHeight
) 
//////////////////////////////////////////////////////////////////////////////
// From SoPlotter::updateYAxis2D
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxis* yAxisNode = (SoAxis*)axis.getValue();

  yAxisNode->tickUp.setValue(FALSE);
  yAxisNode->width.setValue(aHeight);

 {SoTextStyle* style = yAxisNode->getLabelsStyle(); 
  style->orientationX.setValue(SbVec3f(0,1,0));
  style->orientationY.setValue(SbVec3f(1,0,0));
  style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  style->verticalJustification.setValue(SoTextStyle::MIDDLE);}

 {SoTextStyle* style = yAxisNode->getTitleStyle(); 
  style->rotated.setValue(TRUE);
  style->orientationX.setValue(SbVec3f(1, 0,0));
  style->orientationY.setValue(SbVec3f(0,-1,0));
  if(yAxisNode->titleToAxisJustification.getValue()==SoAxis::LEFT) {
    style->horizontalJustification.setValue(SoTextStyle::LEFT);
  } else if(yAxisNode->titleToAxisJustification.getValue()==SoAxis::CENTER) {
    style->horizontalJustification.setValue(SoTextStyle::CENTER);
  } else if(yAxisNode->titleToAxisJustification.getValue()==SoAxis::RIGHT) {
    style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  }  
  style->verticalJustification.setValue(SoTextStyle::BOTTOM);}

 {SoTextStyle* style = yAxisNode->getMagStyle(); 
  style->horizontalJustification.setValue(SoTextStyle::CENTER);
  style->verticalJustification.setValue(SoTextStyle::BOTTOM);}

 {SoTransform* transf = (SoTransform*)transform.getValue();
  SbMatrix matrix;
  matrix.makeIdentity();
  transf->setMatrix(matrix);
  // Attach the axis to lower right corner. (Not in SoPlotter method).
  SbMatrix trans;
  trans.setTranslate(SbVec3f(aWidth*0.5F,-aHeight*0.5F,0));
  transf->multLeft(trans);
  SbMatrix roty;
  roty.setRotate(SbRotation(SbVec3f(0,1,0),FM_PI));
  transf->multLeft(roty);
  SbMatrix rotz;
  rotz.setRotate(SbRotation(SbVec3f(0,0,1),FM_PI_2));
  transf->multLeft(rotz);} //Applied first
}
//////////////////////////////////////////////////////////////////////////////
void SoRulerRegion::setMeasuredRegion(
 SoRegion* aRegion
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  //printf("debug : SoRulerRegion::setMeasuredRegion : %lu mregion\n",
  //  this,aRegion);
  if(fMeasuredRegion) {
    if(fCameraHeightSensor) {
      fCameraHeightSensor->detach();
      delete fCameraHeightSensor;
      fCameraHeightSensor = 0;
    }
    if(fCameraAspectRatioSensor) {
      fCameraAspectRatioSensor->detach();
      delete fCameraAspectRatioSensor;
      fCameraAspectRatioSensor = 0;
    }
    if(fRegionSizeSensor) {
      fRegionSizeSensor->detach();
      delete fRegionSizeSensor;
      fRegionSizeSensor = 0;
    }
    fMeasuredRegion->unref();  
    fMeasuredRegion = 0;
  }
  fMeasuredRegion = aRegion; // A 0 disconnect the ruler to any region.
  if(fMeasuredRegion) {
    fMeasuredRegion->ref();  

    SoCamera* mcamera = fMeasuredRegion->getCamera();
    if(mcamera &&
       mcamera->isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
      fCameraHeightSensor = 
        new SoFieldSensor((SoSensorCB*)sensorCallback,this);
      fCameraHeightSensor->attach
        (&(((SoOrthographicCamera*)mcamera)->height));    

      fCameraAspectRatioSensor = 
        new SoFieldSensor((SoSensorCB*)sensorCallback,this);
      fCameraAspectRatioSensor->attach
        (&(((SoOrthographicCamera*)mcamera)->aspectRatio));    
    }

    SoViewportRegion* mvpr = fMeasuredRegion->getViewportRegion();
    fRegionSizeSensor = new SoFieldSensor((SoSensorCB*)sensorCallback,this);
    fRegionSizeSensor->attach(&(mvpr->size));    
    //fRegionSizeSensor->attach(&(mvpr->resized));    
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoRulerRegion::sensorCallback(
 SoRulerRegion* This
,SoSensor*
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  //printf("debug : SoRulerRegion::sceneSensorCallback : %lu\n",This);
  This->fOldWidth = 0; //To compell passing in matchViewport.
}
//////////////////////////////////////////////////////////////////////////////
void SoRulerRegion::generateAlternateRep(SoAction*) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(getAlternateRep());
}
//////////////////////////////////////////////////////////////////////////////
void SoRulerRegion::clearAlternateRep(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(NULL);
}
