// this :
#include <HEPVis/nodekits/SoPushButton.h>

#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>

#include <HEPVis/SbText.h>

SO_KIT_SOURCE(SoPushButton) 
//////////////////////////////////////////////////////////////////////////////
void SoPushButton::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoPushButton,SoPrimitive,"SoPrimitive");
}
//////////////////////////////////////////////////////////////////////////////
SoPushButton::SoPushButton(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoPushButton);
  SO_KIT_ADD_CATALOG_ENTRY(labelFont,SoFont,FALSE,frontGroup,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(labelText,SoText3,FALSE,frontGroup,\0,TRUE);
  SO_KIT_INIT_INSTANCE();
  //
  SO_NODE_ADD_FIELD(fontList,("Helvetica"));
  SO_NODE_ADD_FIELD(labelString,("PushButton"));
  //
  initialize();
}
//////////////////////////////////////////////////////////////////////////////
SoPushButton::~SoPushButton(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fontList.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  labelString.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  //
  fActivateCallbackList.clearCallbacks();
}
//////////////////////////////////////////////////////////////////////////////
void SoPushButton::initialize(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fontList.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  labelString.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  //
  SoEventCallback* eventCallback = new SoEventCallback;
  eventCallback->
    addEventCallback(SoMouseButtonEvent::getClassTypeId(),eventCB,this);
  setPart("callbackList[0]",eventCallback);
  //
  SoPushButton::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoPushButton::addActivateCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fActivateCallbackList.addCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoPushButton::removeActivateCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fActivateCallbackList.removeCallback((SoCallbackListCB*)aFunction,
                                       aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoPushButton::invokeActivateCallbacks(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fActivateCallbackList.invokeCallbacks(this);
}
//////////////////////////////////////////////////////////////////////////////
void SoPushButton::updateSceneGraph(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoFont* font = (SoFont*)getPart("labelFont",TRUE);
  if(font==NULL) return;
  SoText3* text = SO_GET_PART(this,"labelText",SoText3);
  if(text==NULL) return;
  //
  font->size.setValue(1.0);
  font->name.setValue(fontList.getValue());

  text->string.setValues(0,labelString.getNum(),labelString.getValues(0));

  SoPrimitive::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
SbBox3f SoPushButton::getFrontBBox(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // return SoPrimitive::getFrontBBox();
  // Constant height :
  SoFont* font = (SoFont*)getPart("labelFont",TRUE);
  //SoText3* text = SO_GET_PART(this,"labelText",SoText3);
  float ascent,descent,depth;
  SbText::getAscentDescent
    (font,SoText3::getClassTypeId(),ascent,descent,depth);
  SbBox3f bb = SoPrimitive::getFrontBBox();
  SbVec3f mn(bb.getMin()[0],-descent,bb.getMin()[2]);
  SbVec3f mx(bb.getMax()[0],  ascent,bb.getMax()[2]);
  bb.setBounds(mn,mx);
  return bb;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoPushButton::eventCB (
 void* aData
,SoEventCallback* aEventCallback
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aEventCallback->isHandled()) return;
  const SoMouseButtonEvent* event= 
    (SoMouseButtonEvent*)aEventCallback->getEvent();
  SoPushButton* This = (SoPushButton*)aData;

  if(
     (SoMouseButtonEvent::isButtonPressEvent
      (event,SoMouseButtonEvent::BUTTON1)==FALSE) && 
     (SoMouseButtonEvent::isButtonReleaseEvent
      (event,SoMouseButtonEvent::BUTTON1)==FALSE) )return;
  
  //printf ("debug : Pick something ?\n");

  const SoPickedPoint* pickedPoint = aEventCallback->getPickedPoint();
  //printf ("debug : Pick something ?? %lu\n",pickedPoint);
  if (pickedPoint==NULL) return;
  SoFullPath* path = (SoFullPath*)pickedPoint->getPath();
  //printf ("debug : Pick something ??? %lu\n",path);
  if(path==NULL) return;

  if(path->containsNode((SoNode*)This)==FALSE) return;

  if(SoMouseButtonEvent::isButtonPressEvent
     (event,SoMouseButtonEvent::BUTTON1)==TRUE) {

    //printf("debug : press \n");
    This->setBackground(SbColor(0.,0.,0.));
    This->setForeground(SbColor(1.,1.,1.));

  } else {

    //printf("debug : released \n");
    This->setBackground(This->background.getValue());
    This->setForeground(This->foreground.getValue());

    //printf ("debug : Pick button !!!\n");
    This->invokeActivateCallbacks();

  }

  aEventCallback->setHandled();
}









