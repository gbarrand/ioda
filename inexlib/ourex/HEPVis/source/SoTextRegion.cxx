// this :
#include <HEPVis/nodekits/SoTextRegion.h>

#include <Inventor/SbBox.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterial.h>
//#include <Inventor/nodes/SoNormal.h>
//#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/elements/SoViewportRegionElement.h>

// HEPVis :
#include <HEPVis/SbString.h>
#include <HEPVis/SbMath.h>
#include <HEPVis/misc/SoTools.h>
#include <HEPVis/misc/SoStyleCache.h>
#include <HEPVis/nodes/SoTextStyle.h>
#include <HEPVis/nodes/SoTextHershey.h>
#include <HEPVis/nodes/SoTextTTF.h>
#include <HEPVis/nodes/SoViewportRegion.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

#include <vector>

#define CHILD_MATERIAL 0
#define CHILD_TRANS    1
#define CHILD_TEXT     2

#define YOFFSET 2 //FIXME : have an SoField.

SO_KIT_SOURCE(SoTextRegion) 
//////////////////////////////////////////////////////////////////////////////
void SoTextRegion::initClass (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoTextRegion,SoRegion,"SoRegion");
}
//////////////////////////////////////////////////////////////////////////////
SoTextRegion::SoTextRegion(
)
:fOldWidth(0)
,fOldHeight(0)
,fDefaultStyle(0)
,fOldText(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoTextRegion);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0,FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(light,SoDirectionalLight,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lightModel,SoLightModel,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(camera,SoOrthographicCamera,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lines,SoSeparator,FALSE,topSeparator,\0,TRUE);

  // must not be put under topSeparator because of alternate reps.
  SO_KIT_ADD_CATALOG_ENTRY(styleSeparator,SoSeparator,FALSE,this,\0,TRUE);
  SO_KIT_ADD_CATALOG_LIST_ENTRY(styles,SoSeparator,FALSE,styleSeparator,\0,SoTextStyle,TRUE);
  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(text,(""));
  SO_NODE_ADD_FIELD(horizontalJustification,(LEFT));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,LEFT);
  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,RIGHT);
  SO_NODE_DEFINE_ENUM_VALUE(HorizontalJustification,CENTER);

  SO_NODE_SET_SF_ENUM_TYPE(horizontalJustification,HorizontalJustification);

  //WARNING : if PHONG, problem of rendering with gl2ps production
  //          in opaw with /picture/print command from a -kumac_file.

  // About light, see notes in SoPlotterRegion constructor.
  SoDirectionalLight* soDirLight = (SoDirectionalLight*)light.getValue();
  //soDirLight->on.setValue(FALSE);
  soDirLight->on.setValue(TRUE);
  soDirLight->color.setValue(1,1,1);
  soDirLight->direction.setValue(SbVec3f(1,-1,-10));

  SoLightModel* soLightModel = (SoLightModel*)lightModel.getValue();
  //soLightModel->model.setValue(SoLightModel::BASE_COLOR);
  soLightModel->model.setValue(SoLightModel::PHONG);

  fDefaultStyle = new SoTextStyle;
  fDefaultStyle->ref();
  fDefaultStyle->setFromString("color black\nsmoothing FALSE");

  SoTextRegion::resetCamera();

  fOldText = new SoMFString;

}
//////////////////////////////////////////////////////////////////////////////
SoTextRegion::~SoTextRegion(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fDefaultStyle->unref();
  delete fOldText;
}
//////////////////////////////////////////////////////////////////////////////
void SoTextRegion::clear(
 const SbString& //aWhat
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  text.setValue("");
  horizontalMargin.setValue(2); //PIXELS
  verticalMargin.setValue(2); //PIXELS
  horizontalJustification.setValue(LEFT);

  SoGroup* group = getContainerNode("styles");
  group->removeAllChildren();

  //getViewportRegion()->reset();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
static int sDoCounter = 0;
//////////////////////////////////////////////////////////////////////////////
void SoTextRegion::doAction(
 SoAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ALTERNATEREP_DO_ACTION(aAction)

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoTextRegion::doAction","%lu (%d).",(unsigned long)this,sDoCounter++);
  }

  //WARNING : SoViewportRegion can't be within a SoSeparator in
  //          the nodekit, else state will pop at end of SoBaseKit::doAction
  //          and we will have a bad SbViewportRegion in the below.
  // Execute SoBaseKit in order to execute SoViewportRegion doAction.
  //FIXME : how to traverse SoRegion children only ?
  //SoRegion::doAction(aAction);
  SoViewportRegion* soViewportRegion = SoRegion::getViewportRegion();
  soViewportRegion->doAction(aAction);

  const SbViewportRegion& vp = SoViewportRegionElement::get(aAction->getState());
  short wvp = vp.getViewportSizePixels()[0];
  short hvp = vp.getViewportSizePixels()[1];

  if((wvp!=fOldWidth)||(hvp!=fOldHeight)) {
    fOldWidth = wvp;
    fOldHeight = hvp;
    fUpdate = TRUE;
  }

  updateChildren();
}

