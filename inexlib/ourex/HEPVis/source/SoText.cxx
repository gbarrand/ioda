// this :
#include <HEPVis/nodekits/SoText.h>

#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/nodes/SoEventCallback.h>

#include <HEPVis/SbString.h>
#include <HEPVis/SbText.h>
#include <HEPVis/nodes/SoGrid.h>

#include <ctype.h> // For toupper.
#include <stdlib.h>
#include <string.h>

SoText::KeyboardType SoText::fKeyboardType = SoText::ENGLISH;

SO_KIT_SOURCE(SoText) 
//////////////////////////////////////////////////////////////////////////////
void SoText::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoText,SoPrimitive,"SoPrimitive");
  //
  char* env = getenv("HEPVIS_KEYBOARD");
  if(env!=NULL) {
    if(strcmp(env,"FRENCH")==0)
      fKeyboardType = FRENCH;
    else 
      fKeyboardType = ENGLISH;
  }
}
//////////////////////////////////////////////////////////////////////////////
SoText::SoText(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoText);
  SO_KIT_ADD_CATALOG_ENTRY(textFont,SoFont,TRUE,
                           frontSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(textGroup,SoGroup,FALSE,
                           frontSeparator,\0,TRUE);
  //
  SO_KIT_INIT_INSTANCE();
  //
  SO_NODE_ADD_FIELD(fontList,("Helvetica"));
  SO_NODE_ADD_FIELD(value,("Text"));
  SO_NODE_ADD_FIELD(rows,(5));
  SO_NODE_ADD_FIELD(topPosition,(1));
  SO_NODE_ADD_FIELD(textWidth,(5.));
  SO_NODE_ADD_FIELD(cursorPosition,(0));
  SO_NODE_ADD_FIELD(editable,(FALSE));
  //
  initialize();
}
//////////////////////////////////////////////////////////////////////////////
SoText::~SoText(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fontList.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  value.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  rows.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  topPosition.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  textWidth.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  cursorPosition.removeAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  //
  fActivateCallbackList.clearCallbacks();
}
//////////////////////////////////////////////////////////////////////////////
void SoText::initialize(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fontList.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  value.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  rows.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  topPosition.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  textWidth.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  cursorPosition.addAuditor(&requireUpdateSceneGraph,SoNotRec::FIELD);
  //
  fEventCallback = new SoEventCallback;
  fEventCallback->addEventCallback(SoKeyboardEvent::getClassTypeId(),
                                    eventCB,this);
  setPart("callbackList[0]",fEventCallback);
  //
  SoText::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
void SoText::addActivateCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fActivateCallbackList.
    addCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoText::removeActivateCallback(
 SoCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fActivateCallbackList.
    removeCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoText::invokeActivateCallbacks(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fActivateCallbackList.invokeCallbacks(this);
}
//////////////////////////////////////////////////////////////////////////////
void SoText::updateSceneGraph(
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
  fTextNodes.truncate(0);

  float ascent,descent,depth;
  SbText::getAscentDescent(font,SoText3::getClassTypeId(),ascent,descent,depth);
  float itemHeight = ascent + descent;
  //
  float textSize = textWidth.getValue();
  SbBool showCaret = editable.getValue();
  int caretPosition = cursorPosition.getValue();
  //
  int num = value.getNum(); 
  int begin = topPosition.getValue() - 1;
  if(begin<0) begin = 0;
  int end = begin + rows.getValue();
  if(end>num) end = num;
  float totalHeight = 0.;
  for(int count=begin;count<end;count++) {
    SoTranslation* translation = new SoTranslation;
    SoText3* text = new SoText3;
    grp->addChild(translation);
    grp->addChild(text);  
    fTextNodes.append(text);
    text->string.set1Value(0,
   SbText::getSubString(font,SoText3::getClassTypeId(),value[count],textSize));
    if(count==begin)
      translation->translation.setValue(0.,descent-itemHeight,0.);
    else
      translation->translation.setValue(0.,-itemHeight,0.);
    totalHeight += itemHeight;
    // Caret :
    if(showCaret==TRUE) {
      float w = 0.;
      if(caretPosition!=0) { 
        SbString subString = value[count].getSubString(0,caretPosition-1);
        SbBox3f bbox = 
          SbText::getBBox(font,SoText3::getClassTypeId(),1,&subString);
        w = bbox.getMax()[0]-bbox.getMin()[0];
      }
      SoGrid* grid = new SoGrid;
      grp->addChild(grid);
      grid->width.setValue(0.5F);
      grid->height.setValue(0.1F);
      grid->position.setValue(SbVec3f(w,-descent,0.));
    }
  }
  //
  SbVec3f mn(0,-totalHeight,0);
  SbVec3f mx(textSize,0,depth);
  fBox.setBounds(mn,mx);
  //
  SoPrimitive::updateSceneGraph();
}
//////////////////////////////////////////////////////////////////////////////
SbBox3f SoText::getFrontBBox(
)
//////////////////////////////////////////////////////////////////////////////
// Default method to retreive size of front things.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fBox;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoText::eventCB (
 void* aNode
,SoEventCallback* aEventCallback
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoText* This = (SoText*)aNode;
  if(This==NULL) return;

  if(aEventCallback->isHandled()) return;

  if(This->editable.getValue()==TRUE) {

    const SoKeyboardEvent* event= (SoKeyboardEvent*)aEventCallback->getEvent();
    if (SoKeyboardEvent::isKeyPressEvent(event,event->getKey())==FALSE) return;

    SbBool shift = event->wasShiftDown();
    SbBool ctrl = event->wasCtrlDown();
    SbBool alt = event->wasAltDown();

    SoKeyboardEvent::Key key = event->getKey();

    /*
    printf("debug : SoText : %x : %c : %d %d %d\n",
           key,
           event->getPrintableCharacter(),
           shift,ctrl,alt);
    */

    switch(key) {
    case SoKeyboardEvent::LEFT_CONTROL:
    case SoKeyboardEvent::RIGHT_CONTROL:
    case SoKeyboardEvent::LEFT_SHIFT:
    case SoKeyboardEvent::RIGHT_SHIFT:
    case SoKeyboardEvent::UP_ARROW:
    case SoKeyboardEvent::DOWN_ARROW:
    case SoKeyboardEvent::ANY:
      break;
    case SoKeyboardEvent::RETURN:
      //case SoKeyboardEvent::ENTER:
      This->invokeActivateCallbacks();
      aEventCallback->setHandled();
      This->cursorPosition.setValue(0);
      This->value.set1Value(0,"");
      aEventCallback->setHandled();
      break;
#ifdef _WIN32
    case SoKeyboardEvent::KEY_DELETE:
#else
    case SoKeyboardEvent::DELETE:
#endif
    case SoKeyboardEvent::BACKSPACE:{
      int caretPosition = This->cursorPosition.getValue();
      SbBool hasToDelete = caretPosition==0 ? FALSE : TRUE;
      caretPosition--;
      if(caretPosition<0) caretPosition = 0;
      This->cursorPosition.setValue(caretPosition);
      //
      if(hasToDelete==TRUE) {
        SbString sb = This->value[0];
        sb.deleteSubString(caretPosition,caretPosition);
        This->value.set1Value(0,sb);
      }
      aEventCallback->setHandled();
    }break;
    case SoKeyboardEvent::LEFT_ARROW:{
      int caretPosition = This->cursorPosition.getValue();
      caretPosition--;
      if(caretPosition<0) caretPosition = 0;
      This->cursorPosition.setValue(caretPosition);
      aEventCallback->setHandled();
    }break;
    case SoKeyboardEvent::RIGHT_ARROW:{
      int l = This->value.getValues(0)[0].getLength();
      int caretPosition = This->cursorPosition.getValue();
      caretPosition++;
      if(caretPosition>l) caretPosition = l;
      This->cursorPosition.setValue(caretPosition);
      aEventCallback->setHandled();
    }break;
    default:{
      int caretPosition = This->cursorPosition.getValue();
      //char c = event->getPrintableCharacter();
      char c = This->getCharacter(key,shift,ctrl,alt);
      This->insertCharacter(caretPosition,c);
      caretPosition++;
      This->cursorPosition.setValue(caretPosition);
      aEventCallback->setHandled();
    }break;
    }

  }
}
//////////////////////////////////////////////////////////////////////////////
void SoText::insertCharacter(
 int aPos
,char aChar
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  char s[2];
  s[0] = aChar;
  s[1] = '\0';
  int l = value[0].getLength();
  SbString begin = aPos==0 ? SbString("") : value[0].getSubString(0,aPos-1);
  SbString end = value[0].getSubString(aPos,l-1);
  SbString sb = begin;
  sb += s;
  sb += end;
  value.set1Value(0,sb);
}
//////////////////////////////////////////////////////////////////////////////
char SoText::getCharacter(
 SoKeyboardEvent::Key aKey
,SbBool aShift
,SbBool aCtrl
,SbBool //aAlt
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fKeyboardType==ENGLISH) {
    switch(aKey) {
    case SoKeyboardEvent::NUMBER_0: return (aShift==TRUE ? ')' : '0');
    case SoKeyboardEvent::NUMBER_1: return (aShift==TRUE ? '!' : '1');
    case SoKeyboardEvent::NUMBER_2: return (aShift==TRUE ? '@' : '2');
    case SoKeyboardEvent::NUMBER_3: return (aShift==TRUE ? '#' : '3');
    case SoKeyboardEvent::NUMBER_4: return (aShift==TRUE ? '$' : '4');
    case SoKeyboardEvent::NUMBER_5: return (aShift==TRUE ? '%' : '5');
    case SoKeyboardEvent::NUMBER_6: return (aShift==TRUE ? '^' : '6');
    case SoKeyboardEvent::NUMBER_7: return (aShift==TRUE ? '&' : '7');
    case SoKeyboardEvent::NUMBER_8: return (aShift==TRUE ? '*' : '8');
    case SoKeyboardEvent::NUMBER_9: return (aShift==TRUE ? '(' : '9');
    //
    case SoKeyboardEvent::APOSTROPHE:   return (aShift==TRUE ? '"' : '\'');
    case SoKeyboardEvent::COMMA:        return (aShift==TRUE ? '<' : ',');
    case SoKeyboardEvent::PERIOD:       return (aShift==TRUE ? '>' : '.');
    case SoKeyboardEvent::SLASH:        return (aShift==TRUE ? '?' : '/');
    case SoKeyboardEvent::MINUS:        return (aShift==TRUE ? '_' : '-');
    case SoKeyboardEvent::SEMICOLON:    return (aShift==TRUE ? ':' : ';');
    case SoKeyboardEvent::EQUAL:        return (aShift==TRUE ? '+' : '=');
    case SoKeyboardEvent::BACKSLASH:    return (aShift==TRUE ? '|' : '\\');
    case SoKeyboardEvent::GRAVE:        return (aShift==TRUE ? '~' : '`');
    case SoKeyboardEvent::BRACKETLEFT:  return (aShift==TRUE ? '{' : '[');
    case SoKeyboardEvent::BRACKETRIGHT: return (aShift==TRUE ? '}' : ']');
    //
    default: return (aShift==TRUE ? toupper((char)aKey) : (char)aKey);
    }
  } else if(fKeyboardType==FRENCH) {
    switch(aKey) {
    case SoKeyboardEvent::NUMBER_0: 
      return (aShift==TRUE ? '0' : (aCtrl==TRUE ? '@' : 'x'));
    case SoKeyboardEvent::NUMBER_1: 
      return (aShift==TRUE ? '1' : (aCtrl==TRUE ? '&' : '&'));
    case SoKeyboardEvent::NUMBER_2: 
      return (aShift==TRUE ? '2' : (aCtrl==TRUE ? '~' : 'x'));
    case SoKeyboardEvent::NUMBER_3: 
      return (aShift==TRUE ? '3' : (aCtrl==TRUE ? '#' : '\"'));
    case SoKeyboardEvent::NUMBER_4: 
      return (aShift==TRUE ? '4' : (aCtrl==TRUE ? '{' : '\''));
    case SoKeyboardEvent::NUMBER_5: 
      return (aShift==TRUE ? '5' : (aCtrl==TRUE ? '[' : '('));
    case SoKeyboardEvent::NUMBER_6: 
      return (aShift==TRUE ? '6' : (aCtrl==TRUE ? '|' : '-'));
    case SoKeyboardEvent::NUMBER_7: 
      return (aShift==TRUE ? '7' : (aCtrl==TRUE ? '`' : 'x'));
    case SoKeyboardEvent::NUMBER_8: 
      return (aShift==TRUE ? '8' : (aCtrl==TRUE ? '\\' : '_'));
    case SoKeyboardEvent::NUMBER_9: 
      return (aShift==TRUE ? '9' : (aCtrl==TRUE ? '^' : 'x'));
    //
    case SoKeyboardEvent::APOSTROPHE:   return (aShift==TRUE ? '"' : '\'');
    case SoKeyboardEvent::COMMA:        return (aShift==TRUE ? '?' : ',');
    case SoKeyboardEvent::PERIOD:       return (aShift==TRUE ? '.' : ';');
    case SoKeyboardEvent::SLASH:        return (aShift==TRUE ? '/' : ':');
    //case SoKeyboardEvent::MINUS:        return (aShift==TRUE ? '_' : '-');
    case SoKeyboardEvent::SEMICOLON:    return (aShift==TRUE ? 'x' : '$');
    case SoKeyboardEvent::EQUAL:        
      return (aShift==TRUE ? '+' : (aCtrl==TRUE ? '}' : '='));
    case SoKeyboardEvent::BACKSLASH:    return (aShift==TRUE ? 'x' : '*');
    case SoKeyboardEvent::GRAVE:        return (aShift==TRUE ? '%' : 'x');
    case SoKeyboardEvent::BRACKETLEFT:  
      return (aShift==TRUE ? 'x' : (aCtrl==TRUE ? ']' : ')'));
    case SoKeyboardEvent::BRACKETRIGHT: return (aShift==TRUE ? 'x' : '^');
    // Below ok with SoFree
    case SoKeyboardEvent::PRIOR: return (aShift==TRUE ? '>' : '<');
    //
#ifndef SoFreePackage //TGS
    case 0xFF63: return (aShift==TRUE ? '?' : ',');
    //
    case 0xFF57: return (aShift==TRUE ? '.' : ';');
    case 0xFF56: return (aShift==TRUE ? '/' : ':');
    case 0xFFAB: return (aShift==TRUE ? '%' : '!');
    case 0xFF51: return (aShift==TRUE ? '$' : '*');
#endif
    default: return (aShift==TRUE ? toupper((char)aKey) : (char)aKey);
    }
  }
  return 'x';
}






