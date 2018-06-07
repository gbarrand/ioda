// this :
#include <HEPVis/nodekits/SoList.h>

#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>

#include <HEPVis/SbText.h>

SO_KIT_SOURCE(SoList) 
//////////////////////////////////////////////////////////////////////////////
void SoList::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoList,SoPrimitive,"SoPrimitive");
}
//////////////////////////////////////////////////////////////////////////////
SoList::SoList(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoList);
  SO_KIT_ADD_CATALOG_ENTRY(textFont,SoFont,TRUE,
                           frontSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(textGroup,SoGroup,FALSE,
                           frontSeparator,\0,TRUE);
  
  SO_KIT_INIT_INSTANCE();
  
  SO_NODE_ADD_FIELD(fontList,("Helvetica"));
  SO_NODE_ADD_FIELD(items,("List"));
  SO_NODE_ADD_FIELD(visibleItemCount,(5));
  SO_NODE_ADD_FIELD(topItemPosition,(1));
  SO_NODE_ADD_FIELD(listSizePolicy,(CONSTANT));
  SO_NODE_ADD_FIELD(listWidth,(5.));
  
  SO_NODE_DEFINE_ENUM_VALUE(ListSizePolicy,CONSTANT);
  SO_NODE_DEFINE_ENUM_VALUE(ListSizePolicy,VARIABLE);
  
  SO_NODE_SET_SF_ENUM_TYPE(listSizePolicy,ListSizePolicy);
  
  initialize();
}
//////////////////////////////////////////////////////////////////////////////
SoList::~SoList(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fontList.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  items.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  visibleItemCount.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  topItemPosition.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  //
  fSingleSelectionCallbackList.clearCallbacks();
}
//////////////////////////////////////////////////////////////////////////////
void SoList::initialize(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fontList.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  items.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  visibleItemCount.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  topItemPosition.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  //
  SoEventCallback* eventCallback = new SoEventCallback;
  eventCallback->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
                                    eventCB,this);
  setPart("callbackList[0]",eventCallback);
  //
  SoList::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoList::addSingleSelectionCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fSingleSelectionCallbackList.
    addCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoList::removeSingleSelectionCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fSingleSelectionCallbackList.
    removeCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoList::invokeSingleSelectionCallbacks(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fSingleSelectionCallbackList.invokeCallbacks(this);
}
//////////////////////////////////////////////////////////////////////////////
const SbString& SoList::getSelectedItem(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fSelectedItem;
}
//////////////////////////////////////////////////////////////////////////////
void SoList::updateSceneGraph(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoFont* font = (SoFont*)getPart("textFont",TRUE);
  if(font==NULL) return;
  SoGroup* grp = (SoGroup*)textGroup.getValue();
  if(grp==NULL) return;
  //
  font->size.setValue(1.0);
  font->name.setValue(fontList.getValue());
  // Reset :
  grp->removeAllChildren();
  fItemNodes.truncate(0);

  float ascent,descent,depth;
  SbText::getAscentDescent(font,SoText3::getClassTypeId(),ascent,descent,depth);
  float itemHeight = ascent + descent;
  //
  ListSizePolicy policy = (ListSizePolicy)listSizePolicy.getValue();
  float listSize = listWidth.getValue();
  //
  int num = items.getNum(); 
  int begin = topItemPosition.getValue() - 1;
  int end = begin + visibleItemCount.getValue();
  float totalHeight = 0.;
  for(int count=begin;count<end;count++) {
    SoTranslation* translation = new SoTranslation;
    SoText3* text = new SoText3;
    grp->addChild(translation);
    grp->addChild(text);
    fItemNodes.append(text);
    SbString line;
    if( (count>=0) && (count<num) ){
      line = items[count];
    }
    if(policy==VARIABLE) {
      text->string.set1Value(0,line);
    } else { //CONSTANT
      text->string.set1Value(0,
       SbText::getSubString(font,SoText3::getClassTypeId(),
                            line,listSize));
    }
    if(count==begin)
      translation->translation.setValue(0.,descent-itemHeight,0.);
    else
      translation->translation.setValue(0.,-itemHeight,0.);
    totalHeight += itemHeight;
  }
  //
  if(policy==CONSTANT) {
    SbVec3f mn(0.,-totalHeight,0.);
    SbVec3f mx(listSize,0.,depth);
    fBox.setBounds(mn,mx);
  }
  //
  SoPrimitive::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
SbBox3f SoList::getFrontBBox(
)
//////////////////////////////////////////////////////////////////////////////
// Default method to retreive size of front things.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  ListSizePolicy policy = (ListSizePolicy)listSizePolicy.getValue();
  if(policy==VARIABLE) {
    return SoPrimitive::getFrontBBox();
  } else { //CONSTANT
    return fBox;
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoList::eventCB (
 void* aNode
,SoEventCallback* aEventCallback
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoList* This = (SoList*)aNode;
  if(This==NULL) return;

  if (aEventCallback->isHandled()) return;
  const SoMouseButtonEvent* event= 
    (SoMouseButtonEvent *) aEventCallback->getEvent();

  if (SoMouseButtonEvent::isButtonPressEvent
      (event,SoMouseButtonEvent::BUTTON1)==FALSE) return;
  
  //printf ("debug : Pick something ?\n");

  const SoPickedPoint* pickedPoint = aEventCallback->getPickedPoint();
  if (pickedPoint==NULL) return;

  SoFullPath* path = (SoFullPath*)pickedPoint->getPath();
  if(path==NULL) return;

  //printf ("debug : Pick button !!!\n");

  int number = This->fItemNodes.getLength();
  int num = This->items.getNum(); 
  SoNode* node;
  for(int count=0;count<number;count++) {
    node = (SoNode*)(This->fItemNodes[count]);
    if(path->containsNode(node)==TRUE) {
      int pos = This->topItemPosition.getValue() + count - 1;
      This->fSelectedItem = (pos>=0)&&(pos<num) ? 
                            This->items[pos] : SbString("");
      ((SoList*)This)->invokeSingleSelectionCallbacks();
      This->fSelectedItem = "";
      aEventCallback->setHandled();
      break;
    }
  }
}
