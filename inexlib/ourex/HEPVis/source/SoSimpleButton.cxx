/*
 *  SoSimpleButton.cxx
 *  
 */ 

// this :  
#include <HEPVis/nodekits/SoSimpleButton.h>

// Inventor :
#include <Inventor/SbLinear.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoText3.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/elements/SoViewportRegionElement.h>

#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>

// HEPVis :
#include <HEPVis/SbText.h>
#include <HEPVis/misc/SoStyleCache.h>
#include <HEPVis/nodes/SoAreaStyle.h>
#include <HEPVis/nodes/SoTextStyle.h>
#include <HEPVis/nodes/SoPyramid.h>
#include <HEPVis/nodes/SoTextHershey.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

//#define DEBUG

SO_KIT_SOURCE(SoSimpleButton)

//////////////////////////////////////////////////////////////////////////////
void SoSimpleButton::initClass(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoSimpleButton,SoBaseKit,"BaseKit");
}
//////////////////////////////////////////////////////////////////////////////
SoSimpleButton::SoSimpleButton(
)
:fOld_width(0)
,fOld_height(0)
,fOld_label("")
//,fOld_labelToBorder(0)
//,fOld_labelHeight(0)
,fStyleCache(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoSimpleButton);

  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0, FALSE);

  SO_KIT_ADD_CATALOG_ENTRY(backSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(labelSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);

  // Styles :
  SO_KIT_ADD_CATALOG_ENTRY(backStyle,SoAreaStyle,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(labelStyle,SoTextStyle,FALSE,topSeparator,\0,TRUE);

  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SO_NODE_ADD_FIELD(verbose,(FALSE));

  SO_NODE_ADD_FIELD(width,(1));
  SO_NODE_ADD_FIELD(height,(1));

  SO_NODE_ADD_FIELD(label,(""));
  //SO_NODE_ADD_FIELD(labelToBorder,(0.1F));
  //SO_NODE_ADD_FIELD(labelHeight,(0.8F));

  // Setup styles :
  ((SoAreaStyle*)backStyle.getValue())->setFromString("color black");
  ((SoTextStyle*)labelStyle.getValue())->setFromString(
    "color black\nfontName Hershey\nencoding PAW");

  fOld_backStyle.setTransparency(-1);
  fOld_labelStyle.setTransparency(-1);

  fStyleCache = new SoStyleCache;
  fStyleCache->ref();

  // Setup callback mechanism :
  SoEventCallback* eventCallback = new SoEventCallback;
  eventCallback->
    addEventCallback(SoMouseButtonEvent::getClassTypeId(),eventCB,this);
  setPart("callbackList[0]",eventCallback);
}
//////////////////////////////////////////////////////////////////////////////
SoSimpleButton::~SoSimpleButton(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fStyleCache->unref();
  fCallbackList.clearCallbacks();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoSimpleButton::doAction(
 SoAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  updateChildren();
  SO_ALTERNATEREP_DO_ACTION(aAction)
}
#define ACTION(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::doAction(action);\
  aSuper::aMethod(action);\
}
ACTION(SoSimpleButton,SoBaseKit,SoCallbackAction,callback)
ACTION(SoSimpleButton,SoBaseKit,SoGLRenderAction,GLRender)
ACTION(SoSimpleButton,SoBaseKit,SoGetBoundingBoxAction,getBoundingBox)
ACTION(SoSimpleButton,SoBaseKit,SoGetMatrixAction,getMatrix)
ACTION(SoSimpleButton,SoBaseKit,SoHandleEventAction,handleEvent)
ACTION(SoSimpleButton,SoBaseKit,SoRayPickAction,rayPick)
ACTION(SoSimpleButton,SoBaseKit,SoSearchAction,search)
//ACTION(SoSimpleButton,SoBaseKit,SoWriteAction,write)
ACTION(SoSimpleButton,SoBaseKit,SoPickAction,pick)

#include <HEPVis/nodes/SoSubNode.h>

void SoSimpleButton::write(SoWriteAction* aAction) {
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    // enforce the writing of fields, else a pure Coin reader
    // (not linked to HEPVis libs) produces warnings.

    HEPVIS_SET_FIELDS_DEFAULT(FALSE)

    // no need to do updateChildren since
    // the alternateRep is already created.

    //callbackList is going to be written too.
    //But it is not a problem since a non-HEPVis reader
    //will not know the callbacks methods anyway.
    SoBaseKit::write(aAction);

    HEPVIS_RESTORE_FIELDS_DEFAULT

    // other solution :
    //altRep->write(aAction);
  } else {
    SoSimpleButton::updateChildren();
    // Do not write callbackList.
    // Else at read time, we shall loose the callbacks
    // installed in the constructor.
    SbBool old = callbackList.isDefault();
    callbackList.setDefault(TRUE); //enforce not writing.
    SoBaseKit::write(aAction);
    callbackList.setDefault(old);
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SoAreaStyle* SoSimpleButton::getBackStyle(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoAreaStyle*)backStyle.getValue();
}
//////////////////////////////////////////////////////////////////////////////
SoTextStyle* SoSimpleButton::getLabelStyle(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoTextStyle*)labelStyle.getValue();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoSimpleButton::updateChildren(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SbBool update_back = FALSE;
  SbBool update_label = FALSE;

  if(fOld_width!=width.getValue()) {
    fOld_width = width.getValue();
    update_back = TRUE;
    update_label = TRUE;
  }
  if(fOld_height!=height.getValue()) {
    fOld_height = height.getValue();
    update_back = TRUE;
    update_label = TRUE;
  }

  if(fOld_label!=label.getValue()) {
    fOld_label = label.getValue();
    update_label = TRUE;
  }
/*
  if(fOld_labelToBorder!=labelToBorder.getValue()) {
    fOld_labelToBorder = labelToBorder.getValue();
    update_label = TRUE;
  }
  if(fOld_labelHeight!=labelHeight.getValue()) {
    fOld_labelHeight = labelHeight.getValue();
    update_label = TRUE;
  }
*/

 {SoAreaStyle* soStyle = getBackStyle();
  if(fOld_backStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    fOld_backStyle = soStyle->getSbStyle();
    update_back = TRUE;
  }}

 {SoTextStyle* soStyle = getLabelStyle();
  if(fOld_labelStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    fOld_labelStyle = soStyle->getSbStyle();
    update_label = TRUE;
  }}

#ifdef DEBUG
  printf("SoSimpleButton::updateSceneGraph %ld\n",this);
#endif
  if(width.getValue()<=0) {
    SoSeparator* soBackSep = (SoSeparator*)backSeparator.getValue();
    soBackSep->removeAllChildren();
    SoSeparator* soLabelSep = (SoSeparator*)labelSeparator.getValue();
    soLabelSep->removeAllChildren();
    return;
  }

  if(verbose.getValue()==TRUE) {
    if(update_back) 
      SoDebugError::postInfo("SoSimpleButton::updateChildren","update back");
    if(update_label) 
      SoDebugError::postInfo("SoSimpleButton::updateChildren","update label");
  }

  float bw = width.getValue();
  float bh = height.getValue();
  float fw = bw * 0.9F;
  float fh = bh * 0.9F;
  //float d = bw*0.1F; //Depth same order of magnitude than with.
  float d = 2; //Should be in front the SoFormRegion camera.

  if(update_back==TRUE) {
    update_back = FALSE;

    //printf("debug : SoSimpleButton::updateChildren : update_lines\n");

    SoSeparator* soBackSep = (SoSeparator*)backSeparator.getValue();
    soBackSep->removeAllChildren();

    SoAreaStyle* style = (SoAreaStyle*)backStyle.getValue();

    soBackSep->addChild(fStyleCache->getMaterial 
         (style->color.getValue(),(float)style->transparency.getValue()));

    soBackSep->addChild(fStyleCache->getFilled());

    SoPyramid* soPyramid = new SoPyramid;
    soPyramid->backWidth.setValue(bw);
    soPyramid->backHeight.setValue(bh);
    soPyramid->frontWidth.setValue(fw);
    soPyramid->frontHeight.setValue(fh);
    soPyramid->depth.setValue(d);
    soBackSep->addChild(soPyramid);
  }

  if(update_label == TRUE) {
    update_label = FALSE;

    SoSeparator* soLabelSep = (SoSeparator*)labelSeparator.getValue();
    soLabelSep->removeAllChildren();

    SoTextStyle* style = (SoTextStyle*)labelStyle.getValue();

    if( (label.getValue()!="") && (style->visible.getValue()==TRUE) ) { 

      soLabelSep->addChild(fStyleCache->getMaterial 
        (style->color.getValue(),(float)style->transparency.getValue()));

#ifdef DEBUG
      printf("SoSimpleButton :: dessin du titre \n");
      printf(": %s : height %g : size %g\n",
        title.getValue().getString(),
        titleHeight.getValue(),titleHeight.getValue());
#endif

      SbString fontName = style->fontName.getValue();
      if( (fontName.getLength() >=6) &&
          (fontName.getSubString(0,5)=="Text3/") ) {

        soLabelSep->addChild(fStyleCache->getFilled());

        SoTransform* transform = new SoTransform;

        SoFont* font = new SoFont;
        SbString sf = fontName.getSubString(6,fontName.getLength()-1);
        font->name.setValue(sf);
    
        SoText3* text = new SoText3;
        text->string.setValue(label.getValue());

        // Loop down text height up that text fits in front pyramid :
        float th = fh;
        float dh = fh*0.01F;
        const SbString& s = label.getValue();
        font->ref(); //Because of SbText::getBBox.
        SbBox3f box;
        while(true) {
          font->size.setValue(th);
          box = SbText::getBBox(font,SoText3::getClassTypeId(),1,&s);
          float bxw = box.getMax()[0]-box.getMin()[0];
          float bxh = box.getMax()[1]-box.getMin()[1];
          if( (bxw<fw) && (bxh<fh) ) break;
          th -= dh;
          if(th<=0) break;
        }
   
        float xx = -(box.getMin()[0]+box.getMax()[0])*0.5F;
        float yy = -(box.getMin()[1]+box.getMax()[1])*0.5F;
        float zz = d; //Should be in front of the back pyramid.
        transform->translation.setValue(SbVec3f(xx,yy,zz));

        soLabelSep->addChild(transform);
        soLabelSep->addChild(font);
        font->unref();
        soLabelSep->addChild(text);

      } else if( (fontName.getLength()>=6) && 
                 (fontName.getSubString(0,5)=="Text2/") ) {
/*    } else if(SbStringIsTTF(fontName)==TRUE) {
      text_size *= ttfScale.getValue();*/
      } else {  // Default is Hershey.

        soLabelSep->addChild(fStyleCache->getLineStyle());

        SoTransform* transform = new SoTransform;

        SoTextHershey* text = new SoTextHershey;
        text->encoding.setValue(style->encoding.getValue());
        text->string.setValue(label.getValue());

        // Loop down text height up that text fits in front pyramid :
        float th = fh;
        float dh = fh*0.01F;
        SbBox3f box;
        while(true) {
          text->height.setValue(th);
          text->getTextBounds(box);
          float bxw = box.getMax()[0]-box.getMin()[0];
          //FIXME : float bxh = box.getMax()[1]-box.getMin()[1];
          //FIXME : should be the upper !
          float bxh = text->getAscent() + text->getDescent(); 
          if( (bxw<fw) && (bxh<fh) ) break;
          th -= dh;
          if(th<=0) break;
        }
   
        float xx = -(box.getMin()[0]+box.getMax()[0])*0.5F;
        float yy = -(box.getMin()[1]+box.getMax()[1])*0.5F;
        float zz = d; //Should be in front of the back pyramid.
        transform->translation.setValue(SbVec3f(xx,yy,zz));

        soLabelSep->addChild(transform);
        soLabelSep->addChild(text);
      }

    }
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoSimpleButton::generateAlternateRep(SoAction*) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(topSeparator.getValue());
}
//////////////////////////////////////////////////////////////////////////////
void SoSimpleButton::clearAlternateRep(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(NULL);
}
//////////////////////////////////////////////////////////////////////////////
void SoSimpleButton::addCallback(
 SoSimpleButtonCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fCallbackList.addCallback((SoCallbackListCB*)aFunction,aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoSimpleButton::removeCallback(
 SoSimpleButtonCB* aFunction
,void* aUserData
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fCallbackList.removeCallback((SoCallbackListCB*)aFunction,
                                       aUserData);
}
//////////////////////////////////////////////////////////////////////////////
void SoSimpleButton::invokeCallbacks(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fCallbackList.invokeCallbacks(this);
}
//////////////////////////////////////////////////////////////////////////////
void SoSimpleButton::eventCB (
 void* aData
,SoEventCallback* aEventCallback
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(aEventCallback->isHandled()) return;
  const SoMouseButtonEvent* event= 
    (SoMouseButtonEvent*)aEventCallback->getEvent();
  SoSimpleButton* This = (SoSimpleButton*)aData;

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
    //This->setBackground(SbColor(0.,0.,0.));
    //This->setForeground(SbColor(1.,1.,1.));

  } else {

    //printf("debug : released \n");
    //This->setBackground(This->background.getValue());
    //This->setForeground(This->foreground.getValue());

    //printf ("debug : Pick button !!!\n");
    This->invokeCallbacks();

  }

  aEventCallback->setHandled();
}
