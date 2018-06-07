// this :
#include <HEPVis/nodekits/SoLegendRegion.h>

#include <Inventor/SbBox.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>
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
#include <HEPVis/SbStyles.h>
#include <HEPVis/SbColors.h>
#include <HEPVis/misc/SoTools.h>
#include <HEPVis/misc/SoStyleCache.h>
#include <HEPVis/nodes/SoStyle.h>
#include <HEPVis/nodes/SoTextTTF.h>
#include <HEPVis/nodes/SoTextHershey.h>
#include <HEPVis/nodes/SoViewportRegion.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

#include <vector>

#define CHILD_MODEL_SEP      0
#define CHILD_MODEL_TRANSF   0
#define CHILD_MODEL_MATERIAL 1

#define CHILD_TEXT_MATERIAL   1
#define CHILD_TEXT_TRANS      2
#define CHILD_TEXT_TEXT       3

// width area for the model in "number of space char" :
#define MARKER_SPACE_NUMBER  4
#define LINE_SPACE_NUMBER   20

#define YOFFSET 2 //FIXME : have an SoField.

SO_KIT_SOURCE(SoLegendRegion) 

void SoLegendRegion::initClass(){
  SO_KIT_INIT_CLASS(SoLegendRegion,SoRegion,"SoRegion");
}

SoLegendRegion::SoLegendRegion()
:fOldWidth(0)
,fOldHeight(0)
,fDefaultStyle(0)
,fOldText(0)
{
  SO_KIT_CONSTRUCTOR(SoLegendRegion);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0,FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(light,SoDirectionalLight,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lightModel,SoLightModel,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(camera,SoOrthographicCamera,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lines,SoSeparator,FALSE,topSeparator,\0,TRUE);

  // must not be put under topSeparator because of alternate reps.
  SO_KIT_ADD_CATALOG_ENTRY(styleSeparator,SoSeparator,FALSE,this,\0,TRUE);
  SO_KIT_ADD_CATALOG_LIST_ENTRY(styles,SoSeparator,FALSE,styleSeparator,\0,SoStyle,TRUE);
  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(text,(""));

  SO_NODE_ADD_FIELD(markerOnLeftBorder,(FALSE)); //used in OpenPAW.
  SO_NODE_ADD_FIELD(textInBlack,(FALSE)); //used in OpenPAW.

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  // About light, see notes in SoPlotterRegion constructor.
  SoDirectionalLight* soDirLight = (SoDirectionalLight*)light.getValue();
  //soDirLight->on.setValue(FALSE);
  soDirLight->on.setValue(TRUE);
  soDirLight->color.setValue(1,1,1);
  soDirLight->direction.setValue(SbVec3f(1,-1,-10));

  SoLightModel* soLightModel = (SoLightModel*)lightModel.getValue();
  //soLightModel->model.setValue(SoLightModel::BASE_COLOR);
  soLightModel->model.setValue(SoLightModel::PHONG);

  fDefaultStyle = new SoStyle;
  fDefaultStyle->ref();
  fDefaultStyle->setFromString("color black\nsmoothing FALSE\nhinting FALSE");

  SoLegendRegion::resetCamera();

  fOldText = new SoMFString;
}

SoLegendRegion::~SoLegendRegion(){
  delete fOldText;
  fDefaultStyle->unref();
}

void SoLegendRegion::clear(const SbString&) {
  text.setValue("");
  horizontalMargin.setValue(2);
  verticalMargin.setValue(2);

  SoGroup* group = getContainerNode("styles");
  group->removeAllChildren();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoLegendRegion::doAction(
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

  updateChildren();
}
#define ACTION(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  if(visible.getValue()==FALSE) return;\
  aClass::doAction(action);\
  aSuper::aMethod(action);\
}
ACTION(SoLegendRegion,SoRegion,SoCallbackAction,callback)
ACTION(SoLegendRegion,SoRegion,SoGLRenderAction,GLRender)
ACTION(SoLegendRegion,SoRegion,SoGetBoundingBoxAction,getBoundingBox)
//ACTION(SoLegendRegion,SoRegion,SoGetMatrixAction,getMatrix)
ACTION(SoLegendRegion,SoRegion,SoHandleEventAction,handleEvent)
ACTION(SoLegendRegion,SoRegion,SoRayPickAction,rayPick)
ACTION(SoLegendRegion,SoRegion,SoPickAction,pick)

#define ACTION1(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::updateChildren();\
  aSuper::aMethod(action);\
}
ACTION1(SoLegendRegion,SoRegion,SoGetMatrixAction,getMatrix)


