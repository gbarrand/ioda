// this :
#include <HEPVis/nodekits/SoInfosRegion.h>

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
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoDrawStyle.h>
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
#include <HEPVis/SbTTF.h>
#include <HEPVis/misc/SoTools.h>
#include <HEPVis/misc/SoStyleCache.h>
#include <HEPVis/nodes/SoLineStyle.h>
#include <HEPVis/nodes/SoTextStyle.h>
#include <HEPVis/nodes/SoTextHershey.h>
#include <HEPVis/nodes/SoTextTTF.h>
#include <HEPVis/nodes/SoViewportRegion.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

#include <vector>

#define CHILD_TITLE_MATERIAL 0
#define CHILD_TITLE_TRANS    1
#define CHILD_TITLE_TEXT     2

#define CHILD_TITLE_LINE_MATERIAL    3
#define CHILD_TITLE_LINE_DRAWSTYLE   4
#define CHILD_TITLE_LINE_COORDINATE3 5
#define CHILD_TITLE_LINE_LINESET     6

#define CHILD_MATERIAL 0
#define CHILD_TRANS_L  1
#define CHILD_TEXT_L   2
#define CHILD_TRANS_R  3
#define CHILD_TEXT_R   4

#define YOFFSET 2 //FIXME : have an SoField.

SO_KIT_SOURCE(SoInfosRegion) 
//////////////////////////////////////////////////////////////////////////////
void SoInfosRegion::initClass (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoInfosRegion,SoRegion,"SoRegion");
}
//////////////////////////////////////////////////////////////////////////////
SoInfosRegion::SoInfosRegion(
)
:fOld_title("")
,fOld_text(0)
,fOld_inBetweenSpaces(0)
,fOldWidth(0)
,fOldHeight(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoInfosRegion);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0,FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(light,SoDirectionalLight,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lightModel,SoLightModel,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(camera,SoOrthographicCamera,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(titleSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lines,SoSeparator,FALSE,topSeparator,\0,TRUE);

  // Styles :
  // must not be put under topSeparator because of alternate reps.
  SO_KIT_ADD_CATALOG_ENTRY(styleSeparator,SoSeparator,FALSE,this,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lineStyle,SoLineStyle,FALSE,styleSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(textStyle,SoTextStyle,FALSE,styleSeparator,\0,TRUE);

  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SO_NODE_ADD_FIELD(title,(""));
  SO_NODE_ADD_FIELD(text,(""));
  SO_NODE_ADD_FIELD(inBetweenSpaces,(2));

  // About light, see notes in SoPlotterRegion constructor.
  SoDirectionalLight* soDirLight = (SoDirectionalLight*)light.getValue();
  //soDirLight->on.setValue(FALSE);
  soDirLight->on.setValue(TRUE);
  soDirLight->color.setValue(1,1,1);
  soDirLight->direction.setValue(SbVec3f(1,-1,-10));

  SoLightModel* soLightModel = (SoLightModel*)lightModel.getValue();
  //soLightModel->model.setValue(SoLightModel::BASE_COLOR);
  soLightModel->model.setValue(SoLightModel::PHONG);

  SoTextStyle* soStyle = (SoTextStyle*)textStyle.getValue();
  soStyle->setFromString
 ("color black\nfontName times\nencoding PAW\nsmoothing FALSE\nhinting FALSE");

  SoInfosRegion::resetCamera();

  fOld_text = new SoMFString;
}
//////////////////////////////////////////////////////////////////////////////
SoInfosRegion::~SoInfosRegion(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  delete fOld_text;
}
//////////////////////////////////////////////////////////////////////////////
void SoInfosRegion::clear(
 const SbString& //aWhat
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  title.setValue("");
  text.setValue("");
  horizontalMargin.setValue(2);
  verticalMargin.setValue(2);
  inBetweenSpaces.setValue(2);

  SoTextStyle* soStyle = (SoTextStyle*)textStyle.getValue();
  soStyle->setFromString
 ("color black\nfontName times\nencoding PAW\nsmoothing FALSE\nhinting FALSE");

  SoLineStyle* soLineStyle = (SoLineStyle*)lineStyle.getValue();
  soLineStyle->reset();

  //getViewportRegion()->reset();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoInfosRegion::doAction(
 SoAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ALTERNATEREP_DO_ACTION(aAction)

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

  updateChildren(); //public
}
#define ACTION(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  if(visible.getValue()==FALSE) return;\
  aClass::doAction(action);\
  aSuper::aMethod(action);\
}
ACTION(SoInfosRegion,SoRegion,SoCallbackAction,callback)
ACTION(SoInfosRegion,SoRegion,SoGLRenderAction,GLRender)
ACTION(SoInfosRegion,SoRegion,SoGetBoundingBoxAction,getBoundingBox)
//ACTION(SoInfosRegion,SoRegion,SoGetMatrixAction,getMatrix)
ACTION(SoInfosRegion,SoRegion,SoHandleEventAction,handleEvent)
ACTION(SoInfosRegion,SoRegion,SoRayPickAction,rayPick)
ACTION(SoInfosRegion,SoRegion,SoPickAction,pick)

#define ACTION1(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::updateChildren();\
  aSuper::aMethod(action);\
}
ACTION1(SoInfosRegion,SoRegion,SoGetMatrixAction,getMatrix)