#define ACTION(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  if(visible.getValue()==FALSE) return;\
  aClass::doAction(action);\
  aSuper::aMethod(action);\
}
ACTION(SoTextRegion,SoRegion,SoCallbackAction,callback)
ACTION(SoTextRegion,SoRegion,SoGLRenderAction,GLRender)
ACTION(SoTextRegion,SoRegion,SoGetBoundingBoxAction,getBoundingBox)
//ACTION(SoTextRegion,SoRegion,SoGetMatrixAction,getMatrix)
ACTION(SoTextRegion,SoRegion,SoHandleEventAction,handleEvent)
ACTION(SoTextRegion,SoRegion,SoRayPickAction,rayPick)
ACTION(SoTextRegion,SoRegion,SoPickAction,pick)

#define ACTION1(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::updateChildren();\
  aSuper::aMethod(action);\
}
ACTION1(SoTextRegion,SoRegion,SoGetMatrixAction,getMatrix)

#define ACTION2(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  /*if(visible.getValue()==FALSE) return;*/\
  /* There is no SoViewportRegionElement on search and write action.*/\
  /*aClass::updateChildren();*/\
  aSuper::aMethod(action);\
}
ACTION2(SoTextRegion,SoRegion,SoSearchAction,search)
//ACTION2(SoTextRegion,SoRegion,SoWriteAction,write)