#define ACTION2(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  /*if(visible.getValue()==FALSE) return;*/\
  /* There is no SoViewportRegionElement on search and write action.*/\
  /*aClass::updateChildren();*/\
  aSuper::aMethod(action);\
}
ACTION2(SoLegendRegion,SoRegion,SoSearchAction,search)
//ACTION2(SoLegendRegion,SoRegion,SoWriteAction,write)
void SoLegendRegion::write(SoWriteAction* aAction) {
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

void SoLegendRegion::updateChildren(){
  if(text!=(*fOldText)) {
    *fOldText = text;
    fUpdate = TRUE;
  }

  if(fUpdate==FALSE) return;

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoLegendRegion::doAction","%lu : update",(unsigned long)this);
  }

  SoSeparator* soLines = (SoSeparator*)lines.getValue();
  soLines->removeAllChildren();
  int number = text.getNum();

  for(int index=0;index<number;index++) {
    SoStyle* soStyle = getStyle(index);
    SbString encoding = soStyle->encoding.getValue();
    SbString fontName = soStyle->fontName.getValue();
    SbBool smoothing = soStyle->smoothing.getValue();
    SbBool hinting = soStyle->hinting.getValue();
    SbColor color = soStyle->color.getValue();
    float transparency = soStyle->transparency.getValue();
    SbString modeling = soStyle->modeling.getValue();

    SoSeparator* line = new SoSeparator; 
    soLines->addChild(line);

    // Graphic part :
    //CHILD_MODEL_SEP
   {SoSeparator* sep = new SoSeparator;
    line->addChild(sep);

    //CHILD_MODEL_TRANSF
    sep->addChild(new SoTransform);

    // Model part :
    //CHILD_MODEL_MATERIAL
    sep->addChild(fStyleCache->getMaterial(color,transparency));

    if(modeling==SbModeling_none) {
    } else if(modeling==SbModeling_lines) {
      unsigned short linePattern = soStyle->linePattern.getValue();
      int lineWidth = soStyle->lineWidth.getValue();
      sep->addChild(fStyleCache->getLineStyle(linePattern,lineWidth));
      SbVec3f pts[2];
      pts[0].setValue(-0.5F,0,0);
      pts[1].setValue( 0.5F,0,0);
      SoTools::addLinesToNode(sep,2,pts);
    } else { //markers : default
      SbMarkerStyle markerStyle = 
        (SbMarkerStyle)soStyle->markerStyle.getValue();
      int32_t markerSize = soStyle->markerSize.getValue();
      SbVec3f pts[1];
      pts[0].setValue(0,0,0);
      SoTools::addPointsToNode(sep,1,pts,markerStyle,markerSize);
    }

    } //CHILD_MODEL_SEP

    // Text part :
    //CHILD_TEXT_MATERIAL
    if(fontName==SbFont_Hershey) {
      SoGroup* group = new SoGroup;

      group->addChild(fStyleCache->getLineStyle
        (soStyle->linePattern.getValue(),
         (float)soStyle->lineWidth.getValue()));
      //group->addChild(fStyleCache->getNormalBindingOverall());
      //group->addChild(fStyleCache->getNormalZ());
      if(textInBlack.getValue()==TRUE) {
        group->addChild(fStyleCache->getMaterial(SbColor_black,0));
      } else {
        group->addChild(fStyleCache->getMaterial(color,transparency));
      }

      line->addChild(group);
    } else {
      if(textInBlack.getValue()==TRUE) {
        line->addChild(fStyleCache->getMaterial(SbColor_black,0));
      } else {
        line->addChild(fStyleCache->getMaterial(color,transparency));
      }
    }

    //CHILD_TEXT_TRANS
    line->addChild(new SoTranslation);

    //CHILD_TEXT_TEXT
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


  float wvp = float(fOldWidth);
  float hvp =  float(fOldHeight);

  matchViewport(wvp,hvp);

  fUpdate = FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SoStyle* SoLegendRegion::getStyle(int aIndex) {
  SoGroup* group = getContainerNode("styles");
  if(!group) return 0;
  if(aIndex<0) return 0;
  int number = group->getNumChildren();
  if(aIndex>=number) {
    for(int index=number;index<=aIndex;index++) {
      setPart(SbStringPrintF(32,"styles[%d]",index),fDefaultStyle->copy());
    }
  }
  return (SoStyle*)group->getChild(aIndex);
}

SoSeparator* SoLegendRegion::getTopSeparator() const {
  return (SoSeparator*)topSeparator.getValue();
}

SoCamera* SoLegendRegion::getCamera() const {
  return (SoCamera*)camera.getValue();
}

void SoLegendRegion::resetCamera() {
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

void SoLegendRegion::setTextCharSize(float aCharSize) {
  SoSeparator* soLines = (SoSeparator*)lines.getValue();
  int number = soLines->getNumChildren();
  for(int index=0;index<number;index++) {
    SoSeparator* line = (SoSeparator*)soLines->getChild(index);
   {SoNode* node = line->getChild(CHILD_TEXT_TEXT);
    if(node->isOfType(SoTextHershey::getClassTypeId())==TRUE) {
      ((SoTextHershey*)node)->height.setValue(aCharSize);
    } else {
      ((SoTextTTF*)node)->size.setValue(SbFROUND(aCharSize));
    }}
  }
}

void SoLegendRegion::getInsideSize(
 float& aWidth
,float& aHeight
,float& aPointSize
,float& aAscent
,float& aDescent
,std::vector<float>& aWidths
,float& aWidthModel
){
  SoSeparator* soLines = (SoSeparator*)lines.getValue();
  int number = soLines->getNumChildren();

  aPointSize = 0;
  aAscent = 0;
  aDescent = 0;
  aWidths.resize(number);
  aWidthModel = 0;

  float wmx = 0;
  float wmodelmx = 0;

  for(int index=0;index<number;index++) {
    SoSeparator* line = (SoSeparator*)soLines->getChild(index);
    SoStyle* soStyle = getStyle(index);
    SbString modeling = soStyle->modeling.getValue();

    SoNode* node = line->getChild(CHILD_TEXT_TEXT);
    if(node->isOfType(SoTextHershey::getClassTypeId())==TRUE) {

      SoTextHershey* soText = (SoTextHershey*)node;
      SbBox3f box;
      soText->getTextBounds(box);
      float xmin,ymin,zmin,xmax,ymax,zmax;
      box.getBounds(xmin,ymin,zmin,xmax,ymax,zmax);
      wmx = SbMaximum(wmx,float(xmax - xmin));
      aWidths[index] = float(xmax - xmin);

      if(!index) {
        aAscent = soText->getAscent(); 
        aDescent = soText->getDescent(); 
        aPointSize = soText->height.getValue();
      }

      if(modeling==SbModeling_none) {

      } else if(modeling==SbModeling_lines) {

        SbString* line = SbStringCreateEmpty(LINE_SPACE_NUMBER);
        SbBox3f box;
        soText->getTextBounds(1,line,box);
        float xmin,ymin,zmin,xmax,ymax,zmax;
        box.getBounds(xmin,ymin,zmin,xmax,ymax,zmax);
        wmodelmx = SbMaximum(wmodelmx,float(xmax - xmin));
        delete line;

      } else { //markers : default

        SbString* line = SbStringCreateEmpty(MARKER_SPACE_NUMBER);
        SbBox3f box;
        soText->getTextBounds(1,line,box);
        float xmin,ymin,zmin,xmax,ymax,zmax;
        box.getBounds(xmin,ymin,zmin,xmax,ymax,zmax);
        wmodelmx = SbMaximum(wmodelmx,float(xmax - xmin));
        delete line;

      }

       
    } else {
      SoTextTTF* text = (SoTextTTF*)node;

      SbBox2s box;
      text->getTextBounds(box);
      short xmin,ymin,xmax,ymax;
      box.getBounds(xmin,ymin,xmax,ymax);
      wmx = SbMaximum(wmx,float(xmax - xmin));
      aWidths[index] = float(xmax - xmin);
      if(!index) {
        aAscent = text->getAscent(); 
        aDescent = text->getDescent(); 
        aPointSize = text->size.getValue();
      }

      if(modeling==SbModeling_none) {

      } else if(modeling==SbModeling_lines) {

        SbString* line = SbStringCreateEmpty(LINE_SPACE_NUMBER);
        SbBox2s box;
        SbTTF* sbTextTTF = text->getLatin(); 
        if(!sbTextTTF || (sbTextTTF->getBounds(1,line,"",box)==FALSE)) {
        } else {
          short xmin,ymin,xmax,ymax;
          box.getBounds(xmin,ymin,xmax,ymax);
          wmodelmx = SbMaximum(wmodelmx,float(xmax - xmin));
        }
        delete line;

      } else { //markers : default

        SbString* line = SbStringCreateEmpty(MARKER_SPACE_NUMBER);
        SbBox2s box;
        SbTTF* sbTextTTF = text->getLatin(); 
        if(!sbTextTTF || (sbTextTTF->getBounds(1,line,"",box)==FALSE)) {
        } else {
          short xmin,ymin,xmax,ymax;
          box.getBounds(xmin,ymin,xmax,ymax);
          wmodelmx = SbMaximum(wmodelmx,float(xmax - xmin));
        }
        delete line;

      }

    }
  }
  if(number<=0) {
    aWidth = 0;
    aWidthModel = 0;
  } else {
    aWidth = wmodelmx + wmx;
    aWidthModel = wmodelmx;
  }
  if(number<=0) {
    aHeight = 0;
  } else {
    float hline = aDescent + YOFFSET + aAscent;
    aHeight = aAscent + hline * (number - 1) + aDescent;
  }
}

void SoLegendRegion::matchViewport(float aWidth,float aHeight) {
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

  // Get text size with current point size.
  float winside,hinside;
  float currentCharSize,ascent,descent;
  std::vector<float> widths;
  float widthModel;
  getInsideSize(winside,hinside,
                currentCharSize,ascent,descent,
                widths,widthModel);
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
    setTextCharSize(charSize);
    float dummy;
    getInsideSize(winside,hinside,
                  dummy,ascent,descent,
                  widths,widthModel);
    if( (winside<(aWidth - 2 * marginx)) &&
        (hinside<(aHeight - 2 * marginy)) ) break;
    charSize--;
    if(charSize<=0) break;
  }

  if(charSize>0) {
    // Reposition markers and texts :
    float ulx = -aWidth*0.5F+marginx;
    float uly =  aHeight*0.5F-marginy;
    // To center text in the viewport :
    float xoffset = (aWidth - 2 * marginx - winside)*0.5F;
    float yoffset = (aHeight - 2 * marginy - hinside)*0.5F;
    float hline = descent + YOFFSET + ascent;
    float ty = uly - yoffset - ascent;
    for(int index=0;index<number;index++) {
      SoSeparator* line = (SoSeparator*)soLines->getChild(index);
      SoStyle* soStyle = getStyle(index);
      SbString modeling = soStyle->modeling.getValue();
  
      if(modeling==SbModeling_none) {

        float tx = ulx + xoffset;

       {SoSeparator* sep = (SoSeparator*)line->getChild(CHILD_MODEL_SEP);
        SoTransform* transf = 
          (SoTransform*)sep->getChild(CHILD_MODEL_TRANSF);
        transf->translation.setValue(SbVec3f(0,0,0));}

       {SoTranslation* trans = 
          (SoTranslation*)line->getChild(CHILD_TEXT_TRANS);
        trans->translation.setValue(SbVec3f(tx,ty,0));}

      } else if(modeling==SbModeling_lines) {

        //widthModel is the area width for the model (line or marker).
        //We draw a line of widthModel/2 length.

        float tx = ulx + xoffset;

       {SoSeparator* sep = (SoSeparator*)line->getChild(CHILD_MODEL_SEP);
        SoTransform* transf = 
          (SoTransform*)sep->getChild(CHILD_MODEL_TRANSF);
        transf->translation.setValue
          (SbVec3f(tx+widthModel/2,ty+ascent/2,0));
        transf->scaleFactor.setValue(SbVec3f(widthModel*0.75F,1,1));
        }

       {SoTranslation* trans = 
          (SoTranslation*)line->getChild(CHILD_TEXT_TRANS);
        trans->translation.setValue(SbVec3f(tx+widthModel,ty,0));}

      } else { //markers : default

        SbBool onBorder = markerOnLeftBorder.getValue();
        float tx = onBorder?ulx:(ulx + xoffset);

       {SoSeparator* sep = (SoSeparator*)line->getChild(CHILD_MODEL_SEP);
        SoTransform* transf = 
          (SoTransform*)sep->getChild(CHILD_MODEL_TRANSF);
        transf->translation.setValue
          (SbVec3f(onBorder?tx:(tx+widthModel/2),ty+ascent/2,0));}

       {SoTranslation* trans = 
          (SoTranslation*)line->getChild(CHILD_TEXT_TRANS);
        trans->translation.setValue
          (SbVec3f(onBorder?((tx+widthModel/2)):(tx+widthModel),ty,0));}

      }

      ty -= hline;
    }
  } else {
    //SoDebugError::post("SoLegendRegion::matchViewport",
      //                 "can't fit text into viewport");
  }

  //soLines->enableNotify(flag);
}

void SoLegendRegion::generateAlternateRep(SoAction*) {
  alternateRep.setValue(getAlternateRep());
}

void SoLegendRegion::clearAlternateRep() {
  alternateRep.setValue(NULL);
}
