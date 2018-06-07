/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoDetectorTreeKit                                       */
/* Description:      Represents a single sided silicon strip detector        */
/* Author:           Joe Boudreau Nov 11 1996                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/

// this :
#include <HEPVis/nodekits/SoDetectorTreeKit.h>

#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoUnits.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodekits/SoSeparatorKit.h>
#include <Inventor/nodekits/SoShapeKit.h>
#include <Inventor/nodekits/SoAppearanceKit.h>
#include <Inventor/nodekits/SoNodeKitListPart.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/draggers/SoDragger.h>

#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoCallbackAction.h>

#include <HEPVis/actions/SoAlternateRepAction.h>

#include <math.h>

// This statement is required
SO_KIT_SOURCE(SoDetectorTreeKit) 
 
// initClass
void SoDetectorTreeKit::initClass(){
  SO_KIT_INIT_CLASS(SoDetectorTreeKit,SoBaseKit,"BaseKit");
}

// Constructor
SoDetectorTreeKit::SoDetectorTreeKit() {
  SO_KIT_CONSTRUCTOR(SoDetectorTreeKit);

  SO_NODE_ADD_FIELD(alternateRep, (NULL));

  SO_KIT_ADD_CATALOG_ENTRY     (     topSeparator,         SoSeparator, FALSE,          this,\0, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY     (        pickStyle,         SoSeparator, TRUE ,  topSeparator,\0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY     (       appearance,     SoAppearanceKit, TRUE,  topSeparator ,\0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY     (            units,             SoUnits, TRUE,  topSeparator ,\0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY     (        transform,         SoTransform, TRUE ,  topSeparator,\0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY     (texture2Transform, SoTexture2Transform, TRUE,  topSeparator ,\0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY     (        childList,            SoSwitch, FALSE,  topSeparator,\0, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY     ( previewSeparator,         SoSeparator, FALSE,     childList,\0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY     (    fullSeparator,         SoSeparator, FALSE,     childList,\0, TRUE);

  SO_KIT_INIT_INSTANCE();

  createInitialTree();
}

// Destructor
SoDetectorTreeKit::~SoDetectorTreeKit() {
  fPickCallbackList.clearCallbacks();
}


SbBool SoDetectorTreeKit::affectsState() const {
  return FALSE;
}

void SoDetectorTreeKit::createInitialTree() {
  SoEventCallback* myCallback = new SoEventCallback();

  myCallback->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
                               SoDetectorTreeKit::expand_cbk,
                               this);
  myCallback->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
                               SoDetectorTreeKit::contract_cbk,
                               this);
  myCallback->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
                               SoDetectorTreeKit::pick_cbk,
                               this);
  if(setPart("callbackList[0]",myCallback)==FALSE) myCallback->unref(); 

  SoSwitch* theChildList = (SoSwitch*)childList.getValue();
  theChildList->whichChild.setValue(0);
}

void SoDetectorTreeKit::pick_cbk(void* aUserData,SoEventCallback* aEventCB) {
  if(aEventCB->isHandled()) return;
  const SoMouseButtonEvent* event = (SoMouseButtonEvent*)aEventCB->getEvent();
  if(!SoMouseButtonEvent::isButtonPressEvent
        (event,SoMouseButtonEvent::BUTTON1)) return;
  if(event->wasCtrlDown()) return;
  if(event->wasShiftDown()) return;

  // Which Detector is this being called for?
  SoDetectorTreeKit* This = (SoDetectorTreeKit*)aUserData;

  // Find out whether that's the one that has been picked.  
  // "detKit' is the lowest detector tree kit in the hierarchy.
  SoHandleEventAction* handleEventAction = aEventCB->getAction();
  const SoPickedPoint* pickedPoint = handleEventAction->getPickedPoint();
  if(!pickedPoint) return;

  SoFullPath* path = (SoFullPath*)pickedPoint->getPath();
  SoNode *ancestorNode=NULL;
  for (int i=0;i<path->getLength();i++) {
    ancestorNode = path->getNodeFromTail(i);
    if(ancestorNode->isOfType(SoDetectorTreeKit::getClassTypeId()))  break;
    //If in a dragger, the dragging shape may be picked !
    if(ancestorNode->isOfType(SoDragger::getClassTypeId()))  return;
  }
  if(This!=ancestorNode) return;
  //  if (!ancestorNode->isOfType(SoDetectorTreeKit::getClassTypeId())) return;
 
  This->fPickCallbackList.invokeCallbacks(This);

  aEventCB->setHandled();
}

void SoDetectorTreeKit::expand_cbk(void *userData, SoEventCallback *eventCB){

  // Was the event previously handled? Is it the right kind?

  if (eventCB->isHandled()) return;
  const SoMouseButtonEvent *event= (SoMouseButtonEvent *) eventCB->getEvent();
  if (!SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1)) return;
  if (!event->wasCtrlDown()) return;
  if (event->wasShiftDown()) return;

  // Which Detector is this being called for?
  SoDetectorTreeKit* This = (SoDetectorTreeKit *) userData;

  // Find out whether that's the one that has been picked.  
  // "This' is the lowest detector tree kit in the hierarchy.
  SoHandleEventAction *handleEventAction = eventCB->getAction();
  const SoPickedPoint *pickedPoint = handleEventAction->getPickedPoint();
  if (!pickedPoint) return;

  SoFullPath* path = (SoFullPath*)pickedPoint->getPath();
  SoNode *ancestorNode=NULL;
  for (int i=0;i<path->getLength();i++) {
    ancestorNode  = path->getNodeFromTail(i);
    if (ancestorNode->isOfType(SoDetectorTreeKit::getClassTypeId()))  break;
  }
  if (This!=ancestorNode) return;
  //  if (!ancestorNode->isOfType(SoDetectorTreeKit::getClassTypeId())) return;
 
  // Deactivate the Preview
  // If the full sep contains nothing, then we do not expand
  // because it will lead to invisible (then not 
  // contractable) children !
  if(This->getFullSeparator()->getNumChildren()>0) {
    This->setPreview(FALSE);
    eventCB->setHandled();
  }    
}

void SoDetectorTreeKit::contract_cbk(void *userData, SoEventCallback *eventCB){

  // Was the event previously handled? Is it the right kind?
  if (eventCB->isHandled()) return;
  const SoMouseButtonEvent *event= (SoMouseButtonEvent *) eventCB->getEvent();
  if (!SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1)) return;
  if (event->wasCtrlDown()) return;
  if (!event->wasShiftDown()) return;

  // Which Detector is this being called for?
  SoDetectorTreeKit* This = (SoDetectorTreeKit *) userData;

  // Find out whether that's the one that has been picked
  SoHandleEventAction *handleEventAction = eventCB->getAction();
  const SoPickedPoint *pickedPoint = handleEventAction->getPickedPoint();
  if (!pickedPoint) return;
 
  // Find out whether that's the one that has been picked.  
  // "This" is the lowest detector tree kit in the hierarchy.
  SoFullPath* path = (SoFullPath*)pickedPoint->getPath();
  SoNode *ancestorNode=NULL;
  SbBool firstTreeFound=FALSE;
  for (int i=0;i<path->getLength();i++) {
    ancestorNode  = path->getNodeFromTail(i);
    if (ancestorNode->isOfType(SoDetectorTreeKit::getClassTypeId())) {
      if (!firstTreeFound) {
        if (This!=ancestorNode) return;
        firstTreeFound=TRUE;
      }
      SoDetectorTreeKit *That = (SoDetectorTreeKit *) ancestorNode;
      if (!That->getPreview()) {
        // If the preview sep contains nothing, then we do not contract.
        // because it will lead to an invisible (then not 
        // expandable) mother !
        if(That->getPreviewSeparator()->getNumChildren()>0) {
          That->setPreview(TRUE);
          eventCB->setHandled();
          return;
        }
      }
    }
  }
}

void SoDetectorTreeKit::setPreview(SbBool Flag) {
  SoSwitch *theChildList = (SoSwitch *) childList.getValue();
  if (Flag) {
    theChildList->whichChild.setValue(0);
  }
  else {
    theChildList->whichChild.setValue(1);
  }
}

SbBool SoDetectorTreeKit::getPreview() const {
  SoSwitch *theChildList = (SoSwitch *) childList.getValue();
  if (theChildList->whichChild.getValue()==0) return TRUE;
  return FALSE;
}


void SoDetectorTreeKit::setPreviewAndFull() {
  SoSwitch *theChildList = (SoSwitch *) childList.getValue();
  theChildList->whichChild.setValue(SO_SWITCH_ALL);
}

SoSeparator *SoDetectorTreeKit::getPreviewSeparator() const {
  return (SoSeparator *) previewSeparator.getValue();
}

SoSeparator *SoDetectorTreeKit::getFullSeparator() const {
  return (SoSeparator *) fullSeparator.getValue();
}


void add_for_vrml(SoGroup* aParent,SoGroup* aGroup,SbBool& aEmpty) {
  int num = aGroup->getNumChildren();
  if( (num==1) &&
      (aGroup->getChild(0)->isOfType(SoGroup::getClassTypeId())==TRUE) ){
    add_for_vrml(aParent,(SoGroup*)aGroup->getChild(0),aEmpty);
    return;
  }
  for(int index=0;index<num;index++) {
    aParent->addChild(aGroup->getChild(index));
    aEmpty = FALSE;
  }
}

// generateAlternateRep
void SoDetectorTreeKit::generateAlternateRep(SoAction* aAction) {
  //some sep may be empty and then it would induce
  //warning when reading.
  //alternateRep.setValue(topSeparator.getValue());

#define ADD(a_field) {if(a_field.getValue()) sep->addChild(a_field.getValue());}

  SoAlternateRepAction* action = (SoAlternateRepAction*)aAction;
  if(action->getForWhat()==SoAlternateRepAction::VRML) {
    SoSeparator* sep = new SoSeparator();
    sep->ref();

    ADD(pickStyle)
    ADD(appearance)
    ADD(units)
    ADD(texture2Transform)
    ADD(transform)

    //VRML : save a bit on file size :
    SbBool empty = TRUE;
    SoSwitch* soSwitch = (SoSwitch*)childList.getValue();
    int wc = soSwitch->whichChild.getValue();
    if(wc==0) {
      SoSeparator* s = (SoSeparator*)previewSeparator.getValue();
      if(s) add_for_vrml(sep,s,empty);
    } else if(wc==1) {
      SoSeparator* s = (SoSeparator*)fullSeparator.getValue();
      if(s) add_for_vrml(sep,s,empty);
    }
    if(empty==TRUE) sep->removeAllChildren();
    alternateRep.setValue(sep);
    sep->unref();
  } else {
    SoSeparator* sep = new SoSeparator();
    ADD(pickStyle)
    ADD(appearance)
    ADD(units)
    ADD(transform)
    ADD(texture2Transform)
    ADD(childList)
    alternateRep.setValue(sep);
  }
}

void SoDetectorTreeKit::clearAlternateRep() {
  alternateRep.setValue(NULL);
}
//////////////////////////////////////////////////////////////////////////////
void SoDetectorTreeKit::doAction(
 SoAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ALTERNATEREP_DO_ACTION(aAction)
  SoBaseKit::doAction(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoDetectorTreeKit::addPickCallback(
 SoCallbackListCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fPickCallbackList.addCallback(aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoDetectorTreeKit::removePickCallback(
 SoCallbackListCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fPickCallbackList.removeCallback(aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoDetectorTreeKit::write(
 SoWriteAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    aAction->traverse(altRep);
  } else {
    // Do not write callbackList.
    // Else at read time, we shall loose the callbacks
    // installed in the constructor.
    SbBool old = callbackList.isDefault();
    callbackList.setDefault(TRUE); //enforce not writing.
    SoBaseKit::write(aAction);
    callbackList.setDefault(old);
  }
}

// The SoToVRML2Action may do a callback, search, getMatrix, getBoundingBox :

#define ACTION(a_Class,a_Super,a_Action,a_Method)\
void a_Class::a_Method(a_Action* aAction){\
  SoNode* altRep = alternateRep.getValue();\
  if(altRep) {\
    aAction->traverse(altRep);\
  } else {\
    a_Super::a_Method(aAction);\
  }\
}

ACTION(SoDetectorTreeKit,SoBaseKit,SoCallbackAction,callback)
ACTION(SoDetectorTreeKit,SoBaseKit,SoGetBoundingBoxAction,getBoundingBox)
ACTION(SoDetectorTreeKit,SoBaseKit,SoGetMatrixAction,getMatrix)
ACTION(SoDetectorTreeKit,SoBaseKit,SoSearchAction,search)