void SoTextRegion::write(SoWriteAction* aAction) {
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    altRep->write(aAction);
  } else {
    SoBaseKit::write(aAction);
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoTextRegion::updateChildren(){

  if(text!=(*fOldText)) {
    *fOldText = text;
    fUpdate = TRUE;
  }

  if(fUpdate==FALSE) return;

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoTextRegion::doAction","%lu : update",(unsigned long)this);
  }

  SoSeparator* soLines = (SoSeparator*)lines.getValue();
  //SbBool flag = soLines->enableNotify(FALSE);
  soLines->removeAllChildren();
  int number = text.getNum();
  for(int index=0;index<number;index++) {
    SoSeparator* line = new SoSeparator; 
    soLines->addChild(line);

    SoTextStyle* soStyle = getStyle(index);
    SbString encoding = soStyle->encoding.getValue();
    SbString fontName = soStyle->fontName.getValue();
    SbBool smoothing = soStyle->smoothing.getValue();
    SbBool hinting = soStyle->hinting.getValue();
    SbColor color = soStyle->color.getValue();
    float transparency = soStyle->transparency.getValue();

    //CHILD_MATERIAL
    if(fontName==SbFont_Hershey) {
      SoGroup* group = new SoGroup;

      group->addChild(fStyleCache->getLineStyle
        (soStyle->linePattern.getValue(),
         (float)soStyle->lineWidth.getValue()));
      //group->addChild(fStyleCache->getNormalBindingOverall());
      //group->addChild(fStyleCache->getNormalZ());
      group->addChild(fStyleCache->getMaterial(color,transparency));

      line->addChild(group);
    } else {
      line->addChild(fStyleCache->getMaterial(color,transparency));
    }

    //CHILD_TRANS
    line->addChild(new SoTranslation);

    //CHILD_TEXT
    if(fontName==SbFont_Hershey) {
      SoTextHershey* soText = new SoTextHershey;
      soText->encoding.setValue(encoding);
      SbString value(text[index]);
      soText->string.setValue(value);
      line->addChild(soText);
    } else {
      SoTextTTF* textTTF = new SoTextTTF;
      textTTF->fontName.setValue(fontName);
      textTTF->smoothing.setValue(smoothing);
      textTTF->hinting.setValue(hinting);
      textTTF->encoding.setValue(encoding);
      SbString value(text[index]);
      textTTF->string.setValue(value);
      line->addChild(textTTF);
    }
  }
  //soLines->enableNotify(flag);

  float wvp = float(fOldWidth);
  float hvp =  float(fOldHeight);

  matchViewport(wvp,hvp);

  fUpdate = FALSE;
}
//////////////////////////////////////////////////////////////////////////////
SoTextStyle* SoTextRegion::getStyle(
 int aIndex
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoGroup* group = getContainerNode("styles");
  if(!group) return 0;
  if(aIndex<0) return 0;
  int number = group->getNumChildren();
  if(aIndex>=number) {
    for(int index=number;index<=aIndex;index++) {
      setPart(SbStringPrintF(32,"styles[%d]",index),fDefaultStyle->copy());
    }
  }
  return (SoTextStyle*)group->getChild(aIndex);
}
//////////////////////////////////////////////////////////////////////////////
SoSeparator* SoTextRegion::getTopSeparator(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoSeparator*)topSeparator.getValue();
}
//////////////////////////////////////////////////////////////////////////////
SoCamera* SoTextRegion::getCamera(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoCamera*)camera.getValue();
}
//////////////////////////////////////////////////////////////////////////////
void SoTextRegion::resetCamera(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoCamera* soCamera = (SoCamera*)camera.getValue();
  //WARNING : ADJUST_CAMERA does not work properly
  soCamera->viewportMapping.setValue(SoCamera::LEAVE_ALONE);
  soCamera->aspectRatio.setValue(1);
  soCamera->position.setValue(0,0,4);
  soCamera->orientation.setValue(SbRotation(SbVec3f(0,1,0),0));
  soCamera->nearDistance.setValue(1);
  soCamera->farDistance.setValue(100);
  soCamera->focalDistance.setValue(4);
  if(soCamera->isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
    SoOrthographicCamera* soOrthoCamera = (SoOrthographicCamera*)soCamera;
    soOrthoCamera->height.setValue(200);
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoTextRegion::setCharSize(
 float aCharSize
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSeparator* soLines = (SoSeparator*)lines.getValue();
  int number = soLines->getNumChildren();
  for(int index=0;index<number;index++) {
    SoSeparator* line = (SoSeparator*)soLines->getChild(index);
   {SoNode* node = line->getChild(CHILD_TEXT);
    if(node->isOfType(SoTextHershey::getClassTypeId())==TRUE) {
      ((SoTextHershey*)node)->height.setValue(aCharSize);
    } else {
      ((SoTextTTF*)node)->size.setValue(SbFROUND(aCharSize));
    }}
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoTextRegion::getInsideSize(
 float& aWidth
,float& aHeight
,float& aPointSize
,float& aAscent
,float& aDescent
,std::vector<float>& aWidths
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSeparator* soLines = (SoSeparator*)lines.getValue();
  int number = soLines->getNumChildren();
  aWidth = 0;
  aAscent = 0;
  aDescent = 0;
  aPointSize = 0;
  aWidths.resize(number);
  for(int index=0;index<number;index++) {
    SoSeparator* line = (SoSeparator*)soLines->getChild(index);
    SoNode* node = line->getChild(CHILD_TEXT);
    if(node->isOfType(SoTextHershey::getClassTypeId())==TRUE) {
      SoTextHershey* soText = (SoTextHershey*)node;
      SbBox3f box;
      soText->getTextBounds(box);
      float xmin,ymin,zmin,xmax,ymax,zmax;
      box.getBounds(xmin,ymin,zmin,xmax,ymax,zmax);

      aWidths[index] = xmax - xmin;
      aWidth = SbMaximum(aWidth,aWidths[index]);
      if(!index) {
        aAscent = soText->getAscent(); 
        aDescent = soText->getDescent(); 
        aPointSize = soText->height.getValue();
      }
    } else {
      SoTextTTF* text = (SoTextTTF*)node;
      SbBox2s box;
      text->getTextBounds(box);
      short xmin,ymin,xmax,ymax;
      box.getBounds(xmin,ymin,xmax,ymax);

      aWidths[index] = float(xmax - xmin);
      aWidth = SbMaximum(aWidth,aWidths[index]);
      if(!index) {
        aAscent = float(text->getAscent()); 
        aDescent = float(text->getDescent()); 
        aPointSize = float(text->size.getValue());
      }
    }
  }

  if(number<=0) {
    aHeight = 0;
  } else {
    float hline = aDescent + YOFFSET + aAscent;
    aHeight = aAscent + hline * (number - 1) + aDescent;
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoTextRegion::matchViewport(
 float aWidth
,float aHeight
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if((aWidth<=0)||(aHeight<=0)) return;

  short marginx = horizontalMargin.getValue();
  short marginy = verticalMargin.getValue();
  if(2*marginy>=aHeight) return; //Empty area.
  if(2*marginx>=aWidth) return; //Empty area.

  SoCamera* soCamera = (SoCamera*)camera.getValue();
  //SbBool flag = soCamera->enableNotify(FALSE);
  soCamera->aspectRatio.setValue(aWidth/aHeight);
  soCamera->position.setValue(0,0,4);
  if(soCamera->isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
    SoOrthographicCamera* soOrthoCamera = (SoOrthographicCamera*)soCamera;
    soOrthoCamera->height.setValue(aHeight);
  }
  //soCamera->enableNotify(flag);

  // Get text size with current point size :
  float winside,hinside;
  float currentCharSize,ascent,descent;
  std::vector<float> widths;
  getInsideSize(winside,hinside,currentCharSize,ascent,descent,widths);
  if((winside<=0)||(hinside<=0)||(currentCharSize<=0)) return;

  // Guess new point size in order to have all text visible.
  // The below assumes a linear relationship between charSize
  // and text size.
  float avp = aWidth/aHeight;
  float atx = winside/hinside;
  float charSize = 0;
  if(avp>=atx) {
    //Assume a linear relationship between point size and height !
    float wantedHeight = aHeight-2*marginy;
    charSize = currentCharSize * wantedHeight/hinside;
  } else {
    //Assume a linear relationship between point size and width !
    float wantedWidth = aWidth-2*marginx;
    charSize = currentCharSize * wantedWidth/winside;
  }
  charSize *= 2; //To start with an exceeding size. 
  if(charSize<=0) return;

  SoSeparator* soLines = (SoSeparator*)lines.getValue();
  int number = soLines->getNumChildren();
  //flag = soLines->enableNotify(FALSE);

  // Loop down point size up that text fits in viewport :
  while(1) {
    setCharSize(charSize);
    float dummy;
    getInsideSize(winside,hinside,dummy,ascent,descent,widths);
    if( (winside<(aWidth - 2 * marginx)) &&
        (hinside<(aHeight - 2 * marginy)) ) break;
    charSize -= 1;
    if(charSize<=0) break;
  }

  if(charSize>0) {
    float ulx = -aWidth*0.5F+marginx;
    float uly =  aHeight*0.5F-marginy;
    // To center text in the viewport :
    float xoffset = (aWidth - 2 * marginx - winside)*0.5F;
    // Attach to center along y :
    float yoffset = (aHeight - 2 * marginy - hinside)*0.5F;

    float hline = descent + YOFFSET + ascent;
    float ty = uly - yoffset - ascent ;
    for(int index=0;index<number;index++) {
      SoSeparator* line = (SoSeparator*)soLines->getChild(index);
      SoTranslation* trans = (SoTranslation*)line->getChild(CHILD_TRANS);

      float tx = ulx + xoffset;
      if(horizontalJustification.getValue()==LEFT) {
      } else if(horizontalJustification.getValue()==RIGHT) {
        tx += winside - widths[index];
      } else if(horizontalJustification.getValue()==CENTER) {
        tx += (winside - widths[index])*0.5F;
      }
      trans->translation.setValue(SbVec3f(tx,ty,0));

      ty -= hline;
    }
  } else {
    //SoDebugError::post("SoTextRegion::doAction",
      //                 "can't fit text into viewport");
  }

  //soLines->enableNotify(flag);
}
//////////////////////////////////////////////////////////////////////////////
void SoTextRegion::generateAlternateRep(SoAction*) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(getAlternateRep());
}
//////////////////////////////////////////////////////////////////////////////
void SoTextRegion::clearAlternateRep(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(NULL);
}
