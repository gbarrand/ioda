// this :
#include <HEPVis/nodekits/SoArrowButton.h>

#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoEventCallback.h>
//#include <Inventor/engines/SoCalculator.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/sensors/SoFieldSensor.h>

#include <HEPVis/SbMath.h>

SO_KIT_SOURCE(SoArrowButton) 
//////////////////////////////////////////////////////////////////////////////
void SoArrowButton::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoArrowButton,SoPrimitive,"SoPrimitive");
}
//////////////////////////////////////////////////////////////////////////////
SoArrowButton::SoArrowButton(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoArrowButton);
  SO_KIT_ADD_CATALOG_ENTRY(arrowTransform,SoTransform,
                           FALSE,frontGroup,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(arrowCone,SoCone,FALSE,frontGroup,\0,TRUE);
  SO_KIT_INIT_INSTANCE();
  
  SO_NODE_ADD_FIELD(arrowDirection,(ARROW_UP));
  
  SO_NODE_DEFINE_ENUM_VALUE(ArrowDirection,ARROW_UP);
  SO_NODE_DEFINE_ENUM_VALUE(ArrowDirection,ARROW_DOWN);
  SO_NODE_DEFINE_ENUM_VALUE(ArrowDirection,ARROW_LEFT);
  SO_NODE_DEFINE_ENUM_VALUE(ArrowDirection,ARROW_RIGHT);
  
  SO_NODE_SET_SF_ENUM_TYPE(arrowDirection,ArrowDirection);
  
  initialize();
}
//////////////////////////////////////////////////////////////////////////////
SoArrowButton::~SoArrowButton(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fActivateCallbackList.clearCallbacks();
  if(fSensor!=NULL) {
    fSensor->detach();
    delete fSensor;
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoArrowButton::initialize(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoTransform* transform = (SoTransform*)getPart("arrowTransform",TRUE);
  if(transform!=NULL) {
    transform->rotation.setValue(SbRotation(SbVec3f(0.,0.,1.),0.));
    transform->scaleFactor.setValue(SbVec3f(0.5,0.5,0.5));
    fSensor = new SoFieldSensor(SoArrowButton::sensorCB,this);
    if(fSensor!=NULL) fSensor->attach(&arrowDirection);
  }
  //
  foreground.setValue(SbVec3f(0.3F,0.3F,0.3F));
  //
  SoEventCallback* eventCallback = new SoEventCallback;
  eventCallback->
    addEventCallback(SoMouseButtonEvent::getClassTypeId(),eventCB,this);
  setPart("callbackList[0]",eventCallback);
  //
  SoArrowButton::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoArrowButton::addActivateCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fActivateCallbackList.addCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoArrowButton::removeActivateCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fActivateCallbackList.removeCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoArrowButton::invokeActivateCallbacks(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fActivateCallbackList.invokeCallbacks(this);
}
//////////////////////////////////////////////////////////////////////////////
void SoArrowButton::updateSceneGraph(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoPrimitive::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoArrowButton::eventCB (
 void* This
,SoEventCallback* aEventCallback
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aEventCallback->isHandled()) return;
  const SoMouseButtonEvent* event= 
    (SoMouseButtonEvent *) aEventCallback->getEvent();

  if(SoMouseButtonEvent::isButtonPressEvent
     (event,SoMouseButtonEvent::BUTTON1)==FALSE) return;
  
  //printf ("debug : Pick something ?\n");

  const SoPickedPoint* pickedPoint = aEventCallback->getPickedPoint();
  if (pickedPoint==NULL) return;

  SoFullPath* path = (SoFullPath*)pickedPoint->getPath();
  if(path==NULL) return;

  if(path->containsNode((SoNode*)This)==FALSE) return;

  //printf ("debug : Pick button !!!\n");
  ((SoArrowButton*)This)->invokeActivateCallbacks();

  aEventCallback->setHandled();
}
//////////////////////////////////////////////////////////////////////////////
void SoArrowButton::sensorCB (
 void* aData
,SoSensor* //aSensor
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoArrowButton* This = (SoArrowButton*)aData;
  SoTransform* transform = 
    (SoTransform*)This->getPart("arrowTransform",TRUE);
  if(transform==NULL) return;
  int direction = This->arrowDirection.getValue();
  transform->
    rotation.setValue(SbRotation(SbVec3f(0,0,1),
                                 direction*FM_PI_2));
}