#define ACTION2(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  /*if(visible.getValue()==FALSE) return;*/\
  /* There is no SoViewportRegionElement on search and write action.*/\
  /*aClass::updateChildren();*/\
  aSuper::aMethod(action);\
}
ACTION2(SoInfosRegion,SoRegion,SoSearchAction,search)
//ACTION2(SoInfosRegion,SoRegion,SoWriteAction,write)
void SoInfosRegion::write(SoWriteAction* aAction) {
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
SoLineStyle* SoInfosRegion::getLineStyle(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoLineStyle*)lineStyle.getValue();
}
//////////////////////////////////////////////////////////////////////////////
SoTextStyle* SoInfosRegion::getTextStyle(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoTextStyle*)textStyle.getValue();
}

void SoInfosRegion::updateChildren(){

  if(title.getValue()!=fOld_title) {
    fOld_title = title.getValue();
    fUpdate = TRUE;
  }

  if(inBetweenSpaces.getValue()!=fOld_inBetweenSpaces) {
    fOld_inBetweenSpaces = inBetweenSpaces.getValue();
    fUpdate = TRUE;
  }

  if(text!=(*fOld_text)) {
    *fOld_text = text;
    fUpdate = TRUE;
  }

  if(fUpdate==FALSE) return;

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoInfosRegion::updateChildren","%lu : update",(unsigned long)this);
  }

  SoSeparator* soTitleSeparator = (SoSeparator*)titleSeparator.getValue();
  soTitleSeparator->removeAllChildren();

  SoSeparator* soLines = (SoSeparator*)lines.getValue();
  soLines->removeAllChildren();

  SoTextStyle* soTextStyle = (SoTextStyle*)textStyle.getValue();
  SbString encoding = soTextStyle->encoding.getValue();
  SbString fontName = soTextStyle->fontName.getValue();
  SbBool smoothing = soTextStyle->smoothing.getValue();
  SbBool hinting = soTextStyle->hinting.getValue();
  SbColor text_color = soTextStyle->color.getValue();
  float text_transparency = soTextStyle->transparency.getValue();

 {// Title area :

  //CHILD_TITLE_MATERIAL
  if(fontName==SbFont_Hershey) {
    SoGroup* group = new SoGroup;
    group->addChild(fStyleCache->getLineStyle
      (soTextStyle->linePattern.getValue(),
       (float)soTextStyle->lineWidth.getValue()));
    //group->addChild(fStyleCache->getNormalBindingOverall());
    //group->addChild(fStyleCache->getNormalZ());
    group->addChild(fStyleCache->getMaterial(text_color,text_transparency));
    soTitleSeparator->addChild(group);
  } else {
    soTitleSeparator->addChild(
      fStyleCache->getMaterial(text_color,text_transparency));
  }

  //CHILD_TITLE_TRANS
  soTitleSeparator->addChild(new SoTranslation);

  //CHILD_TITLE_TEXT
  if(fontName==SbFont_Hershey) {
    SoTextHershey* soText = new SoTextHershey;
    soText->encoding.setValue(encoding);
    soText->string.setValue(title.getValue());
    soTitleSeparator->addChild(soText);
  } else {
    SoTextTTF* soText = new SoTextTTF;
    soText->fontName.setValue(fontName);
    soText->smoothing.setValue(smoothing);
    soText->hinting.setValue(hinting);
    soText->encoding.setValue(encoding);
    soText->string.setValue(title.getValue());
    soTitleSeparator->addChild(soText);
  }

  SoLineStyle* soLineStyle = (SoLineStyle*)lineStyle.getValue();
  SbColor color = soLineStyle->color.getValue();
  float transparency = soLineStyle->transparency.getValue();
  SbLinePattern pattern = soLineStyle->pattern.getValue();
  int width = soLineStyle->width.getValue();

  //CHILD_TITLE_LINE_MATERIAL
  soTitleSeparator->addChild(fStyleCache->getMaterial(color,transparency));
  //CHILD_TITLE_LINE_DRAWSTYLE
  soTitleSeparator->addChild(fStyleCache->getLineStyle(pattern,float(width)));
  //CHILD_TITLE_LINE_COORDINATE3
  soTitleSeparator->addChild(new SoCoordinate3);
  //CHILD_TITLE_LINE_LINESET
  soTitleSeparator->addChild(new SoLineSet);}

  int number = text.getNum();
  int linen = number/2;
  for(int index=0;index<linen;index++) {
    SoSeparator* line = new SoSeparator; 
    soLines->addChild(line);

    //CHILD_MATERIAL
    if(fontName==SbFont_Hershey) {
      SoGroup* group = new SoGroup;
      group->addChild(fStyleCache->getLineStyle
        (soTextStyle->linePattern.getValue(),
         (float)soTextStyle->lineWidth.getValue()));
      //group->addChild(fStyleCache->getNormalBindingOverall());
      //group->addChild(fStyleCache->getNormalZ());
      group->addChild(fStyleCache->getMaterial(text_color,text_transparency));
      line->addChild(group);
    } else {
      line->addChild(fStyleCache->getMaterial(text_color,text_transparency));
    }

    // We expect a C string with two parts separated with a \n.
    SbString name(text[2*index]);
    SbString value(text[2*index+1]);

    //CHILD_TRANS_L
    line->addChild(new SoTranslation);

    //CHILD_TEXT_L
    if(fontName==SbFont_Hershey) {
      SoTextHershey* textL = new SoTextHershey;
      textL->encoding.setValue(encoding);
      textL->string.setValue(name);
      line->addChild(textL);
    } else {
      SoTextTTF* textL = new SoTextTTF;
      textL->fontName.setValue(fontName);
      textL->smoothing.setValue(smoothing);
      textL->hinting.setValue(hinting);
      textL->encoding.setValue(encoding);
      textL->string.setValue(name);
      line->addChild(textL);
    }

    //CHILD_TRANS_R
    line->addChild(new SoTranslation);

    //CHILD_TEXT_R
    if(fontName==SbFont_Hershey) {
      SoTextHershey* textR = new SoTextHershey;
      textR->encoding.setValue(encoding);
      textR->string.setValue(value);
      line->addChild(textR);
    } else {
      SoTextTTF* textR = new SoTextTTF;
      textR->fontName.setValue(fontName);
      textR->smoothing.setValue(smoothing);
      textR->hinting.setValue(hinting);
      textR->encoding.setValue(encoding);
      textR->string.setValue(value);
      line->addChild(textR);
    }
  }


  float wvp = float(fOldWidth);
  float hvp =  float(fOldHeight);

  matchViewport(wvp,hvp);

  fUpdate = FALSE;
}
//////////////////////////////////////////////////////////////////////////////
SoSeparator* SoInfosRegion::getTopSeparator(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoSeparator*)topSeparator.getValue();
}
//////////////////////////////////////////////////////////////////////////////
SoCamera* SoInfosRegion::getCamera(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return (SoCamera*)camera.getValue();
}
//////////////////////////////////////////////////////////////////////////////
void SoInfosRegion::resetCamera(
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
void SoInfosRegion::setCharSize(
 float aCharSize
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSeparator* soTitleSeparator = (SoSeparator*)titleSeparator.getValue();
 {SoNode* node = soTitleSeparator->getChild(CHILD_TITLE_TEXT);
  if(node->isOfType(SoTextHershey::getClassTypeId())==TRUE) {
    ((SoTextHershey*)node)->height.setValue(aCharSize);
  } else {
    ((SoTextTTF*)node)->size.setValue(SbFROUND(aCharSize));
  }}
 
  SoSeparator* soLines = (SoSeparator*)lines.getValue();
  int number = soLines->getNumChildren();

  for(int index=0;index<number;index++) {
    SoSeparator* line = (SoSeparator*)soLines->getChild(index);

   {SoNode* node = line->getChild(CHILD_TEXT_L);
    if(node->isOfType(SoTextHershey::getClassTypeId())==TRUE) {
      ((SoTextHershey*)node)->height.setValue(aCharSize);
    } else {
      ((SoTextTTF*)node)->size.setValue(SbFROUND(aCharSize));
    }}

   {SoNode* node = line->getChild(CHILD_TEXT_R);
    if(node->isOfType(SoTextHershey::getClassTypeId())==TRUE) {
      ((SoTextHershey*)node)->height.setValue(aCharSize);
    } else {
      ((SoTextTTF*)node)->size.setValue(SbFROUND(aCharSize));
    }}

  }
}
//////////////////////////////////////////////////////////////////////////////
void SoInfosRegion::getInsideSize(
 float& aWidth
,float& aHeight
,float& aCharSize
,float& aAscent
,float& aDescent
,float& aWtitle
,float& aLmax
,float& aRmax
,std::vector<float>& aRwidths
,float& aWidthSpaces
)
//////////////////////////////////////////////////////////////////////////////
// Arrange to let a given amount of spaces between the
// max width of left part and the max width of the right part
// (which is right justified).
//
//    xxx   |   |   xxxxxx
//    xxxxxx|   |      xxx
//    xxx   |   |xxxxxxxxx
//          -----
//            n spaces 
//
// The below assumes the same font in lines.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  aWidthSpaces = 0;
  float hline = 0;
 {SoSeparator* soTitleSeparator = (SoSeparator*)titleSeparator.getValue();
  SoNode* node = soTitleSeparator->getChild(CHILD_TITLE_TEXT);
  if(node->isOfType(SoTextHershey::getClassTypeId())==TRUE) {
    SoTextHershey* soText = (SoTextHershey*)node;
    SbBox3f box;
    soText->getTextBounds(box);
    float xmin,ymin,zmin,xmax,ymax,zmax;
    box.getBounds(xmin,ymin,zmin,xmax,ymax,zmax);
    aWtitle = xmax - xmin;
    aAscent = soText->getAscent(); 
    aDescent = soText->getDescent(); 
    aCharSize = soText->height.getValue();

   {SbString* line = SbStringCreateEmpty(inBetweenSpaces.getValue());
    SbBox3f box;
    soText->getTextBounds(1,line,box);
    float xmin,ymin,zmin,xmax,ymax,zmax;
    box.getBounds(xmin,ymin,zmin,xmax,ymax,zmax);
    aWidthSpaces = xmax - xmin;
    delete line;}

  } else {
    SbBox2s box;
    SoTextTTF* soText = (SoTextTTF*)node;
    soText->getTextBounds(box);
    short xmin,ymin,xmax,ymax;
    box.getBounds(xmin,ymin,xmax,ymax);
    aWtitle = float(xmax - xmin);
    aAscent = float(soText->getAscent()); 
    aDescent = float(soText->getDescent()); 
    aCharSize = float(soText->size.getValue());

   {SbString* line = SbStringCreateEmpty(inBetweenSpaces.getValue());
    SbBox2s box;
    SbTTF* sbTextTTF = soText->getLatin(); 
    if(!sbTextTTF || (sbTextTTF->getBounds(1,line,"",box)==FALSE)) {
    } else {
      short xmin,ymin,xmax,ymax;
      box.getBounds(xmin,ymin,xmax,ymax);
      aWidthSpaces = float(xmax - xmin);
    }
    delete line;}}
  }
  hline = aDescent + YOFFSET + aAscent;

  aLmax = 0;
  aRmax = 0;
  SoSeparator* soLines = (SoSeparator*)lines.getValue();
  int number = soLines->getNumChildren();
  aRwidths.resize(number);
  // Get max size of left and right part :
  for(int index=0;index<number;index++) {
    SoSeparator* line = (SoSeparator*)soLines->getChild(index);

   {SoNode* node = line->getChild(CHILD_TEXT_L);
    if(node->isOfType(SoTextHershey::getClassTypeId())==TRUE) {
      SbBox3f box;
      ((SoTextHershey*)node)->getTextBounds(box);
      float xmin,ymin,zmin,xmax,ymax,zmax;
      box.getBounds(xmin,ymin,zmin,xmax,ymax,zmax);
      aLmax = SbMaximum(aLmax,xmax - xmin);
    } else {
      SbBox2s box;
      ((SoTextTTF*)node)->getTextBounds(box);
      short xmin,ymin,xmax,ymax;
      box.getBounds(xmin,ymin,xmax,ymax);
      aLmax = SbMaximum(aLmax,xmax - xmin);
    }}

   {SoNode* node = line->getChild(CHILD_TEXT_R);
    if(node->isOfType(SoTextHershey::getClassTypeId())==TRUE) {
      SbBox3f box;
      ((SoTextHershey*)node)->getTextBounds(box);
      float xmin,ymin,zmin,xmax,ymax,zmax;
      box.getBounds(xmin,ymin,zmin,xmax,ymax,zmax);
      aRwidths[index] = xmax - xmin;
      aRmax = SbMaximum(aRmax,xmax - xmin);
    } else {
      SbBox2s box;
      ((SoTextTTF*)node)->getTextBounds(box);
      short xmin,ymin,xmax,ymax;
      box.getBounds(xmin,ymin,xmax,ymax);
      aRwidths[index] = float(xmax - xmin);
      aRmax = SbMaximum(aRmax,xmax - xmin);
    }}

  }
  if(number<=0) {
    aWidth = 0;
  } else {
    aWidth = aLmax + aWidthSpaces + aRmax;
  }
  if(title.getValue()!="") {
    aWidth = SbMaximum(aWidth,aWtitle);
    aHeight = aAscent + hline * (1 + number - 1) + aDescent;
  } else {
    if(number<=0) {
      aHeight = 0;
    } else {
      aHeight = aAscent + hline * (number - 1) + aDescent;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoInfosRegion::getContentSize(
 float& aWidth
,float& aHeight
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
 {SoSeparator* soLines = (SoSeparator*)lines.getValue();
  int number = soLines->getNumChildren();
  if(int(text.getNum()/2)!=number) {
    SoDebugError::post("SoInfosRegion::getContentSize",
      "text / scene graph line number mismatch. Need to call updateChildren.");
  }}

  float winside,hinside;
  float size,ascent,descent;
  float wtitle,lmx,rmx;
  std::vector<float> rwidths;
  float widthSpaces;
  getInsideSize(winside,hinside,
                size,ascent,descent,
                wtitle,lmx,rmx,rwidths,widthSpaces);
  aWidth = winside;
  aHeight = hinside;
}
//////////////////////////////////////////////////////////////////////////////
void SoInfosRegion::matchViewport(
 float aWidth
,float aHeight
)
//////////////////////////////////////////////////////////////////////////////
// Adapth char height of texts so that the content fits within the viewport.
// When done reposition all texts.
//  aWidth, aHeight in pixels.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if((aWidth<=0)||(aHeight<=0)) return;

  short marginx = horizontalMargin.getValue();
  short marginy = verticalMargin.getValue();
  if(2*marginy>=aHeight) return; //Empty area.
  if(2*marginx>=aWidth) return; //Empty area.

  SoCamera* soCamera = (SoCamera*)camera.getValue();
  //Disable since may be called within doAction.
  //SbBool flag = soCamera->enableNotify(FALSE);
  soCamera->aspectRatio.setValue(aWidth/aHeight);
  soCamera->position.setValue(0,0,4);
  if(soCamera->isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
    SoOrthographicCamera* soOrthoCamera = (SoOrthographicCamera*)soCamera;
    soOrthoCamera->height.setValue(aHeight);
  }
  //soCamera->enableNotify(flag);

  SoSeparator* soLines = (SoSeparator*)lines.getValue();
  int number = soLines->getNumChildren();
  if(int(text.getNum()/2)!=number) {
    SoDebugError::post("SoInfosRegion::mapViewport",
      "text / scene graph line number mismatch. Need to call createChildren.");
  }

  // Get text size with current point size.
  float winside,hinside;
  float currentCharSize,ascent,descent;
  float wtitle,lmx,rmx;
  std::vector<float> rwidths;
  float widthSpaces;

  getInsideSize(winside,hinside,
                currentCharSize,ascent,descent,
                wtitle,lmx,rmx,rwidths,widthSpaces);
  if((winside<=0)||(hinside<=0)||(currentCharSize<=0)) return;

  // Guess new char size in order to have all text visible.
  // The below assumes a linear relationship between char size
  // and text size.
  // We start from a slightly exceeding size.
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

  //flag = soLines->enableNotify(FALSE);

  while(1) {
    setCharSize(charSize);
    float dummy;
    getInsideSize(winside,hinside,
                  dummy,ascent,descent,
                  wtitle,lmx,rmx,rwidths,widthSpaces);
    if( (winside<(aWidth - 2 * marginx)) &&
        (hinside<(aHeight - 2 * marginy)) ) break;
    charSize--;
    if(charSize<=0) break;
  }

  if(charSize>0) {
    // Reposition texts :
    float ulx = -aWidth*0.5F+marginx;
    float uly =  aHeight*0.5F-marginy;
    // To center text in the viewport :
    float xoffset = (aWidth - 2 * marginx - winside)*0.5F;
    // Attach to center along y :
    float yoffset = (aHeight - 2 * marginy - hinside)*0.5F;

    float hline = descent + YOFFSET + ascent;

    float ty = uly - ascent;
    if(title.getValue()!="") {
      // Title is attached on top along y.
      SoSeparator* soTitleSeparator = (SoSeparator*)titleSeparator.getValue();
      SoTranslation* trans = 
        (SoTranslation*)soTitleSeparator->getChild(CHILD_TITLE_TRANS);
      float tx = ulx+xoffset + (winside - wtitle)/2;
      trans->translation.setValue(SbVec3f(tx,ty,0));

      SbVec3f points[2];
      points[0].setValue(-aWidth,-(descent+1),0);
      points[1].setValue( aWidth,-(descent+1),0);
      int32_t pointn = 2;
      SoCoordinate3* coordinate3 = 
      (SoCoordinate3*)soTitleSeparator->getChild(CHILD_TITLE_LINE_COORDINATE3);
      coordinate3->point.setValues(0,pointn,points);
      SoLineSet* lineSet = 
        (SoLineSet*)soTitleSeparator->getChild(CHILD_TITLE_LINE_LINESET);
      lineSet->numVertices.setValues(0,1,&pointn);

      ty -= hline;
    }
    ty += -yoffset; //Center the rest along y.

    for(int index=0;index<number;index++) {
      SoSeparator* line = (SoSeparator*)soLines->getChild(index);
  
     {SoTranslation* trans = (SoTranslation*)line->getChild(CHILD_TRANS_L);
      trans->translation.setValue(SbVec3f(ulx+xoffset,ty,0));}
  
      float tRx = lmx + widthSpaces + rmx - rwidths[index];
  
     {SoTranslation* trans = (SoTranslation*)line->getChild(CHILD_TRANS_R);
      trans->translation.setValue(SbVec3f(tRx,0,0));}

      ty -= hline;  
    }
  } else {
    //SoDebugError::post("SoInfosRegion::matchViewport",
      //                 "can't fit text into viewport");
  }

  //soLines->enableNotify(flag);
}
//////////////////////////////////////////////////////////////////////////////
void SoInfosRegion::generateAlternateRep(SoAction*) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(getAlternateRep());
}
//////////////////////////////////////////////////////////////////////////////
void SoInfosRegion::clearAlternateRep(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(NULL);
}
