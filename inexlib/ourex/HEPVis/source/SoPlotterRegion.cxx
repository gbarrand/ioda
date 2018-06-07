// this :
#include <HEPVis/nodekits/SoPlotterRegion.h>

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDirectionalLight.h>
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
#include <HEPVis/SbPlottableThings.h>
#include <HEPVis/SbStyle.h>
#include <HEPVis/SbMath.h>
#include <HEPVis/misc/SoTools.h>
#include <HEPVis/nodes/SoLineStyle.h>
#include <HEPVis/nodes/SoTextStyle.h>
#include <HEPVis/nodes/SoStyle.h>
#include <HEPVis/nodes/SoViewportRegion.h>
#include <HEPVis/nodekits/SoAxes2D.h>
#include <HEPVis/nodekits/SoAxes3D.h>
#include <HEPVis/nodekits/SoAxis.h>
#include <HEPVis/nodekits/SoPlotter.h>
#include <HEPVis/nodekits/SoTextRegion.h>
#include <HEPVis/nodekits/SoInfosRegion.h>
#include <HEPVis/nodekits/SoLegendRegion.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

SO_KIT_SOURCE(SoPlotterRegion) 

void SoPlotterRegion::initClass (){
  SO_KIT_INIT_CLASS(SoPlotterRegion,SoRegion,"SoRegion");
}

