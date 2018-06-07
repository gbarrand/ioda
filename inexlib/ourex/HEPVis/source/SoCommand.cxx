// this :
#include <HEPVis/nodekits/SoCommand.h>

#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/nodes/SoEventCallback.h>

#include <math.h>

SO_NODE_SOURCE(SoCommand) 
//////////////////////////////////////////////////////////////////////////////
void SoCommand::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_INIT_CLASS(SoCommand,SoText,"SoText");
}
//////////////////////////////////////////////////////////////////////////////
SoCommand::SoCommand(
)
:fHistoryPosition(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(SoCommand);
  //
  SO_NODE_ADD_FIELD(historyItems,(""));
  //
  editable.setValue(TRUE);
  // Below do not wotk with SoFree !
  //SoEventCallback* eventCallback = (SoEventCallback*)getPart("callbackList[0]",TRUE);
  if(fEventCallback!=NULL) {
    fEventCallback->addEventCallback(SoKeyboardEvent::getClassTypeId(),
                                     eventCB,this);
  }
  addActivateCallback(activateCB,NULL);
}
//////////////////////////////////////////////////////////////////////////////
SoCommand::~SoCommand(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoCommand::eventCB(
 void* aNode
,SoEventCallback* aEventCallback
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoCommand* This = (SoCommand*)aNode;
  if(This==NULL) return;

  if (aEventCallback->isHandled()) return;
  const SoKeyboardEvent* event= (SoKeyboardEvent*)aEventCallback->getEvent();
  if (SoKeyboardEvent::isKeyPressEvent(event,event->getKey())==FALSE) return;

  //SbBool shift = event->wasShiftDown();
  //SbBool ctrl = event->wasCtrlDown();
  //SbBool alt = event->wasAltDown();

  SoKeyboardEvent::Key key = event->getKey();

  //printf("debug : SoCommand : %x : %d %d %d\n",key,shift,ctrl,alt);

  switch(key) {
  case SoKeyboardEvent::UP_ARROW:{
    int num = This->historyItems.getNum();
    if(num<=0) break;
    const SbString& s = This->historyItems[This->fHistoryPosition];
    This->cursorPosition.setValue(s.getLength());
    This->value.set1Value(0,s);
    This->fHistoryPosition--;
    if(This->fHistoryPosition<=0) This->fHistoryPosition = 0;
    aEventCallback->setHandled();
    }break;
  case SoKeyboardEvent::DOWN_ARROW:{
    int num = This->historyItems.getNum();
    if(num<=0) break;
    This->fHistoryPosition++;
    if(This->fHistoryPosition>=num) {
      This->fHistoryPosition = num-1;
      This->cursorPosition.setValue(0);
      This->value.set1Value(0,"");
    } else {
      const SbString& s = This->historyItems[This->fHistoryPosition];
      This->cursorPosition.setValue(s.getLength());
      This->value.set1Value(0,s);
    }
    aEventCallback->setHandled();
    }break;
  default:
    break;
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoCommand::activateCB(
 void* /*aUserData*/
,SoNode* aNode
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoCommand* This = (SoCommand*)aNode;
  if(This==NULL) return;
  const SbString& s = This->value[0];
  if(s=="") return;
  int num = This->historyItems.getNum();
  if((num==1)&&(This->historyItems[0]=="")) { // First time :
    This->historyItems.set1Value(0,s);
  } else {
    This->historyItems.set1Value(num,s);
  }
  This->fHistoryPosition = This->historyItems.getNum()-1;
}






