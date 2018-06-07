// this :
#include <HEPVis/actions/SoCounterAction.h>

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/elements/SoSwitchElement.h>

SO_ACTION_SOURCE(SoCounterAction)

//////////////////////////////////////////////////////////////////////////////
void SoCounterAction::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ACTION_INIT_CLASS(SoCounterAction,SoAction);

  SO_ENABLE(SoCounterAction,SoSwitchElement); //SGI wants that.

  SO_ACTION_ADD_METHOD(SoNode,nodeAction);
}
//////////////////////////////////////////////////////////////////////////////
SoCounterAction::SoCounterAction(
)
:fCount(0),fLookFor(NODE),fCheckDerived(TRUE)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ACTION_CONSTRUCTOR(SoCounterAction);
}
//////////////////////////////////////////////////////////////////////////////
SoCounterAction::~SoCounterAction(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
void SoCounterAction::beginTraversal(
 SoNode* aNode
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fCount = 0;
  SoAction::beginTraversal(aNode);
}
//////////////////////////////////////////////////////////////////////////////
void SoCounterAction::nodeAction(
 SoAction* aThis
,SoNode* aNode
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  //printf("debug : begin : %s %s\n",aNode->getName().getString(),
    //aNode->getTypeId().getName().getString());
  SoCounterAction* This = (SoCounterAction*)aThis;
  if(This->fLookFor==NODE) {
    This->fCount++;
  } else if(This->fLookFor==TYPE) {
    if(This->fCheckDerived==TRUE) {
      if(aNode->getTypeId().isDerivedFrom(This->fType)) This->fCount++;
    } else {
      if(aNode->getTypeId()==This->fType) This->fCount++;
    }
  } else if(This->fLookFor==NAME) {
    if(This->fName==aNode->getName()) This->fCount++;
  }
  if(aNode->isOfType(SoSwitch::getClassTypeId())) {
    SoSwitch* sw = (SoSwitch*)aNode;
    SbBool flag = sw->whichChild.enableNotify(FALSE);
    int old = sw->whichChild.getValue();
    sw->whichChild.setValue(SO_SWITCH_ALL);
    aNode->doAction(aThis);
    sw->whichChild.setValue(old);
    sw->whichChild.enableNotify(flag);
  } else if(aNode->isOfType(SoGroup::getClassTypeId())) {
    aNode->doAction(aThis);
  } else if(aNode->isOfType(SoBaseKit::getClassTypeId())) {
    aNode->doAction(aThis);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoCounterAction::setLookFor(
 LookFor aLookFor
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fLookFor = aLookFor;
}
//////////////////////////////////////////////////////////////////////////////
void SoCounterAction::setType(
 const SoType aType
,SbBool aCheckDerived
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fType = aType;
  fCheckDerived = aCheckDerived;
}
//////////////////////////////////////////////////////////////////////////////
void SoCounterAction::setName(
 const SbName aName
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fName = aName;
}
//////////////////////////////////////////////////////////////////////////////
unsigned int SoCounterAction::getCount(
) const 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fCount;
}