SoPlotterRegion::SoPlotterRegion()
:fOldWidth(0)
,fOldHeight(0)
,fOld_titleRegionVisible(FALSE)
,fOld_infosRegionVisible(FALSE)
,fOld_legendRegionVisible(FALSE)
,fOld_titleRegionOrigin(SbVec2f(0,0))
,fOld_titleRegionSize(SbVec2f(0,0))
,fOld_infosRegionOrigin(SbVec2f(0,0))
,fOld_infosRegionSize(SbVec2f(0,0))
,fOld_legendRegionOrigin(SbVec2f(0,0))
,fOld_legendRegionOriginUnit(AXIS) //Do not have PERCENT here.
,fOld_legendRegionSize(SbVec2f(0,0))
,fOld_infosRegionStyle("")
,fOld_plotterViewportMapping(0)
,fOld_infosRegionResizePolicy(0)
,fOld_legendAutomated(TRUE)
,fOldPlotter_height(0)
,fOldPlotter_depth(0)
,fOldPlotter_shape(0)
,fOldPlotter_title(FALSE)
,fOldPlotter_infos(0)
,fOldPlotter_legend(0)
,fOld_numberOfLegendRegions(-1)
{
  SO_KIT_CONSTRUCTOR(SoPlotterRegion);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(plotterSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(light,SoDirectionalLight,FALSE,plotterSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lightModel,SoLightModel,FALSE,plotterSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(cameraGroup,SoGroup,FALSE,plotterSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(plotter,SoPlotter,FALSE,plotterSeparator,\0,TRUE);

#define Sb_overlays "overlays"

  SO_KIT_ADD_CATALOG_LIST_ENTRY(overlays,SoSeparator,FALSE,plotterSeparator,\0,SoPlotter,TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(etcSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);

  // Do not put region under topSeparator because of alternateRep.
  SO_KIT_ADD_CATALOG_ENTRY(regionsSeparator,SoSeparator,FALSE,this,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(titleRegion,SoTextRegion,FALSE,regionsSeparator,\0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(infosRegion,SoInfosRegion,FALSE,regionsSeparator,\0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(legendRegion,SoLegendRegion,FALSE,regionsSeparator,\0, TRUE);

#define Sb_legendRegions "legendRegions"

  SO_KIT_ADD_CATALOG_LIST_ENTRY(legendRegions,SoSeparator,FALSE,regionsSeparator,\0,SoLegendRegion,TRUE);

  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SO_NODE_ADD_FIELD(titleRegionVisible,(FALSE));
  SO_NODE_ADD_FIELD(infosRegionVisible,(FALSE));
  SO_NODE_ADD_FIELD(infosRegionStyle,("PAW"));
  SO_NODE_ADD_FIELD(legendRegionVisible,(FALSE));
  SO_NODE_ADD_FIELD(legendAutomated,(TRUE));

  SO_NODE_ADD_FIELD(plotterViewportMapping,(ADJUST_VIEWPORT));
  SO_NODE_DEFINE_ENUM_VALUE(ViewportMapping,ADJUST_VIEWPORT);
  SO_NODE_DEFINE_ENUM_VALUE(ViewportMapping,LEAVE_ALONE);
  SO_NODE_SET_SF_ENUM_TYPE(plotterViewportMapping,ViewportMapping);

  SO_NODE_ADD_FIELD(infosRegionResizePolicy,(RESIZE_HEIGHT));
  SO_NODE_DEFINE_ENUM_VALUE(ResizePolicy,RESIZE_HEIGHT);
  SO_NODE_DEFINE_ENUM_VALUE(ResizePolicy,ASPECT_RATIO);
  SO_NODE_SET_SF_ENUM_TYPE(infosRegionResizePolicy,ResizePolicy);

  SO_NODE_ADD_FIELD(infosRegionAttachedToDataFrame,(FALSE));
  // PERCENT of SoPlotterRegion area (which is not 
  // the same than the page/viewer area).
  // Below origin is the position of the upper left corner 
  // of the titleRegion relative to the upper left corner
  // of the SoPlotterRegion.
  SO_NODE_ADD_FIELD(titleRegionOrigin,(SbVec2f(0.01F,0.01F)));
  SO_NODE_ADD_FIELD(titleRegionSize,(SbVec2f(0.3F,0.05F)));
  // Below origin is the position of the upper right corner 
  // of the infosRegion relative to the upper right corner
  // of the SoPlotterRegion. (x>0 goes to the left, y>0 goes down).
  SO_NODE_ADD_FIELD(infosRegionOrigin,(SbVec2f(0.01F,0.01F)));
  SO_NODE_ADD_FIELD(infosRegionSize,(SbVec2f(0.2F,0.16F)));

  SO_NODE_ADD_FIELD(legendRegionAttachedToInfosRegion,(TRUE));
  //FIXME : have attachment fields for the upper origin. See Motif XmForm.
  // Below origin is the position of the upper right corner 
  // of the legendRegion relative to the upper right corner
  // of the SoPlotterRegion.
  SO_NODE_ADD_FIELD(legendRegionOrigin,(SbVec2f(0.01F,0.01F)));
  SO_NODE_ADD_FIELD(legendRegionOriginUnit,(PERCENT));
  SO_NODE_ADD_FIELD(legendRegionSize,(SbVec2f(0.2F,0.16F)));
  //FIXME : have attachment fields for the upper origin. See Motif XmForm.

  SO_NODE_DEFINE_ENUM_VALUE(Unit,PERCENT);
  SO_NODE_DEFINE_ENUM_VALUE(Unit,AXIS);
  SO_NODE_SET_SF_ENUM_TYPE(legendRegionOriginUnit,Unit);

  // Default camera and light model setting for 2D :
  //FIXME : with NextStep viewer (SC21) 
  // ligh off + BASE_COLOR produces black filled areas !.
  SoDirectionalLight* soDirLight = (SoDirectionalLight*)light.getValue();
  //FIXME : soDirLight->on.setValue(FALSE);
  soDirLight->on.setValue(TRUE); //FIXME
  soDirLight->color.setValue(1,1,1);
  // Same as Coin<Gui>/So<Gui>Viewer :
  soDirLight->direction.setValue(SbVec3f(1,-1,-10));

  SoLightModel* soLightModel = (SoLightModel*)lightModel.getValue();
  //FIXME : with SGI-2.1.5.10 : light on + BASE_COLOR produces
  // black faceset !
  //soLightModel->model.setValue(SoLightModel::BASE_COLOR);
  soLightModel->model.setValue(SoLightModel::PHONG);

  // master plotter set camera.
  SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();

  // cameraGroup avoids to have the type of the camera
  // saved in the catalog list (and then let freedom
  // to change the type of camera).
  SoGroup* soCameraGroup = (SoGroup*)cameraGroup.getValue();
  SoOrthographicCamera* soCamera = new SoOrthographicCamera;
  soCameraGroup->addChild(soCamera);

  soCamera->viewportMapping.setValue(SoCamera::ADJUST_CAMERA);
  soCamera->aspectRatio.setValue(1);
  float height = soPlotter->height.getValue();
  soCamera->height.setValue(height);    
  soCamera->position.setValue(0,0,4*height);
  soCamera->orientation.setValue(SbRotation(SbVec3f(0,1,0),0));
  soCamera->nearDistance.setValue(1);
  soCamera->farDistance.setValue(100*height);
  soCamera->focalDistance.setValue(4*height);

  //verbose.setValue(TRUE);

/*((SoTextRegion*)titleRegion.getValue())->
    getViewportRegion()->shadow.setValue(SbVec2s(2,-2));
  ((SoInfosRegion*)infosRegion.getValue())->
    getViewportRegion()->shadow.setValue(SbVec2s(2,-2));
  ((SoLegendRegion*)legendRegion.getValue())->
    getViewportRegion()->shadow.setValue(SbVec2s(2,-2));*/

  fOldPlotter_infos = new SoMFString;
  fOldPlotter_legend = new SoMFString;
}

SoPlotterRegion::~SoPlotterRegion(){
  delete fOldPlotter_infos;
  delete fOldPlotter_legend;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static int sDoCounter = 0;
void SoPlotterRegion::doAction(SoAction* aAction){
  SO_ALTERNATEREP_DO_ACTION(aAction)

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoPlotterRegion::doAction","%lu (%d).",(unsigned long)this,sDoCounter++);
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
  //short vpx = vp.getViewportOriginPixels()[0];
  //short vpy = vp.getViewportOriginPixels()[1];
  short wvp = vp.getViewportSizePixels()[0];
  short hvp = vp.getViewportSizePixels()[1];

  //WARNING : we must not pass here with a SoGetMatrixAction since it has a vp size not corresponding
  //          to the viewport size that we get from SoGLRenderAction.

  if((wvp!=fOldWidth)||(hvp!=fOldHeight)) {
    fOldWidth = wvp;
    fOldHeight = hvp;
    fUpdate = TRUE;
    if(verbose.getValue()==TRUE) 
      SoDebugError::postInfo("SoPlotterRegion::updateChildren",
                             "update : viewport resized");
  }

  updateChildren();
}
#define ACTION(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  if(visible.getValue()==FALSE) return;\
  aClass::doAction(action);\
  aSuper::aMethod(action);\
}
ACTION(SoPlotterRegion,SoRegion,SoCallbackAction,callback)
ACTION(SoPlotterRegion,SoRegion,SoGLRenderAction,GLRender)
ACTION(SoPlotterRegion,SoRegion,SoGetBoundingBoxAction,getBoundingBox)
//ACTION(SoPlotterRegion,SoRegion,SoGetMatrixAction,getMatrix)
ACTION(SoPlotterRegion,SoRegion,SoHandleEventAction,handleEvent)
ACTION(SoPlotterRegion,SoRegion,SoRayPickAction,rayPick)
ACTION(SoPlotterRegion,SoRegion,SoPickAction,pick)

#define ACTION2(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::updateChildren();\
  aSuper::aMethod(action);\
}
ACTION2(SoPlotterRegion,SoRegion,SoSearchAction,search)
//ACTION2(SoPlotterRegion,SoRegion,SoWriteAction,write)
ACTION2(SoPlotterRegion,SoRegion,SoGetMatrixAction,getMatrix) //IMPORTANT.

void SoPlotterRegion::write(SoWriteAction* aAction) {
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    //NOTE : to avoid writing HEPVis SoSFEnum.
    //       But the below logic loose the fact
    //       that we had a SoPlotterRegion. 
    altRep->write(aAction);
  } else {
    SoPlotterRegion::updateChildren();
    SoBaseKit::write(aAction);
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define CHECK_IF(a__field,a__flag) \
  if(fOld_##a__field!=a__field.getValue()) {\
    fOld_##a__field = a__field.getValue();\
    a__flag = TRUE;\
    if(verbose.getValue()==TRUE)\
      SoDebugError::postInfo("SoPlotterRegion::updateChildren",\
                             "update : %s",#a__field);\
  }

void SoPlotterRegion::updateChildren(){
  float wvp = float(fOldWidth);
  float hvp =  float(fOldHeight);
 
  if(fUpdate==TRUE) {
    // Logic of SoTextTTF::setPointSize of HEPVis/v6r5p1.
    float fact = 0;
    int owin = 200; //pixels
    if(wvp<=hvp) {
      fact = float(wvp) / float(owin);
    } else {
      fact = float(hvp) / float(owin);
    }
    SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();
    soPlotter->ttfScale.setValue(fact);
   {SoGroup* group = getContainerNode(Sb_overlays);
    if(group) {
      int number = group->getNumChildren();
      for(int index=0;index<number;index++) {
        SoPlotter* soPlotter = (SoPlotter*)group->getChild(index);
        soPlotter->ttfScale.setValue(fact);
      }
    }}
  }

  // SoPlotterRegion SoFields :
  CHECK_IF(titleRegionVisible,fUpdate)
  CHECK_IF(infosRegionVisible,fUpdate)
  CHECK_IF(legendRegionVisible,fUpdate)
  CHECK_IF(titleRegionOrigin,fUpdate)

//Set by the updateTitleContent.
//Warning : it oscillates !
//CHECK_IF(titleRegionSize,fUpdate)

  CHECK_IF(infosRegionOrigin,fUpdate)

//Set by the updateInfosContent.
//Warning : it oscillates !
//CHECK_IF(infosRegionSize,fUpdate)

  CHECK_IF(legendRegionOrigin,fUpdate)
  CHECK_IF(legendRegionOriginUnit,fUpdate)

//Set by the updateLegendContent.
//Warning : it oscillates !
//CHECK_IF(legendRegionSize,fUpdate)

  CHECK_IF(infosRegionStyle,fUpdate)
  CHECK_IF(plotterViewportMapping,fUpdate)
  CHECK_IF(infosRegionResizePolicy,fUpdate)
  CHECK_IF(legendAutomated,fUpdate)

  // SoPlotter fields :
  SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();
  updateOneSoPlotter(soPlotter,wvp,hvp);
 {SoGroup* group = getContainerNode(Sb_overlays);
  if(group) {
    int number = group->getNumChildren();
    for(int index=0;index<number;index++) {
      SoPlotter* soPlotter = (SoPlotter*)group->getChild(index);
      updateOneSoPlotter(soPlotter,wvp,hvp);
    }
  }}

#define CHECK_PLOTTER_IF(a__field,a__flag) \
  if(fOldPlotter_##a__field!=soPlotter->a__field.getValue()) {\
    fOldPlotter_##a__field = soPlotter->a__field.getValue();\
    a__flag = TRUE;\
    if(verbose.getValue()==TRUE)\
      SoDebugError::postInfo("SoPlotterRegion::updateChildren",\
                             "update : plotter %s",#a__field);\
  }

  CHECK_PLOTTER_IF(height,fUpdate)
  CHECK_PLOTTER_IF(depth,fUpdate)
  CHECK_PLOTTER_IF(shape,fUpdate)
  CHECK_PLOTTER_IF(title,fUpdate)

  if(soPlotter->infos!=(*fOldPlotter_infos)) {
    *fOldPlotter_infos = soPlotter->infos;
    fUpdate = TRUE;
    if(verbose.getValue()==TRUE) 
      SoDebugError::postInfo("SoPlotterRegion::updateChildren",
                             "update : plotter infos");
  }
  if(soPlotter->legend!=(*fOldPlotter_legend)) {
    *fOldPlotter_legend = soPlotter->legend;
    fUpdate = TRUE;
    if(verbose.getValue()==TRUE) 
      SoDebugError::postInfo("SoPlotterRegion::updateChildren",
                             "update : plotter legend");
  }

 {int n = getNumberOfLegendRegions();
  if(n!=fOld_numberOfLegendRegions) {
    fOld_numberOfLegendRegions = n;
    fUpdate = TRUE;
    if(verbose.getValue()==TRUE) 
      SoDebugError::postInfo("SoPlotterRegion::updateChildren",
                             "update : number of legend regions");
  }}

  if(fUpdate==FALSE) return;

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoPlotterRegion::updateChildren","%lu : update",(unsigned long)this);
  }

  // Update the scene graph :

  // Update the camera :
  SoPlotterRegion::resetCamera();

  // Update the light :
  SoDirectionalLight* soDirLight = (SoDirectionalLight*)light.getValue();
  //SbBool flagDirLight = soDirLight->enableNotify(FALSE);
  SoLightModel* soLightModel = (SoLightModel*)lightModel.getValue();
  //SbBool flagLightModel = soLightModel->enableNotify(FALSE);

  if(soPlotter->shape.getValue()==SoPlotter::XYZ) {
    soDirLight->on.setValue(TRUE);
    soLightModel->model.setValue(SoLightModel::PHONG);
  } else {
    //FIXME : with NextStep viewer (SC21) 
    // ligh off + BASE_COLOR produces black faceset !.
    //FIXME : soDirLight->on.setValue(FALSE);
    soDirLight->on.setValue(TRUE); //FIXME
    soDirLight->color.setValue(1,1,1);
    // Same as Coin<Gui>/So<Gui>Viewer :
    soDirLight->direction.setValue(SbVec3f(1,-1,-10));
    //FIXME : with SGI-2.1.5.10 : light on + BASE_COLOR produces
    // black faceset !
    //soLightModel->model.setValue(SoLightModel::BASE_COLOR);
    soLightModel->model.setValue(SoLightModel::PHONG);
  }
  //soDirLight->enableNotify(flagDirLight);
  //soLightModel->enableNotify(flagLightModel);

  updateTitleRegionContent(); //it sets titleRegionSize
  updateLegendContent();      //it sets legendRegionSize
  updateInfosContent();       //it sets infosRegionSize

  updateTitleRegionGeometry();

  SbVec2f ir_ur;
  if(infosRegionAttachedToDataFrame.getValue()==FALSE) {
    // infosRegionOrigin is versus the upper right 
    // corner of the plotter region with (+x to left,+y to down).
    float xur = infosRegionOrigin.getValue()[0];
    float yur = infosRegionOrigin.getValue()[1];
    ir_ur.setValue(1-xur,1-yur);
    updateInfosRegionGeometry(ir_ur);
  } else {
    //OpenPAW :
    // infosRegionOrigin is versus the upper right 
    // corner of the plotter region data area with (+x to left,+y to down)
    // AND in percent of the data frame size
    // to follow SMGR and SMGU CERN/PAW attributes.

    SbVec3f df_ur;
    soPlotter->dataFrame2vp(SbVec3f(1,1,0),df_ur);

    SbVec3f df_bl;
    soPlotter->dataFrame2vp(SbVec3f(0,0,0),df_bl);

    float df_w = df_ur[0]-df_bl[0];
    float df_h = df_ur[1]-df_bl[1];

    float xur = infosRegionOrigin.getValue()[0];
    float yur = infosRegionOrigin.getValue()[1];

    ir_ur.setValue(df_ur[0]-xur*df_w,df_ur[1]-yur*df_h);

    updateInfosRegionGeometry(ir_ur);
  }

  if(legendRegionAttachedToInfosRegion.getValue()==FALSE) {
    SoLegendRegion* soLegendRegion = 
      (SoLegendRegion*)legendRegion.getValue();
    if(legendRegionOriginUnit.getValue()==AXIS) {        

      // legendRegionOrigin is in axes coordinates. 
      float x = legendRegionOrigin.getValue()[0];
      float y = legendRegionOrigin.getValue()[1];

      SbVec3f pos;
      if(soPlotter->axis2vp(SbVec3f(x,y,0),pos)==FALSE) {
        SoDebugError::postInfo("SoPlotterRegion::updateChildren",
			       "failed for %g %g.",x,y);
      } else {
        float w = legendRegionSize.getValue()[0];
        float h = legendRegionSize.getValue()[1];

        //TRUE : so that the size is relative to the page 
        //       (and not the plotter region)

        //updateLegendRegionGeometry(ur);
        updateLegendRegionGeometry(
          *soLegendRegion,SbVec2f(pos[0],pos[1]),SbVec2f(w,h),TRUE);
      }

    } else { //PERCENT
      // legendRegionOrigin is 
      // the UR corner of legend region relative 
      // to UR of plotter region.
      float xur = legendRegionOrigin.getValue()[0];
      float yur = legendRegionOrigin.getValue()[1];
      SbVec2f ur(1-xur,1-yur);
      //updateLegendRegionGeometry(ur);

      float w = legendRegionSize.getValue()[0];
      float h = legendRegionSize.getValue()[1];
     {float x = ur[0]-w;
      float y = ur[1]-h;
      updateLegendRegionGeometry(
        *soLegendRegion,SbVec2f(x,y),SbVec2f(w,h));}
    }

  } else { // legendRegionAttachedToInfosRegion.getValue()==TRUE

   {SoInfosRegion* soInfosRegion = (SoInfosRegion*)infosRegion.getValue();
    SoViewportRegion* soInfosRegionViewportRegion = 
      soInfosRegion->getViewportRegion();
    soInfosRegionViewportRegion->verticalShadow.setValue(0);}
    //float winfos,hinfos;
    //soInfosRegionViewportRegion->getSizePercent(winfos,hinfos);  

    SoLegendRegion* soLegendRegion = 
      (SoLegendRegion*)legendRegion.getValue();
    if(infosRegionVisible.getValue()==TRUE) {
      float winfos = infosRegionSize.getValue()[0];
      float hinfos = infosRegionSize.getValue()[1];

      float w = winfos;
      float h = legendRegionSize.getValue()[1];
      float x = ir_ur[0]-w;
      float y = ir_ur[1]-h-hinfos;
      //float y = ir_ur[1]-h;

      updateLegendRegionGeometry(
        *soLegendRegion,SbVec2f(x,y),SbVec2f(w,h));
    } else {
      float w = legendRegionSize.getValue()[0];
      float h = legendRegionSize.getValue()[1];
      float x = ir_ur[0]-w;
      float y = ir_ur[1]-h;

      updateLegendRegionGeometry(
        *soLegendRegion,SbVec2f(x,y),SbVec2f(w,h));
    }

  }

  // legendRegions :
 {SoGroup* group = getContainerNode(Sb_legendRegions);
  if(group) {
  int number = group->getNumChildren();
  for(int index=0;index<number;index++) {
    SoLegendRegion* soLegendRegion = (SoLegendRegion*)group->getChild(index);

    if(soLegendRegion->originUnit.getValue()==AXIS) {        
      // legendRegionOrigin is in axes coordinates. 
      float x = soLegendRegion->origin.getValue()[0];
      float y = soLegendRegion->origin.getValue()[1];
      SbVec3f pos;
      if(soPlotter->axis2vp(SbVec3f(x,y,0),pos)==FALSE){
        SoDebugError::postInfo("SoPlotterRegion::updateChildren",
			       "failed for %g %g.",x,y);
      } else {
        //TRUE : so that the size is relative to the page 
        //       (and not the plotter region)
        updateLegendRegionGeometry(
          *soLegendRegion,
          SbVec2f(pos[0],pos[1]),
          soLegendRegion->size.getValue(),TRUE);
      }

    } else { //PERCENT
      updateLegendRegionGeometry(
        *soLegendRegion,
        soLegendRegion->origin.getValue(),
        soLegendRegion->size.getValue());
    }
     
  }}}


  fUpdate = FALSE;
}

SoPlotter* SoPlotterRegion::getPlotter() const{return (SoPlotter*)plotter.getValue();}

int SoPlotterRegion::getNumberOfOverlays() {
  SoGroup* group = getContainerNode(Sb_overlays);
  if(!group) return 0;
  return group->getNumChildren();
}

SoPlotter* SoPlotterRegion::getOverlay(int aIndex) {
  if(aIndex<0) return 0;
  SoGroup* group = getContainerNode(Sb_overlays);
  int number = group->getNumChildren();
  if(aIndex>=number) {
    for(int index=number;index<=aIndex;index++) {
      group->addChild(new SoPlotter);
    }
  }
  return (SoPlotter*)group->getChild(aIndex);
}

SoLegendRegion* SoPlotterRegion::getLegendRegion(int aIndex) {
  if(aIndex<0) return 0;
  SoGroup* group = getContainerNode(Sb_legendRegions);  
  int number = group->getNumChildren();
  if(aIndex>=number) {
    for(int index=number;index<=aIndex;index++) {
      group->addChild(new SoLegendRegion);
    }
  }
  return (SoLegendRegion*)group->getChild(aIndex);
}

SoTextRegion* SoPlotterRegion::getTitleRegion() const{return (SoTextRegion*)titleRegion.getValue();}

SoInfosRegion* SoPlotterRegion::getInfosRegion() const{return (SoInfosRegion*)infosRegion.getValue();}

SoLegendRegion* SoPlotterRegion::getLegendRegion() const{return (SoLegendRegion*)legendRegion.getValue();}

SoSeparator* SoPlotterRegion::getEtcSeparator() const{return (SoSeparator*)etcSeparator.getValue();}

//void SoPlotterRegion::addPlottableObject(SbPlottableObject* aObject){
//  SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();
//  soPlotter->addPlottableObject(aObject);
//}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoPlotterRegion::updateOneSoPlotter(SoPlotter* aPlotter,float a_wvp,float a_hvp) {
  if(aPlotter->verbose.getValue()!=verbose.getValue()) {
    //SbBool flag = aPlotter->verbose.enableNotify(FALSE);
    aPlotter->verbose.setValue(verbose.getValue());
    //aPlotter->verbose.enableNotify(flag);
  }

  float XSIZ = aPlotter->width.getValue();
  float YSIZ = aPlotter->height.getValue();
  if( (XSIZ>0)&&(YSIZ>0)&&(a_wvp>0)&&(a_hvp>0) &&
      (aPlotter->getShape()==SoPlotter::XY) && 
      (plotterViewportMapping.getValue()==ADJUST_VIEWPORT) ) {
    // Adjust the plotter XSIZ and YSIZ so that YSIZ matches viewport height
    // and plotter XSIZ matches viewport width.
    // It it the ROOT and Lab logic.
    float viewportAspect = float(a_wvp)/float(a_hvp);
    if(viewportAspect>=1) { 
      //Camera will match in height YSIZ. Adjust XSIZ.
      float old_width = aPlotter->width.getValue();
      float new_width = YSIZ * viewportAspect;
      SbBool flag = aPlotter->width.enableNotify(FALSE);
      if(new_width!=old_width) aPlotter->width.setValue(new_width);
      aPlotter->width.enableNotify(flag);
    } else { 
      //Camera will match in width XSIZ. Adjust YSIZ.
      float old_height = aPlotter->height.getValue();
      float new_height = XSIZ / viewportAspect;
      SbBool flag = aPlotter->height.enableNotify(FALSE);
      if(new_height!=old_height) aPlotter->height.setValue(new_height);
      aPlotter->height.enableNotify(flag);
    }
  }
  aPlotter->updateChildren(); //To update title, infos and legend.
}


void SoPlotterRegion::updateTitleRegionContent(){
  SoTextRegion* soTitleRegion = (SoTextRegion*)titleRegion.getValue();
  soTitleRegion->verbose.setValue(verbose.getValue());

  soTitleRegion->visible.setValue(titleRegionVisible.getValue());
  if(soTitleRegion->visible.getValue()==FALSE) return;

  SoViewportRegion* soTitleRegionViewportRegion = 
    soTitleRegion->getViewportRegion();

  if(soTitleRegionViewportRegion->isResizing()==TRUE) {
    // Corner of the title region is being dragged.
    // Then the user want to fix by hand the size of this region.
    // We have to disable the below resizing.
    return;
  }

  // Set title text :
  // master plotter feeds title text.
  SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();
  SbString s = soPlotter->title.getValue();

  //SbBool flag = soTitleRegion->text.enableNotify(FALSE);
  soTitleRegion->text.set1Value(0,s);
  //soTitleRegion->text.enableNotify(flag);

  //FIXME : we should have some titleRegionSizeAutomatic field.
  // Should have a flag resizable or not.

  // ROOT : info height  = 3 * title height for four lines of infos.
  //float hline = 3 * 0.05F/4; //FIXME : have an SoField;

  // Resize width of title region according width of title text.
  //Percent of the plotterRegion size
  float w = s.getLength() * 0.01F; //FIXME : have an SoField.
  float h = 0.05F;

  //flag = titleRegionSize.enableNotify(FALSE);
  titleRegionSize.setValue(SbVec2f(w,h));
  //titleRegionSize.enableNotify(flag);
}

void SoPlotterRegion::updateInfosContent(){
  SoInfosRegion* soInfosRegion = (SoInfosRegion*)infosRegion.getValue();
  soInfosRegion->verbose.setValue(verbose.getValue());

  soInfosRegion->visible.setValue(infosRegionVisible.getValue());
  if(soInfosRegion->visible.getValue()==FALSE) return;

  // master plotter feeds infosRegion.
  SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();

  // ROOT : info height  = 3 * title height for four lines of infos.
  float hline = 3 * 0.05F/4; //FIXME : have an SoField;

  //Set size of infosRegion :
  //SbBool flag1 = soInfosRegion->title.enableNotify(FALSE);
  //SbBool flag2 = soInfosRegion->text.enableNotify(FALSE);
  // Set infos title and text :
  if(infosRegionStyle.getValue()=="ROOT") { //ROOT style :
    soInfosRegion->title.setValue("");
    soInfosRegion->text.deleteValues(0);
    int texti = 0;
    int number = soPlotter->infos.getNum();
    int linen = number/2;
    // We expect an even number of lines :
    for(int index=0;index<linen;index++) {
      SbString name(soPlotter->infos[2*index]);
      SbString value(soPlotter->infos[2*index+1]);
      if(name=="Name") {
        soInfosRegion->title.setValue(value);
      } else {
        soInfosRegion->text.set1Value(texti,name);
        texti++;
        soInfosRegion->text.set1Value(texti,value);
        texti++;
      }
    }
  } else { // PAW style :
    soInfosRegion->title.setValue("");
    soInfosRegion->text = soPlotter->infos;
  }
  //soInfosRegion->title.enableNotify(flag1);
  //soInfosRegion->text.enableNotify(flag2);

  // Adapt height to number of lines. 
  // (Function infos has less lines than histo infos).

  float w = 0;
  float h = 0;
  if(infosRegionResizePolicy.getValue()==ASPECT_RATIO) { //PAW
    float wvp = float(fOldWidth);
    float hvp =  float(fOldHeight);

    soInfosRegion->updateChildren();
    soInfosRegion->matchViewport(100,100);
    float cw,ch;
    soInfosRegion->getContentSize(cw,ch);    

    if((wvp>0)&&(hvp>0)&&(cw>0)&&(ch>0)) {
      h = infosRegionSize.getValue()[1];
      if(h<=0) h = 0.16F; //set default.
      w = h * (cw/ch) * (hvp/wvp);
      if(w>0.5F) { // Too large, adapt the h to have w = 0.5F
        w = 0.5F;  // (w = 0.5F should be lower than the inner frame width)
        h = w * (ch/cw) * (wvp/hvp);
      }
    } else { //set defaults :
      w = 0.2F;
      h = 0.16F;
    }

  } else { //ROOT
    w = infosRegionSize.getValue()[0];
    int number = soPlotter->infos.getNum();
    int linen = number/2;
    h = linen * hline;
  }
  //SbBool flag = infosRegionSize.enableNotify(FALSE);
  infosRegionSize.setValue(SbVec2f(w,h));
  //infosRegionSize.enableNotify(flag);
}

void SoPlotterRegion::updateLegendContent(){
  SoLegendRegion* soLegendRegion = (SoLegendRegion*)legendRegion.getValue();
  soLegendRegion->verbose.setValue(verbose.getValue());

  soLegendRegion->visible.setValue(legendRegionVisible.getValue());
  if(soLegendRegion->visible.getValue()==FALSE) return;

  if(legendAutomated.getValue()==FALSE) return;

  // master plotter feeds legendRegion.
  SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();

  //SbBool flag = soLegendRegion->text.enableNotify(FALSE);
  soLegendRegion->text = soPlotter->legend;
  //soLegendRegion->text.enableNotify(flag);

  //flag = soLegendRegion->enableNotify(FALSE);
  SoNodeKitListPart* legendStyle = soPlotter->getLegendStyleList();
  int number = legendStyle->getNumChildren();
  for(int index=0;index<number;index++) {
    soLegendRegion->setPart
     (SbStringPrintF(32,(char*)"styles[%d]",index),
      soPlotter->getPart
        (SbStringPrintF(32,(char*)"legendStyle[%d]",index),FALSE));
  }
  //soLegendRegion->enableNotify(flag);

  // ROOT : info height  = 3 * title height for four lines of infos.
  float hline = 3 * 0.05F/4; //FIXME : have an SoField;

  //Set size of legendRegion :
  // Adapt height to number of lines. 
  // (Function legend as less lines than histo legend).
  float w = legendRegionSize.getValue()[0];
  float h = soPlotter->legend.getNum() * hline;
  //SbBool flag = legendRegionSize.enableNotify(FALSE);
  legendRegionSize.setValue(SbVec2f(w,h));
  //legendRegionSize.enableNotify(flag);
}

void SoPlotterRegion::updateTitleRegionGeometry(){
  // Have the titleRegion attached to the upper left corner of the
  // SoPlotterRegion.
  SoTextRegion* soTitleRegion = (SoTextRegion*)titleRegion.getValue();
  if(soTitleRegion->visible.getValue()==FALSE) return;

  SoViewportRegion* soTitleRegionViewportRegion = 
    soTitleRegion->getViewportRegion();

  SbBool resize = TRUE;
  if(soTitleRegionViewportRegion->isResizing()==TRUE) {
    // Corner of the title region is being dragged.
    // Then the user want to fix by hand the size of this region.
    // We have to disable the below resizing.
    resize = FALSE;
    return;
  }

  SbBool move = TRUE;
  if(soTitleRegionViewportRegion->isMoving()==TRUE) {
    // Border of the title region is being dragged.
    // Then the user want to fix by hand the position of this region.
    // We have to disable the below moving.
    move = FALSE;
    return;
  }

  SoViewportRegion* soViewportRegion = 
    (SoViewportRegion*)viewportRegion.getValue();

  //SbBool flag = soTitleRegionViewportRegion->enableNotify(FALSE);

  // Resize :
  float vw,vh;
  soViewportRegion->getSizePercent(vw,vh);
  float w = vw * titleRegionSize.getValue()[0];
  float h = vh * titleRegionSize.getValue()[1];
  if(resize==TRUE) {
    soTitleRegionViewportRegion->setSizePercent(w,h);
    soTitleRegion->raiseUpdateFlag(); //to enforce a matchViewport()
  }

  float vx,vy;
  soViewportRegion->getPositionPercent(vx,vy);
  float xul = titleRegionOrigin.getValue()[0];
  float yul = titleRegionOrigin.getValue()[1];
  float x = vx + vw * xul; //In PERCENT page/viewer coordinate.
  float y = vy + vh - h - vh * yul;
  if(move==TRUE) soTitleRegionViewportRegion->setPositionPercent(x,y);

  //soTitleRegionViewportRegion->enableNotify(flag);
}

void SoPlotterRegion::updateInfosRegionGeometry(const SbVec2f& aUR){
  // Have the infosRegion attached to the upper right corner of the
  // SoPlotterRegion.
  // aUR is the wanted position of the upper right corner 
  // of the region box in NDC.
  SoInfosRegion* soInfosRegion = (SoInfosRegion*)infosRegion.getValue();
  if(soInfosRegion->visible.getValue()==FALSE) return;

  SoViewportRegion* soInfosRegionViewportRegion = 
    soInfosRegion->getViewportRegion();
  
  SbBool resize = TRUE;
  if(soInfosRegionViewportRegion->isResizing()==TRUE) {
    // Corner of the title region is being dragged.
    // Then the user want to fix by hand the size of this region.
    // We have to disable the below resizing.
    resize = FALSE;
    return;
  }

  SbBool move = TRUE;
  if(soInfosRegionViewportRegion->isMoving()==TRUE) {
    // Border of the title region is being dragged.
    // Then the user want to fix by hand the position of this region.
    // We have to disable the below moving.
    move = FALSE;
    return;
  }

  SoViewportRegion* soViewportRegion = 
    (SoViewportRegion*)viewportRegion.getValue();

  float vx,vy;
  soViewportRegion->getPositionPercent(vx,vy);
  float vw,vh;
  soViewportRegion->getSizePercent(vw,vh);

  // Move and resize infosRegion :
  float w = vw * infosRegionSize.getValue()[0];
  float h = vh * infosRegionSize.getValue()[1];
  float x = vx + vw * aUR[0] - w;
  float y = vy + vh * aUR[1] - h;
  //SbBool flag = soInfosRegionViewportRegion->enableNotify(FALSE);
  if(resize==TRUE) {
    soInfosRegionViewportRegion->setSizePercent(w,h);
    soInfosRegion->raiseUpdateFlag(); //to enforce a matchViewport()
  }
  if(move==TRUE) soInfosRegionViewportRegion->setPositionPercent(x,y);
  //soInfosRegionViewportRegion->enableNotify(flag);
}

void SoPlotterRegion::updateLegendRegionGeometry(SoLegendRegion& aRegion,const SbVec2f& aPos,const SbVec2f& aSize,SbBool aAXIS /*FALSE*/){
  // aPos is the wanted position of the lower left corner 
  // of the region box in NDC.
  if(aRegion.visible.getValue()==FALSE) return;

  SoViewportRegion* soLegendRegionViewportRegion = aRegion.getViewportRegion();
  
  SbBool resize = TRUE;
  if(soLegendRegionViewportRegion->isResizing()==TRUE) {
    // Corner of the title region is being dragged.
    // Then the user want to fix by hand the size of this region.
    // We have to disable the below resizing.
    resize = FALSE;
    return;
  }

  SbBool move = TRUE;
  if(soLegendRegionViewportRegion->isMoving()==TRUE) {
    // Border of the title region is being dragged.
    // Then the user want to fix by hand the position of this region.
    // We have to disable the below moving.
    move = FALSE;
    return;
  }

  SoViewportRegion* soViewportRegion = 
    (SoViewportRegion*)viewportRegion.getValue();

  float vx,vy;
  soViewportRegion->getPositionPercent(vx,vy);
  float vw,vh;
  soViewportRegion->getSizePercent(vw,vh);

  float w = aAXIS ? aSize[0] : vw * aSize[0];
  float h = aAXIS ? aSize[1] : vh * aSize[1];
  if(resize==TRUE) {
    soLegendRegionViewportRegion->setSizePercent(w,h);
    aRegion.raiseUpdateFlag(); //to enforce a matchViewport()
  }

  float x,y;
  if(aRegion.horizontalJustification.getValue()==SoRegion::LEFT) {
    x = vx + vw * aPos[0];
  } else if(aRegion.horizontalJustification.getValue()==SoRegion::CENTER) {
    x = vx + vw * aPos[0] - w*0.5F;
  } else if(aRegion.horizontalJustification.getValue()==SoRegion::RIGHT) {
    x = vx + vw * aPos[0] - w;
  }

  if(aRegion.verticalJustification.getValue()==SoRegion::BOTTOM) {
    y = vy + vh * aPos[1];
  } else if(aRegion.verticalJustification.getValue()==SoRegion::MIDDLE) {
    y = vy + vh * aPos[1] - h*0.5F;
  } else if(aRegion.verticalJustification.getValue()==SoRegion::TOP) {
    y = vy + vh * aPos[1] - h;
  }

  if(move==TRUE) soLegendRegionViewportRegion->setPositionPercent(x,y);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SoSeparator* SoPlotterRegion::getTopSeparator() const{return (SoSeparator*)topSeparator.getValue();}

SoCamera* SoPlotterRegion::getCamera() const{
  SoGroup* soCameraGroup = (SoGroup*)cameraGroup.getValue();
  if(!soCameraGroup->getNumChildren()) return 0;
  return (SoCamera*)soCameraGroup->getChild(0);
}

void SoPlotterRegion::setCamera(SoCamera* aCamera){
  // Used in SoCanvas::highlightRegion.
  SoGroup* soCameraGroup = (SoGroup*)cameraGroup.getValue();
  soCameraGroup->removeAllChildren();
  soCameraGroup->addChild(aCamera);
}

void SoPlotterRegion::clear(const SbString& aWhat){
  if(!aWhat.getLength()||(aWhat=="all")) { //clear all.

   {SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();
    soPlotter->clear();
   {SoGroup* group = getContainerNode(Sb_overlays);
    if(group) {
      int number = group->getNumChildren();
      for(int index=0;index<number;index++) {
        SoPlotter* soPlotter = (SoPlotter*)group->getChild(index);
        soPlotter->clear();
      }
    }}}

   {infosRegionVisible.setValue(FALSE);
    SoInfosRegion* soInfosRegion = (SoInfosRegion*)infosRegion.getValue();
    soInfosRegion->clear();}
  
   {titleRegionVisible.setValue(FALSE);
    SoTextRegion* soTitleRegion = (SoTextRegion*)titleRegion.getValue();
    soTitleRegion->clear();}
  
   {legendRegionVisible.setValue(FALSE);
    SoLegendRegion* soLegendRegion = (SoLegendRegion*)legendRegion.getValue();
    soLegendRegion->clear();}
  
   {SoGroup* group = getContainerNode(Sb_legendRegions);
    if(group) group->removeAllChildren();}

    getEtcSeparator()->removeAllChildren();

  } else {
    // Used by OnX/scripts/OnX/Plotter.onx

    SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();
    soPlotter->removePlottableObjectsOfClass(aWhat.getString());  
  }
}

void SoPlotterRegion::refresh(){

  SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();
  soPlotter->raiseUpdateFlag(SoPlotter::ALL);
 {SoGroup* group = getContainerNode(Sb_overlays);
  if(group) {
    int number = group->getNumChildren();
    for(int index=0;index<number;index++) {
      SoPlotter* soPlotter = (SoPlotter*)group->getChild(index);
      soPlotter->raiseUpdateFlag(SoPlotter::ALL);
    }
  }}

  raiseUpdateFlag();
  updateChildren();  
}

SbBool SoPlotterRegion::doIt(const SbDoIt& aDoIt){
  if(SbPlotterDoIt* doIt = (SbPlotterDoIt*)aDoIt.cast("SbPlotterDoIt")) {
    SbPlottableObject* sbPlottable = doIt->getPlottableObject();
    if(!sbPlottable) return FALSE;

    SoPlotter* soPlotter = doIt->getPlotter();
    if(!soPlotter) soPlotter = (SoPlotter*)plotter.getValue();

    soPlotter->addPlottableObject(sbPlottable);
    fDoList.push_back(sbPlottable);    
    doIt->reset(); // So that the aDoIt delete does not delete sbPlottable.
    return TRUE;

  } else if(SbAddNode* doIt = (SbAddNode*)aDoIt.cast("SbAddNode")) {
    SoNode* node = doIt->getNode();
    if(!node) return FALSE;
    SoNode* soPlace = getPart(doIt->getPlacement(),FALSE);
    if(!soPlace) {
      SoDebugError::postInfo("SoPlotterRegion::doIt",
        "placement \"%s\" not found.",doIt->getPlacement().getString());
      return FALSE;
    }
    if(soPlace->isOfType(SoGroup::getClassTypeId())==FALSE) {
      SoDebugError::postInfo("SoPlotterRegion::doIt",
        "placement \"%s\" not an SoGroup.",doIt->getPlacement().getString());
      return FALSE;
    }
    ((SoGroup*)soPlace)->addChild(node);
    //fDoList.push_back(std::pair<SoGroup*,SoNode*>((SoGroup*)soPlace,node));    
    doIt->reset(); // So that the aDoIt delete does not unref node.
    return TRUE;

  } else {
    SoDebugError::postInfo("SoPlotterRegion::doIt","unknown SbDoIt.");
    return FALSE;
  }
}

void SoPlotterRegion::undo(){
  SoPlotter* soPlotter = (SoPlotter*)plotter.getValue(); //overlays ?
  unsigned int number = fDoList.size();
  for(unsigned int index=0;index<number;index++) {
    SbPlottableObject* sbPlottable = fDoList[index];
    soPlotter->removePlottableObject(sbPlottable);
  }
  fDoList.clear();
}

void SoPlotterRegion::resetUndo(){fDoList.clear();}

void SoPlotterRegion::resetCamera(){
  SoCamera* soCamera = getCamera();
  if(!soCamera) return;
  float wvp = float(fOldWidth);
  float hvp =  float(fOldHeight);

  // master plotter feeds the camera.
  SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();

  float XSIZ = soPlotter->width.getValue();
  float YSIZ = soPlotter->height.getValue();
  if((XSIZ>0)&&(YSIZ>0)&&(wvp>0)&&(hvp>0)) {
    // Update the camera :
    soCamera->viewportMapping.setValue(SoCamera::ADJUST_CAMERA);
    //SbBool flag = soCamera->enableNotify(FALSE);
    if(soPlotter->shape.getValue()==SoPlotter::XYZ) {
      float height = YSIZ;
      soCamera->position.setValue(0,0,4*height);
      soCamera->orientation.setValue(SbRotation(SbVec3f(0,1,0),0));
      soCamera->nearDistance.setValue(1);
      soCamera->farDistance.setValue(100*height);
      soCamera->focalDistance.setValue(4*height);
    } else {
      float z = 4 * soPlotter->depth.getValue();
      soCamera->position.setValue(0,0,z);
      soCamera->orientation.setValue(SbRotation(SbVec3f(0,1,0),0));
      soCamera->nearDistance.setValue(0.01F*z);
      soCamera->farDistance.setValue(100*z);
      soCamera->focalDistance.setValue(z);
    }
    if(soCamera->isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
      SoOrthographicCamera* soOrthoCamera = (SoOrthographicCamera*)soCamera;
      if(soPlotter->shape.getValue()==SoPlotter::XYZ) {
        float height = YSIZ;
        soOrthoCamera->height.setValue(height);
      } else {
        float height = 0;
        float viewportAspect = float(wvp)/float(hvp);
        if(plotterViewportMapping.getValue()==LEAVE_ALONE) {
          // Adjust camera so that plotter XSIZ/YSIZ be not distorded.
          // If viewport aspect ratio > plotter aspect ratio
          // the plotter YSIZ matches the viewport height. If not
          // it is the plotter XSIZ that will match the viewport width.
          // It is the PAW logic for one region.
          float plotterAspect = XSIZ/YSIZ;
          if(viewportAspect<plotterAspect) {
            height =  XSIZ;    
          } else {
            height =  YSIZ;    
          }
        } else if(plotterViewportMapping.getValue()==ADJUST_VIEWPORT) {
          if(viewportAspect>=1) {
            height =  YSIZ;    
          } else {
            height =  XSIZ;    
          }
        } 
        soOrthoCamera->height.setValue(height);
      }
    } else if(soCamera->isOfType(SoPerspectiveCamera::getClassTypeId())==TRUE){
      SoPerspectiveCamera* soPersCamera = (SoPerspectiveCamera*)soCamera;
      soPersCamera->heightAngle.setValue(FM_PI/4.0F);
    }
    //soCamera->enableNotify(flag);
  }
}

SoNode* SoPlotterRegion::getNodeOfInterest(const SbString& aWhat) const{
  if(aWhat=="scene") {
    return plotter.getValue();
  } else if(aWhat=="plotter") {
    return plotter.getValue();
  } else { //all or ""
    return topSeparator.getValue();
  }
}

void SoPlotterRegion::placeOneSoPlotterInPage(
 SoPlotter* aPlotter
,float //aPageWidth
,float aPageHeight
,float aWidth 
,float aHeight
,float aLeftMargin
,float aRightMargin
,float aTopMargin
,float aBottomMargin
){
  // Used in SoPage::updateChildren.
  //float pw = aPageWidth;
  float ph = aPageHeight;
  float wr = aWidth;
  float hr = aHeight;
  float lm = aLeftMargin;
  float rm = aRightMargin;
  float tm = aTopMargin;
  float bm = aBottomMargin;

  float XSIZ = wr;
  float YSIZ = hr;

  aPlotter->width.setValue(XSIZ);
  aPlotter->leftMargin.setValue(lm);
  aPlotter->rightMargin.setValue(rm);

  aPlotter->height.setValue(YSIZ);           
  aPlotter->topMargin.setValue(tm);
  aPlotter->bottomMargin.setValue(bm);

  // Map Z with X values. Else XYZ plots will 
  // be distorded in Z when put in a sub zone.
  aPlotter->depth.setValue(wr);           
  aPlotter->downMargin.setValue(lm);
  aPlotter->upMargin.setValue(rm);

  //OpenPAW : with the below, in case of zones, we do not
  //          have the same size than CERN/PAW (for exa with pawex13)).
  // Adjust title position and size :
/*
 {float xfac = XSIZ/20.0F;
  float yfac = YSIZ/20.0F;
  float YHTI = 1.2F * yfac;  //0.06  //y distance of title to x axis.
  float TSIZ = 0.28F * yfac; //0.014 //title character size
  aPlotter->titleToAxis.setValue(YHTI);
  aPlotter->titleHeight.setValue(TSIZ);}

  //OpenPAW : with the below, in case of zones, we do not
  //          have the same size than CERN/PAW (for exa with pawex13)).
  // Adjust axes text position and size :
  aPlotter->setPAW_AxesTextSize(XSIZ,YSIZ);
*/

}

void SoPlotterRegion::placeInPage(float aPageWidth,float aPageHeight,
                                  float aWidth,float aHeight,
                                  float aLeftMargin,float aRightMargin,
                                  float aTopMargin,float aBottomMargin){
  // Used in SoPage::updateChildren.
  plotterViewportMapping.setValue(SoPlotterRegion::ADJUST_VIEWPORT);

  SoPlotter* soPlotter = (SoPlotter*)plotter.getValue();
  placeOneSoPlotterInPage(soPlotter,
                          aPageWidth,aPageHeight,aWidth,aHeight,
                          aLeftMargin,aRightMargin,aTopMargin,aBottomMargin);
 {SoGroup* group = getContainerNode(Sb_overlays);
  if(group) {
    int number = group->getNumChildren();
    for(int index=0;index<number;index++) {
      SoPlotter* soPlotter = (SoPlotter*)group->getChild(index);
      placeOneSoPlotterInPage(soPlotter,
                              aPageWidth,aPageHeight,aWidth,aHeight,
                              aLeftMargin,aRightMargin,
                              aTopMargin,aBottomMargin);
    }
  }}

  // Infos regions :
  // In CERN/PAW, for all regions, the infos region size 
  // depends of the page size (and not of each relative region).
  infosRegionResizePolicy.setValue(SoPlotterRegion::ASPECT_RATIO);
  //float hinfos = infosRegionSize.getValue()[1];
  //infosRegionSize.setValue(SbVec2f(0,hinfos*ph/hr));
  float hinfos = 0.16F;
  infosRegionSize.setValue(SbVec2f(0,hinfos));

}

int SoPlotterRegion::getNumberOfLegendRegions(){ 
  SoGroup* group = getContainerNode(Sb_legendRegions);
  if(!group) return 0;
  return group->getNumChildren();
}

void SoPlotterRegion::generateAlternateRep(SoAction*){
  // We do NOT write the subregions
  // since we do not have an alternateRep
  // for the SoViewportRegion.
  alternateRep.setValue(getAlternateRep());
}

void SoPlotterRegion::clearAlternateRep(){alternateRep.setValue(NULL);}

//////////////////////////////////////////////////////////////////////////////
/// Style ////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoPlotterRegion::setStyle(const SbString& aStyle){
  if(aStyle=="Lab_Default") {
    setStyleLab_Default();
  } else if(aStyle=="PAW_Default") {
    setStylePAW_Default();
  } else if(aStyle=="Hippo_Default") {
    setStyleHippo_Default();
  } else if(aStyle=="ROOT_Default") {
    setStyleROOT_Default();
  } else if(aStyle=="ROOT_Plain") {
    setStyleROOT_Plain();
  } else if(aStyle=="ROOT_Bold") {
    setStyleROOT_Bold();
  } else if(aStyle=="ROOT_Video") {
    setStyleROOT_Video();
  } else if(aStyle=="ROOT_Pub") {
    setStyleROOT_Pub();
  } else if(aStyle=="ROOT_LHCb") {
    setStyleROOT_LHCb();
  }
}
#include <HEPVis/SbColors.h>

#define xAxisNode soPlotter->getXAxis()
#define yAxisNode soPlotter->getYAxis()
#define zAxisNode soPlotter->getZAxis()

void SoPlotterRegion::setStyleROOT_Default(){
  // From ROOT-4.00.08 TStyle::Reset.
  enum EColor {kWhite,kBlack,kRed,kGreen,kBlue,kYellow,kMagenta,kCyan};

  SoPlotter* soPlotter = getPlotter();

  SbStyleROOT_Colormap cmap;

  plotterViewportMapping.setValue(ADJUST_VIEWPORT);

  infosRegionAttachedToDataFrame.setValue(FALSE);

  soPlotter->width.setValue(1);
  soPlotter->height.setValue(1);
  soPlotter->setAxesModeling("hplot");
  soPlotter->setAxesColor(SbColor(0,0,0));

  soPlotter->valueTopMargin.setValue(0.05F);

  legendRegionVisible.setValue(FALSE);

 {SoStyle* style = soPlotter->getWallStyle();
  style->visible.setValue(FALSE);}
 {SoStyle* style = soPlotter->getBinsStyle(0);
  style->modeling.setValue(SbModeling_top_lines);}

  // To have a good matching with ROOT for text size :
  //FIXME : avoid this below cooking...
  //float majic = 0.014F/0.027F; //=0.518
  float majic = 0.6F;
  soPlotter->setAxesTextScale(majic);
  soPlotter->getTitleStyle()->scale.setValue(majic);

  float XLAB = 1.4F/20.0F; //0.07 //x distance of y title to data frame.
  float YLAB = 0.8F/20.0F; //0.04 //y distance of x title to data frame.

  YLAB = 0.05F; //FIXME : cooking.

  SbBool smoothing = TRUE;

  //TAttLine::ResetAttLine();
    //fLineColor  = 1;
    //fLineStyle  = 1;
    soPlotter->setAxesLinePattern(SbLinePattern_solid);
    //fLineWidth  = 1;
    soPlotter->setAxesLineWidth(0);

  //TAttFill::ResetAttFill();
  //TAttText::ResetAttText();
  //TAttMarker::ResetAttMarker();
  //SetFillStyle(1001);
  //SetFillColor(19);
  //fXaxis.ResetAttAxis("X");
    xAxisNode->titleToAxisJustification.setValue(SoAxis::RIGHT);
    xAxisNode->getTitleStyle()->visible.setValue(TRUE);
    //fNdivisions  = 510;
    xAxisNode->divisions.setValue(510);
    //fAxisColor   = 1;
    xAxisNode->getLineStyle()->color.setValue(cmap.getColor(kBlack));
    //fLabelColor  = 1;
    //fLabelFont   = 62;
   {SoTextStyle* style = xAxisNode->getLabelsStyle();
    style->color.setValue(cmap.getColor(kBlack));
    style->fontName.setValue(SbFont_ROOT_62);
    style->smoothing.setValue(smoothing);}
   {SoTextStyle* style = xAxisNode->getTitleStyle();
    style->fontName.setValue(SbFont_ROOT_62);
    style->smoothing.setValue(smoothing);}
    //fLabelOffset = 0.005;
    xAxisNode->labelToAxis.setValue(0.005F);
    //fLabelSize   = 0.04;
    xAxisNode->labelHeight.setValue(0.04F);
    //fTickLength  = 0.03;
    xAxisNode->tickLength.setValue(0.03F);  //FIXME %?
    //fTitleOffset = 1;
    xAxisNode->titleToAxis.setValue(1.0F*YLAB);
    //fTitleSize   = fLabelSize;
    xAxisNode->titleHeight.setValue(0.04F);
    //fTitleColor  = 1;
    //FIXME SETPAR("plotter.xAxisStyle.color",cmap.getColor(1));
    //fTitleFont   = 62;
    //FIXME SETPAR("plotter.xAxisStyle.fontName",SbFont_ROOT_62);
    //FIXME SETPAR("plotter.xAxisStyle.smoothing",smoothing);

   {SoTextStyle* style = xAxisNode->getMagStyle();
    style->color.setValue(cmap.getColor(kBlack));
    style->fontName.setValue(SbFont_ROOT_62);
    style->smoothing.setValue(smoothing);}

  //fYaxis.ResetAttAxis("Y");
    yAxisNode->titleToAxisJustification.setValue(SoAxis::RIGHT);
    yAxisNode->getTitleStyle()->visible.setValue(TRUE);
    //fNdivisions  = 510;
    yAxisNode->divisions.setValue(510);
    //fAxisColor   = 1;
    yAxisNode->getLineStyle()->color.setValue(cmap.getColor(1));
    //fLabelColor  = 1;
    //fLabelFont   = 62;
   {SoTextStyle* style = yAxisNode->getLabelsStyle();
    style->color.setValue(cmap.getColor(kBlack));
    style->fontName.setValue(SbFont_ROOT_62);
    style->smoothing.setValue(smoothing);}
   {SoTextStyle* style = yAxisNode->getTitleStyle();
    style->fontName.setValue(SbFont_ROOT_62);
    style->smoothing.setValue(smoothing);}
    //fLabelOffset = 0.005;
    yAxisNode->labelToAxis.setValue(0.005F);
    //fLabelSize   = 0.04;
    yAxisNode->labelHeight.setValue(0.04F);
    //fTickLength  = 0.03;
    yAxisNode->tickLength.setValue(0.03F);  //FIXME %?
    //fTitleOffset = 1;
    yAxisNode->titleToAxis.setValue(1.0F*XLAB);
    //fTitleSize   = fLabelSize;
    yAxisNode->titleHeight.setValue(0.04F);
    //fTitleColor  = 1;
    //FIXME SETPAR("plotter.xAxisStyle.color",cmap.getColor(1));
    //fTitleFont   = 62;
    //FIXME SETPAR("plotter.xAxisStyle.fontName",SbFont_ROOT_62);
    //FIXME SETPAR("plotter.xAxisStyle.smoothing",smoothing);

   {SoTextStyle* style = yAxisNode->getMagStyle();
    style->color.setValue(cmap.getColor(kBlack));
    style->fontName.setValue(SbFont_ROOT_62);
    style->smoothing.setValue(smoothing);}

  //fZaxis.ResetAttAxis("Z"); //FIXME
  //fCanvasColor    = 19;
  //fCanvasBorderSize= 2;
  //fCanvasBorderMode= 1;
  //fCanvasDefH     = 500;
  //fCanvasDefW     = 700;
  //fCanvasDefX     = 10;
  //fCanvasDefY     = 10;
  //fPadColor       = fCanvasColor;
  getViewportRegion()->background.setValue(TRUE);
  getViewportRegion()->backgroundColor.setValue(cmap.getColor(19));
  //fPadBorderSize  = fCanvasBorderSize;
  getViewportRegion()->horizontalBorder.setValue(1);
  getViewportRegion()->verticalBorder.setValue(1);
  //fPadBorderMode  = fCanvasBorderMode;
  //fPadBottomMargin= 0.1;
  soPlotter->bottomMargin.setValue(0.1F);
  //fPadTopMargin   = 0.1;
  soPlotter->topMargin.setValue(0.1F);
  //fPadLeftMargin  = 0.1;
  soPlotter->leftMargin.setValue(0.1F);
  //fPadRightMargin = 0.1;
  soPlotter->rightMargin.setValue(0.1F);
  //fPadGridX       = kFALSE;
  soPlotter->getGridStyle()->visible.setValue(FALSE);
  //fPadGridY       = kFALSE;
  //FIXME SETPAR("plotter.gridStyle.visible","FALSE");
  //fPadTickX       = 0;
  soPlotter->rightAxisVisible.setValue(FALSE);
  //fPadTickY       = 0;
  soPlotter->topAxisVisible.setValue(FALSE);
  //fFuncColor      = 1;
  soPlotter->getFunctionStyle(0)->color.setValue(cmap.getColor(kBlack));
  //fFuncStyle      = 1;
  soPlotter->getFunctionStyle(0)->linePattern.setValue(SbLinePattern_solid);
  //fFuncWidth      = 3;
  soPlotter->getFunctionStyle(0)->lineWidth.setValue(3);
  //fGridColor      = 0;
  soPlotter->getGridStyle()->color.setValue(cmap.getColor(0));
  //fGridStyle      = 3;
  //fGridWidth      = 1;
  //fHistLineColor  = 1;
  soPlotter->getBinsStyle(0)->color.setValue(cmap.getColor(1));
  //fHistFillColor  = 0;
  //fHistFillStyle  = 1001;
  //fHistLineStyle  = 1;
  soPlotter->getBinsStyle(0)->linePattern.setValue(SbLinePattern_solid);
  //fHistLineWidth  = 1;
  soPlotter->getBinsStyle(0)->lineWidth.setValue(1);
  //fFrameLineColor = 1;
  soPlotter->getInnerFrameStyle()->color.setValue(cmap.getColor(1));
  //fFrameFillColor = 0;
  //fFrameFillStyle = 1001;
  //fFrameLineStyle = 1;
  soPlotter->getInnerFrameStyle()->linePattern.setValue(SbLinePattern_solid);
  //fFrameLineWidth = 1;
  soPlotter->getInnerFrameStyle()->lineWidth.setValue(1);
  //fFrameBorderSize= 1;
  //fFrameBorderMode= 1;
  //fBarWidth       = 1;
  //fBarOffset      = 0;
  //fDrawBorder     = 0;
  //fOptLogx        = 0;
  xAxisNode->logScale.setValue(FALSE);
  //fOptLogy        = 0;
  yAxisNode->logScale.setValue(FALSE);
  //fOptLogz        = 0;
  //FIXME SETPAR("plotter.zAxisLogScale","FALSE");
  //fOptDate        = 0;
  //fOptFile        = 0;
  //fOptFit         = 0;
  //fOptStat        = 1;
  infosRegionResizePolicy.setValue("RESIZE_HEIGHT");
  infosRegionVisible.setValue(TRUE);
  infosRegionStyle.setValue("ROOT");
  getInfosRegion()->inBetweenSpaces.setValue(2);

  //See Lab/source/Inventor/PlottableThings.cxx/getInfos methods.
  // underflow overflow fit_errors 
  // fit_quality fit_ndf fit_parameters fit_errors
  soPlotter->getInfosStyle()->options.setValue("name entries mean rms");
  //fOptTitle       = 1;
  //Use the titleRegion. Do not visualize the title in the SoPlotter.
  soPlotter->getTitleStyle()->visible.setValue(FALSE);
  titleRegionVisible.setValue(TRUE);
  //fErrorMarker    = 21;
  //fEndErrorSize   = 2;
  //fErrorX         = 0.5;
  //fScreenFactor   = 1;
  //fStatColor      = fCanvasColor;
  getInfosRegion()->getViewportRegion()->backgroundColor.setValue(cmap.getColor(19));
  //fStatTextColor  = 1;
  getInfosRegion()->getTextStyle()->color.setValue(cmap.getColor(1));
  //fStatBorderSize = 2;
  getInfosRegion()->getViewportRegion()->horizontalBorder.setValue(1);
  getInfosRegion()->getViewportRegion()->verticalBorder.setValue(1);
  getInfosRegion()->getViewportRegion()->horizontalShadow.setValue(2);
  getInfosRegion()->getViewportRegion()->verticalShadow.setValue(-2);
  getInfosRegion()->getLineStyle()->color.setValue(cmap.getColor(kBlack));
  getInfosRegion()->getLineStyle()->width.setValue(1);
  //fStatFont       = 62;
  getInfosRegion()->getTextStyle()->fontName.setValue(SbFont_ROOT_62);
  getInfosRegion()->getTextStyle()->smoothing.setValue(smoothing);
  //fStatFontSize   = 0;
  //fStatStyle      = 1001;
  //fStatW          = 0.20;
  //fStatH          = 0.16;
  infosRegionSize.setValue(SbVec2f(0.2F,0.16F)); //height automatic.
  //fStatX          = 0.98;
  //fStatY          = 0.995;
  infosRegionOrigin.setValue(SbVec2f(0.02F,0.005F)); //PERCENT : ur info to ur plotter.
  //SetStatFormat();
  //SetFitFormat();
  //SetPaintTextFormat();
  //fTitleColor     = fCanvasColor;
  getTitleRegion()->getViewportRegion()->backgroundColor.setValue(cmap.getColor(19));
  //fTitleTextColor = 1;
  getTitleRegion()->getStyle(0)->color.setValue(cmap.getColor(1));
  //fTitleFont      = 62;
  getTitleRegion()->getStyle(0)->fontName.setValue(SbFont_ROOT_62);
  getTitleRegion()->getStyle(0)->smoothing.setValue(smoothing);
  //fTitleFontSize  = 0;
  //fTitleStyle     = 1001;
  //fTitleBorderSize= 2;
  getTitleRegion()->getViewportRegion()->horizontalBorder.setValue(1);
  getTitleRegion()->getViewportRegion()->verticalBorder.setValue(1);
  getTitleRegion()->getViewportRegion()->horizontalShadow.setValue(2);
  getTitleRegion()->getViewportRegion()->verticalShadow.setValue(-2);
  //fTitleW         = 0;
  //fTitleH         = 0;
  titleRegionSize.setValue(SbVec2f(0.3F,0.05F)); //width automatic.
  //fTitleX         = 0.01;
  //fTitleY         = 0.995;
  titleRegionOrigin.setValue(SbVec2f(0.01F,0.005F)); //PERCENT ul title to ul plotter.
  //fShowEventStatus= 0;
  //fShowEditor     = 0;
  //fShowToolBar    = 0;
  //fLegoInnerR     = 0.5;
  //fHeaderPS       = "";
  //fTitlePS        = "";
  //fStripDecimals  = kTRUE;
  //fNumberContours = 20;

  //SetDateX();
  //SetDateY();
  //fAttDate.SetTextSize(0.025);
  //fAttDate.SetTextAlign(11);
  //SetLineScalePS();
  //SetLineStyleString(1,"[]");
  //SetLineStyleString(2,"[12 12]");
  //SetLineStyleString(3,"[4 8]");
  //SetLineStyleString(4,"[12 15 4 15]");
  //for (Int_t i=5;i<30;i++) SetLineStyleString(i,"[]");

  //SetPaperSize();
  //SetPalette(); //FIXME
  //fTimeOffset = 788918400; // UTC time at 01/01/95


}

void SoPlotterRegion::setStyleROOT_Plain(){
  // From ROOT-4.00.08 TStyle::TStyle.
  //SoPlotter* soPlotter = getPlotter();

  SbStyleROOT_Colormap cmap;

  setStyleROOT_Default();

  //SetFrameBorderMode(0);
  //SetCanvasBorderMode(0);
  //SetPadBorderMode(0);

  //SetPadColor(0);
  getViewportRegion()->backgroundColor.setValue(cmap.getColor(0));
  //SetCanvasColor(0);
  //SetTitleFillColor(0);a
  getTitleRegion()->getViewportRegion()->backgroundColor.setValue(cmap.getColor(0));
  //SetTitleBorderSize(1);
  getTitleRegion()->getViewportRegion()->horizontalBorder.setValue(1);
  getTitleRegion()->getViewportRegion()->verticalBorder.setValue(1);
  getTitleRegion()->getViewportRegion()->horizontalShadow.setValue(0);
  getTitleRegion()->getViewportRegion()->verticalShadow.setValue(0);
  //SetStatColor(0);
  getInfosRegion()->getViewportRegion()->backgroundColor.setValue(cmap.getColor(0));
  //SetStatBorderSize(1);
  getInfosRegion()->getViewportRegion()->horizontalBorder.setValue(1);
  getInfosRegion()->getViewportRegion()->verticalBorder.setValue(1);
  getInfosRegion()->getViewportRegion()->horizontalShadow.setValue(0);
  getInfosRegion()->getViewportRegion()->verticalShadow.setValue(0);
  getInfosRegion()->getLineStyle()->width.setValue(1);

}

void SoPlotterRegion::setStyleROOT_Bold(){
  // From ROOT-4.00.08 TStyle::TStyle.
  SoPlotter* soPlotter = getPlotter();

  SbStyleROOT_Colormap cmap;
  enum EColor {kWhite,kBlack,kRed,kGreen,kBlue,kYellow,kMagenta,kCyan};

  float XLAB = 1.4F/20.0F; //0.07
  //float YLAB = 0.8F/20.0F; //0.04

  setStyleROOT_Default();

  getTitleRegion()->getViewportRegion()->horizontalBorder.setValue(2);
  getTitleRegion()->getViewportRegion()->verticalBorder.setValue(2);
  getTitleRegion()->getViewportRegion()->horizontalShadow.setValue(0);
  getTitleRegion()->getViewportRegion()->verticalShadow.setValue(0);

  getInfosRegion()->getViewportRegion()->horizontalBorder.setValue(2);
  getInfosRegion()->getViewportRegion()->verticalBorder.setValue(2);
  getInfosRegion()->getViewportRegion()->horizontalShadow.setValue(0);
  getInfosRegion()->getViewportRegion()->verticalShadow.setValue(0);

  getInfosRegion()->getLineStyle()->width.setValue(2);

  //SetPalette(1,0); //FIXME.
  //SetCanvasColor(10);
  //SetCanvasBorderMode(0);
  //SetFrameLineWidth(3);
  soPlotter->getInnerFrameStyle()->lineWidth.setValue(3);
  //SetFrameFillColor(10);
  //SetPadColor(10);
  //SetPadTickX(1);
  soPlotter->rightAxisVisible.setValue(TRUE);
  //SetPadTickY(1);
  soPlotter->topAxisVisible.setValue(TRUE);
  //SetPadBottomMargin(0.15);
  soPlotter->bottomMargin.setValue(0.15F);
  //SetPadLeftMargin(0.15);
  soPlotter->leftMargin.setValue(0.15F);
  //SetHistLineWidth(3);
  soPlotter->getBinsStyle(0)->lineWidth.setValue(3);
  //SetHistLineColor(kRed);
  soPlotter->getBinsStyle(0)->color.setValue(cmap.getColor(kRed));
  //SetFuncWidth(3);
  soPlotter->getFunctionStyle(0)->lineWidth.setValue(3);
  //SetFuncColor(kGreen);
  soPlotter->getFunctionStyle(0)->color.setValue(cmap.getColor(kGreen));
  //SetLineWidth(3);
  soPlotter->setAxesLineWidth(3);
  //SetLabelSize(0.05,"X");
  //SetLabelSize(0.05,"Y");
  //SetLabelSize(0.05,"Z");
  soPlotter->setAxesLabelHeight(0.05F);
  //SetLabelOffset(0.01,"Y");
  yAxisNode->labelToAxis.setValue(0.01F);
  //SetLabelColor(kBlue,"X");
  xAxisNode->getLabelsStyle()->color.setValue(cmap.getColor(kBlue));
  //SetLabelColor(kBlue,"Y");
  yAxisNode->getLabelsStyle()->color.setValue(cmap.getColor(kBlue));
  //SetTitleSize(0.06,"X");
  //SetTitleSize(0.06,"Y");
  //SetTitleSize(0.06,"Z");
  soPlotter->setAxesTitleHeight(0.06F);
  //SetTitleOffset(1.3,"Y");
  yAxisNode->titleToAxis.setValue(1.3F*XLAB);
  //SetTitleFillColor(10);
  getTitleRegion()->getViewportRegion()->backgroundColor.setValue(cmap.getColor(0));
  //SetTitleTextColor(kBlue);
  getTitleRegion()->getStyle(0)->color.setValue(cmap.getColor(kBlue));
  //SetStatColor(10);
  getInfosRegion()->getViewportRegion()->backgroundColor.setValue(cmap.getColor(0));
}

void SoPlotterRegion::setStyleROOT_Video(){
  // From ROOT-4.00.08 TStyle::TStyle.
  setStyleROOT_Bold();

  SoPlotter* soPlotter = getPlotter();

  //SetPadBottomMargin(0.2);
  soPlotter->bottomMargin.setValue(0.2F);
  //SetPadLeftMargin(0.2);
  soPlotter->leftMargin.setValue(0.2F);
  //SetHistLineWidth(8);
  soPlotter->getBinsStyle(0)->lineWidth.setValue(8);
  //SetLabelSize(0.06,"X");
  //SetLabelSize(0.06,"Y");
  //SetLabelSize(0.06,"Z");
  soPlotter->setAxesLabelHeight(0.06F);
  //SetTitleSize(0.08,"X");
  //SetTitleSize(0.08,"Y");
  //SetTitleSize(0.08,"Z");
  soPlotter->setAxesTitleHeight(0.08F);
  //SetFuncWidth(8);
  soPlotter->getFunctionStyle(0)->lineWidth.setValue(8);
}

void SoPlotterRegion::setStyleROOT_Pub(){
  // From ROOT-4.00.08 TStyle::TStyle.
  SoPlotter* soPlotter = getPlotter();

  SbStyleROOT_Colormap cmap;
  enum EColor {kWhite,kBlack,kRed,kGreen,kBlue,kYellow,kMagenta,kCyan};

  float XLAB = 1.4F/20.0F; //0.07
  //float YLAB = 0.8F/20.0F; //0.04

  setStyleROOT_Default();

  //SetOptTitle(0);
  titleRegionVisible.setValue(FALSE);
  //SetOptStat(0);
  infosRegionVisible.setValue(FALSE);
  //SetPalette(8,0); //FIXME
  //SetCanvasColor(10);
  //SetCanvasBorderMode(0);
  //SetFrameLineWidth(3);
  soPlotter->getInnerFrameStyle()->lineWidth.setValue(3);
  //SetFrameFillColor(10);
  //SetPadColor(10);
  //SetPadTickX(1);
  soPlotter->rightAxisVisible.setValue(TRUE);
  //SetPadTickY(1);
  soPlotter->topAxisVisible.setValue(TRUE);
  //SetPadBottomMargin(0.15);
  soPlotter->bottomMargin.setValue(0.15F);
  //SetPadLeftMargin(0.15);
  soPlotter->leftMargin.setValue(0.15F);
  //SetHistLineWidth(3);
  soPlotter->getBinsStyle(0)->lineWidth.setValue(3);
  //SetHistLineColor(kRed);
  soPlotter->getBinsStyle(0)->color.setValue(cmap.getColor(kRed));
  //SetFuncWidth(3);
  soPlotter->getFunctionStyle(0)->lineWidth.setValue(3);
  //SetFuncColor(kGreen);
  soPlotter->getFunctionStyle(0)->color.setValue(cmap.getColor(kGreen));
  //SetLineWidth(3);
  soPlotter->setAxesLineWidth(3);
  //SetLabelSize(0.05,"X");
  //SetLabelSize(0.05,"Y");
  //SetLabelSize(0.05,"Z");
  soPlotter->setAxesLabelHeight(0.05F);
  //SetLabelOffset(0.01,"Y");
  yAxisNode->labelToAxis.setValue(0.01F);
  //SetLabelColor(kBlack,"X"); //FIXME
  //SetLabelColor(kBlack,"Y"); //FIXME
  //SetLabelColor(kBlack,"Z"); //FIXME
  //SetTitleSize(0.06,"X");
  //SetTitleSize(0.06,"Y");
  //SetTitleSize(0.06,"Z");
  soPlotter->setAxesTitleHeight(0.06F);
  //SetTitleOffset(1.3,"Y");
  yAxisNode->titleToAxis.setValue(1.3F*XLAB);
  //SetTitleFillColor(10);
  //SetTitleTextColor(kBlue); //FIXME
}

void SoPlotterRegion::setStyleROOT_LHCb(){
  // From LHCb Tools/Styles/v1r2/root/lhcbstyle.C.
  SoPlotter* soPlotter = getPlotter();
  SbStyleROOT_Colormap cmap;

  //gROOT->Reset();
  setStyleROOT_Default();

  ::printf("\
\n\
                         $      $   $   $$$   $    \n\
                         $      $   $  $      $    \n\
                         $      $$$$$  $      $$$  \n\
                         $      $   $  $      $  $ \n\
                         $$$$$  $   $   $$$   $$$  \n\
\n\
\n\
     Inventor/SoPlotter version done by Guy Barrand\n\
     from the ROOT version done by Thomas Schietinger\n\
\n");

  //TStyle *lhcbStyle= new TStyle("lhcbStyle","LHCb official plots style");

  //// use helvetica-bold-r-normal, precision 2 (rotatable)
  //Int_t lhcbFont = 62;
  SbString lhcbFont = "TTF/arialbd";
  //// line thickness
  //Double_t lhcbWidth = 3;
  int lhcbWidth = 3;
  SbBool smoothing = TRUE;

  //// use plain black on white colors
  //lhcbStyle->SetFrameBorderMode(0);
  //lhcbStyle->SetCanvasBorderMode(0);
  //lhcbStyle->SetPadBorderMode(0);
  //lhcbStyle->SetPadColor(0);
  getViewportRegion()->backgroundColor.setValue(cmap.getColor(0));
  //lhcbStyle->SetCanvasColor(0);
  //lhcbStyle->SetStatColor(0);
  getInfosRegion()->getViewportRegion()->backgroundColor.setValue(cmap.getColor(0));
  //lhcbStyle->SetPalette(1);
  //lhcbStyle->SetTitleColor(0);
  getTitleRegion()->getViewportRegion()->backgroundColor.setValue(cmap.getColor(0));
  //lhcbStyle->SetFillColor(0);
  
  //// set the paper & margin sizes
  //lhcbStyle->SetPaperSize(20,26);
  //lhcbStyle->SetPadTopMargin(0.05);
  soPlotter->topMargin.setValue(0.05F);
  //lhcbStyle->SetPadRightMargin(0.05); // increase for colz plots!!
  soPlotter->rightMargin.setValue(0.05F);
  //lhcbStyle->SetPadBottomMargin(0.16);
  soPlotter->bottomMargin.setValue(0.16F);
  //lhcbStyle->SetPadLeftMargin(0.14);
  soPlotter->leftMargin.setValue(0.14F);
  
  //// use large fonts
  //lhcbStyle->SetTextFont(lhcbFont);
  //lhcbStyle->SetLabelFont(lhcbFont,"x");
  //lhcbStyle->SetLabelFont(lhcbFont,"y");
  //lhcbStyle->SetLabelFont(lhcbFont,"z");
  soPlotter->setFont(lhcbFont,TRUE,FALSE);
  //lhcbStyle->SetTextSize(0.08);
  //lhcbStyle->SetLabelSize(0.05,"x");
  //lhcbStyle->SetLabelSize(0.05,"y");
  //lhcbStyle->SetLabelSize(0.05,"z");
  soPlotter->setAxesLabelHeight(0.05F);
  //lhcbStyle->SetTitleFont(lhcbFont);
  //lhcbStyle->SetTitleSize(0.06,"x");
  //lhcbStyle->SetTitleSize(0.06,"y");
  //lhcbStyle->SetTitleSize(0.06,"z");
  soPlotter->setAxesTitleHeight(0.06F);
  
  //// use bold lines and markers
  //lhcbStyle->SetLineWidth(lhcbWidth);
  soPlotter->setAxesLineWidth(lhcbWidth);
  //lhcbStyle->SetFrameLineWidth(lhcbWidth);
  soPlotter->getInnerFrameStyle()->lineWidth.setValue(lhcbWidth);
  //lhcbStyle->SetHistLineWidth(lhcbWidth);
  soPlotter->getBinsStyle(0)->lineWidth.setValue(lhcbWidth);
  //lhcbStyle->SetFuncWidth(lhcbWidth);
  soPlotter->getFunctionStyle(0)->lineWidth.setValue(lhcbWidth);
  //lhcbStyle->SetGridWidth(lhcbWidth);
  //lhcbStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes
  //lhcbStyle->SetMarkerStyle(8);
  //lhcbStyle->SetMarkerSize(1.5);
  
  //// label offsets
  //lhcbStyle->SetLabelOffset(0.015);
  xAxisNode->getTitleStyle()->visible.setValue(FALSE);
  xAxisNode->labelToAxis.setValue(0.015F);
  yAxisNode->labelToAxis.setValue(0.015F);
  
  //// by default, do not display histogram decorations:
  //lhcbStyle->SetOptStat(0);  
  infosRegionVisible.setValue(FALSE);
  soPlotter->getInfosStyle()->options.setValue("name entries mean rms");
  //lhcbStyle->SetOptTitle(0);
  titleRegionVisible.setValue(FALSE);
  //lhcbStyle->SetOptFit(0);
  ////lhcbStyle->SetOptFit(1011); // show probability, parameters and errors
  
  //// look of the statistics box:
  getInfosRegion()->getTextStyle()->color.setValue(cmap.getColor(1));
  //lhcbStyle->SetStatBorderSize(1);
  getInfosRegion()->getViewportRegion()->horizontalBorder.setValue(lhcbWidth);
  getInfosRegion()->getViewportRegion()->verticalBorder.setValue(lhcbWidth);
  getInfosRegion()->getViewportRegion()->horizontalShadow.setValue(0);
  getInfosRegion()->getViewportRegion()->verticalShadow.setValue(0);
  getInfosRegion()->getLineStyle()->width.setValue(lhcbWidth);
  //lhcbStyle->SetStatFont(lhcbFont);
  getInfosRegion()->getTextStyle()->fontName.setValue(lhcbFont);
  getInfosRegion()->getTextStyle()->smoothing.setValue(smoothing);
  //lhcbStyle->SetStatFontSize(0.05);
  //lhcbStyle->SetStatX(0.9);
  //lhcbStyle->SetStatY(0.9);
  infosRegionOrigin.setValue(SbVec2f(0.1F,0.1F)); //PERCENT : ur info to ur plotter.
  //lhcbStyle->SetStatW(0.25);
  //lhcbStyle->SetStatH(0.15);
  infosRegionSize.setValue(SbVec2f(0.25F,0.15F)); //height automatic.
  
  //// put tick marks on top and RHS of plots
  //lhcbStyle->SetPadTickX(1);
  soPlotter->rightAxisVisible.setValue(TRUE);
  //lhcbStyle->SetPadTickY(1);
  soPlotter->topAxisVisible.setValue(TRUE);
  
  //// histogram divisions: only 5 in x to avoid label overlaps
  //lhcbStyle->SetNdivisions(505,"x");
  xAxisNode->divisions.setValue(505);
  //lhcbStyle->SetNdivisions(510,"y");
  yAxisNode->divisions.setValue(510);
  
  //gROOT->SetStyle("lhcbStyle");
  //gROOT->ForceStyle();

  //TPaveText *lhcbName = new TPaveText(0.65,0.8,0.9,0.9,"BRNDC");
  //lhcbName->SetFillColor(0);
  //lhcbName->SetTextAlign(12);
  //lhcbName->SetBorderSize(0);
  //lhcbName->AddText("LHCb");
  
  //TText *lhcbLabel = new TText();
  //lhcbLabel->SetTextFont(lhcbFont);
  //lhcbLabel->SetTextColor(1);
  //lhcbLabel->SetTextSize(0.04);
  //lhcbLabel->SetTextAlign(12);
  
  //TLatex *lhcbLatex = new TLatex();
  //lhcbLatex->SetTextFont(lhcbFont);
  //lhcbLatex->SetTextColor(1);
  //lhcbLatex->SetTextSize(0.04);
  //lhcbLatex->SetTextAlign(12);
}

void SoPlotterRegion::setStylePAW_Default(){
  // See also Lab/source/PAW/PAW_Manager.cxx/regionSetLayout.
  plotterViewportMapping.setValue(LEAVE_ALONE);

  infosRegionAttachedToDataFrame.setValue(TRUE);
  infosRegionOrigin.setValue(SbVec2f(0.0F,0.0F));

  getViewportRegion()->background.setValue(TRUE);
  getViewportRegion()->backgroundColor.setValue(SbColor(1,1,1));

  SoPlotter* soPlotter = getPlotter();
  //SoTextRegion* soTitleRegion = (SoTextRegion*)titleRegion.getValue();
  SoLegendRegion* soLegendRegion = (SoLegendRegion*)legendRegion.getValue();

  // Plotter :
  soPlotter->titleUp.setValue(FALSE);
  soPlotter->getWallStyle()->visible.setValue(FALSE);
  soPlotter->getGridStyle()->visible.setValue(FALSE);
  soPlotter->getInnerFrameStyle()->visible.setValue(TRUE);
  soPlotter->rightAxisVisible.setValue(FALSE);
  soPlotter->topAxisVisible.setValue(FALSE);

  soPlotter->valueTopMargin.setValue(0.1F);

  soPlotter->colorMappingVisible.setValue(FALSE);

  xAxisNode->modeling.setValue("hplot");
  yAxisNode->modeling.setValue("hplot");
  zAxisNode->modeling.setValue("hplot");
  xAxisNode->getTitleStyle()->encoding.setValue("PAW");
  yAxisNode->getTitleStyle()->encoding.setValue("PAW");
  zAxisNode->getTitleStyle()->encoding.setValue("PAW");

  soPlotter->setAxesColor(SbColor(0,0,0));
  SbString font = SbFont_Hershey;
  //TFON
  soPlotter->getTitleStyle()->fontName.setValue(font);
  //VFON
  xAxisNode->getLabelsStyle()->fontName.setValue(font);
  yAxisNode->getLabelsStyle()->fontName.setValue(font);
  zAxisNode->getLabelsStyle()->fontName.setValue(font);
  //LFONT
  xAxisNode->getTitleStyle()->fontName.setValue(font);
  yAxisNode->getTitleStyle()->fontName.setValue(font);
  zAxisNode->getTitleStyle()->fontName.setValue(font);

  xAxisNode->titleToAxisJustification.setValue(SoAxis::RIGHT);
  yAxisNode->titleToAxisJustification.setValue(SoAxis::RIGHT);
  zAxisNode->titleToAxisJustification.setValue(SoAxis::RIGHT);

  titleRegionVisible.setValue(FALSE);
  // Infos region :
  infosRegionResizePolicy.setValue("ASPECT_RATIO");
  infosRegionStyle.setValue("PAW");
  SoInfosRegion* soInfosRegion = (SoInfosRegion*)infosRegion.getValue();
  soInfosRegion->getTextStyle()->fontName.setValue(font);
  soInfosRegion->getViewportRegion()->backgroundColor.setValue(SbColor_white);
  soInfosRegion->getViewportRegion()->horizontalShadow.setValue(0);
  soInfosRegion->getViewportRegion()->verticalShadow.setValue(0);

  soInfosRegion->inBetweenSpaces.setValue(10);
  legendRegionVisible.setValue(FALSE);
  soLegendRegion->getViewportRegion()->horizontalShadow.setValue(0);
  soLegendRegion->getViewportRegion()->verticalShadow.setValue(0);

  float XSIZ = 20;
  float YSIZ = 20;
  soPlotter->width.setValue(XSIZ);
  soPlotter->height.setValue(YSIZ);
  float XMGL = 2;
  float XMGR = 2;
  soPlotter->leftMargin.setValue(XMGL);
  soPlotter->rightMargin.setValue(XMGR);
  float YMGL = 2;
  float YMGU = 2;
  soPlotter->bottomMargin.setValue(YMGL);
  soPlotter->topMargin.setValue(YMGU);

  float ZSIZ = 20;  
  soPlotter->depth.setValue(ZSIZ);
  float ZMGD = 2;
  float ZMGU = 2;
  soPlotter->downMargin.setValue(ZMGD);
  soPlotter->upMargin.setValue(ZMGU);

  float YHTI = 1.20F;
  float TSIZ = 0.28F;

  SbString HTIT = "HTIT"; //UTIT
  if(HTIT=="HTIT") {
    soPlotter->getTitleStyle()->visible.setValue(TRUE);
    soPlotter->titleAutomated.setValue(TRUE);
    soPlotter->titleUp.setValue(FALSE);
    soPlotter->titleHeight.setValue(TSIZ);
    soPlotter->titleToAxis.setValue(YHTI);
  } else if(HTIT=="UTIT") {
    SbString UTIT = "";
    if(UTIT=="") {
      soPlotter->getTitleStyle()->visible.setValue(FALSE);
    } else {
      soPlotter->getTitleStyle()->visible.setValue(TRUE);
      soPlotter->titleAutomated.setValue(FALSE);
      soPlotter->titleUp.setValue(FALSE);
      soPlotter->titleHeight.setValue(TSIZ);
      soPlotter->titleToAxis.setValue(YHTI);
      soPlotter->title.setValue(UTIT);
    }
  }

  // Axis title SIZe
  float ASIZ = 0.28F;
  xAxisNode->titleHeight.setValue(ASIZ);
  yAxisNode->titleHeight.setValue(ASIZ);

  // y length of X axis ticks. Def 0.015 
  float XTIC = 0.30F;
  xAxisNode->tickLength.setValue(XTIC);

  // x length of Y axis ticks. Def 0.015 
  float YTIC = 0.30F;
  yAxisNode->tickLength.setValue(YTIC);

  // y distance of x tick label to data frame. Def 0.02 
  float YVAL = 0.20F;
  xAxisNode->labelToAxis.setValue(YVAL);

  // x distance of y tick label to data frame. Def 0.02 
  float XVAL = 0.40F;
  yAxisNode->labelToAxis.setValue(XVAL);

  // y distance of x title to data frame. Def 0.04  
  float YLAB = 0.80F;
  xAxisNode->titleToAxis.setValue(YLAB);

  // x distance of y title to data frame. Def 0.07  
  float XLAB = 1.40F;
  yAxisNode->titleToAxis.setValue(XLAB);

  // Axis labels SIZe. Def : 0.28F.
  float VSIZ = 0.28F;
  xAxisNode->labelHeight.setValue(VSIZ);
  yAxisNode->labelHeight.setValue(VSIZ);
  zAxisNode->labelHeight.setValue(VSIZ);

  SbString LINX = "LINX"; //LOGX
  if(LINX=="LOGX") {
    soPlotter->xAxisLogScale.setValue(TRUE);
  } else {
    soPlotter->xAxisLogScale.setValue(FALSE);
  }
  SbString LINY = "LINY"; //LOGY
  if(LINY=="LOGY") {
    soPlotter->yAxisLogScale.setValue(TRUE);
  } else {
    soPlotter->yAxisLogScale.setValue(FALSE);
  }
  SbString LINZ = "LINZ"; //LOGZ
  if(LINZ=="LOGZ") {
    soPlotter->zAxisLogScale.setValue(TRUE);
  } else {
    soPlotter->zAxisLogScale.setValue(FALSE);
  }

  float NDVX = 10510.0F;
  xAxisNode->divisions.setValue(int(NDVX));
  float NDVY = 10510.0F;
  yAxisNode->divisions.setValue(int(NDVY));
  float NDVZ = 10510.0F;
  zAxisNode->divisions.setValue(int(NDVZ));

  // Box line WIDth      
  float BWID = 1;
  //From graflib/higz/ikernel/islwsc.F
  BWID = float(SbMaximum(int(BWID/2.2),1)); 
  soPlotter->getInnerFrameStyle()->lineWidth.setValue(int(BWID));
  xAxisNode->getLineStyle()->width.setValue(int(BWID));
  yAxisNode->getLineStyle()->width.setValue(int(BWID));

  // Picture line WIDth  
  //float PWID = 1;

  // X ticks WIDth       
  float XWID = 1;
  //From graflib/higz/ikernel/islwsc.F
  XWID = float(SbMaximum(int(XWID/2.2),1));  
  xAxisNode->getTicksStyle()->width.setValue(int(XWID));

  // Y ticks WIDth       
  float YWID = 1;
  //From graflib/higz/ikernel/islwsc.F
  YWID = float(SbMaximum(int(YWID/2.2),1));
  yAxisNode->getTicksStyle()->width.setValue(int(YWID));

  // Color map WIDth     
  //float CWID = 0.50F;

  SbString NGRI = "NGRI"; //GRID
  if(NGRI=="NGRI") {
    soPlotter->getGridStyle()->visible.setValue(FALSE);
  } else {
    soPlotter->getGridStyle()->visible.setValue(TRUE);
  }

  int linen = 0;
  SbString opts;
  // STAT infos :
 {float STAT = 1111;
  //p120 : set STAT [OURMEI]  //default 1111
  // O=1 overflow
  // U=1 underflow
  // R=1 RMS
  // M=1 mean
  // E=1 entries
  // I=1 ID
  int code = (int)STAT;
  int O = code/100000;
  int U = (code - O * 100000)/10000; 
  int R = (code - O * 100000 - U * 10000)/1000; 
  int M = (code - O * 100000 - U * 10000 - R * 1000)/100; 
  int E = (code - O * 100000 - U * 10000 - R * 1000 - M *100)/10; 
  int I = (code - O * 100000 - U * 10000 - R * 1000 - M *100 - E*10)/1; 
  //printf("debug : stat : %d %d %d %d %d %d\n",O,U,R,M,E,I);
  if(I) {opts += "name ";linen++;}
  if(E) {opts += "entries ";linen++;}
  if(M) {opts += "mean ";linen++;}
  if(R) {opts += "rms ";linen++;}
  if(U) {opts += "underflow ";linen++;}
  if(O) {opts += "overflow ";}linen++;}
  // FIT infos :
 {float FIT = 101;
  // PAW set FIT [CEP] //default 101
  //    C = 1;  print Chisquare/Number of degress of freedom
  //    E = 1;  print errors (if e=1, v must be 1)
  //    P = 1;  print name/values of parameters
  //FIXME : from ROOT TPaveStats.
  //  The parameter mode can be = pcev  (default = 0111)
  //    v = 1;  print name/values of parameters
  //    e = 1;  print errors (if e=1, v must be 1)
  //    c = 1;  print Chisquare/Number of degrees of freedom
  //    p = 1;  print Probability
  //    When "v"=1 is specified, only the non-fixed parameters are shown.
  //    When "v"=2 all parameters are shown.
  //  Example: gStyle->SetOptFit(1011);
  //           print fit probability, parameter names/values and errors.
  int code = (int)FIT;
  int C = code/100;
  int E = (code - C * 100)/10; 
  int P = (code - C * 100 - E * 10)/1; 
  //printf("debug : fit : C %d E %d P %d\n",C,E,P);
  if(C) {
    opts += "fit_quality ";
    opts += "fit_ndf ";
    linen++;
  }
  if(E) {
    opts += "fit_errors ";
    linen++;
  }
  if(P) {opts += "fit_parameters ";}linen++;}

  SbString NSTA = "NSTA"; //STA
  SbString NFIT = "NFIT"; //FIT
  if( (NSTA=="STA") || (NFIT=="FIT") )  {
    infosRegionVisible.setValue(TRUE);
    soPlotter->getInfosStyle()->options.setValue(opts);
    //size of text in infos box.  
    // CSIZ is the height of hershey text in the infos box given in page unit.
    //float CSIZ = 0.28F;
  } else {
    infosRegionVisible.setValue(FALSE);
  }

  float HWID = 1;
  float FWID = 1;  // Function line WIDth 

  for(int index=0;index<10;index++) {
    // bins :
    soPlotter->getBinsStyle(index)->color.setValue(SbColor_black);
    soPlotter->getBinsStyle(index)->linePattern.setValue(SbLinePattern_solid);
    soPlotter->getBinsStyle(index)->markerStyle.setValue(SbMarkerCross);
    soPlotter->getBinsStyle(index)->modeling.setValue(SbModeling_top_lines);
    soPlotter->getBinsStyle(index)->lineWidth.setValue(int(HWID));

    soPlotter->getRightHatchStyle(index)->color.setValue(SbColor_black);
    soPlotter->getRightHatchStyle(index)->linePattern.setValue(SbLinePattern_solid);
    soPlotter->getRightHatchStyle(index)->lineWidth.setValue(1);
    soPlotter->getRightHatchStyle(index)->angle.setValue(FM_PI/4);
    soPlotter->getRightHatchStyle(index)->spacing.setValue(0.05F);
    soPlotter->getRightHatchStyle(index)->offset.setValue(0);

    soPlotter->getLeftHatchStyle(index)->color.setValue(SbColor_black);
    soPlotter->getLeftHatchStyle(index)->linePattern.setValue(SbLinePattern_solid);
    soPlotter->getLeftHatchStyle(index)->lineWidth.setValue(1);
    soPlotter->getLeftHatchStyle(index)->angle.setValue(FM_PI/4);
    soPlotter->getLeftHatchStyle(index)->spacing.setValue(0.05F);
    soPlotter->getLeftHatchStyle(index)->offset.setValue(0);

    soPlotter->getErrorsStyle(index)->visible.setValue(FALSE);

    // points :
    soPlotter->getPointsStyle(index)->color.setValue(SbColor_black);
    soPlotter->getPointsStyle(index)->linePattern.setValue(SbLinePattern_solid);
    soPlotter->getPointsStyle(index)->markerStyle.setValue(SbMarkerCross);
    soPlotter->getPointsStyle(index)->modeling.setValue(SbModeling_lines);
    soPlotter->getPointsStyle(index)->lineWidth.setValue(int(1)); //FIXME

    // functions :
    soPlotter->getFunctionStyle(index)->color.setValue(SbColor_black);
    soPlotter->getFunctionStyle(index)->linePattern.setValue(SbLinePattern_solid);
    soPlotter->getFunctionStyle(index)->lineWidth.setValue(int(FWID));

  }
}

void SoPlotterRegion::setStyleLab_Default(){
  titleRegionVisible.setValue(FALSE);
  infosRegionVisible.setValue(FALSE);
  infosRegionStyle.setValue("PAW");
  legendRegionVisible.setValue(FALSE);
  verbose.setValue(FALSE);
  plotterViewportMapping.setValue(ADJUST_VIEWPORT);
  infosRegionResizePolicy.setValue(RESIZE_HEIGHT);
  titleRegionOrigin.setValue(SbVec2f(0.01F,0.01F));
  titleRegionSize.setValue(SbVec2f(0.3F,0.05F));
  infosRegionOrigin.setValue(SbVec2f(0.01F,0.01F));
  infosRegionSize.setValue(SbVec2f(0.2F,0.16F));
  legendRegionOrigin.setValue(SbVec2f(0.01F,0.01F));
  legendRegionSize.setValue(SbVec2f(0.2F,0.16F));
  getViewportRegion()->background.setValue(TRUE);
  getViewportRegion()->backgroundColor.setValue(SbColor(1,1,1));
  infosRegionAttachedToDataFrame.setValue(FALSE);

  //getViewportRegion()->reset();

  // Sub regions :
  SoPlotter* soPlotter = getPlotter();
  soPlotter->resetStyles();

  SoTextRegion* soTitleRegion = (SoTextRegion*)titleRegion.getValue();
  soTitleRegion->clear();

  SoLegendRegion* soLegendRegion = (SoLegendRegion*)legendRegion.getValue();
  soLegendRegion->clear();

  SoInfosRegion* soInfosRegion = (SoInfosRegion*)infosRegion.getValue();
  soInfosRegion->clear();
}

void SoPlotterRegion::setStyleHippo_Default(){
  // See also Lab/source/PAW/PAW_Manager.cxx/regionSetLayout.
  setStyleLab_Default();

  // Plotter :
  SoPlotter* soPlotter = getPlotter();

  soPlotter->width.setValue(1);
  soPlotter->height.setValue(1);
  soPlotter->bottomMargin.setValue(0.12F);
  soPlotter->topMargin.setValue(0.16F);
  soPlotter->leftMargin.setValue(0.3F);
  soPlotter->rightMargin.setValue(0.16F);

  soPlotter->getWallStyle()->visible.setValue(FALSE);
  soPlotter->getGridStyle()->visible.setValue(FALSE);
  soPlotter->getInnerFrameStyle()->visible.setValue(TRUE);
  soPlotter->rightAxisVisible.setValue(TRUE);
  soPlotter->topAxisVisible.setValue(TRUE);
  //FIXME : justify center vs data frame.
  xAxisNode->getTitleStyle()->visible.setValue(TRUE);
  yAxisNode->getTitleStyle()->visible.setValue(TRUE);

  soPlotter->setFont("TTF/Helvetica",TRUE,FALSE);
  soPlotter->titleToAxis.setValue(0.1F); //def 0.06
  soPlotter->titleHeight.setValue(0.04F); //def 0.014

  // Axes :
  xAxisNode->labelToAxis.setValue(0.015F); //def 0.025
  xAxisNode->labelHeight.setValue(0.0175F * 1.5F);
  xAxisNode->titleToAxis.setValue(0.07F); //def 0.05
  xAxisNode->titleHeight.setValue(0.0175F * 2.0F);
  xAxisNode->titleToAxisJustification.setValue(SoAxis::CENTER);
  //xAxisNode->getMagStyle()->scale.setValue(1.5F);

  yAxisNode->labelHeight.setValue(0.0175F * 1.5F);
  yAxisNode->titleToAxis.setValue(0.2F); //def 0.05
  yAxisNode->titleHeight.setValue(0.0175F * 2.0F);
  yAxisNode->titleToAxisJustification.setValue(SoAxis::CENTER);
  //yAxisNode->getMagStyle()->scale.setValue(1.5F);

  float HWID = 1;
  float FWID = 1;
  for(int index=0;index<10;index++) {
    // bins :
    soPlotter->getBinsStyle(index)->color.setValue(SbColor_black);
    soPlotter->getBinsStyle(index)->linePattern.setValue(SbLinePattern_solid);
    soPlotter->getBinsStyle(index)->markerStyle.setValue(SbMarkerCross);
    soPlotter->getBinsStyle(index)->modeling.setValue(SbModeling_top_lines);
    soPlotter->getBinsStyle(index)->lineWidth.setValue(int(HWID));

    soPlotter->getRightHatchStyle(index)->color.setValue(SbColor_black);
    soPlotter->getRightHatchStyle(index)->linePattern.setValue(SbLinePattern_solid);
    soPlotter->getRightHatchStyle(index)->lineWidth.setValue(1);
    soPlotter->getRightHatchStyle(index)->angle.setValue(FM_PI/4);
    soPlotter->getRightHatchStyle(index)->spacing.setValue(0.05F);
    soPlotter->getRightHatchStyle(index)->offset.setValue(0);

    soPlotter->getLeftHatchStyle(index)->color.setValue(SbColor_black);
    soPlotter->getLeftHatchStyle(index)->linePattern.setValue(SbLinePattern_solid);
    soPlotter->getLeftHatchStyle(index)->lineWidth.setValue(1);
    soPlotter->getLeftHatchStyle(index)->angle.setValue(FM_PI/4);
    soPlotter->getLeftHatchStyle(index)->spacing.setValue(0.05F);
    soPlotter->getLeftHatchStyle(index)->offset.setValue(0);

    soPlotter->getErrorsStyle(index)->visible.setValue(FALSE);

    // points :
    soPlotter->getPointsStyle(index)->color.setValue(SbColor_black);
    soPlotter->getPointsStyle(index)->linePattern.setValue(SbLinePattern_solid);
    soPlotter->getPointsStyle(index)->markerStyle.setValue(SbMarkerCross);
    soPlotter->getPointsStyle(index)->modeling.setValue(SbModeling_lines);
    soPlotter->getPointsStyle(index)->lineWidth.setValue(int(1)); //FIXME

    // functions :
    soPlotter->getFunctionStyle(index)->color.setValue(SbColor_black);
    soPlotter->getFunctionStyle(index)->linePattern.setValue(SbLinePattern_solid);
    soPlotter->getFunctionStyle(index)->lineWidth.setValue(int(FWID));

  }
}
