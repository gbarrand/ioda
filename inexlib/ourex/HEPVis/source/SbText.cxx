// this :
#include <HEPVis/SbText.h>

#include <Inventor/SoType.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

//#include <HEPVis/SbString.h>

#include <cmath>

//////////////////////////////////////////////////////////////////////////////
SbBox3f SbText::getBBox(
 SoFont* aFont
,SoType aType
,int aNumber
,const SbString* aStrings
)
//////////////////////////////////////////////////////////////////////////////
// If not referenced aFont will be deleted !
// aType should correspond to an SoNode with a "string" SoMFString field.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbBox3f bb;
  if(aFont==NULL) return bb;
  SbViewportRegion vp; 
  SoGetBoundingBoxAction getBB(vp);
  SoGroup* local = new SoGroup;
  SoNode* node = (SoNode*)aType.createInstance();
  local->ref();
  local->addChild(aFont);    
  local->addChild(node);    
  SoMFString* field = (SoMFString*)node->getField("string");
  if(field!=NULL) field->setValues(0,aNumber,aStrings);
  getBB.apply(local);
  bb = getBB.getBoundingBox();
  local->removeAllChildren();
  local->unref();
  return bb;
}
//////////////////////////////////////////////////////////////////////////////
SbString SbText::getSubString(
 SoFont* aFont
,SoType aType
,const SbString& aString
,float aWidth
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbString value;
  if(aFont==NULL) return value;
  int number = aString.getLength();
  if(number<=0) return value;
  //
  SbViewportRegion vp; 
  SoGetBoundingBoxAction getBB(vp);
  //
  SoGroup* local = new SoGroup;
  SoNode* node = (SoNode*)aType.createInstance();
  local->ref();
  local->addChild(aFont);    
  local->addChild(node);    

  SoMFString* field = (SoMFString*)node->getField("string");

  char* s = new char[aString.getLength()+1];
  s = ::strcpy(s,aString.getString());

  SbBox3f bb;
  float w;
  for(int count=number-1;count>=0;count--) {
    if(field!=NULL) field->set1Value(0,SbString(s));
    getBB.apply(local);
    bb = getBB.getBoundingBox();
    w = bb.getMax()[0] - bb.getMin()[0];
    if(w<aWidth) break;
    s[count] = '\0';
  }

  value = s;

  delete [] s;

  local->removeAllChildren();
  local->unref();
  //
  return value;
}
//////////////////////////////////////////////////////////////////////////////
void SbText::getAscentDescent(
 SoFont* aFont
,SoType aType
,float& aAscent
,float& aDescent
,float& aDepth
)
//////////////////////////////////////////////////////////////////////////////
// aAscent, aDescent are going to be positive numbers.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aAscent = 1.;
  aDescent = 0.;
  aDepth = 0.;
  if(aFont==NULL) return;
  // Get ascent with $ (???) :
  SbString s = "$";
  SbBox3f bb = SbText::getBBox(aFont,aType,1,&s);
  aAscent = bb.getMax()[1];
  // Get descent with ) (???) :
  s = ")";
  bb = getBBox(aFont,aType,1,&s);
  aDescent = -bb.getMin()[1]; //positive.
  // Any character is ok for depth :
  aDepth = bb.getMax()[2] - bb.getMin()[2];
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SbChar::SbChar(
)
:fChar(0)
,fFont(LATIN)
,fYMove(NONE)
,fBack(FALSE)
,fBar(FALSE)
,fCR(FALSE)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
SbChar::SbChar(
 const SbChar& aFrom
)
:fChar(aFrom.fChar)
,fFont(aFrom.fFont)
,fYMove(aFrom.fYMove)
,fBack(aFrom.fBack)
,fBar(aFrom.fBar)
,fCR(aFrom.fCR)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
SbChar& SbChar::operator=(
 const SbChar& aFrom
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fChar = aFrom.fChar;
  fFont = aFrom.fFont;
  fYMove = aFrom.fYMove;
  fBack = aFrom.fBack;
  fBar = aFrom.fBar;
  fCR = aFrom.fCR;
  return *this;
}
//////////////////////////////////////////////////////////////////////////////
SbEncodedString SbText::decodePlain(
 const SbString& aString 
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbEncodedString sbs;
  int n = aString.getLength();
  for(int index=0;index<n;index++) {
    SbChar sbChar;
    sbChar.fChar = aString.getString()[index];
    sbs.push_back(sbChar);
  }
  if(sbs.size()) sbs[sbs.size()-1].fCR = TRUE;
  return sbs;
}
//////////////////////////////////////////////////////////////////////////////
/// PAW encoding /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// PAW control characters :
//  [ go to greek (roman = default)
//  ] end of greek
//  " go to special
//  # end of special
//  ! go to normal level of script
//  ^ go to superscript
//  ? go to subscript
//  & backscpace one charachter
// Extension :
//  | draw a bar over one character
//////////////////////////////////////////////////////////////////////////////
SbEncodedString SbText::decodePAW(
 const SbString& aString 
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbEncodedString sbs;

  SbChar::Font font = SbChar::LATIN;
  SbChar::Move move = SbChar::NONE;
  SbBool back = FALSE;
  SbBool bar = FALSE;

  int n = aString.getLength();
  for(int index=0;index<n;index++) {
    char c = aString.getString()[index];
    // Control characters :
    if(c=='[') {
      font = SbChar::GREEK;
      continue;
    } else if(c==']') {
      font = SbChar::LATIN;
      continue;
    } else if(c=='"') {
      font = SbChar::SPECIAL;
      continue;
    } else if(c=='#') {
      font = SbChar::LATIN;
      continue;
    } else if(c=='!') {
      move = SbChar::NONE;
      continue;
    } else if(c=='^') {
      move = SbChar::UP;
      continue;
    } else if(c=='?') {
      move = SbChar::DOWN;
      continue;
    } else if(c=='&') {
      back = TRUE;
      continue;
    } else if(c=='|') {
      bar = TRUE;
      continue;
    }

    SbChar sbChar;
    sbChar.fYMove = move;
    sbChar.fBack = back;
    sbChar.fBar = bar;

    if(font==SbChar::SPECIAL) {
      sbChar.fFont = SbChar::LATIN;
    //if(c=='A') {
    //  sbChar.fChar = '';
    //} else 
      if(c=='B') {
        sbChar.fChar = '|';
    //} else if(c=='C') {
    //  sbChar.fChar = '';
      } else if(c=='D') {
        sbChar.fChar = '$';
      } else if(c=='E') {
        sbChar.fChar = '!';
      } else if(c=='F') {
        sbChar.fChar = '#';
      } else if(c=='G') {
        sbChar.fChar = '>';
      } else if(c=='H') {
        sbChar.fChar = '?';
    //} else if(c=='I') {
    //  sbChar.fChar = '';
      } else if(c=='J') {
        sbChar.fChar = ':';
      } else if(c=='K') {
        sbChar.fChar = ';';
      } else if(c=='L') {
        sbChar.fChar = '<';
      } else if(c=='M') {
        sbChar.fChar = '[';
      } else if(c=='N') {
        sbChar.fChar = ']';

      } else if(c=='W') {
        sbChar.fChar = '&';
      } else if(c=='Y') {
        sbChar.fChar = '%';
      } else {
        //FIXME : handle other characters.
        sbChar.fChar = c;
      }

    } else {
      sbChar.fFont = font;
      sbChar.fChar = c;
    }


    sbs.push_back(sbChar);

    back = FALSE;
    bar = FALSE;
  }

  if(sbs.size()) sbs[sbs.size()-1].fCR = TRUE;
  return sbs;
}
