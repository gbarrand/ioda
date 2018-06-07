// this :
#include <HEPVis/nodekits/SoSelectionBox.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>

#include <HEPVis/nodekits/SoScrolledList.h>

#include <math.h>

SO_KIT_SOURCE(SoSelectionBox) 
//////////////////////////////////////////////////////////////////////////////
void SoSelectionBox::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoSelectionBox,SoForm,"SoForm");
}
//////////////////////////////////////////////////////////////////////////////
SoSelectionBox::SoSelectionBox(
)
:fScrolledList(NULL)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoSelectionBox);
  SO_KIT_INIT_INSTANCE();
  //
  SO_NODE_ADD_FIELD(items,(""));
  //
  initialize();
}
//////////////////////////////////////////////////////////////////////////////
SoSelectionBox::~SoSelectionBox(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fScrolledList!=NULL) fScrolledList->items.disconnect();
  fOkCallbackList.clearCallbacks();
}
//////////////////////////////////////////////////////////////////////////////
void SoSelectionBox::initialize(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fScrolledList = new SoScrolledList;
  if(fScrolledList!=NULL) {
    fScrolledList->
      addSingleSelectionCallback(SoSelectionBox::singleSelectionCB,this);
    addChild(fScrolledList);
  }
  //
  fScrolledList->items.connectFrom(&items);
  //
  SoSelectionBox::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoSelectionBox::addOkCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fOkCallbackList.
    addCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoSelectionBox::removeOkCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fOkCallbackList.
    removeCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoSelectionBox::invokeOkCallbacks(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fOkCallbackList.invokeCallbacks(this);
}
//////////////////////////////////////////////////////////////////////////////
const SbString& SoSelectionBox::getSelection(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fSelection;
}
//////////////////////////////////////////////////////////////////////////////
void SoSelectionBox::updateSceneGraph(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoForm::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoSelectionBox::singleSelectionCB (
 void* aUserData
 ,SoNode* /*aNode*/
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSelectionBox* This = (SoSelectionBox*)aUserData;
  if(This==NULL) return;
  if(This->fScrolledList==NULL) return;
  This->fSelection = This->fScrolledList->getSelectedItem();
  This->invokeOkCallbacks();
  This->fSelection = "";
}
