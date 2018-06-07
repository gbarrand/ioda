// this :
#include <HEPVis/nodekits/SoScrolledList.h>

#include <HEPVis/nodekits/SoForm.h>
#include <HEPVis/nodekits/SoList.h>
#include <HEPVis/nodekits/SoArrowButton.h>

#include <math.h>

SO_KIT_SOURCE(SoScrolledList) 
//////////////////////////////////////////////////////////////////////////////
void SoScrolledList::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoScrolledList,SoForm,"SoForm");
}
//////////////////////////////////////////////////////////////////////////////
SoScrolledList::SoScrolledList(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoScrolledList);
  SO_KIT_INIT_INSTANCE();
  //
  SO_NODE_ADD_FIELD(fontList,("Helvetica"));
  SO_NODE_ADD_FIELD(items,("List"));
  SO_NODE_ADD_FIELD(visibleItemCount,(5));
  SO_NODE_ADD_FIELD(topItemPosition,(1));
  SO_NODE_ADD_FIELD(listWidth,(5.));
  //
  initialize();
}
//////////////////////////////////////////////////////////////////////////////
SoScrolledList::~SoScrolledList(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fList!=NULL) {
    fList->fontList.disconnect();
    fList->items.disconnect();
    fList->visibleItemCount.disconnect();
    fList->topItemPosition.disconnect();
    fList->listWidth.disconnect();
    //
    fList->leftNode.setValue(NULL);
    fDown->topNode.setValue(NULL);
    fUp->bottomNode.setValue(NULL);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoScrolledList::initialize(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fList = new SoList;
  fUp = new SoArrowButton;
  fUp->arrowDirection.setValue(SoArrowButton::ARROW_UP);
  fDown = new SoArrowButton;
  fDown->arrowDirection.setValue(SoArrowButton::ARROW_DOWN);
  //
  /* 
  // Scroll bar at right :
  fDown->leftAttachment.setValue(SoPrimitive::ATTACH_NODE);
  fDown->leftNode.setValue(fList);
  fDown->topAttachment.setValue(SoPrimitive::ATTACH_OPPOSITE_NODE);
  fDown->topNode.setValue(fList);
  //
  fUp->leftAttachment.setValue(SoPrimitive::ATTACH_NODE);
  fUp->leftNode.setValue(fList);
  fUp->bottomAttachment.setValue(SoPrimitive::ATTACH_OPPOSITE_NODE);
  fUp->bottomNode.setValue(fList);
  */
  // Scroll bar at left :
  fDown->topAttachment.setValue(SoPrimitive::ATTACH_OPPOSITE_NODE);
  fDown->topNode.setValue(fList);
  //
  fUp->bottomAttachment.setValue(SoPrimitive::ATTACH_OPPOSITE_NODE);
  fUp->bottomNode.setValue(fList);
  //
  fList->leftAttachment.setValue(SoPrimitive::ATTACH_NODE);
  fList->leftNode.setValue(fUp);
  //
  addChild(fList);
  addChild(fUp);
  addChild(fDown);
  //
  fUp->addActivateCallback(upCB,fList);
  fDown->addActivateCallback(downCB,fList);
  //
  fList->fontList.connectFrom(&fontList);
  fList->items.connectFrom(&items);
  fList->visibleItemCount.connectFrom(&visibleItemCount);
  fList->topItemPosition.connectFrom(&topItemPosition);
  fList->listWidth.connectFrom(&listWidth);
  //
  SoScrolledList::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoScrolledList::addSingleSelectionCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fList==NULL) return;
  fList->addSingleSelectionCallback(aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoScrolledList::removeSingleSelectionCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fList==NULL) return;
  fList->removeSingleSelectionCallback(aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
const SbString& SoScrolledList::getSelectedItem(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  //have to return something : if(fList==NULL) return NULL;
  return fList->getSelectedItem();
}
//////////////////////////////////////////////////////////////////////////////
void SoScrolledList::updateSceneGraph(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoForm::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoScrolledList::downCB (
 void* aUserData
 ,SoNode* //aNode
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoList* list = (SoList*)aUserData;
  if(list==NULL) return;
  int value = list->topItemPosition.getValue();
  int pos = list->items.getNum() - list->visibleItemCount.getValue() + 1;
  value++;
  if(value>pos) value = pos;
  list->topItemPosition.setValue(value);
}
//////////////////////////////////////////////////////////////////////////////
void SoScrolledList::upCB (
 void* aUserData
 ,SoNode* //aNode
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoList* list = (SoList*)aUserData;
  if(list==NULL) return;
  int value = list->topItemPosition.getValue();
  value--;
  if(value<=0) value = 1;
  list->topItemPosition.setValue(value);
}
