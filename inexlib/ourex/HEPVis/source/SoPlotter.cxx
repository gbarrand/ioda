// this :
#include <HEPVis/nodekits/SoPlotter.h>

// Inventor :
#include <Inventor/SbClip.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>

// HEPVis :
// inline :
#include <HEPVis/SbMath.h>
#include <HEPVis/SbStyle.h>
#include <HEPVis/SbColors.h>
#include <HEPVis/SbPlottableThings.h>
#include <HEPVis/SbColormap.h>
#include <HEPVis/SbAxis.h>
#include <HEPVis/SbPrimitives.h>
#include <HEPVis/SbConvert.h>

#include <HEPVis/SbHatching.h>
#include <HEPVis/SbTessContour.h>
//#include <HEPVis/SbMutex.h>
#include <HEPVis/misc/SoStyleCache.h>

#include <HEPVis/nodes/SoSceneGraph.h>
#include <HEPVis/nodes/SoLineStyle.h>
#include <HEPVis/nodes/SoTextStyle.h>
#include <HEPVis/nodes/SoTextHershey.h>
#include <HEPVis/nodes/SoTextTTF.h>
#include <HEPVis/nodes/SoMarkerSet.h>
#include <HEPVis/nodes/SoGrid.h>
#include <HEPVis/nodes/SoPlottableStyles.h>
//#include <HEPVis/nodes/SoPolyhedron.h>

#include <HEPVis/nodekits/SoAxis.h>

#include <HEPVis/actions/SoAlternateRepAction.h>
#include <HEPVis/draggers/SoDragPoint2D.h>

#include <HEPVis/misc/SoTools2.h>

#include <inlib/pointer>
#include <inlib/clist_contour>

// Else :
//#define DEBUG
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

static float TakeLog(float aVal) {
  if(aVal<=0) {
    return -FLT_MAX;
  } else {
    return FLOG10(aVal);
  }
}

static float VerifyLog(float,float,float,SbBool);
static float VerifyLogInv(float,float,float,SbBool);
static void BarChart(float,float,float&,float&);
static void getHeight(int,const std::vector<SbPlottableBins1D*>&,const SbPList&,int,float&,float&);
static bool bins2DToFunc(const SbPlottableBins2D&,float,float,float&);

#define LIST_SET(list,number,x,y,z) list[number].setValue((x),(y),(z))

#define NotFound (-1)
#define NoLimit (-1)
#define xAxisNode ((SoAxis*)xAxis.getValue())
#define yAxisNode ((SoAxis*)yAxis.getValue())
#define zAxisNode ((SoAxis*)zAxis.getValue())
#define colorMappingAxisNode ((SoAxis*)colorMappingAxis.getValue())

inline std::string p2sx(void* a_pointer) {
  std::string sp;
  if(!inlib::p2sx(a_pointer,sp)){}
  return sp;
}

#define DUMP_UPDATE_WHAT(aString) \
  if(verbose.getValue()==TRUE) SoDebugError::postInfo("SoPlotter","%s : UPDATE WHAT : %s",p2sx(this).c_str(),(aString));

#define Cast(aObject,aClass) (aClass*)(aObject).cast(#aClass)
#define eq_Cast(aEq,aObject,aClass) \
  aClass* aEq = (aClass*)(aObject).cast(#aClass)

SO_KIT_SOURCE(SoPlotter) 

void SoPlotter::initClass(){
  SO_KIT_INIT_CLASS(SoPlotter,SoBaseKit,"BaseKit");
}

SoPlotter::SoPlotter()
:fDefault__wallStyle(0)
,fDefault__gridStyle(0)
,fDefault__innerFrameStyle(0)
,fDefault__infosStyle(0)
,fDefault__titleStyle(0)
,fStyleCache(0)
,fShape(-1)
,f_shape_Update(FALSE) //data
,f_bins_Update(FALSE) //data
,f_points_Update(FALSE) //data
,f_function_Update(FALSE) //data
,f_wall_Update(FALSE)
,f_grid_Update(FALSE)
,f_primitives_Update(FALSE)
,f_innerFrame_Update(FALSE)
,f_title_Update(FALSE)
,f_xAxis_Update(FALSE)
,f_yAxis_Update(FALSE)
,f_zAxis_Update(FALSE)
,f_colorMapping_Update(FALSE)
,f_layout_Update(FALSE)
,f_ttfScale_Update(FALSE)

,fOld_width(0)
,fOld_height(0)
,fOld_depth(0)
,fOld_leftMargin(0)
,fOld_rightMargin(0)
,fOld_bottomMargin(0)
,fOld_topMargin(0)
,fOld_downMargin(0)
,fOld_upMargin(0)
,fOld_titleUp(FALSE)
,fOld_titleToAxis(0)
,fOld_titleHeight(0)
,fOld_titleAutomated(FALSE)
,fOld_superposeBins(FALSE)
,fOld_colorMappingVisible(FALSE)
,fOld_colorMappingAxisLabeling(-1)
,fOld_colorMappingAttached(FALSE)
,fOld_wallEnforced(FALSE)
,fOld_gridEnforced(FALSE)
,fOld_primitivesEnforced(FALSE)
,fOld_innerFrameEnforced(FALSE)
,fOld_xAxisEnforced(FALSE)
,fOld_xAxisAutomated(FALSE)
,fOld_xAxisMinimum(-1)
,fOld_xAxisMaximum(-1)
,fOld_xAxisLogScale(FALSE)
,fOld_yAxisEnforced(FALSE)
,fOld_yAxisAutomated(FALSE)
,fOld_yAxisMinimum(-1)
,fOld_yAxisMaximum(-1)
,fOld_yAxisLogScale(FALSE)
,fOld_zAxisEnforced(FALSE)
,fOld_zAxisAutomated(FALSE)
,fOld_zAxisMinimum(-1)
,fOld_zAxisMaximum(-1)
,fOld_valueTopMargin(-1)
,fOld_zAxisLogScale(FALSE)
,fOld_topAxisVisible(FALSE)  
,fOld_rightAxisVisible(FALSE)  
,fOld_numberOfLevels(0)
,fOld_levels(0)
,fOld_shapeAutomated(FALSE)
,fOld_shape(-1)
,fOld_ttfScale(0)  
,fOld_theta(0)  
,fOld_phi(0)  
//,fMutex(0)
{
  SO_KIT_CONSTRUCTOR(SoPlotter);

  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0, FALSE);

  SO_KIT_ADD_CATALOG_ENTRY(colorMappingSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(colorMappingTransform,SoTransform,FALSE,colorMappingSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(colorMappingCellsSeparator,SoSeparator,FALSE,colorMappingSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(colorMappingAxisSeparator,SoSeparator,FALSE,colorMappingSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(colorMappingAxisTransform,SoTransform,FALSE,colorMappingAxisSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(colorMappingAxis,SoAxis,FALSE,colorMappingAxisSeparator, \0,TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(layout,SoTransform,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(titleSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(gridSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(wallSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(xAxisSeparator,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(xAxisTransform,SoTransform,FALSE,xAxisSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(xAxis,SoAxis,FALSE,xAxisSeparator, \0,TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(yAxisSeparator,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(yAxisTransform,SoTransform,FALSE,yAxisSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(yAxis,SoAxis,FALSE,yAxisSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(zAxisSeparator,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(zAxisTransform,SoTransform,FALSE,zAxisSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(zAxis,SoAxis,FALSE,zAxisSeparator, \0,TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(topAxisSeparator,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(rightAxisSeparator,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(etcSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);

  SO_KIT_ADD_CATALOG_ENTRY(dataSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(dataScale,SoScale,FALSE,dataSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(binsSeparator,SoSeparator,FALSE,dataSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(pointsSeparator,SoSeparator,FALSE,dataSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(functionSeparator,SoSeparator,FALSE,dataSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(errorsSeparator,SoSeparator,FALSE,dataSeparator,\0, TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(innerFrameSeparator,SoSeparator,FALSE,dataSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(etcDataSeparator,SoSeparator,FALSE,dataSeparator,\0,TRUE);
  // Styles :
  // must not be put under topSeparator because of alternate reps.
  SO_KIT_ADD_CATALOG_ENTRY(styleSeparator,SoSeparator,FALSE,this,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(wallStyle,SoStyle,FALSE,styleSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(gridStyle,SoStyle,FALSE,styleSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(innerFrameStyle,SoStyle,FALSE,styleSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(infosStyle,SoStyle,FALSE,styleSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(titleStyle,SoTextStyle,FALSE,styleSeparator,\0,TRUE);
  
  SO_KIT_ADD_CATALOG_LIST_ENTRY(binsStyle,SoSeparator,FALSE,styleSeparator,\0,SoBinsStyle,TRUE);
  SO_KIT_ADD_CATALOG_LIST_ENTRY(pointsStyle,SoSeparator,FALSE,styleSeparator,\0,SoPointsStyle,TRUE);
  SO_KIT_ADD_CATALOG_LIST_ENTRY(functionStyle,SoSeparator,FALSE,styleSeparator,\0,SoFunctionStyle,TRUE);
  SO_KIT_ADD_CATALOG_LIST_ENTRY(rightHatchStyle,SoSeparator,FALSE,styleSeparator,\0,SoRightHatchStyle,TRUE);
  SO_KIT_ADD_CATALOG_LIST_ENTRY(leftHatchStyle,SoSeparator,FALSE,styleSeparator,\0,SoLeftHatchStyle,TRUE);
  SO_KIT_ADD_CATALOG_LIST_ENTRY(errorsStyle,SoSeparator,FALSE,styleSeparator,\0,SoErrorsStyle,TRUE);
  SO_KIT_ADD_CATALOG_LIST_ENTRY(legendStyle,SoSeparator,FALSE,styleSeparator,\0,SoStyle,TRUE);
  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SO_NODE_ADD_FIELD(verbose,(FALSE));

  float xfac = 1.0F/20.0F; //0.05
  float yfac = 1.0F/20.0F; //0.05

  // Take PAW defaults :
  float XSIZ = 20 * xfac;    //1     //page width
  float YSIZ = 20 * yfac;    //1     //page height
  float XMGL = 2 * xfac;     //0.1   //left x margin (to data frame).
  float XMGR = 2 * xfac;     //0.1   //right y margin (to data frame).
  float YMGL = 2 * yfac;     //0.1   //low y margin (to data frame).
  float YMGU = 2 * yfac;     //0.1   //up y margin (to data frame).
  // Axes :
  //float VSIZ = 0.28F * yfac; //0.014 //tick label character size.
  //float XVAL = 0.4F * xfac;  //0.02 //x distance of y tick label to data frame.
  //float YVAL = 0.4F * yfac;  //0.02 //y distance of x tick label to data frame.
  //float XTIC = 0.3F * yfac;  //0.015 //y length of X axis ticks.
  //float YTIC = 0.3F * xfac;  //0.015 //x length of Y axis ticks.
  //float XLAB = 1.4F * xfac;  //0.07  //x distance of y title to data frame.
  //float YLAB = 0.8F * yfac;  //0.04  //y distance of x title to data frame.
  //float ASIZ = 0.28F * yfac; //0.014 // axis title (label) character size.

  float YHTI = 1.2F * yfac;  //0.06  //y distance of title to x axis.
  float TSIZ = 0.28F * yfac; //0.014 //title character size

  float zfac = 1.0F/20.0F; //0.05
  float ZSIZ = 20 * zfac;    //1     //page height
  float ZMGD = 2 * zfac;     //0.1   //low y margin (to data frame).
  float ZMGU = 2 * zfac;     //0.1   //up y margin (to data frame).

  // Data area :
  //float wData = XSIZ-XMGL-XMGR;
  //float hData = YSIZ-YMGL-YMGU;
  //float dData = ZSIZ-ZMGD-ZMGU;

  SO_NODE_ADD_FIELD(width,(XSIZ));
  SO_NODE_ADD_FIELD(height,(YSIZ));
  SO_NODE_ADD_FIELD(leftMargin,(XMGL));
  SO_NODE_ADD_FIELD(rightMargin,(XMGR));
  SO_NODE_ADD_FIELD(bottomMargin,(YMGL));
  SO_NODE_ADD_FIELD(topMargin,(YMGU));

  SO_NODE_ADD_FIELD(depth,(ZSIZ));
  SO_NODE_ADD_FIELD(downMargin,(ZMGD));
  SO_NODE_ADD_FIELD(upMargin,(ZMGU));

  SO_NODE_ADD_FIELD(titleUp,(TRUE));

  SO_NODE_ADD_FIELD(titleToAxis,(YHTI));
  SO_NODE_ADD_FIELD(titleHeight,(TSIZ));
  SO_NODE_ADD_FIELD(titleAutomated,(TRUE));
  SO_NODE_ADD_FIELD(title,(""));
  SO_NODE_ADD_FIELD(superposeBins,(FALSE));
  SO_NODE_ADD_FIELD(colorMappingVisible,(TRUE));
  SO_NODE_ADD_FIELD(colorMappingAxisLabeling,(CELLS));
  SO_NODE_ADD_FIELD(colorMappingAttached,(TRUE));//in case shape XYZ

  SO_NODE_ADD_FIELD(wallEnforced,(FALSE));
  SO_NODE_ADD_FIELD(gridEnforced,(FALSE));
  SO_NODE_ADD_FIELD(primitivesEnforced,(FALSE));
  SO_NODE_ADD_FIELD(innerFrameEnforced,(FALSE));

  SO_NODE_ADD_FIELD(xAxisLogScale,(FALSE));
  SO_NODE_ADD_FIELD(yAxisLogScale,(FALSE));
  SO_NODE_ADD_FIELD(zAxisLogScale,(FALSE));
  
  SO_NODE_ADD_FIELD(xAxisEnforced,(FALSE));
  SO_NODE_ADD_FIELD(xAxisAutomated,(TRUE));
  SO_NODE_ADD_FIELD(xAxisMinimum,(0));
  SO_NODE_ADD_FIELD(xAxisMaximum,(1));

  SO_NODE_ADD_FIELD(yAxisEnforced,(FALSE));
  SO_NODE_ADD_FIELD(yAxisAutomated,(TRUE));
  SO_NODE_ADD_FIELD(yAxisMinimum,(0));
  SO_NODE_ADD_FIELD(yAxisMaximum,(1));

  SO_NODE_ADD_FIELD(zAxisEnforced,(FALSE));
  SO_NODE_ADD_FIELD(zAxisAutomated,(TRUE));
  SO_NODE_ADD_FIELD(zAxisMinimum,(0));
  SO_NODE_ADD_FIELD(zAxisMaximum,(1));

  //PAW seems to have 0.1F and ROOT 0.05F
  SO_NODE_ADD_FIELD(valueTopMargin,(0.1F)); //percent 

  SO_NODE_ADD_FIELD(numberOfLevels,(10));
  SO_NODE_ADD_FIELD(levels,(0));
  levels.setNum(0);

  SO_NODE_ADD_FIELD(topAxisVisible,(FALSE));
  SO_NODE_ADD_FIELD(rightAxisVisible,(FALSE));

  SO_NODE_ADD_FIELD(ttfScale,(1));

  SO_NODE_ADD_FIELD(theta,(30));
  SO_NODE_ADD_FIELD(phi,(30));

  SO_NODE_ADD_FIELD(shapeAutomated,(TRUE));

  SO_NODE_ADD_FIELD(frozen,(FALSE));

  // Output fields :  
  SO_NODE_ADD_FIELD(shape,(XY));
  SO_NODE_ADD_FIELD(infos,(""));
  SO_NODE_ADD_FIELD(legend,(""));
  SO_NODE_ADD_FIELD(numberOfPlottedObject,(0));
  SO_NODE_ADD_FIELD(numberOfPlottedBins1D,(0));
  SO_NODE_ADD_FIELD(numberOfPlottedBins2D,(0));
  SO_NODE_ADD_FIELD(numberOfPlottedPoints2D,(0));
  SO_NODE_ADD_FIELD(numberOfPlottedPoints3D,(0));
  SO_NODE_ADD_FIELD(numberOfPlottedFunction1D,(0));
  SO_NODE_ADD_FIELD(numberOfPlottedFunction2D,(0));
  SO_NODE_ADD_FIELD(plottedObjectNames,(""));

  SO_NODE_DEFINE_ENUM_VALUE(Shape,XY);
  SO_NODE_DEFINE_ENUM_VALUE(Shape,XYZ);
  SO_NODE_SET_SF_ENUM_TYPE(shape,Shape);
  
  SO_NODE_DEFINE_ENUM_VALUE(ColorMappingAxisLabeling,CELLS);
  SO_NODE_DEFINE_ENUM_VALUE(ColorMappingAxisLabeling,MIN_MAX);
  SO_NODE_SET_SF_ENUM_TYPE(colorMappingAxisLabeling,ColorMappingAxisLabeling);
  
  setPAW_AxesTextSize(XSIZ,YSIZ);

  fDefault__wallStyle = new SoStyle;
  fDefault__wallStyle->ref();
  fDefault__wallStyle->setFromString("color lightgrey\nmodeling boxes");
  setPart("wallStyle",fDefault__wallStyle->copy());

  fDefault__gridStyle = new SoStyle;
  fDefault__gridStyle->ref();
  fDefault__gridStyle->setFromString("color black\nlinePattern dashed");
  setPart("gridStyle",fDefault__gridStyle->copy());

  fDefault__innerFrameStyle = new SoStyle;
  fDefault__innerFrameStyle->ref();
  fDefault__innerFrameStyle->setFromString("color black\nlinePattern solid");
  setPart("innerFrameStyle",fDefault__innerFrameStyle->copy());

  fDefault__infosStyle = new SoStyle;
  fDefault__infosStyle->ref();
  fDefault__infosStyle->setFromString("color black\nfontName Hershey\nencoding PAW\nsmoothing FALSE");
  fDefault__infosStyle->options.setValue
    ("name entries mean rms fit_quality fit_ndf fit_parameters");
  setPart("infosStyle",fDefault__infosStyle->copy());

  fDefault__titleStyle = new SoTextStyle;
  fDefault__titleStyle->ref();
  fDefault__titleStyle->setFromString("color black\nfontName Hershey\nencoding PAW\nsmoothing FALSE");
  setPart("titleStyle",fDefault__titleStyle->copy());

  fStyleCache = new SoStyleCache;
  fStyleCache->ref();

  plottedObjectNames.deleteValues(0);

  // Set a fancy value on some field of SbStyle 
  // so that the first doAction start correctly :
  fOld_wallStyle.setTransparency(-1);
  fOld_gridStyle.setTransparency(-1);
  fOld_innerFrameStyle.setTransparency(-1);
  fOld_infosStyle.setTransparency(-1);
  fOld_titleStyle.setTransparency(-1);

  fOld_xAxis_lineStyle.setTransparency(-1);
  fOld_xAxis_ticksStyle.setTransparency(-1);
  fOld_xAxis_labelsStyle.setTransparency(-1);
  fOld_xAxis_titleStyle.setTransparency(-1);
  fOld_xAxis_magStyle.setTransparency(-1);

  fOld_yAxis_lineStyle.setTransparency(-1);
  fOld_yAxis_ticksStyle.setTransparency(-1);
  fOld_yAxis_labelsStyle.setTransparency(-1);
  fOld_yAxis_titleStyle.setTransparency(-1);
  fOld_yAxis_magStyle.setTransparency(-1);

  fOld_zAxis_lineStyle.setTransparency(-1);
  fOld_zAxis_ticksStyle.setTransparency(-1);
  fOld_zAxis_labelsStyle.setTransparency(-1);
  fOld_zAxis_titleStyle.setTransparency(-1);
  fOld_zAxis_magStyle.setTransparency(-1);

  fOld_levels = new SoMFFloat;

  xAxisNode->width.setValue(0);
  yAxisNode->width.setValue(0);
  zAxisNode->width.setValue(0);
 {SoAxis* axis = (SoAxis*)colorMappingAxis.getValue();
  axis->width.setValue(0);}
}

SoPlotter::~SoPlotter(){
  //delete fMutex;
  //fMutex = 0;

  clearPlottableObjects();
  clearPlottablePrimitives();

 {std::vector<SbBaseColormap*>::iterator it;
  for(it=f_bins_cmaps.begin();it!=f_bins_cmaps.end();++it) delete *it;
  f_bins_cmaps.clear();}

 {std::vector<SbBaseColormap*>::iterator it;
  for(it=f_points_cmaps.begin();it!=f_points_cmaps.end();++it) delete *it;
  f_points_cmaps.clear();}

 {std::vector<SbBaseColormap*>::iterator it;
  for(it=f_function_cmaps.begin();it!=f_function_cmaps.end();++it) delete *it;
  f_function_cmaps.clear();}

  delete fOld_levels;

  fDefault__wallStyle->unref();
  fDefault__gridStyle->unref();
  fDefault__innerFrameStyle->unref();
  fDefault__infosStyle->unref();
  fDefault__titleStyle->unref();
  
  fStyleCache->unref();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static int sDoCounter = 0;

void SoPlotter::doAction(SoAction* aAction){
  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoPlotter::doAction","%s (%d).",p2sx(this).c_str(),sDoCounter++);
  }

  updateChildren();

  // aAction needs the SoModelMatrixElement and SoViewVolumeElement.
  if(aAction->isOfType(SoHandleEventAction::getClassTypeId())) return;
  if(aAction->isOfType(SoGetMatrixAction::getClassTypeId())) return;

  SO_ALTERNATEREP_DO_ACTION(aAction)

}
#define ACTION(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::doAction(action);\
  aSuper::aMethod(action);\
}
ACTION(SoPlotter,SoBaseKit,SoCallbackAction,callback)
ACTION(SoPlotter,SoBaseKit,SoGLRenderAction,GLRender)
ACTION(SoPlotter,SoBaseKit,SoGetBoundingBoxAction,getBoundingBox)
ACTION(SoPlotter,SoBaseKit,SoGetMatrixAction,getMatrix)
ACTION(SoPlotter,SoBaseKit,SoHandleEventAction,handleEvent)
ACTION(SoPlotter,SoBaseKit,SoRayPickAction,rayPick)
ACTION(SoPlotter,SoBaseKit,SoPickAction,pick)

#define ACTION2(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::updateChildren();\
  aSuper::aMethod(action);\
}
ACTION2(SoPlotter,SoBaseKit,SoSearchAction,search)

void SoPlotter::write(SoWriteAction* aAction){
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    //NOTE : the below logic loose the fact
    //       that we had a SoPlotter. 
    //       We may also introduce a coworking SoAltPlotter
    //       that has no polyhedron field, and arrange
    //       that the SoAlternateRepAction swaps 
    //       from SoPlotter to SoAltPlotter.
    altRep->write(aAction);
  } else {
    updateChildren();

    // freeze for writing.
    // When read back, the created SoPlotter 
    // will have the frozen field to TRUE and
    // then the updateChildren method will not
    // demolish the read scene.
    SbBool flag = frozen.getValue();
    frozen.setValue(TRUE);

    SoBaseKit::write(aAction);

    frozen.setValue(flag);
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SoTransform* SoPlotter::getLayout() const { return (SoTransform*)layout.getValue();}
SoScale* SoPlotter::getDataScale() const { return (SoScale*)dataScale.getValue();}
SoSeparator* SoPlotter::getEtcSeparator() const { return (SoSeparator*)etcSeparator.getValue();}
SoSeparator* SoPlotter::getTopSeparator() const { return (SoSeparator*)topSeparator.getValue();}
SoSeparator* SoPlotter::getEtcDataSeparator() const { return (SoSeparator*)etcDataSeparator.getValue();}
SoAxis* SoPlotter::getXAxis() const { return xAxisNode;}
SoAxis* SoPlotter::getYAxis() const { return yAxisNode;}
SoAxis* SoPlotter::getZAxis() const { return zAxisNode;}
SoStyleCache* SoPlotter::getStyleCache() const { return fStyleCache;}

SoAxis* SoPlotter::getColorMappingAxis() const { return colorMappingAxisNode;}

SoNodeKitListPart* SoPlotter::getLegendStyleList() const { return (SoNodeKitListPart*)legendStyle.getValue();}

SoStyle* SoPlotter::getWallStyle() const {return (SoStyle*)wallStyle.getValue();}
SoStyle* SoPlotter::getGridStyle() const {return (SoStyle*)gridStyle.getValue();}
SoStyle* SoPlotter::getInnerFrameStyle() const {return (SoStyle*)innerFrameStyle.getValue();}
SoStyle* SoPlotter::getInfosStyle() const {return (SoStyle*)infosStyle.getValue();}
SoTextStyle* SoPlotter::getTitleStyle() const {return (SoTextStyle*)titleStyle.getValue();}

//NOTE :
//  doing a :
//    getPart("binsStyle[1]",TRUE)
//  without having done the same with [0]
//  returns 0 !

#define SO_GET_STYLE(aMethod,aStyle,aStyleNode) \
SoStyle* SoPlotter::aMethod(int aIndex) {\
  if(aIndex<0) return 0;\
  SoGroup* group = getContainerNode(#aStyle);\
  int number = group->getNumChildren();\
  if(aIndex>=number) {\
    for(int index=number;index<=aIndex;index++) {\
      group->addChild(new aStyleNode);\
    }\
  }\
  return (SoStyle*)group->getChild(aIndex);\
}


SO_GET_STYLE(getBinsStyle,binsStyle,SoBinsStyle)
SO_GET_STYLE(getPointsStyle,pointsStyle,SoPointsStyle)
SO_GET_STYLE(getFunctionStyle,functionStyle,SoFunctionStyle)
SO_GET_STYLE(getRightHatchStyle,rightHatchStyle,SoRightHatchStyle)
SO_GET_STYLE(getLeftHatchStyle,leftHatchStyle,SoLeftHatchStyle)
SO_GET_STYLE(getErrorsStyle,errorsStyle,SoErrorsStyle)

#define SO_MERGE_STYLE(aMethod) \
SoStyle* SoPlotter::merge##aMethod(int aIndex,SbPlottableObject& aSb) {\
  SoStyle* soStyle = (SoStyle*)get##aMethod(aIndex);\
  if(soStyle->automated.getValue()==FALSE){\
    soStyle->ref();\
    return soStyle;\
  }\
  SbString sinfos = aSb.getInfos(Sb_plotter);\
  if(!sinfos.getLength()){\
    soStyle->ref();\
    return soStyle;\
  }\
  SbStyle sbStyle = soStyle->getSbStyle();\
  if(sbStyle.setFromString(sinfos)==FALSE){\
    soStyle->ref();\
    return soStyle;\
  }\
  if(sbStyle.isEqual(soStyle->getSbStyle())==TRUE){\
    soStyle->ref();\
    return soStyle;\
  }\
  soStyle = (SoStyle*)soStyle->copy();\
  soStyle->set(sbStyle);\
  soStyle->ref();\
  return soStyle;\
}

SO_MERGE_STYLE(BinsStyle)
SO_MERGE_STYLE(PointsStyle)
SO_MERGE_STYLE(FunctionStyle)

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#define SO_IS_STYLE_CHANGED(aMethod,aStyle,aOld) \
SbBool SoPlotter::aMethod(){\
  SoGroup* group = getContainerNode(#aStyle);\
  int number = group->getNumChildren();\
  if(int(aOld.size())!=number) return TRUE;\
  for(int index=0;index<number;index++) {\
    SoStyle* soStyle = (SoStyle*)group->getChild(index);\
    if(aOld[index].isEqual(soStyle->getSbStyle())==FALSE) {\
      return TRUE;\
      break;\
    }\
  }\
  return FALSE;\
}

SO_IS_STYLE_CHANGED(isBinsStyleChanged,binsStyle,fOld_binsStyle)
SO_IS_STYLE_CHANGED(isPointsStyleChanged,pointsStyle,fOld_pointsStyle)
SO_IS_STYLE_CHANGED(isFunctionStyleChanged,functionStyle,fOld_functionStyle)
SO_IS_STYLE_CHANGED(isRightHatchStyleChanged,rightHatchStyle,fOld_rightHatchStyle)
SO_IS_STYLE_CHANGED(isLeftHatchStyleChanged,leftHatchStyle,fOld_leftHatchStyle)
SO_IS_STYLE_CHANGED(isErrorsStyleChanged,errorsStyle,fOld_errorsStyle)

#define SO_SET_OLD_STYLE(aMethod,aStyle,aOld) \
void SoPlotter::aMethod(){\
  SoGroup* group = getContainerNode(#aStyle);\
  int number = group->getNumChildren();\
  aOld.resize(number);\
  for(int index=0;index<number;index++) {\
    SoStyle* soStyle = (SoStyle*)group->getChild(index);\
    aOld[index] = soStyle->getSbStyle();\
  }\
}

SO_SET_OLD_STYLE(setOldBinsStyle,binsStyle,fOld_binsStyle)
SO_SET_OLD_STYLE(setOldPointsStyle,pointsStyle,fOld_pointsStyle)
SO_SET_OLD_STYLE(setOldFunctionStyle,functionStyle,fOld_functionStyle)
SO_SET_OLD_STYLE(setOldRightHatchStyle,rightHatchStyle,fOld_rightHatchStyle)
SO_SET_OLD_STYLE(setOldLeftHatchStyle,leftHatchStyle,fOld_leftHatchStyle)
SO_SET_OLD_STYLE(setOldErrorsStyle,errorsStyle,fOld_errorsStyle)

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

const SbPList& SoPlotter::getPlottableObjects() const{return fPlottableObjects;}

void SoPlotter::clearPlottableObjects(){
  int objn = fPlottableObjects.getLength();
  for(int index=0;index<objn;index++) {
    SbPlottableObject* object = (SbPlottableObject*)fPlottableObjects[index];
    if(object) delete object;
  }
  fPlottableObjects.truncate(0);

  f_binsList.clear();
  f_pointsList.clear();
  f_functionList.clear();
  f_bins1DList.clear();
  f_bins2DList.clear();
  f_points2DList.clear();
  f_points3DList.clear();
  f_function1DList.clear();
  f_function2DList.clear();

  numberOfPlottedObject.setValue(0);
  numberOfPlottedBins1D.setValue(0);
  numberOfPlottedBins2D.setValue(0);
  numberOfPlottedPoints2D.setValue(0);
  numberOfPlottedPoints3D.setValue(0);
  numberOfPlottedFunction1D.setValue(0);
  numberOfPlottedFunction2D.setValue(0);
  plottedObjectNames.deleteValues(0);

  if(objn) {
    // If a title (logScale) had been given on some axis,
    // it is probably no more pertinent for further data.
    if(xAxisEnforced.getValue()==FALSE)  {
      xAxisNode->title.setValue("");
      xAxisLogScale.setValue(FALSE);
    }
    if(yAxisEnforced.getValue()==FALSE)  {
      yAxisNode->title.setValue("");
      yAxisLogScale.setValue(FALSE);
    }
    if(zAxisEnforced.getValue()==FALSE)  {
      zAxisNode->title.setValue("");
      zAxisLogScale.setValue(FALSE);
    }
  }

  DUMP_UPDATE_WHAT("clearPlottableObjects")

  f_bins_Update = TRUE; 
  f_points_Update = TRUE;
  f_function_Update = TRUE;
}

void SoPlotter::setPlottableObject(SbPlottableObject* aPlottableObject){
  // Given thing deleted by the plotter.
  // Giving 0 clear the plotter.
  clearPlottableObjects();
  SbPList list; // An empty list.
  if(aPlottableObject) list.append(aPlottableObject);
  setPlottableObjects(list);
}

SbBool SoPlotter::isEmpty() const{return ( fPlottableObjects.getLength()<=0 ? TRUE : FALSE );}

void SoPlotter::addPlottableObject(SbPlottableObject* aPlottableObject){
  // SoPlotter takes ownership of aPlottableObject
  SbPList list(fPlottableObjects);
  list.append(aPlottableObject);
  setPlottableObjects(list);
}

void SoPlotter::removePlottableObject(SbPlottableObject* aPlottableObject){
  SbPList list(fPlottableObjects);
  int index = list.find(aPlottableObject);
  if(index==NotFound) return;
  list.remove(index);
  delete aPlottableObject;
  setPlottableObjects(list);
}

void SoPlotter::removePlottableObjectsOfClass(const SbString& aClass){
  SbPList list;
  int objn = fPlottableObjects.getLength();
  for(int index=0;index<objn;index++) {
    SbPlottableObject* object = (SbPlottableObject*)fPlottableObjects[index];
    if(!object) continue; 
    if(!object->isValid()) { 
      // Could be ok if multiple SbPlottable on the same data.
      delete object;
      continue; 
    }
    if(object->cast(aClass.getString())) {
      delete object;
    } else {
      list.append(object);
    }
  }
  setPlottableObjects(list);
}

void SoPlotter::setPlottableObjects(const SbPList& aPlottableObjects){

  //  Main method to declare to the plotter a list of things to represent.
  //  Given thing in the list is deleted by the plotter.

  fPlottableObjects.truncate(0);

  plottedObjectNames.deleteValues(0);
 {int objn = aPlottableObjects.getLength();
  for(int index=0;index<objn;index++) {
    SbPlottableObject* object = (SbPlottableObject*)aPlottableObjects[index];
    if(!object) continue; 
    if(!object->isValid()) {delete object;continue;}
    fPlottableObjects.append(object);
    const char* name = object->getName();
    plottedObjectNames.set1Value(plottedObjectNames.getNum(),SbString(name));
  }
  numberOfPlottedObject.setValue(plottedObjectNames.getNum());}

  set_lists(FALSE);

  numberOfPlottedBins1D.setValue(f_bins1DList.size());
  numberOfPlottedBins2D.setValue(f_bins2DList.size());
  numberOfPlottedPoints2D.setValue(f_points2DList.size());
  numberOfPlottedPoints3D.setValue(f_points3DList.size());
  numberOfPlottedFunction1D.setValue(f_function1DList.size());
  numberOfPlottedFunction2D.setValue(f_function2DList.size());

  DUMP_UPDATE_WHAT("setPlottableObjects")

  f_shape_Update = TRUE;
}

void SoPlotter::set_lists(SbBool aCheckValid){
  // To minimize the number of casts and isValid() in updateChildren().

  f_binsList.clear();
  f_pointsList.clear();
  f_functionList.clear();
  f_bins1DList.clear();
  f_bins2DList.clear();
  f_points2DList.clear();
  f_points3DList.clear();
  f_function1DList.clear();
  f_function2DList.clear();

  int objn = fPlottableObjects.getLength();
  for(int index=0;index<objn;index++) {
    SbPlottableObject* object = (SbPlottableObject*)fPlottableObjects[index];
    if(!object) continue; 
    //const char* name = object->getName();
    if(aCheckValid && !object->isValid()) {
      //uuuu SoDebugError::postInfo
      //("SoPlotter::setPlottableObjects","object \"%s\" not valid.",name);
      delete object;
      fPlottableObjects.set(index,0);
      continue; 
    }
    if(eq_Cast(h1,*object,SbPlottableBins1D)) {
      f_binsList.push_back(object);
      f_bins1DList.push_back(h1);
    } else if(eq_Cast(h2,*object,SbPlottableBins2D)) {
      f_binsList.push_back(object);
      f_bins2DList.push_back(h2);
    } else if(eq_Cast(c2,*object,SbPlottablePoints2D)) {
      f_pointsList.push_back(object);
      f_points2DList.push_back(c2);
    } else if(eq_Cast(c3,*object,SbPlottablePoints3D)) {
      f_pointsList.push_back(object);
      f_points3DList.push_back(c3);
    } else if(eq_Cast(f1,*object,SbPlottableFunction1D)) {
      f_functionList.push_back(object);
      f_function1DList.push_back(f1);
    } else if(eq_Cast(f2,*object,SbPlottableFunction2D)) {
      f_functionList.push_back(object);
      f_function2DList.push_back(f2);
    } else {
      SoDebugError::postInfo
        ("SoPlotter::setPlottableObjects","unkown plottable object type.");
    }
  }
}

SbBool SoPlotter::isUpdateFlagRaised(UpdateFlag aWhat){
  if((aWhat==SHAPE)&&(f_shape_Update==TRUE)) return TRUE;
  if((aWhat==BINS)&&(f_bins_Update==TRUE)) return TRUE;
  if((aWhat==POINTS)&&(f_points_Update==TRUE)) return TRUE;
  if((aWhat==FUNCTION)&&(f_function_Update==TRUE)) return TRUE;
  if((aWhat==WALL)&&(f_wall_Update==TRUE)) return TRUE;
  if((aWhat==GRID)&&(f_grid_Update==TRUE)) return TRUE;
  if((aWhat==PRIMITIVES)&&(f_primitives_Update==TRUE)) return TRUE;
  if((aWhat==INNER_FRAME)&&(f_innerFrame_Update==TRUE)) return TRUE;
  if((aWhat==TITLE)&&(f_title_Update==TRUE)) return TRUE;
  if((aWhat==X_AXIS)&&(f_xAxis_Update==TRUE)) return TRUE;
  if((aWhat==Y_AXIS)&&(f_yAxis_Update==TRUE)) return TRUE;
  if((aWhat==Z_AXIS)&&(f_zAxis_Update==TRUE)) return TRUE;
  if((aWhat==COLOR_MAPPING)&&(f_colorMapping_Update==TRUE)) return TRUE;
  if((aWhat==LAYOUT)&&(f_layout_Update==TRUE)) return TRUE;
  if((aWhat==TTF_SCALE)&&(f_ttfScale_Update==TRUE)) return TRUE;
  return FALSE;
}

void SoPlotter::raiseUpdateFlag(UpdateFlag aWhat){
  // globals :
  if(aWhat==NONE) { //do nothing. Used in OnXLab.

  } if(aWhat==ALL) {
    f_shape_Update = TRUE;
    f_bins_Update = TRUE;
    f_points_Update = TRUE;
    f_function_Update = TRUE;
    f_wall_Update = TRUE;
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
    f_innerFrame_Update = TRUE;
    f_title_Update = TRUE;
    f_xAxis_Update = TRUE;
    f_yAxis_Update = TRUE;
    f_zAxis_Update = TRUE;
    f_colorMapping_Update = TRUE;
    f_layout_Update = TRUE;
    f_ttfScale_Update = TRUE;

  } if(aWhat==RESET) {
    f_shape_Update = FALSE;
    f_bins_Update = FALSE;
    f_points_Update = FALSE;
    f_function_Update = FALSE;
    f_wall_Update = FALSE;
    f_grid_Update = FALSE;
    f_primitives_Update = FALSE;
    f_innerFrame_Update = FALSE;
    f_title_Update = FALSE;
    f_xAxis_Update = FALSE;
    f_yAxis_Update = FALSE;
    f_zAxis_Update = FALSE;
    f_colorMapping_Update = FALSE;
    f_layout_Update = FALSE;
    f_ttfScale_Update = FALSE;

  // per scene components :
  } if(aWhat==SHAPE) {
    f_shape_Update = TRUE;
  } else if(aWhat==BINS) {
    f_bins_Update = TRUE;
  } else if(aWhat==POINTS) {
    f_points_Update = TRUE;
  } else if(aWhat==FUNCTION) {
    f_function_Update = TRUE;
  } else if(aWhat==WALL) {
    f_wall_Update = TRUE;
  } else if(aWhat==GRID) {
    f_grid_Update = TRUE;
  } else if(aWhat==PRIMITIVES) {
    f_primitives_Update = TRUE;
  } else if(aWhat==INNER_FRAME) {
    f_innerFrame_Update = TRUE;
  } else if(aWhat==TITLE) {
    f_title_Update = TRUE;
  } else if(aWhat==X_AXIS) {
    f_xAxis_Update = TRUE;
  } else if(aWhat==Y_AXIS) {
    f_yAxis_Update = TRUE;
  } else if(aWhat==Z_AXIS) {
    f_zAxis_Update = TRUE;
  } else if(aWhat==COLOR_MAPPING) {
    f_colorMapping_Update = TRUE;
  } else if(aWhat==LAYOUT) {
    f_layout_Update = TRUE;
  } else if(aWhat==TTF_SCALE) {
    f_ttfScale_Update = TRUE;
  }
}

SbBool SoPlotter::someFlagRaised() const {
  if(f_shape_Update==TRUE) return TRUE;
  if(f_bins_Update==TRUE) return TRUE;
  if(f_points_Update==TRUE) return TRUE;
  if(f_function_Update==TRUE) return TRUE;
  if(f_wall_Update==TRUE) return TRUE;
  if(f_grid_Update==TRUE) return TRUE;
  if(f_primitives_Update==TRUE) return TRUE;
  if(f_innerFrame_Update==TRUE) return TRUE;
  if(f_title_Update==TRUE) return TRUE;
  if(f_xAxis_Update==TRUE) return TRUE;
  if(f_yAxis_Update==TRUE) return TRUE;
  if(f_zAxis_Update==TRUE) return TRUE;
  if(f_colorMapping_Update==TRUE) return TRUE;
  if(f_layout_Update==TRUE) return TRUE;
  if(f_ttfScale_Update==TRUE) return TRUE;
  return FALSE;
}


void SoPlotter::raiseAllUpdateFlags(){raiseUpdateFlag(ALL);}

void SoPlotter::resetAllUpdateFlags(){raiseUpdateFlag(RESET);}

void SoPlotter::updateWhat(){
  if(fOld_width!=width.getValue()) {
    DUMP_UPDATE_WHAT("width")
    fOld_width = width.getValue();
    f_xAxis_Update = TRUE;
    f_wall_Update = TRUE;
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
    f_colorMapping_Update = TRUE;
    f_title_Update = TRUE;
    f_layout_Update = TRUE;
    if(rightAxisVisible.getValue()==TRUE) {
      f_yAxis_Update = TRUE; //FIXME : we should have a f_rightAxis_Update
    }    
  }    
  if(fOld_height!=height.getValue()) {
    DUMP_UPDATE_WHAT("height")
    fOld_height = height.getValue();
    f_yAxis_Update = TRUE;
    f_wall_Update = TRUE;
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
    f_colorMapping_Update = TRUE;
    f_title_Update = TRUE;
    f_layout_Update = TRUE;
    if(topAxisVisible.getValue()==TRUE) {
      f_xAxis_Update = TRUE; //FIXME : we should have a f_topAxis_Update
    }    
  }    
  if(fOld_depth!=depth.getValue()) {
    DUMP_UPDATE_WHAT("depth")
    fOld_depth = depth.getValue();
    f_zAxis_Update = TRUE;
    f_wall_Update = TRUE;
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
    f_colorMapping_Update = TRUE;
    f_layout_Update = TRUE;
  }    
  if(fOld_leftMargin!=leftMargin.getValue()) {
    DUMP_UPDATE_WHAT("leftMargin")
    fOld_leftMargin = leftMargin.getValue();
    f_xAxis_Update = TRUE;
    f_wall_Update = TRUE;
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
    f_colorMapping_Update = TRUE;
    f_title_Update = TRUE;
    f_layout_Update = TRUE;
    if(rightAxisVisible.getValue()==TRUE) {
      f_yAxis_Update = TRUE; //FIXME : we should have a f_rightAxis_Update
    }    
  }    
  if(fOld_rightMargin!=rightMargin.getValue()) {
    DUMP_UPDATE_WHAT("rightMargin")
    fOld_rightMargin = rightMargin.getValue();
    f_xAxis_Update = TRUE;
    f_wall_Update = TRUE;
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
    f_colorMapping_Update = TRUE;
    f_title_Update = TRUE;
    f_layout_Update = TRUE;
    if(rightAxisVisible.getValue()==TRUE) {
      f_yAxis_Update = TRUE; //FIXME : we should have a f_rightAxis_Update
    }    
  }    
  if(fOld_bottomMargin!=bottomMargin.getValue()) {
    DUMP_UPDATE_WHAT("bottomMargin")
    fOld_bottomMargin = bottomMargin.getValue();
    f_yAxis_Update = TRUE;
    f_wall_Update = TRUE;
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
    f_colorMapping_Update = TRUE;
    f_title_Update = TRUE;
    f_layout_Update = TRUE;
    if(topAxisVisible.getValue()==TRUE) {
      f_xAxis_Update = TRUE; //FIXME : we should have a f_topAxis_Update
    }    
  }    
  if(fOld_topMargin!=topMargin.getValue()) {
    DUMP_UPDATE_WHAT("topMargin")
    fOld_topMargin = topMargin.getValue();
    f_yAxis_Update = TRUE;
    f_wall_Update = TRUE;
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
    f_colorMapping_Update = TRUE;
    f_title_Update = TRUE;
    f_layout_Update = TRUE;
    if(topAxisVisible.getValue()==TRUE) {
      f_xAxis_Update = TRUE; //FIXME : we should have a f_topAxis_Update
    }    
  }    
  if(fOld_downMargin!=downMargin.getValue()) {
    DUMP_UPDATE_WHAT("downMargin")
    fOld_downMargin = downMargin.getValue();
    f_zAxis_Update = TRUE;
    f_wall_Update = TRUE;
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
    f_colorMapping_Update = TRUE;
    f_title_Update = TRUE;
    f_layout_Update = TRUE;
  }    
  if(fOld_upMargin!=upMargin.getValue()) {
    DUMP_UPDATE_WHAT("upMargin")
    fOld_upMargin = upMargin.getValue();
    f_zAxis_Update = TRUE;
    f_wall_Update = TRUE;
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
    f_colorMapping_Update = TRUE;
    f_title_Update = TRUE;
    f_layout_Update = TRUE;
  }    

#define CHECK_IF(a__field,a__flag) \
  if(fOld_##a__field!=a__field.getValue()) {\
    DUMP_UPDATE_WHAT(#a__field)\
    fOld_##a__field = a__field.getValue();\
    a__flag = TRUE;\
  }

#define CHECK_IF_MF(a__field,a__flag) \
  if(*fOld_##a__field!=a__field) {\
    DUMP_UPDATE_WHAT(#a__field)\
    *fOld_##a__field = a__field;\
    a__flag = TRUE;\
  }

  CHECK_IF(shapeAutomated,f_shape_Update)
  CHECK_IF(shape,f_shape_Update)

  CHECK_IF(theta,f_layout_Update)
  CHECK_IF(phi,f_layout_Update)

  CHECK_IF(titleUp,f_title_Update)
  CHECK_IF(titleToAxis,f_title_Update)
  CHECK_IF(titleHeight,f_title_Update)
  CHECK_IF(titleAutomated,f_title_Update)

  CHECK_IF(superposeBins,f_bins_Update)

  CHECK_IF(colorMappingVisible,f_colorMapping_Update)
  CHECK_IF(colorMappingAxisLabeling,f_colorMapping_Update)
  CHECK_IF(colorMappingAttached,f_colorMapping_Update)

  CHECK_IF(wallEnforced,f_wall_Update)
  CHECK_IF(gridEnforced,f_grid_Update)
  CHECK_IF(primitivesEnforced,f_primitives_Update)
  CHECK_IF(innerFrameEnforced,f_innerFrame_Update)

  CHECK_IF(xAxisEnforced,f_xAxis_Update)
  CHECK_IF(xAxisAutomated,f_xAxis_Update)
  CHECK_IF(xAxisMinimum,f_xAxis_Update)
  CHECK_IF(xAxisMaximum,f_xAxis_Update)
  CHECK_IF(xAxisLogScale,f_xAxis_Update)

  CHECK_IF(yAxisEnforced,f_yAxis_Update)
  CHECK_IF(yAxisAutomated,f_yAxis_Update)
  CHECK_IF(yAxisMinimum,f_yAxis_Update)
  CHECK_IF(yAxisMaximum,f_yAxis_Update)
  CHECK_IF(yAxisLogScale,f_yAxis_Update)

  CHECK_IF(zAxisEnforced,f_zAxis_Update)
  CHECK_IF(zAxisAutomated,f_zAxis_Update)
  CHECK_IF(zAxisMinimum,f_zAxis_Update)
  CHECK_IF(zAxisMaximum,f_zAxis_Update)
  CHECK_IF(zAxisLogScale,f_zAxis_Update)

  CHECK_IF(topAxisVisible,f_xAxis_Update)
  CHECK_IF(rightAxisVisible,f_yAxis_Update)

  CHECK_IF(numberOfLevels,f_function_Update)
  CHECK_IF_MF(levels,f_function_Update)

  // X axis :
 {SoLineStyle* soStyle = xAxisNode->getLineStyle();
  if(fOld_xAxis_lineStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("xAxis_lineStyle")
    fOld_xAxis_lineStyle = soStyle->getSbStyle();
    f_xAxis_Update = TRUE;
  }}
 {SoLineStyle* soStyle = xAxisNode->getTicksStyle();
  if(fOld_xAxis_ticksStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("xAxis_ticksStyle")
    fOld_xAxis_ticksStyle = soStyle->getSbStyle();
    f_xAxis_Update = TRUE;
  }}
 {SoTextStyle* soStyle = xAxisNode->getLabelsStyle();
  if(fOld_xAxis_labelsStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("xAxis_labelsStyle")
    fOld_xAxis_labelsStyle = soStyle->getSbStyle();
    f_xAxis_Update = TRUE;
  }}
 {SoTextStyle* soStyle = xAxisNode->getTitleStyle();
  if(fOld_xAxis_titleStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("xAxis_titleStyle")
    fOld_xAxis_titleStyle = soStyle->getSbStyle();
    f_xAxis_Update = TRUE;
  }}
 {SoTextStyle* soStyle = xAxisNode->getMagStyle();
  if(fOld_xAxis_magStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("xAxis_magStyle")
    fOld_xAxis_magStyle = soStyle->getSbStyle();
    f_xAxis_Update = TRUE;
  }}
  if(fOld_xAxis_modeling!=xAxisNode->modeling.getValue()) {
    DUMP_UPDATE_WHAT("xAxis_modeling")
    fOld_xAxis_modeling = xAxisNode->modeling.getValue();
    f_xAxis_Update = TRUE;
  }

  // Y axis :
 {SoLineStyle* soStyle = yAxisNode->getLineStyle();
  if(fOld_yAxis_lineStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("yAxis_lineStyle")
    fOld_yAxis_lineStyle = soStyle->getSbStyle();
    f_yAxis_Update = TRUE;
  }}
 {SoLineStyle* soStyle = yAxisNode->getTicksStyle();
  if(fOld_yAxis_ticksStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("yAxis_ticksStyle")
    fOld_yAxis_ticksStyle = soStyle->getSbStyle();
    f_yAxis_Update = TRUE;
  }}
 {SoTextStyle* soStyle = yAxisNode->getLabelsStyle();
  if(fOld_yAxis_labelsStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("yAxis_labelsStyle")
    fOld_yAxis_labelsStyle = soStyle->getSbStyle();
    f_yAxis_Update = TRUE;
  }}
 {SoTextStyle* soStyle = yAxisNode->getTitleStyle();
  if(fOld_yAxis_titleStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("yAxis_titleStyle")
    fOld_yAxis_titleStyle = soStyle->getSbStyle();
    f_yAxis_Update = TRUE;
  }}
 {SoTextStyle* soStyle = yAxisNode->getMagStyle();
  if(fOld_yAxis_magStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("yAxis_magStyle")
    fOld_yAxis_magStyle = soStyle->getSbStyle();
    f_yAxis_Update = TRUE;
  }}
  if(fOld_yAxis_modeling!=yAxisNode->modeling.getValue()) {
    DUMP_UPDATE_WHAT("yAxis_modeling")
    fOld_yAxis_modeling = yAxisNode->modeling.getValue();
    f_yAxis_Update = TRUE;
  }

  // Z axis :
 {SoLineStyle* soStyle = zAxisNode->getLineStyle();
  if(fOld_zAxis_lineStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("zAxis_lineStyle")
    fOld_zAxis_lineStyle = soStyle->getSbStyle();
    f_zAxis_Update = TRUE;
  }}
 {SoLineStyle* soStyle = zAxisNode->getTicksStyle();
  if(fOld_zAxis_ticksStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("zAxis_ticksStyle")
    fOld_zAxis_ticksStyle = soStyle->getSbStyle();
    f_zAxis_Update = TRUE;
  }}
 {SoTextStyle* soStyle = zAxisNode->getLabelsStyle();
  if(fOld_zAxis_labelsStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("zAxis_labelsStyle")
    fOld_zAxis_labelsStyle = soStyle->getSbStyle();
    f_zAxis_Update = TRUE;
  }}
 {SoTextStyle* soStyle = zAxisNode->getTitleStyle();
  if(fOld_zAxis_titleStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("zAxis_titleStyle")
    fOld_zAxis_titleStyle = soStyle->getSbStyle();
    f_zAxis_Update = TRUE;
  }}
 {SoTextStyle* soStyle = zAxisNode->getMagStyle();
  if(fOld_zAxis_magStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("zAxis_magStyle")
    fOld_zAxis_magStyle = soStyle->getSbStyle();
    f_zAxis_Update = TRUE;
  }}
  if(fOld_zAxis_modeling!=zAxisNode->modeling.getValue()) {
    DUMP_UPDATE_WHAT("zAxis_modeling")
    fOld_zAxis_modeling = zAxisNode->modeling.getValue();
    f_zAxis_Update = TRUE;
  }
  if(f_yAxis_Update==TRUE) {
    // In shape = XYZ z axis position depends of axis y width.
    f_zAxis_Update = TRUE;
  }

 {SoStyle* soStyle = getWallStyle();
  if(fOld_wallStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("wallStyle")
    fOld_wallStyle = soStyle->getSbStyle();
    f_wall_Update = TRUE;
  }}

 {SoStyle* soStyle = getGridStyle();
  if(fOld_gridStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("gridStyle")
    fOld_gridStyle = soStyle->getSbStyle();
    f_grid_Update = TRUE;
  }}

 {SoStyle* soStyle = getInnerFrameStyle();
  if(fOld_innerFrameStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("innerFrameStyle")
    fOld_innerFrameStyle = soStyle->getSbStyle();
    f_innerFrame_Update = TRUE;
  }}

 {SoTextStyle* soStyle = getTitleStyle();
  if(fOld_titleStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    DUMP_UPDATE_WHAT("titleStyle")
    fOld_titleStyle = soStyle->getSbStyle();
    f_title_Update = TRUE;
  }}

  // f_[bins,points,function]_Update also touched 
  // in [clear,set]PlottableObjects.
  if(isBinsStyleChanged()==TRUE) {
    DUMP_UPDATE_WHAT("BinsStyleChanged")
    setOldBinsStyle();
    f_bins_Update = TRUE;
  }
  if(isRightHatchStyleChanged()==TRUE) {
    DUMP_UPDATE_WHAT("RightHatchStyleChanged")
    setOldRightHatchStyle();
    f_bins_Update = TRUE;
  }
  if(isLeftHatchStyleChanged()==TRUE) {
    DUMP_UPDATE_WHAT("LeftHatchStyleChanged")
    setOldLeftHatchStyle();
    f_bins_Update = TRUE;
  }
  if(isErrorsStyleChanged()==TRUE) {
    DUMP_UPDATE_WHAT("ErrorsStyleChanged")
    setOldErrorsStyle();
    f_bins_Update = TRUE;
  }
  if(isPointsStyleChanged()==TRUE) {
    DUMP_UPDATE_WHAT("PointsStyleChanged")
    setOldPointsStyle();
    f_points_Update = TRUE;
  }
  if(isFunctionStyleChanged()==TRUE) {
    DUMP_UPDATE_WHAT("FunctionStyleChanged")
    setOldFunctionStyle();
    f_function_Update = TRUE;
  }

  if(fOld_ttfScale!=ttfScale.getValue()) {
    fOld_ttfScale = ttfScale.getValue();
    f_ttfScale_Update = TRUE;
   {SoTextStyle* style = (SoTextStyle*)titleStyle.getValue();
    if(SoTools_IsTTF(style->fontName.getValue())==TRUE) {
      f_title_Update = TRUE;
    }}
  }

  if(f_shape_Update==TRUE) {
    f_bins_Update = TRUE; 
    f_points_Update = TRUE;
    f_function_Update = TRUE;
    f_wall_Update = TRUE;
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
    f_colorMapping_Update = TRUE;
    f_title_Update = TRUE;
    f_innerFrame_Update = TRUE;
  }

  if( (f_bins_Update==TRUE) || 
      (f_points_Update==TRUE) ||
      (f_function_Update==TRUE)) {
    DUMP_UPDATE_WHAT("bins points function")
    f_xAxis_Update = TRUE;
    f_yAxis_Update = TRUE;
    f_zAxis_Update = TRUE;
    f_layout_Update = TRUE;
    f_innerFrame_Update = TRUE;
  }

  if(f_bins_Update==TRUE) {
    DUMP_UPDATE_WHAT("bins")
    f_colorMapping_Update = TRUE;
  }
  if(f_function_Update==TRUE) {
    DUMP_UPDATE_WHAT("function")
    f_colorMapping_Update = TRUE;
  }

  if(f_colorMapping_Update==TRUE) {
    DUMP_UPDATE_WHAT("colorMapping")
    f_bins_Update = TRUE;
    f_function_Update = TRUE;
  }

  if( (f_xAxis_Update==TRUE) || 
      (f_yAxis_Update==TRUE) ||
      (f_zAxis_Update==TRUE)) {
    DUMP_UPDATE_WHAT("xyzAxis")
    f_grid_Update = TRUE;
    f_primitives_Update = TRUE;
  }

  if(verbose.getValue()==TRUE) {
    if(f_shape_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update shape");
    if(f_xAxis_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update xAxis");
    if(f_yAxis_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update yAxis");
    if(f_zAxis_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update zAxis");
    if(f_wall_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update wall");
    if(f_grid_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update grid");
    if(f_innerFrame_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update innerFrame");
    if(f_title_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update title");
    if(f_bins_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update bins");
    if(f_points_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update points");
    if(f_function_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update function");
    if(f_layout_Update==TRUE) 
      SoDebugError::postInfo("SoPlotter::updateWhat","update layout");
  }
}

//FIXME : we should avoid ERASE_ALL (and f_*_Update = TRUE in 
// the updateChildren method).

// Will clear all sub scene graphs.
#define ERASE_ALL \
    f_bins_Update = TRUE;\
    f_points_Update = TRUE;\
    f_function_Update = TRUE;\
    f_wall_Update = TRUE;\
    f_grid_Update = TRUE;\
    f_primitives_Update = TRUE;\
    f_innerFrame_Update = TRUE;\
    f_title_Update = TRUE;\
    f_colorMapping_Update = TRUE;\
    f_xAxis_Update = TRUE;\
    f_yAxis_Update = TRUE;\
    f_zAxis_Update = TRUE;\
    drawData = FALSE;\
    drawWall = FALSE;\
    drawGrid = FALSE;\
    drawPrimitives = FALSE;\
    drawInnerFrame = FALSE;\
    drawTitle = FALSE;\
    drawColorMapping = FALSE;\
    drawXaxis = FALSE;\
    drawYaxis = FALSE;\
    drawZaxis = FALSE;\
    drawInfos = FALSE;\
    drawLegend = FALSE;

void SoPlotter::updateChildren(){
  if(frozen.getValue()==TRUE) return;

#ifdef DEBUG
  printf("debug : SoPlotter::updateChildren : ////////////// %ld\n",this);
#endif

  updateWhat();

  set_lists(TRUE); //check valid.

  if(f_shape_Update==TRUE) {
    f_shape_Update = FALSE;
    updateShape();
  }

  if(fShape==XY) {
    CHECK_IF(valueTopMargin,f_yAxis_Update)
  } else if(fShape==XYZ) {
    CHECK_IF(valueTopMargin,f_zAxis_Update)
  }

  if( (f_bins_Update==TRUE) || 
      (f_points_Update==TRUE) ||
      (f_function_Update==TRUE)) {
    updateSbAxes();
  }

  if(f_layout_Update==TRUE) {
    f_layout_Update = FALSE;
    updateLayout();
  }

  // A priori we build a scene graph for everything.
  SbBool drawInnerFrame = TRUE;
  SbBool drawWall = TRUE;
  SbBool drawGrid = TRUE;
  SbBool drawPrimitives = TRUE;
  SbBool drawTitle = TRUE;
  SbBool drawData = TRUE;
  SbBool drawXaxis = TRUE;
  SbBool drawYaxis = TRUE;
  SbBool drawZaxis = TRUE;
  SbBool drawInfos = TRUE;
  SbBool drawLegend = TRUE;
  SbBool drawColorMapping = TRUE;

  SbPList bins1DListSwMnMx;
 {int objn = f_bins1DList.size();
  for(int index=0;index<objn;index++) {
    float bmin,bmax; // Oftenly used, save them.
    f_bins1DList[index]->getBinsSumOfWeightsRange(bmin,bmax);
    bins1DListSwMnMx.append(new SbVec2f(bmin,bmax));
  }}

 {std::vector<SbBaseColormap*>::iterator it;
  for(it=f_bins_cmaps.begin();it!=f_bins_cmaps.end();++it) {
    delete *it;
    *it = 0;
  }
  f_bins_cmaps.resize(f_binsList.size(),0);}

 {std::vector<SbBaseColormap*>::iterator it;
  for(it=f_points_cmaps.begin();it!=f_points_cmaps.end();++it) {
    delete *it;
    *it = 0;
  }
  f_points_cmaps.resize(f_pointsList.size(),0);}

 {std::vector<SbBaseColormap*>::iterator it;
  for(it=f_function_cmaps.begin();it!=f_function_cmaps.end();++it) {
    delete *it;
    *it = 0;
  }
  f_function_cmaps.resize(f_functionList.size(),0);}

  int bins1DNumber = f_bins1DList.size();
  int bins2DNumber = f_bins2DList.size();
  int points2DNumber = f_points2DList.size();
  int points3DNumber = f_points3DList.size();
  int function1DNumber = f_function1DList.size();
  int function2DNumber = f_function2DList.size();
  if( (bins1DNumber==0) && (bins2DNumber==0) && 
      (points2DNumber==0) && (points3DNumber==0) && 
      (function1DNumber==0) && (function2DNumber==0) ) { 
    DUMP_UPDATE_WHAT("no plottable")
    // Will clear all sub scene graphs.
    ERASE_ALL
  }

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoPlotter::updateChildren",
      "%s : bins1D = %d ; bins2D = %d ; points2D = %d ; points3D = %d ; function1D = %d ; function2D = %d",
			   p2sx(this).c_str(),bins1DNumber,bins2DNumber,
                           points2DNumber,points3DNumber,function1DNumber,function2DNumber);
  }

  SbBool superpose = superposeBins.getValue();
  if(superpose==TRUE) {
    // Check compatibility of bins :
    if( (bins1DNumber<=0) || (fShape!=XY) ) {
      superpose = FALSE;
    } else {
      SbPlottableBins1D* bins = f_bins1DList[0];
      int xnbin = bins->getAxisNumberOfBins();
      float xmn = bins->getAxisMinimum();
      float xmx = bins->getAxisMaximum();
      superpose = TRUE;
      for(int ibins=1;ibins<bins1DNumber;ibins++) {
        SbPlottableBins1D* binsloop = f_bins1DList[ibins];
        if( (xnbin!=binsloop->getAxisNumberOfBins()) ||
            (xmn!=binsloop->getAxisMinimum()) ||
            (xmx!=binsloop->getAxisMaximum()) ) {
          superpose = FALSE;
          break;
        }       
      }       
      if(superpose==TRUE) { //Compatible bins :
        if(yAxisAutomated.getValue()==TRUE) {
          // Correct Y axis if XY shape and superposing bins.
          // Get min/max
          float bmin,bmax;
          getHeight(bins1DNumber-1,f_bins1DList,bins1DListSwMnMx,0,bmin,bmax);
          bmin = bmax;            
          for(int ibin=1;ibin<xnbin;ibin++) {
            float mini,maxi;
            getHeight
              (bins1DNumber-1,f_bins1DList,bins1DListSwMnMx,ibin,mini,maxi);
            bmin = SbMinimum(bmin,maxi);
            bmax = SbMaximum(bmax,maxi);
          }
          f_yDataAxis.setMinimumValue(bmin);
          f_yDataAxis.setMaximumValue(bmax);
          f_yDataAxis.adjustAxis();
        }
      }       
    }
  }

  float xmin =  f_xDataAxis.getMinimumValue();
  float xmax =  f_xDataAxis.getMaximumValue();
  SbBool xlog = f_xDataAxis.isLogScale();
  if(xlog==TRUE) {
    if((xmin<=0) || (xmax<=0) ) {
      f_xDataAxis.adjustAxis();
      xmin = f_xDataAxis.getMinimumValue();
      xmax = f_xDataAxis.getMaximumValue();
      // Now should have reasonable values.
    }
    if((xmin<=0) || (xmax<=0) ) {
      xlog = FALSE;
    } else {
      xmin = FLOG10(xmin);
      xmax = FLOG10(xmax);
    }
  }

  float ymin =  f_yDataAxis.getMinimumValue();
  float ymax =  f_yDataAxis.getMaximumValue();
  SbBool ylog = f_yDataAxis.isLogScale();
  if(ylog==TRUE) {
    if((ymin<=0) || (ymax<=0) ) {
      f_yDataAxis.adjustAxis();
      ymin = f_yDataAxis.getMinimumValue();
      ymax = f_yDataAxis.getMaximumValue();
      // Now should have reasonable values.
    }
    if((ymin<=0) || (ymax<=0) ) {
      ylog = FALSE;
    }else{
      ymin = FLOG10(ymin);
      ymax = FLOG10(ymax);
    }
  }

  float zmin =  f_zDataAxis.getMinimumValue();
  float zmax =  f_zDataAxis.getMaximumValue();
  SbBool zlog = f_zDataAxis.isLogScale();
  if(zlog==TRUE) {
    if((zmin<=0) || (zmax<=0) ) {
      f_zDataAxis.adjustAxis();
      zmin = f_zDataAxis.getMinimumValue();
      zmax = f_zDataAxis.getMaximumValue();
      // Now should have reasonable values.
    }
    if((zmin<=0) || (zmax<=0) ) {
      zlog = FALSE;
    }else{
      zmin = FLOG10(zmin);
      zmax = FLOG10(zmax);
    }
  }

  if(fShape==XY) {
    if(xmin>=xmax) {
      DUMP_UPDATE_WHAT("bad min/max x axes")
      ERASE_ALL
    }
    if(ymin>=ymax) {
      DUMP_UPDATE_WHAT("bad min/max y axes")
      ERASE_ALL
    }
  } else if(fShape==XYZ) {
    if(xmin>=xmax) {
      DUMP_UPDATE_WHAT("bad min/max x axes")
      ERASE_ALL
    }
    if(ymin>=ymax) {
      DUMP_UPDATE_WHAT("bad min/max y axes")
      ERASE_ALL
    }
    if(zmin>=zmax) {
      DUMP_UPDATE_WHAT("bad min/max z axes")
      ERASE_ALL
    }
  }

 {float XSIZ = width.getValue();
  float XMGL = leftMargin.getValue();
  float XMGR = rightMargin.getValue();
  float wData = XSIZ-XMGL-XMGR;

  float YSIZ = height.getValue();
  float YMGL = bottomMargin.getValue();
  float YMGU = topMargin.getValue();
  float hData = YSIZ-YMGL-YMGU;
  if(fShape==XY) {
    if(wData<=0) {
      DUMP_UPDATE_WHAT("null w data area")
      ERASE_ALL
      SoDebugError::postInfo("SoPlotter::updateChildren","null w data area.");
    }
    if(hData<=0) {
      DUMP_UPDATE_WHAT("null h data area")
      ERASE_ALL
      SoDebugError::postInfo("SoPlotter::updateChildren","null h data area.");
    }
  } else if(fShape==XYZ) {
    float ZSIZ = depth.getValue();
    float ZMGD = downMargin.getValue();
    float ZMGU = upMargin.getValue();
    float dData = ZSIZ-ZMGD-ZMGU;
    if(wData<=0) {
      DUMP_UPDATE_WHAT("null w data area")
      ERASE_ALL
      SoDebugError::postInfo("SoPlotter::updateChildren","null w data area.");
    }
    if(hData<=0) {
      DUMP_UPDATE_WHAT("null h data area")
      ERASE_ALL
      SoDebugError::postInfo("SoPlotter::updateChildren","null h data area.");
    }
    if(dData<=0) {
      DUMP_UPDATE_WHAT("null d data area")
      ERASE_ALL
      SoDebugError::postInfo("SoPlotter::updateChildren","null d data area.");
    }
  }}

  float dx   = xmax - xmin;
  float dy   = ymax - ymin;
  float dz   = zmax - zmin;

  SbRepBox boxX(xmin,dx,xlog);
  SbRepBox boxY(ymin,dy,ylog);
  SbRepBox boxZ(zmin,dz,zlog);

  if((f_title_Update==TRUE) ||
     (f_bins_Update==TRUE) ||
     (f_points_Update==TRUE) ||
     (f_function_Update==TRUE) ) {
    if(titleAutomated.getValue()==TRUE) {
      SbString s = getTitle();
      if(title.getValue()!=s) {
        //SbBool flag = title.enableNotify(FALSE);//FIXME
        title.setValue(s); //Output
        //title.enableNotify(flag);//FIXME
      }
    }
  }

  if((f_bins_Update==TRUE) ||
     (f_points_Update==TRUE) ||
     (f_function_Update==TRUE) ) {

    // Infos, legend concern data only.
    // WARNING : infos field is an "output" field. 

    infos.deleteValues(0);
    if(drawInfos==TRUE ) {

      //SbBool flag = infos.enableNotify(FALSE);//FIXME
      SbString sinfos = getInfos();
      std::vector<SbString> lines = SbGetLines(sinfos);
      int linen = lines.size();
      for(int index=0;index<linen;index++) {
        infos.set1Value(index,lines[index]);
      }
      //infos.enableNotify(flag);

    }

    if(drawLegend==FALSE) {
      legend.deleteValues(0);
    } else {
      updateLegend();
    }

    //f_innerFrame_Update = TRUE; //FIXME : needed ?
  }

  if(fShape==XY) {

#define XY_DEPTH_DATA 0.25F

    float d = XY_DEPTH_DATA;

    float ZOFFSET = d/(bins1DNumber+bins2DNumber+points2DNumber+1);
    float zz = 0;

    if(f_bins_Update==TRUE) {
      f_bins_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : XY : update bins",p2sx(this).c_str());
      }

      SoSeparator* binsNode = (SoSeparator*)binsSeparator.getValue();
      //SbBool flag = binsNode->enableNotify(FALSE);//FIXME
      binsNode->removeAllChildren();
      SoSeparator* errorsNode = (SoSeparator*)errorsSeparator.getValue();
      //SbBool flag2 = errorsNode->enableNotify(FALSE);//FIXME
      errorsNode->removeAllChildren();
      if(drawData==TRUE) {
        int ibins1D = 0;
        int number = f_binsList.size();
        for(int ibins=0;ibins<number;ibins++) {
          SbPlottableObject* obj = f_binsList[ibins];
          if(eq_Cast(b1,*obj,SbPlottableBins1D)){
            zz += ZOFFSET; // ibins = 0 back (PAW convention).
            SoStyle* graphicStyle = mergeBinsStyle(ibins,*b1);
            SoStyle* rightHatchGC = getRightHatchStyle(ibins);
            SoStyle* leftHatchGC = getLeftHatchStyle(ibins);
            SoStyle* errorStyle = getErrorsStyle(ibins);
            updateBins1D_XY(*b1,ibins,*graphicStyle,
                            *rightHatchGC,*leftHatchGC,*errorStyle,
                            ibins1D,f_bins1DList,bins1DListSwMnMx,superpose,
                            boxX,boxY,zz);        
            graphicStyle->unref();
            ibins1D++;
          } else if(eq_Cast(b2,*obj,SbPlottableBins2D)){
            SoStyle* graphicStyle = mergeBinsStyle(ibins,*b2);
            updateBins2D_XY(*b2,ibins,*graphicStyle,boxX,boxY,boxZ,zz);
            graphicStyle->unref();
          }
        }
      }
      //binsNode->enableNotify(flag);//FIXME
      //errorsNode->enableNotify(flag2);//FIXME
    }

    if(f_points_Update==TRUE) {
      f_points_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : XY : update points",p2sx(this).c_str());
      }

      SoSeparator* pointsNode = (SoSeparator*)pointsSeparator.getValue();
      //SbBool flag = pointsNode->enableNotify(FALSE);//FIXME
      pointsNode->removeAllChildren();
      if(drawData==TRUE) {
        int number = f_pointsList.size();
        for(int ipoints=0;ipoints<number;ipoints++) {
          SbPlottableObject* obj = f_pointsList[ipoints];
          if(eq_Cast(p2,*obj,SbPlottablePoints2D)){
            zz += ZOFFSET; // ipoints = 0 back (PAW convention).
            SoStyle* graphicStyle = mergePointsStyle(ipoints,*p2);
            updatePoints2D_XY(*p2,*graphicStyle,boxX,boxY,zz);
            graphicStyle->unref();
          //} else if(eq_Cast(p3,*obj,SbPlottablePoints3D)){
          }
        }
      }
      //pointsNode->enableNotify(flag);//FIXME
    }

    if(f_function_Update==TRUE) {
      f_function_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : XY : update function",p2sx(this).c_str());
      }

      SoSeparator* functionNode = (SoSeparator*)functionSeparator.getValue();
      //SbBool flag = functionNode->enableNotify(FALSE);//FIXME
      functionNode->removeAllChildren();
      if(drawData==TRUE) {
        //zz = 0; // Functions in front.
        int number = f_functionList.size();
        for(int ifunc=0;ifunc<number;ifunc++) {
          SbPlottableObject* obj = f_functionList[ifunc];
          if(eq_Cast(f1,*obj,SbPlottableFunction1D)){
            zz += ZOFFSET;
            SoStyle* graphicStyle = mergeFunctionStyle(ifunc,*f1);
            updateFunction_XY(*f1,*graphicStyle,boxX,boxY,boxZ,zz);
            graphicStyle->unref();
          } else if(eq_Cast(f2,*obj,SbPlottableFunction2D)){
            zz += ZOFFSET;
            SoStyle* graphicStyle = mergeFunctionStyle(ifunc,*f2);
            updateFunction_XY(*f2,ifunc,*graphicStyle,boxX,boxY,boxZ,zz);
            graphicStyle->unref();
          }
        }
      }
      //functionNode->enableNotify(flag);//FIXME
    }    

  } else if(fShape==XYZ) {
    
    if(f_bins_Update==TRUE) {
      f_bins_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : XYZ : update bins",p2sx(this).c_str());
      }

      SoSeparator* binsNode = (SoSeparator*)binsSeparator.getValue();
      //SbBool flag = binsNode->enableNotify(FALSE);//FIXME
      binsNode->removeAllChildren();
      if(drawData==TRUE) {
        int number = f_binsList.size();
        for(int ibins=0;ibins<number;ibins++) {
          SbPlottableObject* obj = f_binsList[ibins];
          //if(eq_Cast(b1,*obj,SbPlottableBins1D)){
          //} else
          if(eq_Cast(b2,*obj,SbPlottableBins2D)){
            SoStyle* graphicStyle = mergeBinsStyle(ibins,*b2);
            updateBins2D_XYZ(*b2,ibins,*graphicStyle,boxX,boxY,boxZ);
            graphicStyle->unref();
          }
        }
      }
      //binsNode->enableNotify(flag);//FIXME
    }

    if(f_points_Update==TRUE) {
      f_points_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : XYZ : update points",p2sx(this).c_str());
      }

      SoSeparator* pointsNode = (SoSeparator*)pointsSeparator.getValue();
      //SbBool flag = pointsNode->enableNotify(FALSE);//FIXME
      pointsNode->removeAllChildren();
      if(drawData==TRUE) {
        int number = f_pointsList.size();
        for(int ipoints=0;ipoints<number;ipoints++) {
          SbPlottableObject* obj = f_pointsList[ipoints];
          if(eq_Cast(p3,*obj,SbPlottablePoints3D)){
            SoStyle* graphicStyle = mergePointsStyle(ipoints,*p3);
            updatePoints3D_XYZ(*p3,*graphicStyle,boxX,boxY,boxZ);
            graphicStyle->unref();
          }
        }
      }
      //pointsNode->enableNotify(flag);//FIXME
    }

    if(f_function_Update==TRUE) {
      f_function_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : XYZ : update function",p2sx(this).c_str());
      }

      SoSeparator* functionNode = (SoSeparator*)functionSeparator.getValue();
      //SbBool flag = functionNode->enableNotify(FALSE);//FIXME
      functionNode->removeAllChildren();
      if(drawData==TRUE) {
        int number = f_functionList.size();
        for(int ifunc=0;ifunc<number;ifunc++) {
          SbPlottableObject* obj = f_functionList[ifunc];
          if(eq_Cast(f2,*obj,SbPlottableFunction2D)){
            SoStyle* graphicStyle = mergeFunctionStyle(ifunc,*f2);
            updateFunction_XYZ(*f2,ifunc,*graphicStyle,boxX,boxY,boxZ);
            graphicStyle->unref();
          }
        }
      }
      //functionNode->enableNotify(flag);//FIXME
    }

  }

  // Inner frame for XY, must be in front of grid :
  if(f_innerFrame_Update==TRUE) {
    f_innerFrame_Update = FALSE;

    if(verbose.getValue()==TRUE) {
      SoDebugError::postInfo
        ("SoPlotter::updateChildren","%s : update innerFrame : draw %d : shape %d",p2sx(this).c_str(),drawInnerFrame,fShape);
    }

    SoSeparator* innerFrameNode = (SoSeparator*)innerFrameSeparator.getValue();

    if(innerFrameEnforced.getValue()==TRUE) {
      SbBool flag = innerFrameNode->enableNotify(FALSE);
      innerFrameNode->removeAllChildren();
      if(fShape==XY) {
        updateInnerFrame_XY();
      } else {
        updateInnerFrame_XYZ();
      }
      innerFrameNode->enableNotify(flag);
    } else {
      innerFrameNode->removeAllChildren();
      if(drawInnerFrame==TRUE) {
        if(fShape==XY) {
          updateInnerFrame_XY();
        } else {
          updateInnerFrame_XYZ();
        }
      }	
    }
  }

#ifdef DEBUG
  printf("SoPlotter::updateChildren : Creation of Axis in USG Shape : %d  X?:%d Y?:%d Z?:%d\n",fShape,f_xAxis_Update,f_yAxis_Update,f_zAxis_Update);
#endif

  if(f_ttfScale_Update) {
    f_ttfScale_Update = FALSE;
    xAxisNode->ttfScale.setValue(ttfScale.getValue());
    yAxisNode->ttfScale.setValue(ttfScale.getValue());
    zAxisNode->ttfScale.setValue(ttfScale.getValue());
  }

  if(fShape==XY) {
    if(f_xAxis_Update==TRUE) {
      f_xAxis_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : update X axis",p2sx(this).c_str());
      }

      if(xAxisEnforced.getValue()==TRUE) {
        updateXAxis2D(FALSE);    
        updateAxis(xAxisNode,f_xDataAxis,FALSE);
       {SoSeparator* sep = (SoSeparator*)topAxisSeparator.getValue();
        SbBool flag = sep->enableNotify(FALSE);
        sep->removeAllChildren();
        if(topAxisVisible.getValue()==TRUE) updateTopAxis(FALSE); 
        sep->enableNotify(flag);}
      } else {
        if(drawXaxis==FALSE) {
          if(xAxisNode->width.getValue()!=0) xAxisNode->width.setValue(0);
          ((SoSeparator*)topAxisSeparator.getValue())->removeAllChildren();
        } else {
          updateXAxis2D();    
          updateAxis(xAxisNode,f_xDataAxis);
          ((SoSeparator*)topAxisSeparator.getValue())->removeAllChildren();
          if(topAxisVisible.getValue()==TRUE) updateTopAxis(); 
        }
      }
    }
    if(f_yAxis_Update==TRUE) {
      f_yAxis_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : update Y axis",p2sx(this).c_str());
      }

      if(yAxisEnforced.getValue()==TRUE) {
        updateYAxis2D(FALSE);
        updateAxis(yAxisNode,f_yDataAxis,FALSE);
       {SoSeparator* sep = (SoSeparator*)rightAxisSeparator.getValue();
        SbBool flag = sep->enableNotify(FALSE);
        sep->removeAllChildren();
        if(rightAxisVisible.getValue()==TRUE) updateRightAxis(FALSE); 
        sep->enableNotify(flag);}
      } else {
        if(drawYaxis==FALSE) {
          if(yAxisNode->width.getValue()!=0) yAxisNode->width.setValue(0);
          ((SoSeparator*)rightAxisSeparator.getValue())->removeAllChildren();
        } else {
          updateYAxis2D();
          updateAxis(yAxisNode,f_yDataAxis);
          ((SoSeparator*)rightAxisSeparator.getValue())->removeAllChildren();
          if(rightAxisVisible.getValue()==TRUE) updateRightAxis(); 
        }
      }
    }
    if(f_zAxis_Update==TRUE) {
      f_zAxis_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : update Z axis",p2sx(this).c_str());
      }

      if(zAxisEnforced.getValue()==TRUE) {
        updateZAxis2D(FALSE);    
      } else {
        if(drawZaxis==FALSE) {
          if(zAxisNode->width.getValue()!=0) zAxisNode->width.setValue(0);
        } else {
          updateZAxis2D();    
        }
      }
    }
  } else if(fShape==XYZ) {
    ((SoSeparator*)topAxisSeparator.getValue())->removeAllChildren();
    ((SoSeparator*)rightAxisSeparator.getValue())->removeAllChildren();
    if(f_xAxis_Update==TRUE) {
      f_xAxis_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : update X axis",p2sx(this).c_str());
      }

      if(xAxisEnforced.getValue()==TRUE) {
        updateXAxis3D(FALSE);    
        updateAxis(xAxisNode,f_xDataAxis,FALSE);
      } else {
        if(drawXaxis==FALSE) {
          if(xAxisNode->width.getValue()!=0) xAxisNode->width.setValue(0);
        } else {
          updateXAxis3D();    
          updateAxis(xAxisNode,f_xDataAxis);
        }
      }
    }
    if(f_yAxis_Update==TRUE) {
      f_yAxis_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : update Y axis",p2sx(this).c_str());
      }

      if(yAxisEnforced.getValue()==TRUE) {
        updateYAxis3D(FALSE);
        updateAxis(yAxisNode,f_yDataAxis,FALSE);
      } else {
        if(drawYaxis==FALSE) {
          if(yAxisNode->width.getValue()!=0) yAxisNode->width.setValue(0);
        } else {
          updateYAxis3D();    
          updateAxis(yAxisNode,f_yDataAxis);
        }
      }
    }
    if(f_zAxis_Update==TRUE) {
      f_zAxis_Update = FALSE;

      if(verbose.getValue()==TRUE) {
        SoDebugError::postInfo("SoPlotter::updateChildren","%s : update Z axis",p2sx(this).c_str());
      }

      if(zAxisEnforced.getValue()==TRUE) {
        updateZAxis3D(FALSE);    
        updateAxis(zAxisNode,f_zDataAxis,FALSE);
      } else {
        if(drawZaxis==FALSE) {
          if(zAxisNode->width.getValue()!=0) zAxisNode->width.setValue(0);
        } else {
          updateZAxis3D();    
          updateAxis(zAxisNode,f_zDataAxis);
        }
      }
    }
  }

  if(f_wall_Update==TRUE) {
    f_wall_Update = FALSE;

    if(verbose.getValue()==TRUE) {
      SoDebugError::postInfo("SoPlotter::updateChildren","%s : update wall",p2sx(this).c_str());
    }

    SoSeparator* wallSeparatorNode = (SoSeparator*)wallSeparator.getValue();

    if(wallEnforced.getValue()==TRUE) {
      SbBool flag = wallSeparatorNode->enableNotify(FALSE);
      wallSeparatorNode->removeAllChildren();
      if(fShape==XY) {
        updateWall2D();
      } else if(fShape==XYZ) {
        updateWall3D();
      }
      wallSeparatorNode->enableNotify(flag);
    } else {
      wallSeparatorNode->removeAllChildren();
      if(drawWall==TRUE) {
        if(fShape==XY) {
          updateWall2D();
        } else if(fShape==XYZ) {
          updateWall3D();
        }
      }
    }
  }

  if(f_grid_Update==TRUE) {
    f_grid_Update = FALSE;

    if(verbose.getValue()==TRUE) {
      SoDebugError::postInfo("SoPlotter::updateChildren","%s : update grid",p2sx(this).c_str());
    }

    SoSeparator* sep = (SoSeparator*)gridSeparator.getValue();

    if(gridEnforced.getValue()==TRUE) {
      SbBool flag = sep->enableNotify(FALSE);
      sep->removeAllChildren();
      if(fShape==XY) {
        updateGrid2D();
      } else if(fShape==XYZ) {
        updateGrid3D();
      }
      sep->enableNotify(flag);
    } else {
      sep->removeAllChildren();
      if(drawGrid==TRUE) {
        if(fShape==XY) {
          updateGrid2D();
        } else if(fShape==XYZ) {
          updateGrid3D();
        }
      }
    }

  }

  if(f_title_Update==TRUE) {
    f_title_Update = FALSE;

    if(verbose.getValue()==TRUE) {
      SoDebugError::postInfo("SoPlotter::updateChildren","%s : update title",p2sx(this).c_str());
    }

    SoSeparator* titleNode = (SoSeparator*)titleSeparator.getValue();
    //SbBool flag = titleNode->enableNotify(FALSE);//FIXME
    titleNode->removeAllChildren();
    if(drawTitle==TRUE) updateTitle();
    //titleNode->enableNotify(flag);//FIXME
  }
  
  if(f_colorMapping_Update==TRUE) {
    f_colorMapping_Update = FALSE;

    if(verbose.getValue()==TRUE) {
      SoDebugError::postInfo("SoPlotter::updateChildren","%s : update colormapping",p2sx(this).c_str());
    }

    SoSeparator* sep = (SoSeparator*)colorMappingCellsSeparator.getValue();
    sep->removeAllChildren();

    //drawColorMapping = FALSE;
    if(drawColorMapping) {
      if(f_bins_cmaps.size() && f_bins_cmaps[0] &&            
         f_bins_cmaps[0]->valn()) {     //major_bins
        updateColorMapping(*(f_bins_cmaps[0]));
      } else if(f_points_cmaps.size() && f_points_cmaps[0] &&
                f_points_cmaps[0]->valn()) { //major_points
        updateColorMapping(*(f_points_cmaps[0]));
      } else if(f_function_cmaps.size() && f_function_cmaps[0] &&
                f_function_cmaps[0]->valn()) { //major_function
        updateColorMapping(*(f_function_cmaps[0]));
      } 
    } else {
      SoAxis* axis = (SoAxis*)colorMappingAxis.getValue();
      if(axis->width.getValue()!=0) axis->width.setValue(0);
    }
  }
 
  if(f_primitives_Update==TRUE) {
    f_primitives_Update = FALSE;

    SoSeparator* sep = getEtcSeparator(); //and not etcData.

    if(primitivesEnforced.getValue()==TRUE) {
      SbBool flag = sep->enableNotify(FALSE);
      sep->removeAllChildren();
      std::vector<SbPlottablePrimitive*>::const_iterator it;
      for(it=f_primitives.begin();it!=f_primitives.end();++it) {
        if(eq_Cast(sp,*(*it),SbPlottableText)) {
          updateText(*sep,*sp);
        } else if(eq_Cast(sp,*(*it),SbPlottableBox)) {
          updateBox(*sep,*sp);
        }
      }
      sep->enableNotify(flag);
    } else {
      sep->removeAllChildren();
      if(drawPrimitives==TRUE) {
        std::vector<SbPlottablePrimitive*>::const_iterator it;
        for(it=f_primitives.begin();it!=f_primitives.end();++it) {
          if(eq_Cast(sp,*(*it),SbPlottableText)) {
            updateText(*sep,*sp);
          } else if(eq_Cast(sp,*(*it),SbPlottableBox)) {
            updateBox(*sep,*sp);
          }
        }
      }
    }

  }

 {for(int count=0;count<bins1DNumber;count++) {
    delete ((SbVec2f*)bins1DListSwMnMx[count]);
  }}      
}

SoPlotter::Shape SoPlotter::getShape() const{
  const_cast<SoPlotter*>(this)->set_lists(TRUE);
  return get_shape();
}

SoPlotter::Shape SoPlotter::get_shape() const{
  if(shapeAutomated.getValue()==FALSE) return (Shape)shape.getValue();

  // Guess XY or XYZ shape :

  if(f_binsList.size()) { // major bins compells shape type.
    if(f_binsList[0]->getDimension()==1) {
      return XY;
    } else if(f_binsList[0]->getDimension()==2) {
      return XY; //lego is not the default.
    } else {
      return XYZ;
    }
  } else if(f_pointsList.size()) { // major points compells shape type.
    if(f_pointsList[0]->getDimension()==1) { //?
      return XY;
    } else if(f_pointsList[0]->getDimension()==2) {
      return XY;
    } else {
      return XYZ;
    }
  } else if(f_functionList.size()) { // major function compell shape type.
    if(f_functionList[0]->getDimension()==1) {
      return XY;
    } else {
      return XYZ;
    }
  } else {
    return XY; //Default.
  }
}

void SoPlotter::updateShape(){
  fShape = get_shape();
  if(shapeAutomated.getValue()==TRUE) {
    //SbBool flag = shape->enableNotify(FALSE);
    shape.setValue(fShape);
    //shape->enableNotify(flag);
  }
}

void SoPlotter::updateSbAxes(){

#ifdef DEBUG
  printf("debug : SoPlotter::updateSbAxes : %ld\n",this);
#endif

  f_xDataAxis.setMinimumValue(0);
  f_xDataAxis.setMaximumValue(0);
  f_xDataAxis.setLogScale(xAxisLogScale.getValue());

  f_yDataAxis.setMinimumValue(0);
  f_yDataAxis.setMaximumValue(0);
  f_yDataAxis.setLogScale(yAxisLogScale.getValue());

  f_zDataAxis.setMinimumValue(0);
  f_zDataAxis.setMaximumValue(0);
  f_zDataAxis.setLogScale(zAxisLogScale.getValue());

  if(xAxisAutomated.getValue()==FALSE) {
    f_xDataAxis.setMinimumValue(xAxisMinimum.getValue());
    f_xDataAxis.setMaximumValue(xAxisMaximum.getValue());
  }

  if(yAxisAutomated.getValue()==FALSE) {
    f_yDataAxis.setMinimumValue(yAxisMinimum.getValue());
    f_yDataAxis.setMaximumValue(yAxisMaximum.getValue());
  }

  if(zAxisAutomated.getValue()==FALSE) {
    f_zDataAxis.setMinimumValue(zAxisMinimum.getValue());
    f_zDataAxis.setMaximumValue(zAxisMaximum.getValue());
  }

  if(f_binsList.size()) { 
    
    if(f_binsList[0]->getDimension()==1) {
      SbPlottableBins1D* bins = Cast(*(f_binsList[0]),SbPlottableBins1D);

      if(xAxisAutomated.getValue()==TRUE) {
        f_xDataAxis.setMinimumValue(bins->getAxisMinimum());
        f_xDataAxis.setMaximumValue(bins->getAxisMaximum());
      }
      
      if(yAxisAutomated.getValue()==TRUE) {
        float mn,mx;
        bins->getBinsSumOfWeightsRange(mn,mx);
        //f_yDataAxis.setMinimumValue(mn);
        if(mn>=0) mn = 0; //PAW convention.
        f_yDataAxis.setMinimumValue(mn);
        mx = mx + (mx-mn)*valueTopMargin.getValue();
        f_yDataAxis.setMaximumValue(mx);

        f_yDataAxis.adjustAxis();
      }
      
    } else if(f_binsList[0]->getDimension()==2) {
      SbPlottableBins2D* bins = Cast(*(f_binsList[0]),SbPlottableBins2D);

      if(xAxisAutomated.getValue()==TRUE) {
        f_xDataAxis.setMinimumValue(bins->getAxisMinimumX());
        f_xDataAxis.setMaximumValue(bins->getAxisMaximumX());
      }

      if(yAxisAutomated.getValue()==TRUE) {
        f_yDataAxis.setMinimumValue(bins->getAxisMinimumY());
        f_yDataAxis.setMaximumValue(bins->getAxisMaximumY());
      }

      if(zAxisAutomated.getValue()==TRUE) {
        float mn,mx;
        bins->getBinsSumOfWeightsRange(mn,mx);
        f_zDataAxis.setMinimumValue(mn);
        mx = mx + (mx-mn)*valueTopMargin.getValue();
        f_zDataAxis.setMaximumValue(mx);
        f_zDataAxis.adjustAxis();
      }
    } else if(f_binsList[0]->getDimension()==3) {
      //FIXME : should do something.
    } else {
      // Unusual case.
    }
    
  } else if(f_pointsList.size()) { 
    
    if(f_pointsList[0]->getDimension()==2) {
      SbPlottablePoints2D* points = 
        Cast(*(f_pointsList[0]),SbPlottablePoints2D);

      if(xAxisAutomated.getValue()==TRUE) {
        f_xDataAxis.setMinimumValue(points->getAxisMinimumX());
        f_xDataAxis.setMaximumValue(points->getAxisMaximumX());
      }      
      if(yAxisAutomated.getValue()==TRUE) {
        float ymn = points->getAxisMinimumY();
        float ymx = points->getAxisMaximumY();
        // For pawex22 ?
        //f_yDataAxis.setMinimumValue(ymn*1.1F);
        //f_yDataAxis.setMaximumValue(ymx*1.1F);
        f_yDataAxis.setMinimumValue(ymn);
        f_yDataAxis.setMaximumValue(ymx);
      }
      
    } else if(f_pointsList[0]->getDimension()==3) {
      SbPlottablePoints3D* points =
        Cast(*(f_pointsList[0]),SbPlottablePoints3D);

      if(xAxisAutomated.getValue()==TRUE) {
        f_xDataAxis.setMinimumValue(points->getAxisMinimumX());
        f_xDataAxis.setMaximumValue(points->getAxisMaximumX());
      }      
      
      if(yAxisAutomated.getValue()==TRUE) {
        f_yDataAxis.setMinimumValue(points->getAxisMinimumY());
        f_yDataAxis.setMaximumValue(points->getAxisMaximumY());
      }
      
      if(zAxisAutomated.getValue()==TRUE) {
        f_zDataAxis.setMinimumValue(points->getAxisMinimumZ());
        f_zDataAxis.setMaximumValue(points->getAxisMaximumZ());
      }
      
    } else {
      // Unusual case.
    }
    
  } else if(f_functionList.size()) { 
    
    if(f_functionList[0]->getDimension()==1) {
      SbPlottableFunction1D* func = Cast(*(f_functionList[0]),SbPlottableFunction1D);

      if(xAxisAutomated.getValue()==TRUE) {
        float xmn = func->getMinimumX();
        float xmx = func->getMaximumX();
        if(xmx<=xmn) {
          xmn = -1;
          xmx = 1;
        }
        f_xDataAxis.setMinimumValue(xmn);
        f_xDataAxis.setMaximumValue(xmx);
      }
      
      if(yAxisAutomated.getValue()==TRUE) {
        //same logic as updateFunction_XY
        float xmn = func->getMinimumX();
        float xmx = func->getMaximumX();
        if(xmx<=xmn) {
          xmn = f_xDataAxis.getMinimumValue();
          xmx = f_xDataAxis.getMaximumValue();
        }
        int nstp = func->getNumberOfStepsX();
        nstp = nstp <=0 ? 100 : nstp;

        float df = (xmx - xmn)/nstp;      
        SbBool problem = FALSE;
        float vmin;        
        if(!func->value(xmn,vmin)) problem = TRUE;
        float vmax = vmin;
        for(int ibin=0;ibin<=nstp;ibin++) { 
          float xx = xmn + ibin * df;
          float val;
          if(!func->value(xx,val)) problem = TRUE;
          vmax = SbMaximum(vmax,val);
          vmin = SbMinimum(vmin,val);
        }
        if(problem==TRUE) {
          SoDebugError::postInfo("SoPlotter::updateSbAxes","(yAxis.automated) problem when getting some function value.");
        }
        f_yDataAxis.setMinimumValue(vmin);
        f_yDataAxis.setMaximumValue(vmax);
        f_yDataAxis.adjustAxis();
      }
      
    } else if(f_functionList[0]->getDimension()==2) {
      SbPlottableFunction2D* func = Cast(*(f_functionList[0]),SbPlottableFunction2D);

      if(xAxisAutomated.getValue()==TRUE) {
        float xmn = func->getMinimumX();
        float xmx = func->getMaximumX();
        if(xmx<=xmn) {
          xmn = -1;
          xmx = 1;
        }
        f_xDataAxis.setMinimumValue(xmn);
        f_xDataAxis.setMaximumValue(xmx);
      }

      if(yAxisAutomated.getValue()==TRUE) {
        float ymn =  func->getMinimumY();
        float ymx =  func->getMaximumY();
        if(ymx<=ymn) {
          ymn = -1;
          ymx = 1;
        }
        f_yDataAxis.setMinimumValue(ymn);
        f_yDataAxis.setMaximumValue(ymx);
      }
      
      if(zAxisAutomated.getValue()==TRUE) {

        //same logic as updateFunction_XYZ
        float xmn = func->getMinimumX();
        float xmx = func->getMaximumX();
        if(xmx<=xmn) {
          xmn = f_xDataAxis.getMinimumValue();
          xmx = f_xDataAxis.getMaximumValue();
        }
        int nx = func->getNumberOfStepsX();
        nx = nx <=0 ? 20 : nx;

        float ymn = func->getMinimumY();
        float ymx = func->getMaximumY();
        if(ymx<=ymn) {
          ymn = f_yDataAxis.getMinimumValue();
          ymx = f_yDataAxis.getMaximumValue();
        }
        int ny = func->getNumberOfStepsY();
        ny = ny <=0 ? 20 : ny;

        float dfx = (xmx - xmn)/nx;
        float dfy = (ymx - ymn)/ny;
        
        SbBool problem = FALSE;
        float vmin;
        if(!func->value(xmn,ymn,vmin)) problem = TRUE;
        float vmax = vmin;
        for(int jbin=ny-1;jbin>=0;jbin--) { 
          for(int ibin=nx-1;ibin>=0;ibin--) { 
            float xx = xmn + ibin * dfx;
            float yy = ymn + jbin * dfy;
            float val;
            if(!func->value(xx,yy,val)) problem = TRUE;
            vmax = SbMaximum(vmax,val);
            vmin = SbMinimum(vmin,val);
          }
        }
        if(problem==TRUE) {
          SoDebugError::postInfo("SoPlotter::updateSbAxes","(zAxis.automated) problem when getting some function value.");
        }
        f_zDataAxis.setMinimumValue(vmin);
        f_zDataAxis.setMaximumValue(vmax);
        f_zDataAxis.adjustAxis();
      }
    } else if(f_functionList[0]->getDimension()==3) {
      //FIXME : should do something.
    } else {
      // Unusual case.
    } 
  }
}

void SoPlotter::updateLayout(){
  float XSIZ = width.getValue();
  float XMGL = leftMargin.getValue();
  float XMGR = rightMargin.getValue();
  float wData = XSIZ-XMGL-XMGR;

  float YSIZ = height.getValue();
  float YMGL = bottomMargin.getValue();
  float YMGU = topMargin.getValue();
  float hData = YSIZ-YMGL-YMGU;

 {SoTransform* transform = (SoTransform*)layout.getValue();
  //SbBool flag = transform->enableNotify(FALSE);

 {//initialize :
  SbMatrix matrix;
  matrix.makeIdentity();
  transform->setMatrix(matrix);}

  if(fShape==XY) {
    // In rep primitives (SoLineSet, etc...), (0,0)
    // is the lower left corner of the data area square; 
    transform->translation.setValue(-XSIZ/2+XMGL,-YSIZ/2+YMGL,-0.5F);
  } else { //XYZ
    float ZSIZ = depth.getValue();
    float ZMGD = downMargin.getValue();
    float ZMGU = upMargin.getValue();
    float dData = ZSIZ-ZMGD-ZMGU;

    //printf("debug : updateLayout : X : %g %g %g %g\n",XSIZ,XMGL,XMGR,wData);
    //printf("debug : updateLayout : Y : %g %g %g %g\n",YSIZ,YMGL,YMGU,hData);

    // XYZ global transformation (to have a "lego" layout) :
    //  translate so that the center of the scene 
    //  is the center of the data area cube; 
    //  then rotate to have lego 3D layout.

   {SoRotationXYZ* rot = new SoRotationXYZ;
    rot->ref();
    rot->axis = SoRotationXYZ::X;
    rot->angle = theta.getValue() * FM_PI/180.0F;
    transform->combineLeft(rot);
    rot->unref();}

   {SoRotationXYZ* rot = new SoRotationXYZ;
    rot->ref();
    rot->axis = SoRotationXYZ::Y;
    rot->angle = phi.getValue() * FM_PI/180.0F;
    transform->combineLeft(rot);
    rot->unref();}

   {SoRotationXYZ* rot = new SoRotationXYZ;
    rot->ref();
    rot->axis = SoRotationXYZ::X;
    rot->angle = -90.0F * FM_PI/180.0F;
    transform->combineLeft(rot);
    rot->unref();}

    // To place as CERN/PAW default.
    // In CERN/PAW, it is the projection
    // which fits in the (XSIZ,XMGL,XMGR)/(YSIZ,YMGL,YMGU)
    // page setup.

   {SbBox3f box(-0.5F*wData,-0.5F*hData,-0.5F*dData,
                 0.5F*wData, 0.5F*hData, 0.5F*dData);
    SbRotation r1(SbVec3f(1,0,0),theta.getValue() * FM_PI/180.0F);
    SbRotation r2(SbVec3f(0,1,0),phi.getValue() * FM_PI/180.0F);
    SbRotation r3(SbVec3f(1,0,0),-90.0F * FM_PI/180.0F);
    SbMatrix matrix;
    matrix.makeIdentity();
    matrix.setRotate(r3*r2*r1);
    box.transform(matrix);
    float xfac = box.getMax()[0]-box.getMin()[0];
    float yfac = box.getMax()[1]-box.getMin()[1];
    float zfac = box.getMax()[2]-box.getMin()[2];
    SoScale* scale = new SoScale;
    scale->ref();
    scale->scaleFactor.setValue(wData/xfac,hData/yfac,dData/zfac);
    transform->combineLeft(scale);
    scale->unref();}

   {SoTranslation* trans = new SoTranslation;
    trans->ref();
    trans->translation.setValue(-wData/2,-hData/2,-dData/2);
    transform->combineLeft(trans); //Applied first.
    trans->unref();}

  }
  //transform->enableNotify(flag);
  }

 {SoScale* soDataScale = (SoScale*)dataScale.getValue();
  //FIXME : if doing the below we have problem with picking
  // of bins, innerFrame when passing to PAW style !
  //SbBool flag = soDataScale->scaleFactor.enableNotify(FALSE);
  if(fShape==XY) {
    soDataScale->scaleFactor.setValue(wData,hData,depth.getValue()*0.5F);
  } else if(fShape==XYZ) {
    float ZSIZ = depth.getValue();
    float ZMGD = downMargin.getValue();
    float ZMGU = upMargin.getValue();
    float dData = ZSIZ-ZMGD-ZMGU;

    soDataScale->scaleFactor.setValue(wData,hData,dData);
  }
  //soDataScale->scaleFactor.enableNotify(flag);
  }
}

void SoPlotter::updateXAxis2D(SbBool aNotify){
  float XSIZ = width.getValue();
  float XMGL = leftMargin.getValue();
  float XMGR = rightMargin.getValue();
  float wData = XSIZ-XMGL-XMGR;

  SbBool flag;
  if(aNotify==FALSE) flag = xAxisNode->enableNotify(FALSE);

  xAxisNode->verbose.setValue(verbose.getValue());
  xAxisNode->tickUp.setValue(TRUE);
  xAxisNode->width.setValue(wData);

 {SoTextStyle* style = xAxisNode->getLabelsStyle(); 
  if(style->enforced.getValue()==FALSE) {
    style->orientationX.setValue(SbVec3f(1,0,0));
    style->orientationY.setValue(SbVec3f(0,1,0));
    style->horizontalJustification.setValue(SoTextStyle::CENTER);
    style->verticalJustification.setValue(SoTextStyle::TOP);
  }}
  
 {SoTextStyle* style = xAxisNode->getTitleStyle(); 
  style->orientationX.setValue(SbVec3f(1,0,0));
  style->orientationY.setValue(SbVec3f(0,1,0));
  if(xAxisNode->titleToAxisJustification.getValue()==SoAxis::LEFT) {
    style->horizontalJustification.setValue(SoTextStyle::LEFT);
  } else if(xAxisNode->titleToAxisJustification.getValue()==SoAxis::CENTER) {
    style->horizontalJustification.setValue(SoTextStyle::CENTER);
  } else if(xAxisNode->titleToAxisJustification.getValue()==SoAxis::RIGHT) {
    style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  }  
  style->verticalJustification.setValue(SoTextStyle::TOP);}

 {SoTextStyle* style = xAxisNode->getMagStyle(); 
  style->horizontalJustification.setValue(SoTextStyle::LEFT);
  style->verticalJustification.setValue(SoTextStyle::MIDDLE);}

  if(aNotify==FALSE) xAxisNode->enableNotify(flag);

 {SoTransform* transf = (SoTransform*)xAxisTransform.getValue();
  if(aNotify==FALSE) flag = transf->enableNotify(FALSE);
  SbMatrix matrix;
  matrix.makeIdentity();
  transf->setMatrix(matrix);
  if(aNotify==FALSE) transf->enableNotify(flag);
  }
}

void SoPlotter::updateYAxis2D(SbBool aNotify){
  float YSIZ = height.getValue();
  float YMGL = bottomMargin.getValue();
  float YMGU = topMargin.getValue();
  float hData = YSIZ-YMGL-YMGU;

  SbBool flag;
  if(aNotify==FALSE) flag = yAxisNode->enableNotify(FALSE);

  yAxisNode->verbose.setValue(verbose.getValue());
  yAxisNode->tickUp.setValue(TRUE);
  yAxisNode->width.setValue(hData);

 {SoTextStyle* style = yAxisNode->getLabelsStyle(); 
  if(style->enforced.getValue()==FALSE) {
    style->orientationX.setValue(SbVec3f(0,1,0));
    style->orientationY.setValue(SbVec3f(1,0,0));
    style->horizontalJustification.setValue(SoTextStyle::RIGHT);
    style->verticalJustification.setValue(SoTextStyle::MIDDLE);
  }}

 {SoTextStyle* style = yAxisNode->getTitleStyle(); 
  style->rotated.setValue(TRUE);
  style->orientationX.setValue(SbVec3f(1, 0,0));
  style->orientationY.setValue(SbVec3f(0,-1,0));
  if(yAxisNode->titleToAxisJustification.getValue()==SoAxis::LEFT) {
    style->horizontalJustification.setValue(SoTextStyle::LEFT);
  } else if(yAxisNode->titleToAxisJustification.getValue()==SoAxis::CENTER) {
    style->horizontalJustification.setValue(SoTextStyle::CENTER);
  } else if(yAxisNode->titleToAxisJustification.getValue()==SoAxis::RIGHT) {
    style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  }  
  style->verticalJustification.setValue(SoTextStyle::BOTTOM);}

 {SoTextStyle* style = yAxisNode->getMagStyle(); 
  style->horizontalJustification.setValue(SoTextStyle::CENTER);
  style->verticalJustification.setValue(SoTextStyle::BOTTOM);}

  if(aNotify==FALSE) yAxisNode->enableNotify(flag);

 {SoTransform* transf = (SoTransform*)yAxisTransform.getValue();
  if(aNotify==FALSE) flag = transf->enableNotify(FALSE);
  SbMatrix matrix;
  matrix.makeIdentity();
  transf->setMatrix(matrix);
  SbMatrix roty;
  roty.setRotate(SbRotation(SbVec3f(0,1,0),FM_PI));
  transf->multLeft(roty);
  SbMatrix rotz;
  rotz.setRotate(SbRotation(SbVec3f(0,0,1),FM_PI_2));
  transf->multLeft(rotz); //Applied first.
  if(aNotify==FALSE) transf->enableNotify(flag);
 }
}

void SoPlotter::updateZAxis2D(SbBool aNotify){
  SbBool flag;
  if(aNotify==FALSE) flag = zAxisNode->enableNotify(FALSE);

  zAxisNode->verbose.setValue(verbose.getValue());
  zAxisNode->width.setValue(0);

  if(aNotify==FALSE) zAxisNode->enableNotify(flag);

 {SoTransform* transf = (SoTransform*)zAxisTransform.getValue();
  if(aNotify==FALSE) flag = transf->enableNotify(FALSE);
  SbMatrix matrix;
  matrix.makeIdentity();
  transf->setMatrix(matrix);
  if(aNotify==FALSE) transf->enableNotify(flag);
 }
}

void SoPlotter::updateXAxis3D(SbBool aNotify){
  float XSIZ = width.getValue();
  float XMGL = leftMargin.getValue();
  float XMGR = rightMargin.getValue();
  float wData = XSIZ-XMGL-XMGR;

  SbBool flag;
  if(aNotify==FALSE) flag = xAxisNode->enableNotify(FALSE);

  xAxisNode->verbose.setValue(verbose.getValue());
  xAxisNode->tickUp.setValue(FALSE);
  xAxisNode->width.setValue(wData);

 {SoTextStyle* style = xAxisNode->getLabelsStyle(); 
  if(style->enforced.getValue()==FALSE) {
    style->fontName.setValue(SbFont_Hershey); //Enforce Hershey.
    style->orientationX.setValue(SbVec3f(1,0,0));
    style->orientationY.setValue(SbVec3f(0,1,0));
    style->horizontalJustification.setValue(SoTextStyle::CENTER);
    style->verticalJustification.setValue(SoTextStyle::TOP);
  }}

 {SoTextStyle* style = xAxisNode->getTitleStyle(); 
  style->orientationX.setValue(SbVec3f(1,0,0));
  style->orientationY.setValue(SbVec3f(0,1,0));
  style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  style->verticalJustification.setValue(SoTextStyle::TOP);}
 //{SoTextStyle* style = xAxisNode->getMagStyle(); 
 // style->horizontalJustification.setValue(SoTextStyle::LEFT);
 // style->verticalJustification.setValue(SoTextStyle::MIDDLE);}

  if(aNotify==FALSE) xAxisNode->enableNotify(flag);

 {SoTransform* transf = (SoTransform*)xAxisTransform.getValue();
  if(aNotify==FALSE) flag = transf->enableNotify(FALSE);
  SbMatrix matrix;
  matrix.makeIdentity();
  transf->setMatrix(matrix);
  SbMatrix rotx;
  rotx.setRotate(SbRotation(SbVec3f(1,0,0),FM_PI_2));
  transf->multLeft(rotx);
  if(aNotify==FALSE) transf->enableNotify(flag);
 }
}

void SoPlotter::updateYAxis3D(SbBool aNotify){  
  float YSIZ = height.getValue();
  float YMGL = bottomMargin.getValue();
  float YMGU = topMargin.getValue();
  float hData = YSIZ-YMGL-YMGU;

  SbBool flag;
  if(aNotify==FALSE) flag = yAxisNode->enableNotify(FALSE);

  yAxisNode->verbose.setValue(verbose.getValue());
  yAxisNode->tickUp.setValue(FALSE);
  yAxisNode->width.setValue(hData);

 {SoTextStyle* style = yAxisNode->getLabelsStyle(); 
  if(style->enforced.getValue()==FALSE) {
    style->fontName.setValue(SbFont_Hershey); //Enforce Hershey.
    style->orientationX.setValue(SbVec3f(-1,0,0));
    style->orientationY.setValue(SbVec3f( 0,1,0));
    style->horizontalJustification.setValue(SoTextStyle::CENTER);
    style->verticalJustification.setValue(SoTextStyle::TOP);
  }}

 {SoTextStyle* style = yAxisNode->getTitleStyle(); 
  style->orientationX.setValue(SbVec3f(-1,0,0));
  style->orientationY.setValue(SbVec3f( 0,1,0));
  style->horizontalJustification.setValue(SoTextStyle::LEFT);
  style->verticalJustification.setValue(SoTextStyle::TOP);}
 //{SoTextStyle* style = yAxisNode->getMagStyle(); 
 // style->horizontalJustification.setValue(SoTextStyle::CENTER);
 // style->verticalJustification.setValue(SoTextStyle::BOTTOM);}

  if(aNotify==FALSE) yAxisNode->enableNotify(flag);

 {SoTransform* transf = (SoTransform*)yAxisTransform.getValue();
  if(aNotify==FALSE) flag = transf->enableNotify(FALSE);
  SbMatrix matrix;
  matrix.makeIdentity();
  transf->setMatrix(matrix);
  SbMatrix roty;
  roty.setRotate(SbRotation(SbVec3f(0,1,0),FM_PI_2));
  transf->multLeft(roty);
  SbMatrix rotz;
  rotz.setRotate(SbRotation(SbVec3f(0,0,1),FM_PI_2));
  transf->multLeft(rotz);
  if(aNotify==FALSE) transf->enableNotify(flag);
 }
}

void SoPlotter::updateZAxis3D(SbBool aNotify){
  float ZSIZ = depth.getValue();
  float ZMGD = downMargin.getValue();
  float ZMGU = upMargin.getValue();
  float dData = ZSIZ-ZMGD-ZMGU;

  SbBool flag;
  if(aNotify==FALSE) flag = zAxisNode->enableNotify(FALSE);

  zAxisNode->verbose.setValue(verbose.getValue());
  zAxisNode->tickUp.setValue(FALSE);
  zAxisNode->width.setValue(dData);

 {SoTextStyle* style = zAxisNode->getLabelsStyle(); 
  if(style->enforced.getValue()==FALSE) {
    style->fontName.setValue(SbFont_Hershey); //Enforce Hershey.
    style->orientationX.setValue(SbVec3f(0,1,0));
    style->orientationY.setValue(SbVec3f(1,0,0));
    style->horizontalJustification.setValue(SoTextStyle::RIGHT);
    style->verticalJustification.setValue(SoTextStyle::MIDDLE);
  }}

 {SoTextStyle* style = zAxisNode->getTitleStyle(); 
  style->orientationX.setValue(SbVec3f(0,1,0));
  style->orientationY.setValue(SbVec3f(1,0,0));
  style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  style->verticalJustification.setValue(SoTextStyle::BOTTOM);}
 //{SoTextStyle* style = zAxisNode->getMagStyle(); 
 // style->horizontalJustification.setValue(SoTextStyle::CENTER);
 // style->verticalJustification.setValue(SoTextStyle::BOTTOM);}

  if(aNotify==FALSE) zAxisNode->enableNotify(flag);

 {SoTransform* transf = (SoTransform*)zAxisTransform.getValue();
  if(aNotify==FALSE) flag = transf->enableNotify(FALSE);
  SbMatrix matrix;
  matrix.makeIdentity();
  transf->setMatrix(matrix);
  SbMatrix trans;
  trans.setTranslate(SbVec3f(0,yAxisNode->width.getValue(),0));
  transf->multLeft(trans);
  SbMatrix rotz;
  rotz.setRotate(SbRotation(SbVec3f(0,0,1),-FM_PI_2));
  transf->multLeft(rotz);
  SbMatrix roty;
  roty.setRotate(SbRotation(SbVec3f(0,1,0),-FM_PI_2));
  transf->multLeft(roty); //Applied first.
  if(aNotify==FALSE) transf->enableNotify(flag);
 }
}

void SoPlotter::updateAxis(SoAxis* aAxis,SbAxis& aAxisData,SbBool aNotify){
#ifdef DEBUG
  printf("debug : SoPlotter::updateAxis %ld : %g %g\n\n",aAxis,aAxisData.getMinimumValue(),aAxisData.getMaximumValue());
#endif

  SbBool flag;
  if(aNotify==FALSE) flag = aAxis->enableNotify(FALSE);

  aAxis->minimumValue.setValue(aAxisData.getMinimumValue());
  aAxis->maximumValue.setValue(aAxisData.getMaximumValue());
  aAxis->logScale.setValue(aAxisData.isLogScale());

  aAxis->updateChildren(); // So that the grid be correct.

  if(aNotify==FALSE) aAxis->enableNotify(flag);
}

inline SbBool SbSTR_EMPTY(const char* aString) {
  return ((aString&&(*aString))?false:true);
}

void SoPlotter::updateLegend(){
  // WARNING : legend field is an "output" field. 
  //printf("debug : SoPlotter::updateLegend : begin\n");

  legend.deleteValues(0);
  int index = 0;

 {int number = f_binsList.size();
  for(int ibins=0;ibins<number;ibins++) {
    SbPlottableObject* obj = f_binsList[ibins];
    const char* leg = obj->getLegend();
    if(SbSTR_EMPTY(leg)) continue;
    //printf("debug : SoPlotter::updateLegend \"%s\"\n",leg);
    if(eq_Cast(b1,*obj,SbPlottableBins1D)){
      legend.set1Value(index,leg);
      SoStyle* style = mergeBinsStyle(ibins,*b1);
      char s[64];
      ::sprintf(s,"legendStyle[%d]",index);
      setPart(SbString(s),style);
      style->unref();
      index++;
    } else if(eq_Cast(b2,*obj,SbPlottableBins2D)){
      legend.set1Value(index,leg);
      SoStyle* style = mergeBinsStyle(ibins,*b2);
      char s[64];
      ::sprintf(s,"legendStyle[%d]",index);
      setPart(SbString(s),style);
      style->unref();
      index++;
    }
  }}

 {int number = f_pointsList.size();
  for(int ipoints=0;ipoints<number;ipoints++) {
    SbPlottableObject* obj = f_pointsList[ipoints];
    const char* leg = obj->getLegend();
    if(SbSTR_EMPTY(leg)) continue;
    if(eq_Cast(p2,*obj,SbPlottablePoints2D)){
      legend.set1Value(index,leg);
      SoStyle* style = mergePointsStyle(ipoints,*p2);
      char s[64];
      ::sprintf(s,"legendStyle[%d]",index);
      setPart(SbString(s),style);
      style->unref();
      index++;
    } else if(eq_Cast(p3,*obj,SbPlottablePoints3D)){
      legend.set1Value(index,leg);
      SoStyle* style = mergePointsStyle(ipoints,*p3);
      char s[64];
      ::sprintf(s,"legendStyle[%d]",index);
      setPart(SbString(s),style);
      style->unref();
      index++;
    }
  }}

 {int number = f_functionList.size();
  for(int ifunc=0;ifunc<number;ifunc++) {
    SbPlottableObject* obj = f_functionList[ifunc];
    const char* leg = obj->getLegend();
    if(SbSTR_EMPTY(leg)) continue;
    if(eq_Cast(f1,*obj,SbPlottableFunction1D)){
      legend.set1Value(index,leg);
      SoStyle* style = mergeFunctionStyle(ifunc,*f1);
      char s[64];
      ::sprintf(s,"legendStyle[%d]",index);
      setPart(SbString(s),style);
      style->unref();
      index++;
    } else if(eq_Cast(f2,*obj,SbPlottableFunction2D)){
      legend.set1Value(index,leg);
      SoStyle* style = mergeFunctionStyle(ifunc,*f2);
      char s[64];
      ::sprintf(s,"legendStyle[%d]",index);
      setPart(SbString(s),style);
      style->unref();
      index++;
    }
  }}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SbString SoPlotter::getTitle(){
  if(f_binsList.size()) { 
    return f_binsList[0]->getLabel();
  } else if(f_pointsList.size()) { 
    return f_pointsList[0]->getLabel();    
  } else if(f_functionList.size()) { 
    return f_functionList[0]->getLabel();
  } else {
    return "";
  }
}

SbString SoPlotter::getInfos(){
  SoStyle* soInfosStyle = (SoStyle*)infosStyle.getValue();
  const SbString& options = soInfosStyle->options.getValue();

  if(f_binsList.size()) { 
    return f_binsList[0]->getInfos(options.getString());
  } else if(f_pointsList.size()) { 
    return f_pointsList[0]->getInfos(options.getString());
  } else if(f_functionList.size()) { 
    return f_functionList[0]->getInfos(options.getString());
  } else {
    return SbString();
  }
}

SoDrawStyle* SoPlotter::getLineStyle(const SbStyle& aStyle){
  return fStyleCache->getLineStyle(aStyle.getLinePattern(),(float)aStyle.getLineWidth());
}

SoDrawStyle* SoPlotter::getLineStyle(SoStyle& aStyle){
  return fStyleCache->getLineStyle(aStyle.linePattern.getValue(),(float)aStyle.lineWidth.getValue());
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoPlotter::project2D(std::vector<SbVec3f>& aPoints,const SbRepBox& aBoxX,const SbRepBox& aBoxY){
  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  for(std::vector<SbVec3f>::iterator it = aPoints.begin();
      it!=aPoints.end();++it) {
    SbVec3f& v = (*it);
    float xx = v[0];
    float yy = v[1];
    xx = VerifyLog(xx,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    v[0] = xx;
    v[1] = yy;
  }
}

SbVec3f* SoPlotter::clipPoints2D(
 const std::vector<SbVec3f>& aPoints
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZ
,int& aNumber
){
  aNumber = 0;
  int number = aPoints.size();
  if(number<=0) return 0;

  SbVec3f* points = new SbVec3f[number];
  int ipoint = 0;

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  for(int index=0;index<number;index++) {
    float xx = aPoints[index][0];
    float yy = aPoints[index][1];

    xx = VerifyLog(xx,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);

    if((xx>=0)&&(xx<=1)&&(yy>=0)&&(yy<=1)) {
      LIST_SET(points,ipoint,xx,yy,aZ);
      ipoint++;
    }
  }

  if(ipoint<=0) {
    aNumber = 0;
    delete [] points;
    return 0;
  } else {
    aNumber = ipoint;
    return points;
  }
}

SbVec3f* SoPlotter::clipPoints3D(
 const std::vector<SbVec3f>& aPoints
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,int& aNumber
){
  aNumber = 0;
  int number = aPoints.size();
  if(number<=0) return 0;

  SbVec3f* points = new SbVec3f[number];
  int ipoint = 0;

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float zmin = aBoxZ.fPos;
  float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  for(int index=0;index<number;index++) {
    float xx = aPoints[index][0];
    float yy = aPoints[index][1];
    float zz = aPoints[index][2];

    xx = VerifyLog(xx,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    zz = VerifyLog(zz,zmin,dz,zlog);

    if( (xx>=0)&&(xx<=1) && 
        (yy>=0)&&(yy<=1) &&
        (zz>=0)&&(zz<=1)
        ) {
      LIST_SET(points,ipoint,xx,yy,zz);
      ipoint++;
    }
  }

  if(ipoint<=0) {
    aNumber = 0;
    delete [] points;
    return 0;
  } else {
    aNumber = ipoint;
    return points;
  }
}

SbVec3f* SoPlotter::clipPolyLine2D(
 const std::vector<SbVec3f>& aPoints
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,int& aNumber
)
{
  //  Clip line in aBoxX, aBoxY.

  //NOTE : it is not a general algorithm.
  //       It is assumed that aPoints are ordered with increasing x.
  //       And the algorithm clips against up and bottom BoxY lines.
  //       (Use SbClip for a more general algorithm ?)

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  int number = aPoints.size();
  SbVec3f* points = new SbVec3f[3 * number + 10];
  int ipoint = 0;

  float xprev = 0;
  float yprev = 0;
  for(int index=0;index<number;index++) {
    float xx = aPoints[index][0];
    float yy = aPoints[index][1];
    float zz = aPoints[index][2];
    xx = VerifyLog(xx,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    if((xx>=0)&&(xx<=1) ) {
      if(yy>1) {
        if(index==0) {
          LIST_SET(points,ipoint,xx,1,zz);ipoint++;
        } else {
          if(yprev>1) {
            LIST_SET(points,ipoint,xx,1,zz);ipoint++;
          } else if(yprev<0) {
            float a = (yy - yprev)/(xx - xprev);
            float b = yy - a * xx;
            LIST_SET(points,ipoint,-b/a,0,zz);ipoint++;
            LIST_SET(points,ipoint,(1 - b)/a,1,zz);ipoint++;
            LIST_SET(points,ipoint,xx,1,zz);ipoint++;
          } else {
            float a = (yy - yprev)/(xx - xprev);
            float b = yy - a * xx;
            LIST_SET(points,ipoint,(1 - b)/a,1,zz);ipoint++;
            LIST_SET(points,ipoint,xx,1,zz);ipoint++;
          }
        }
      } else if (yy < 0) {
        if(index==0) {
          LIST_SET(points,ipoint,xx,0,zz);ipoint++;
        } else {
          if(yprev<0) {
            LIST_SET(points,ipoint,xx,0,zz);ipoint++;
          } else if(yprev>1) {
            float a = (yy - yprev)/(xx - xprev);
            float b = yy - a * xx;
            LIST_SET(points,ipoint,(1 - b)/a,1,zz);ipoint++;
            LIST_SET(points,ipoint,-b/a,0,zz);ipoint++;
            LIST_SET(points,ipoint,xx,0,zz);ipoint++;
          } else {
            float a = (yy - yprev)/(xx - xprev);
            float b = yy - a * xx;
            LIST_SET(points,ipoint,-b/a,0,zz);ipoint++;
            LIST_SET(points,ipoint,xx,0,zz);ipoint++;
           }
        }
      } else {
        if(index==0) {
          LIST_SET(points,ipoint,xx,yy,zz);ipoint++;
        } else if( (yprev>1) || (yprev<0) ) {
          // interpolate :
          float a = (yy - yprev)/(xx - xprev);
          float b = yy - a * xx;
          if(yprev>1) {
            LIST_SET(points,ipoint,(1 - b)/a,1,zz);ipoint++;
          } else {
            LIST_SET(points,ipoint,-b/a,0,zz);ipoint++;
          }
          LIST_SET(points,ipoint,xx,yy,zz);ipoint++;
        } else {
          LIST_SET(points,ipoint,xx,yy,zz);ipoint++;
        }
      }
    }
    xprev = xx;
    yprev = yy;
  }

  if(ipoint<=0) {
    aNumber = 0;
    delete [] points;
    return 0;
  } else {
    aNumber = ipoint;
    return points;
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoPlotter::updateTopAxis(SbBool aNotify){
  float YSIZ = height.getValue();
  float YMGL = bottomMargin.getValue();
  float YMGU = topMargin.getValue();
  float hData = YSIZ-YMGL-YMGU;

  SoSeparator* sep = (SoSeparator*)topAxisSeparator.getValue();

  SbBool flag;
  if(aNotify==FALSE) flag = sep->enableNotify(FALSE);

  SoTransform* transf = new SoTransform;
  transf->translation.setValue(SbVec3f(0,hData,0));
  sep->addChild(transf);

  SoAxis* axis = (SoAxis*)xAxisNode->copy();
  SoTextStyle* style = axis->getLabelsStyle(); 
  style->visible.setValue(FALSE);
  axis->tickUp.setValue(FALSE);
  axis->title.setValue("");
  sep->addChild(axis);

  if(aNotify==FALSE) sep->enableNotify(flag);
}

void SoPlotter::updateRightAxis(SbBool aNotify){
  float XSIZ = width.getValue();
  float XMGL = leftMargin.getValue();
  float XMGR = rightMargin.getValue();
  float wData = XSIZ-XMGL-XMGR;

  SbBool flag;

  SoSeparator* sep = (SoSeparator*)rightAxisSeparator.getValue();
  if(aNotify==FALSE) flag = sep->enableNotify(FALSE);

  SoTransform* transf = new SoTransform;
  SbMatrix trans;
  trans.setTranslate(SbVec3f(wData,0,0));
  transf->multLeft(trans);
  SbMatrix roty;
  roty.setRotate(SbRotation(SbVec3f(0,1,0),FM_PI));
  transf->multLeft(roty);
  SbMatrix rotz;
  rotz.setRotate(SbRotation(SbVec3f(0,0,1),FM_PI_2));
  transf->multLeft(rotz); //Applied first.
  sep->addChild(transf);

  SoAxis* axis = (SoAxis*)yAxisNode->copy();
  SoTextStyle* style = axis->getLabelsStyle(); 
  style->visible.setValue(FALSE);
  axis->tickUp.setValue(FALSE);
  axis->title.setValue("");
  sep->addChild(axis);

  if(aNotify==FALSE) sep->enableNotify(flag);
}

static void buildWall(
 const SbVec3f& aVertex
,const SbVec3f& aX
,const SbVec3f& aY
,float aSx  
,float aSy
,SoSeparator* aWall
){
  if(aSx<=0) return;
  if(aSy<=0) return;

  SoGrid* grid = new SoGrid;

  // Fields :
 
  grid->position.setValue (aVertex);
  grid->X.setValue (aX);
  grid->Y.setValue (aY);
  grid->rows.setValue (1);
  grid->columns.setValue (1);
  grid->width.setValue (aSx);
  grid->height.setValue (aSy);
  
  aWall->addChild(grid);
}

void SoPlotter::updateWall2D(){
  //printf("debug : SoPlotter::updateWall2D\n");
 
  SoStyle* style = getWallStyle();
  if(style->visible.getValue()==FALSE) return;

  SoSeparator* sep = (SoSeparator*)wallSeparator.getValue();

  sep->addChild(fStyleCache->getMaterial(style->color.getValue(),style->transparency.getValue()));
      
  sep->addChild(fStyleCache->getFilled());
  
  buildWall(SbVec3f(0,0,-depth.getValue()/2),
            SbVec3f(1,0,0),SbVec3f(0,1,0),
            xAxisNode->width.getValue(),
            yAxisNode->width.getValue(),
            sep);
}

void SoPlotter::updateWall3D(){
  //printf("debug : SoPlotter::updateWall3D\n");

  SoStyle* style = getWallStyle();
  if(style->visible.getValue()==FALSE) return;

  SoSeparator* sep = (SoSeparator*)wallSeparator.getValue();

  sep->addChild(fStyleCache->getMaterial(style->color.getValue(),style->transparency.getValue()));
      
  sep->addChild(fStyleCache->getFilled());
    
  // plan x-y at z = 0.
 {float ZSIZ = depth.getValue();
  float ZMGD = downMargin.getValue();
  float ZMGU = upMargin.getValue();
  float dData = ZSIZ-ZMGD-ZMGU;
  float woffset = dData * 0.01F; //FIXME : PLANE_OFFSET
  //float woffset = 0;
  buildWall(SbVec3f(0,0,-woffset),
            SbVec3f(1,0,0),
            SbVec3f(0,1,0),
            xAxisNode->width.getValue()+woffset,
            yAxisNode->width.getValue()+woffset,
            sep);}

  // plan z-y at x = 1.
 {float XSIZ = width.getValue();
  float XMGL = leftMargin.getValue();
  float XMGR = rightMargin.getValue();
  float wData = XSIZ-XMGL-XMGR;
  float woffset = wData * 0.01F;
  buildWall(SbVec3f(xAxisNode->width.getValue()+woffset,0,-woffset),
            SbVec3f(0,0,1),
            SbVec3f(0,1,0),
            zAxisNode->width.getValue()+woffset,
            yAxisNode->width.getValue()+woffset,
            sep);}

  // plan x-z at y = 1.
 {float YSIZ = height.getValue();
  float YMGL = bottomMargin.getValue();
  float YMGU = topMargin.getValue();
  float hData = YSIZ-YMGL-YMGU;
  float woffset = hData * 0.01F;
  buildWall(SbVec3f(0,yAxisNode->width.getValue()+woffset,-woffset),
            SbVec3f(1,0,0),
            SbVec3f(0,0,1),
            xAxisNode->width.getValue()+woffset,
            zAxisNode->width.getValue()+woffset,
            sep);}
}

void SoPlotter::updateGrid2D(){
  SoStyle* style = getGridStyle();
  if(style->visible.getValue()==FALSE) return;

  int pos = 0;
  int number = xAxisNode->tickNumber.getValue() + yAxisNode->tickNumber.getValue();
  if(number<=0) return;

  SbBool draw_vertical = TRUE;
  SbBool draw_horizontal = TRUE;
  if(style->options.getValue()=="vertical") draw_horizontal = FALSE;
  if(style->options.getValue()=="horizontal") draw_vertical = FALSE;

  SoSeparator* sep = (SoSeparator*)gridSeparator.getValue();

  sep->addChild(fStyleCache->getMaterial(style->color.getValue(),style->transparency.getValue()));

  sep->addChild(getLineStyle(*style));

  float gridOffset = depth.getValue()/2; //In from of data.

  // number of points :
  number = 0;
  if(draw_vertical==TRUE) {
    number += xAxisNode->coords.getNum()*2;
    if(xAxisNode->logScale.getValue()==TRUE) {
      number += xAxisNode->subCoords.getNum()*2;
    }
  }
  if(draw_horizontal==TRUE) {
    number += yAxisNode->coords.getNum()*2;
    if(yAxisNode->logScale.getValue()==TRUE) {
      number += yAxisNode->subCoords.getNum()*2;
    }
  }

  SbVec3f* points = new SbVec3f[number];

  if(draw_vertical==TRUE) {
    float width = yAxisNode->width.getValue();
   {int number = xAxisNode->coords.getNum();
    for(int count=0;count<number;count++) { 
      float xx = xAxisNode->coords[count];
      LIST_SET(points,pos,xx, 0,gridOffset);pos++;
      LIST_SET(points,pos,xx, width,gridOffset);pos++;
    }}
    if(xAxisNode->logScale.getValue()==TRUE) {
      int number = xAxisNode->subCoords.getNum();
      for(int count=0;count<number;count++) { 
        float xx = xAxisNode->subCoords[count];
        LIST_SET(points,pos,xx, 0,gridOffset);pos++;
        LIST_SET(points,pos,xx, width,gridOffset);pos++;
      }
    }
  }

  if(draw_horizontal==TRUE) {
    float width = xAxisNode->width.getValue();
   {int number = yAxisNode->coords.getNum();
    for(int count=0;count<number;count++) {
      float yy = yAxisNode->coords[count];
      LIST_SET(points,pos,0,yy,gridOffset);pos++;
      LIST_SET(points,pos,width,yy,gridOffset);pos++;
    }}
    if(yAxisNode->logScale.getValue()==TRUE) {
      int number = yAxisNode->subCoords.getNum();
      for(int count=0;count<number;count++) { 
        float yy = yAxisNode->subCoords[count];
        LIST_SET(points,pos,0,yy,gridOffset);pos++;
        LIST_SET(points,pos,width,yy,gridOffset);pos++;
      }
    }
  }

  if(pos>0) {
    SoCoordinate3* coordinate3 = new SoCoordinate3;
    int32_t pointn = pos;
    coordinate3->point.setValues(0,pointn,points);
    sep->addChild(coordinate3);

    SoLineSet* lineSet = new SoLineSet;
    int segmentn = pointn/2;
    int32_t* vertices = new int32_t[segmentn];
    for (int count=0;count<segmentn;count++) vertices[count] = 2;
    lineSet->numVertices.setValues(0,segmentn,vertices);
    delete [] vertices;
    sep->addChild(lineSet);
  }
  delete [] points;

}

void SoPlotter::updateGrid3D(){
  SoStyle* style = getGridStyle();
  if(style->visible.getValue()==FALSE) return;

  int ntick = zAxisNode->tickNumber.getValue();
  if(ntick<=0) return;

  SoSeparator* sep = (SoSeparator*)gridSeparator.getValue();

  sep->addChild(fStyleCache->getMaterial(style->color.getValue(),style->transparency.getValue()));

  sep->addChild(getLineStyle(*style));

  SbVec3f* points = new SbVec3f[4 * ntick];
  int pos = 0;
  for(int count=0;count<ntick;count++) {
    float xe = xAxisNode->width.getValue();
    float ye = yAxisNode->width.getValue();
    float zz = zAxisNode->coords[count];
    LIST_SET(points,pos,0 ,ye,zz);pos++;
    LIST_SET(points,pos,xe,ye,zz);pos++;
    LIST_SET(points,pos,xe,ye,zz);pos++;
    LIST_SET(points,pos,xe,0 ,zz);pos++;
  }
  if(pos>0) {
    SoCoordinate3* coordinate3 = new SoCoordinate3;
    int32_t pointn = pos;
    coordinate3->point.setValues(0,pointn,points);
    sep->addChild(coordinate3);

    SoLineSet* lineSet = new SoLineSet;
    int segmentn = pointn/2;
    int32_t* vertices = new int32_t[segmentn];
    for (int count=0;count<segmentn;count++) vertices[count] = 2;
    lineSet->numVertices.setValues(0,segmentn,vertices);
    delete [] vertices;
    sep->addChild(lineSet);
  }

  delete [] points;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoPlotter::updateBins1D_XY(
 const SbPlottableBins1D& aBins
,int aIndex
,SoStyle& aGraphicStyle
,SoStyle& aRightHatchStyle
,SoStyle& aLeftHatchStyle
,SoStyle& aErrorsStyle
,int aIndex1D
,const std::vector<SbPlottableBins1D*>& aBins1DList
,const SbPList& aBins1DListSwMnMx
,SbBool aSuperpose
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZZ
){
  //printf("debug : SoPlotter::updateBins1D_XY : %d\n",aIndex1D);

  std::string sid;
 {std::string sp;
  if(!inlib::p2sx(const_cast<SbPlottableBins1D*>(&aBins),sp)){}
  sid = "SbObject/"+sp;}

  int xnbin = aBins.getAxisNumberOfBins();
  std::vector<SbRepBin1D> bins(xnbin);
  for(int ibin=0;ibin<xnbin;ibin++) { 
    float xx = aBins.getBinLowerEdge(ibin);
    float xe = aBins.getBinUpperEdge(ibin);
    float vmin,vmax;
    if(aSuperpose==TRUE) {
      getHeight(aIndex1D,aBins1DList,aBins1DListSwMnMx,ibin,vmin,vmax);
    } else {
      vmin = 0;
      vmax = aBins.getBinSumOfWeights(ibin);
    }
    bins[ibin] = SbRepBin1D(xx,xe,vmin,vmax);
  }

  float zHatchOffset = 0.01F;
  float zErrorOffset = 2 * zHatchOffset;

  if(aGraphicStyle.visible.getValue()==TRUE) {

    SbPaintingPolicy painting = (SbPaintingPolicy)aGraphicStyle.painting.getValue();
    if(painting==SbPaintingByValue) {
      f_bins_cmaps[aIndex] = new SbByValueColormap(aGraphicStyle.colorMapping.getValue());
    } else if( (painting==SbPaintingGreyScale) ||
               (painting==SbPaintingGreyScaleInverse) ||
               (painting==SbPaintingVioletToRed) ){
      //WARNING : if multi-thread filling, bmin/bmax may be not in sync !
      float bmin = (*((SbVec2f*)aBins1DListSwMnMx[aIndex1D]))[0];
      float bmax = (*((SbVec2f*)aBins1DListSwMnMx[aIndex1D]))[1];
     {float dbins = bmax-bmin;
      if(dbins!=0.0F) {
        for(int ibin=0;ibin<xnbin;ibin++) { 
          bins[ibin].fRatio = aBins.getBinSumOfWeights(ibin)/dbins;
        }
      }}
      if(painting==SbPaintingGreyScale) {
        f_bins_cmaps[aIndex] = new SbGreyScaleColormap(bmin,bmax,50);
      } else if(painting==SbPaintingGreyScaleInverse) {
        f_bins_cmaps[aIndex] = new SbGreyScaleInverseColormap(bmin,bmax,50);
      } else if(painting==SbPaintingVioletToRed) {
        f_bins_cmaps[aIndex] = new SbVioletToRedColormap(bmin,bmax,50);
      }
    } else {
      f_bins_cmaps[aIndex] = new SbConstColormap(aGraphicStyle.color.getValue());
    }

    const SbString& modeling = aGraphicStyle.modeling.getValue();

    SbBool oneNode = FALSE;
   {int nlimit = aGraphicStyle.multiNodeLimit.getValue();
    if(nlimit!=NoLimit) {
      oneNode = (xnbin>nlimit?TRUE:FALSE);
    }}

    SbBool barChart = FALSE;

    SoSeparator* binsNode = (SoSeparator*)binsSeparator.getValue();

    std::string sid_bin;
   {std::string sp;
    if(!inlib::p2sx(&aBins,sp)){}
    sid_bin = "SbBin1D/"+sp;}

    if(modeling==SbModeling_top_lines){
      if(oneNode) {
        repBins1D_xy_top_lines_one(binsNode,aGraphicStyle,bins,aBoxX,aBoxY,aZZ,SbString(sid.c_str()));
      } else {
        repBins1D_xy_top_lines(binsNode,aGraphicStyle,
                               painting,*(f_bins_cmaps[aIndex]),
                               bins,aBoxX,aBoxY,aZZ,SbString(sid_bin.c_str()));
      }
    } else if(modeling==SbModeling_points) {
      if(oneNode) {
        repBins1D_xy_points_one(binsNode,aGraphicStyle,bins,aBoxX,aBoxY,aZZ,SbString(sid.c_str()));
      } else {
        repBins1D_xy_points(binsNode,aGraphicStyle,
                            painting,*(f_bins_cmaps[aIndex]),
			    bins,aBoxX,aBoxY,aZZ,SbString(sid_bin.c_str()));
      }
    } else if(modeling==SbModeling_boxes) {
      if(oneNode) {
        repBins1D_xy_boxes_one(binsNode,aGraphicStyle,bins,aBoxX,aBoxY,aZZ,SbString(sid.c_str()));
      } else {
        repBins1D_xy_boxes(binsNode,aGraphicStyle,
                           painting,*(f_bins_cmaps[aIndex]),
                           bins,aBoxX,aBoxY,aZZ,SbString(sid_bin.c_str()));
      }
    } else if(modeling==SbModeling_wire_boxes) {
      if(oneNode) {
        repBins1D_xy_wire_boxes_one(binsNode,aGraphicStyle,barChart,bins,aBoxX,aBoxY,aZZ,SbString(sid.c_str()));
      } else {
        repBins1D_xy_wire_boxes(binsNode,aGraphicStyle,barChart,
                                painting,*(f_bins_cmaps[aIndex]),
				bins,aBoxX,aBoxY,aZZ,SbString(sid_bin.c_str()));
      }
    } else if(modeling==SbModeling_bar_chart) {
      barChart = TRUE;
      if(oneNode) {
        repBins1D_xy_wire_boxes_one(binsNode,aGraphicStyle,barChart,bins,aBoxX,aBoxY,aZZ,SbString(sid.c_str()));
      } else {
        repBins1D_xy_wire_boxes(binsNode,aGraphicStyle,barChart,
                                painting,*(f_bins_cmaps[aIndex]),
				bins,aBoxX,aBoxY,aZZ,SbString(sid_bin.c_str()));
      }
    } else if(modeling==SbModeling_lines){
      repBins1D_xy_lines_one(binsNode,aGraphicStyle,bins,aBoxX,aBoxY,aZZ,SbString(sid.c_str()));
    } else if(modeling==SbModeling_curve){
      repBins1D_xy_curve_one(binsNode,aGraphicStyle,bins,aBoxX,aBoxY,aZZ,SbString(sid.c_str()));
    }
  
    SbHatchingPolicy hatching = (SbHatchingPolicy)aGraphicStyle.hatching.getValue();
    if(hatching!=SbHatchingNone) {
      // WARNING : must come AFTER repBins1D_xy.
      if((hatching==SbHatchingRight)||((hatching==SbHatchingLeftAndRight))) {
        if(oneNode) {
          repHatch1D_xy_one(binsNode,aRightHatchStyle,barChart,bins,aBoxX,aBoxY,aZZ+zHatchOffset,SbString(sid.c_str()));
        } else {
          repHatch1D_xy(binsNode,aRightHatchStyle,barChart,bins,aBoxX,aBoxY,aZZ+zHatchOffset);
        }
      }
      if((hatching==SbHatchingLeft)||((hatching==SbHatchingLeftAndRight))) {
        if(oneNode) {
          repHatch1D_xy_one(binsNode,aLeftHatchStyle,barChart,bins,aBoxX,aBoxY,aZZ+zHatchOffset,SbString(sid.c_str()));
        } else {
          repHatch1D_xy(binsNode,aLeftHatchStyle,barChart,bins,aBoxX,aBoxY,aZZ+zHatchOffset);
        }
      }
    }

  } //end graphicStyle visible

 {// Errors :
  if(aErrorsStyle.visible.getValue()==TRUE) { 
    std::vector<float> bars(xnbin);
    for(int ibin=0;ibin<xnbin;ibin++) bars[ibin] = aBins.getBinBar(ibin);

    SoSeparator* errorsNode = (SoSeparator*)errorsSeparator.getValue();
    SbMarkerStyle markerStyle = 
    (SbMarkerStyle)aErrorsStyle.markerStyle.getValue();
    if(markerStyle==SbMarkerPlus)
      repErrorsPlus_xy(errorsNode,aErrorsStyle,bins,aBoxX,aBoxY,bars,aZZ+zErrorOffset,SbString(sid.c_str()));
    else 
      repErrorsI_xy(errorsNode,aErrorsStyle,bins,aBoxX,aBoxY,bars,aZZ+zErrorOffset,SbString(sid.c_str()));
 }}
}

typedef struct {
  const SbPlottableFunction2D* fFunc2D;
  const SbPlottableBins2D* fBins2D;
  double fLimits[4];
  double fLimitsIn[4];
  SbBool fProblem;
} SbFunc;

static double bins2D_to_contour(double aX,double aY,void* aData) {
  SbFunc* func =(SbFunc*)aData;
  if( (aX<func->fLimitsIn[0])||
      (aX>func->fLimitsIn[1])||
      (aY<func->fLimitsIn[2])||
      (aY>func->fLimitsIn[3])
    ) return -FLT_MAX;
  float value;
  if(!bins2DToFunc(*(func->fBins2D),(float)aX,(float)aY,value)) func->fProblem = TRUE;
  return value;
}
static double log_bins2D_to_contour(double aX,double aY,void* aData) {
  SbFunc* func =(SbFunc*)aData;
  if( (aX<func->fLimitsIn[0])||
      (aX>func->fLimitsIn[1])||
      (aY<func->fLimitsIn[2])||
      (aY>func->fLimitsIn[3])
    ) return -FLT_MAX;
  float value;
  if(!bins2DToFunc(*(func->fBins2D),(float)aX,(float)aY,value)) func->fProblem = TRUE;
  return TakeLog(value);
}

void SoPlotter::updateBins2D_XY(const SbPlottableBins2D& aBins,int aIndex,SoStyle& aGraphicStyle,
                                const SbRepBox& aBoxX,const SbRepBox& aBoxY,const SbRepBox& aBoxZ,float aZZ) {
  SoSeparator* binsNode = (SoSeparator*)binsSeparator.getValue();

  if(aGraphicStyle.visible.getValue()==TRUE) {

  const SbString& modeling = aGraphicStyle.modeling.getValue();
  SbPaintingPolicy painting = (SbPaintingPolicy)aGraphicStyle.painting.getValue();

  int xnbin = aBins.getAxisNumberOfBinsX();
  int ynbin = aBins.getAxisNumberOfBinsY();

  float bmin;
  float bmax;
  aBins.getBinsSumOfWeightsRange(bmin,bmax);

  std::string sid;
 {std::string sp;
  if(!inlib::p2sx(const_cast<SbPlottableBins2D*>(&aBins),sp)){}
  sid = "SbObject/"+sp;}

  if( (modeling==SbModeling_curve)  || (modeling==SbModeling_filled_curve) ){

    float zmin = aBoxZ.fPos;
    float dz = aBoxZ.fWidth;
    SbBool zlog = aBoxZ.fLog;

    float xmn = f_xDataAxis.getMinimumValue();
    float xmx = f_xDataAxis.getMaximumValue();
    float ymn = f_yDataAxis.getMinimumValue();
    float ymx = f_yDataAxis.getMaximumValue();

    inlib::clist_contour listContour;
    //int nFir = 32;
    int nFir = 128;
    listContour.set_first_grid(nFir,nFir); //Default : 32,32 
    //int nSec = 256;
    int nSec = 512;
    listContour.set_secondary_grid(nSec,nSec); //Default : 256,256.

    double limits[4];
    // User limits :
    limits[0] = xmn;
    limits[1] = xmx;    
    limits[2] = ymn;    
    limits[3] = ymx;    

    SbFunc sbFunc;
    sbFunc.fFunc2D = 0;
    sbFunc.fProblem = FALSE;
    sbFunc.fBins2D = &aBins;
    sbFunc.fLimitsIn[0] = limits[0];
    sbFunc.fLimitsIn[1] = limits[1];
    sbFunc.fLimitsIn[2] = limits[2];
    sbFunc.fLimitsIn[3] = limits[3];

    // Extend the grid to have some borders in order to close contours :
    int n = nSec - 2 * 10;
    double dx = (limits[1]-limits[0]) /n;
    double dy = (limits[3]-limits[2]) /n;
    limits[0] = limits[0] - 10 * dx;
    limits[1] = limits[1] + 10 * dx;
    limits[2] = limits[2] - 10 * dy;
    limits[3] = limits[3] + 10 * dy;

    sbFunc.fLimits[0] = limits[0];
    sbFunc.fLimits[1] = limits[1];
    sbFunc.fLimits[2] = limits[2];
    sbFunc.fLimits[3] = limits[3];

    listContour.set_limits(limits);

    if(levels.getNum()) {
      int zn = levels.getNum();
      std::vector<double> zs(zn);
      for(int zi=0;zi<zn;zi++) zs[zi] = levels[zi];
      listContour.set_planes(zs);
    } else {
      int zn = numberOfLevels.getValue();
      if(zn<=0) zn = 1;
      std::vector<double> zs(zn+1);
      float zmax = zmin + dz;
      double zd = (zmax-zmin)/zn;
      for(int zi=0;zi<=zn;zi++) zs[zi] = zmin + zi * zd;
      listContour.set_planes(zs);
    }
    if(zlog==TRUE)
      listContour.set_field_fcn(log_bins2D_to_contour,(void*)&sbFunc);
    else
      listContour.set_field_fcn(bins2D_to_contour,(void*)&sbFunc);

    listContour.ccontour::generate();
    if(!listContour.compact_strips ()) {
      SoDebugError::postInfo("SoPlotter::updateBins2D_XY"," clist_contour::compact_strips () : failure.");
    } else {
  
      if( (painting==SbPaintingByLevel) || (painting==SbPaintingByValue) ){
        f_bins_cmaps[aIndex] = new SbByValueColormap(aGraphicStyle.colorMapping.getValue());
        SbBool zlog = aBoxZ.fLog;
        if(zlog==TRUE) f_bins_cmaps[aIndex]->set_PAW_coloring();
      } else {
        f_bins_cmaps[aIndex] = new SbConstColormap(aGraphicStyle.color.getValue());
      }

      if(modeling==SbModeling_filled_curve) {
        repContour_xy_filled(binsNode,
                             aGraphicStyle,
                             painting,*(f_bins_cmaps[aIndex]),
                             listContour,aBoxX,aBoxY,aBoxZ,aZZ,SbString(sid.c_str()));
      } else {
        repContour_xy(binsNode,aGraphicStyle,
                      painting,*(f_bins_cmaps[aIndex]),
                      listContour,aBoxX,aBoxY,aBoxZ,aZZ,SbString(sid.c_str()));
      }

    }

    if(sbFunc.fProblem==TRUE) {
      SoDebugError::postInfo("SoPlotter::updateFunction_XY(SbPlottableFunction2D)",
                             "problem when getting some function value.");
    }

  } else {

  bool hbe = aBins.hasEntriesPerBin();

  std::vector<SbRepBin2D> bins;
 {for(int jbin=ynbin-1;jbin>=0;jbin--) {
    for(int ibin=xnbin-1;ibin>=0;ibin--) { 
      if(hbe && (aBins.getBinNumberOfEntries(ibin,jbin)<=0)) continue;

      float val = aBins.getBinSumOfWeights(ibin,jbin);

      float xx = aBins.getBinLowerEdgeX(ibin);
      float xe = aBins.getBinUpperEdgeX(ibin);
      float yy = aBins.getBinLowerEdgeY(jbin);
      float ye = aBins.getBinUpperEdgeY(jbin);
          
      bins.push_back(SbRepBin2D(xx,xe,yy,ye,val,ibin,jbin));
    }
  }}
  int number = bins.size();

  if(painting==SbPaintingByValue) {
    f_bins_cmaps[aIndex] = 
      new SbByValueColormap(aGraphicStyle.colorMapping.getValue());
  } else if( (painting==SbPaintingGreyScale) ||
             (painting==SbPaintingGreyScaleInverse) ||
             (painting==SbPaintingVioletToRed) ){
    //WARNING : if multi-thread filling, bmin/bmax may be not in sync !
    float bmin,bmax;
    aBins.getBinsSumOfWeightsRange(bmin,bmax);
    float dbins = bmax-bmin;
    for(int index=0;index<number;index++) {
      bins[index].fRatio = bins[index].fVal/dbins;
    }
    if(painting==SbPaintingGreyScale) {
      f_bins_cmaps[aIndex] = new SbGreyScaleColormap(bmin,bmax,50);
    } else if(painting==SbPaintingGreyScaleInverse) {
      f_bins_cmaps[aIndex] = new SbGreyScaleInverseColormap(bmin,bmax,50);
    } else if(painting==SbPaintingVioletToRed) {
      f_bins_cmaps[aIndex] = new SbVioletToRedColormap(bmin,bmax,50);
    }
  } else {
    f_bins_cmaps[aIndex] = 
      new SbConstColormap(aGraphicStyle.color.getValue());
  }
          
  std::string sid_bin;
 {std::string sp;
  if(!inlib::p2sx(&aBins,sp)){}
  sid_bin = "SbBin2D/"+sp;}

  if(modeling==SbModeling_solid) {
    SbBool oneNode = FALSE;
   {int nlimit = aGraphicStyle.multiNodeLimit.getValue();
    if(nlimit!=NoLimit) {
      oneNode = (number>nlimit?TRUE:FALSE);
    }}

    if(painting==SbPaintingByValue) {
      SbBool zlog = aBoxZ.fLog;
      if(zlog==TRUE) f_bins_cmaps[aIndex]->set_PAW_coloring();
    }

    if(oneNode) {
      repBins2D_xy_solid_one(binsNode,
                             aGraphicStyle,
                             painting,*(f_bins_cmaps[aIndex]),
                             bins,aBoxX,aBoxY,aBoxZ,SbString(sid.c_str()));
    } else {
      repBins2D_xy_solid(binsNode,
                         aGraphicStyle,
                         painting,*(f_bins_cmaps[aIndex]),
                         bins,aBoxX,aBoxY,aBoxZ,
                         SbString(sid_bin.c_str()));
    }

  } else if(modeling==SbModeling_points) {
    repBins2D_xy_random_one(binsNode,
                            aGraphicStyle,
                            bins,aBoxX,aBoxY,bmin,bmax,SbString(sid.c_str()));
  } else if(modeling==SbModeling_boxes) {
    // one node decision :
    SbBool oneNode = FALSE;
   {int nlimit = aGraphicStyle.multiNodeLimit.getValue();
    if(nlimit!=NoLimit) {
      oneNode = (number>nlimit?TRUE:FALSE);
    }}

    if(oneNode) {
      repBins2D_xy_box_one(binsNode,
                           aGraphicStyle,
                           bins,aBoxX,aBoxY,bmin,bmax,SbString(sid.c_str()));
    } else {
      repBins2D_xy_box(binsNode,
                       aGraphicStyle,
                       bins,aBoxX,aBoxY,bmin,bmax,
                       SbString(sid_bin.c_str()));
    }
  } else if(modeling==SbModeling_wire_boxes) {
    // one node decision :
    SbBool oneNode = FALSE;
   {int nlimit = aGraphicStyle.multiNodeLimit.getValue();
    if(nlimit!=NoLimit) {
      oneNode = (number>nlimit?TRUE:FALSE);
    }}

    if(oneNode) {
      repBins2D_xy_wire_box_one(binsNode,
                                aGraphicStyle,
                                bins,aBoxX,aBoxY,bmin,bmax,SbString(sid.c_str()));
    } else {
      repBins2D_xy_wire_box(binsNode,
                            aGraphicStyle,
                            bins,aBoxX,aBoxY,bmin,bmax,
                            SbString(sid_bin.c_str()));
    }
  } else if(modeling==SbModeling_texts) {
    // one node decision :
    SbBool oneNode = FALSE;
   {int nlimit = aGraphicStyle.multiNodeLimit.getValue();
    if(nlimit!=NoLimit) {
      oneNode = (number>nlimit?TRUE:FALSE);
    }}

    if(oneNode) {
      repBins2D_xy_text_one(binsNode,
                            aGraphicStyle,
                            bins,aBoxX,aBoxY,SbString(sid.c_str()));
    } else {
      repBins2D_xy_text(binsNode,
                        aGraphicStyle,
                        bins,aBoxX,aBoxY,
                        SbString(sid_bin.c_str()));
     }
  }

  } //end else modeling curve
  } //end graphicStyle visible

}

static double function_to_contour(double aX,double aY,void* aData) {
  SbFunc* func =(SbFunc*)aData;
  if( (aX<func->fLimitsIn[0])||
      (aX>func->fLimitsIn[1])||
      (aY<func->fLimitsIn[2])||
      (aY>func->fLimitsIn[3])
    ) return -FLT_MAX;
  float value;
  if(!func->fFunc2D->value((float)aX,(float)aY,value)) func->fProblem = TRUE;
  return double(value);
}
static double log_function_to_contour(double aX,double aY,void* aData) {
  SbFunc* func =(SbFunc*)aData;
  if( (aX<func->fLimitsIn[0])||
      (aX>func->fLimitsIn[1])||
      (aY<func->fLimitsIn[2])||
      (aY>func->fLimitsIn[3])
    ) return -FLT_MAX;
  float value;
  if(!func->fFunc2D->value((float)aX,(float)aY,value)) func->fProblem = TRUE;
  return TakeLog(double(value));
}

void SoPlotter::updateFunction_XY(const SbPlottableFunction1D& aFunction,SoStyle& aGraphicStyle,
                                  const SbRepBox& aBoxX,const SbRepBox& aBoxY,const SbRepBox& aBoxZ,float aZZ){
  SoSeparator* functionNode = (SoSeparator*)functionSeparator.getValue();

  if(aGraphicStyle.visible.getValue()==TRUE) {

  const SbString& modeling = aGraphicStyle.modeling.getValue();

  float xmn = aFunction.getMinimumX();
  float xmx = aFunction.getMaximumX();
  if(xmx<=xmn) {
    xmn = f_xDataAxis.getMinimumValue();
    xmx = f_xDataAxis.getMaximumValue();
  }
  int nstp = aFunction.getNumberOfStepsX();
  nstp = nstp <=0 ? 100 : nstp;
  
  float df = (xmx - xmn)/nstp;      
    
  SbBool problem = FALSE;
  std::vector<SbVec3f> points(nstp+1);
  for(int ibin=0;ibin<=nstp;ibin++) { 
    float xx = xmn + ibin * df;
    float val;
    if(!aFunction.value(xx,val)) problem = TRUE;
    points[ibin] = SbVec3f(xx,val,aZZ);
  }
  if(problem==TRUE) {
    SoDebugError::postInfo("SoPlotter::updateFunction_XY(SbPlottableFunction1D)",
                           "problem when getting some function value.");
  }
  
  std::string sid;
 {std::string sp;
  if(!inlib::p2sx(&aFunction,sp)){}
  sid = "SbFunction1D/"+sp;}

  if(modeling==SbModeling_points)  {

    int ptn;
    SbVec3f* pts = clipPoints2D(points,aBoxX,aBoxY,0,ptn);
    if(ptn) {
      SoSceneGraph* separator = new SoSceneGraph();
      separator->setString(sid.c_str());
      functionNode->addChild(separator);
      separator->addChild(fStyleCache->getPoints());
      separator->addChild(fStyleCache->getMaterial(aGraphicStyle.color.getValue(),aGraphicStyle.transparency.getValue()));
      SoTools_addPointsToNode(separator,ptn,pts,
                              (SbMarkerStyle)aGraphicStyle.markerStyle.getValue(),aGraphicStyle.markerSize.getValue());
      delete [] pts;
    }  

  } else { //Default.

    int ptn;
    SbVec3f* pts = clipPolyLine2D(points,aBoxX,aBoxY,ptn);
    if(ptn) {
      SoSceneGraph* separator = new SoSceneGraph();
      separator->setString(sid.c_str());
      functionNode->addChild(separator);
      separator->addChild(getLineStyle(aGraphicStyle));
      separator->addChild(fStyleCache->getNormalBindingOverall());
      separator->addChild(fStyleCache->getNormalZ());
      separator->addChild(fStyleCache->getMaterial(aGraphicStyle.color.getValue(),aGraphicStyle.transparency.getValue()));
      SoTools_addLinesToNode(separator,ptn,pts);
      delete [] pts;
    }  

  }

  } //end graphicStyle visible
}

class SoPlotter_dragger : public SoDragPoint2D {
public: //SoDragPoint2D
  virtual void updateCoordinate3() {
    if(!fFunction.dragger_update_points()) return;

   {SbVec3f p;
    fPlotter.dataFrame2axis(point(),p);
    SbBool flag = fPlotter.frozen.getValue();
    fPlotter.frozen.setValue(TRUE);
    fFunction.setIthPoint(index(),p[0],p[1]);
    fPlotter.resetAllUpdateFlags();
    fPlotter.frozen.setValue(flag);}

    int npoint = fFunction.getNumberOfPoints();
    std::vector<SbVec3f> points(npoint);
    for(int count=0;count<npoint;count++) {
      float xx,yy;
      bool isControl;
      fFunction.getIthPoint(count,xx,yy,isControl);
      points[count] = SbVec3f(xx,yy,fZZ);
    }
    //check closure :
    if((points.size()>=2) && (points[points.size()-1]!=points[0]) ) {
      points.push_back(points[0]);
    }
    fPlotter.project2D(points,fBoxX,fBoxY);
    int ptn = points.size();
    if(ptn) {
      SbVec3f* pts = &(points[0]);
      coordinate3().point.setValues(0,ptn,pts);
    }
  }
public:
  SoPlotter_dragger(SoCoordinate3& aCoord,int aIndex,float aScale,SoPlotter& aPlotter,SbPlottableFunction2D& aFunction,
                    const SbRepBox& aBoxX,const SbRepBox& aBoxY,float aZZ)
  :SoDragPoint2D(aCoord,aIndex,aScale),fPlotter(aPlotter),fFunction(aFunction),fBoxX(aBoxX),fBoxY(aBoxY),fZZ(aZZ){
    SoDragger::addFinishCallback(finish_CB);
  }
protected:
  virtual ~SoPlotter_dragger(){
    SoDragger::removeFinishCallback(finish_CB);
  }
private:
  static void finish_CB(void*,SoDragger* aDragger) {
    SoPlotter_dragger* This = (SoPlotter_dragger*)aDragger;
    SbVec3f p;
    This->fPlotter.dataFrame2axis(This->point(),p);
    //printf("debug : finish_CB : %g %g %g\n",p[0],p[1],p[2]);

    //NOTE : the setIthPoint may induce an updateChildren
    //       and then a delete of the dragger.

    SbBool flag = This->fPlotter.frozen.getValue();
    This->fPlotter.frozen.setValue(TRUE);
    This->fFunction.setIthPoint(This->index(),p[0],p[1]);
    This->fPlotter.frozen.setValue(flag);
  }
private:
  SoPlotter& fPlotter;
  SbPlottableFunction2D& fFunction;
  SbRepBox fBoxX;
  SbRepBox fBoxY;
  float fZZ;
};

void SoPlotter::updateFunction_XY(const SbPlottableFunction2D& aFunction,int aIndex,SoStyle& aGraphicStyle,
                                  const SbRepBox& aBoxX,const SbRepBox& aBoxY,const SbRepBox& aBoxZ,float aZZ){
  //::printf("debug : SoPlotter : updateFunction_XY(Function2D) : begin : 005\n");
  SoSeparator* functionNode = (SoSeparator*)functionSeparator.getValue();

  if(aGraphicStyle.visible.getValue()==TRUE) {

  std::string sid;
 {std::string sp;
  if(!inlib::p2sx(&aFunction,sp)){}
  sid = "SbFunction2D/"+sp;}

  const SbString& modeling = aGraphicStyle.modeling.getValue();
  SbPaintingPolicy painting = (SbPaintingPolicy)aGraphicStyle.painting.getValue();

  if( (modeling==SbModeling_curve) || (modeling==SbModeling_filled_curve) ) {

    float zmin = aBoxZ.fPos;
    float dz = aBoxZ.fWidth;
    SbBool zlog = aBoxZ.fLog;

    float xmn = aFunction.getMinimumX();
    float xmx = aFunction.getMaximumX();
    if(xmx<=xmn) {
      xmn = f_xDataAxis.getMinimumValue();
      xmx = f_xDataAxis.getMaximumValue();
    }
    float ymn = aFunction.getMinimumY();
    float ymx = aFunction.getMaximumY();
    if(ymx<=ymn) {
      ymn = f_yDataAxis.getMinimumValue();
      ymx = f_yDataAxis.getMaximumValue();
    }

    inlib::clist_contour listContour;
    //int nFir = 32;
    int nFir = 128;
    listContour.set_first_grid(nFir,nFir); //Default : 32,32 
    //int nSec = 256;
    int nSec = 512;
    listContour.set_secondary_grid(nSec,nSec); //Default : 256,256.

    double limits[4];
    // User limits :
    limits[0] = xmn;
    limits[1] = xmx;    
    limits[2] = ymn;    
    limits[3] = ymx;    

    SbFunc sbFunc;
    sbFunc.fFunc2D = &aFunction;
    sbFunc.fProblem = FALSE;
    sbFunc.fBins2D = 0;
    sbFunc.fLimitsIn[0] = limits[0];
    sbFunc.fLimitsIn[1] = limits[1];
    sbFunc.fLimitsIn[2] = limits[2];
    sbFunc.fLimitsIn[3] = limits[3];

    // Extend the grid to have some borders in order to close contours :
    int n = nSec - 2 * 10;
    double dx = (limits[1]-limits[0]) /n;
    double dy = (limits[3]-limits[2]) /n;
    limits[0] = limits[0] - 10 * dx;
    limits[1] = limits[1] + 10 * dx;
    limits[2] = limits[2] - 10 * dy;
    limits[3] = limits[3] + 10 * dy;

    sbFunc.fLimits[0] = limits[0];
    sbFunc.fLimits[1] = limits[1];
    sbFunc.fLimits[2] = limits[2];
    sbFunc.fLimits[3] = limits[3];

    listContour.set_limits(limits);

    if(levels.getNum()) {
      int zn = levels.getNum();
      std::vector<double> zs(zn);
      for(int zi=0;zi<zn;zi++) zs[zi] = levels[zi];
      listContour.set_planes(zs);
    } else {
      int zn = numberOfLevels.getValue();
      if(zn<=0) zn = 1;
      std::vector<double> zs(zn+1);
      float zmax = zmin + dz;
      double zd = (zmax-zmin)/zn;
      for(int zi=0;zi<=zn;zi++) zs[zi] = zmin + zi * zd;
      listContour.set_planes(zs);
    }

    if(zlog==TRUE) {
      listContour.set_field_fcn(log_function_to_contour,(void*)&sbFunc);
    } else {
      listContour.set_field_fcn(function_to_contour,(void*)&sbFunc);
    }

    listContour.ccontour::generate();
    if(!listContour.compact_strips ()) {
      SoDebugError::postInfo("SoPlotter::updateFunction_XY(SbPlottableFunction2D)",
                             " clist_contour::compact_strips () : failure.");
    } else {

      if( (painting==SbPaintingByLevel) || (painting==SbPaintingByValue) ){
        f_function_cmaps[aIndex] = new SbByValueColormap(aGraphicStyle.colorMapping.getValue());
        SbBool zlog = aBoxZ.fLog;
        if(zlog==TRUE) f_function_cmaps[aIndex]->set_PAW_coloring();
      } else {
        f_function_cmaps[aIndex] = new SbConstColormap(aGraphicStyle.color.getValue());
      }

      if(modeling==SbModeling_filled_curve) {
        repContour_xy_filled(functionNode,
                             aGraphicStyle,
                             painting,*(f_function_cmaps[aIndex]),
                             listContour,aBoxX,aBoxY,aBoxZ,aZZ,SbString(sid.c_str()));
      } else {
        repContour_xy(functionNode,aGraphicStyle,
                      painting,*(f_function_cmaps[aIndex]),
                      listContour,aBoxX,aBoxY,aBoxZ,aZZ,SbString(sid.c_str()));
      }
    }

    if(sbFunc.fProblem==TRUE) {
      SoDebugError::postInfo("SoPlotter::updateFunction_XY(SbPlottableFunction2D)",
                             "problem when getting some function value.");
    }

  } else if(modeling==SbModeling_polygon) {

    int npoint = aFunction.getNumberOfPoints();
    if(npoint) {

    std::vector<SbVec3f> points(npoint);
    std::vector<int> controls;
    for(int count=0;count<npoint;count++) {
      float xx,yy;
      bool isControl;
      aFunction.getIthPoint(count,xx,yy,isControl);
      points[count] = SbVec3f(xx,yy,aZZ);
      if(isControl) controls.push_back(count);
    }
    //check closure :
    if((points.size()>=2) && (points[points.size()-1]!=points[0]) ) {
      points.push_back(points[0]);
    }

    project2D(points,aBoxX,aBoxY);

    SbBool editable = aGraphicStyle.editable.getValue();
    if(!editable) {
      SbClip clipper;
     {unsigned int ptn = points.size();
      for(unsigned int index=0;index<ptn;index++) {
        clipper.addVertex(points[index]);
      }}
    
      SbPlane plane_xy_bot(SbVec3f(0, 1,0),SbVec3f(0,0,0));
      SbPlane plane_xy_top(SbVec3f(0,-1,0),SbVec3f(0,1,0));
      SbPlane plane_yz_left (SbVec3f( 1, 0,0),SbVec3f(0,0,0));
      SbPlane plane_yz_right(SbVec3f(-1, 0,0),SbVec3f(1,0,0));

      clipper.clip(plane_xy_bot);
      clipper.clip(plane_xy_top);
      clipper.clip(plane_yz_left);
      clipper.clip(plane_yz_right);

     {int n = clipper.getNumVertices();
      points.resize(n);
      for(int index=0;index<n;index++) {
        clipper.getVertex(index,points[index]);
      }}
      if((points.size()>=2) && (points[points.size()-1]!=points[0]) ) {
        points.push_back(points[0]);
      }
    }

    int ptn = points.size();
    if(ptn) {
    SbVec3f* pts = &(points[0]);

    SoSceneGraph* separator = new SoSceneGraph();
    separator->setString(sid.c_str());
    functionNode->addChild(separator);

    SoSeparator* sep = new SoSeparator;
    separator->addChild(sep);
   
    sep->addChild(fStyleCache->getFilled());
    sep->addChild(fStyleCache->getNormalBindingOverall());
    sep->addChild(fStyleCache->getNormalZ());
    sep->addChild(fStyleCache->getMaterial(aGraphicStyle.color.getValue(),aGraphicStyle.transparency.getValue()));

    SoCoordinate3* coordinate3 = new SoCoordinate3;
    coordinate3->point.setValues(0,ptn,pts);
    sep->addChild(coordinate3);

    SoFaceSet* faceSet = new SoFaceSet;
    faceSet->numVertices.set1Value(0,ptn);
    sep->addChild(faceSet);

    sep->addChild(fStyleCache->getLineStyle(SbLinePattern_solid,1));
    sep->addChild(fStyleCache->getMaterial(SbColor_black,0));

    //NOTE : we could simply add faceSet again !
    SoLineSet* lineSet = new SoLineSet;
    lineSet->numVertices.set1Value(0,ptn);
    sep->addChild(lineSet);

    if(editable) {
      SoSeparator* sep = new SoSeparator;
      separator->addChild(sep);

      float scale = 0.05F;

      for(int index=0;index<controls.size();index++) {
        int icontrol = controls[index];
        const SbVec3f& pt = pts[icontrol];

        SoSeparator* sp = new SoSeparator;
        sep->addChild(sp);

        SoTransform* tsf = new SoTransform();
        tsf->translation.setValue(pt);
        tsf->scaleFactor.setValue(scale,scale,scale);
        sp->addChild(tsf);

        SoPlotter_dragger* dragger =
          new SoPlotter_dragger(*coordinate3,icontrol,scale,*this,const_cast<SbPlottableFunction2D&>(aFunction),aBoxX,aBoxY,aZZ);
        SoTools_setDraggerColor(*dragger,SbColor_red);
        sp->addChild(dragger);
      }
    }

    } //ptn
    } //npoint

  } else {
    SoDebugError::postInfo("SoPlotter::updateFunction_XY",
      "modeling \"%s\" does not apply on Function2D in XY. Valid modelings ared curve, filled_curve and polygon.",
      modeling.getString());
  }


  } //end graphicStyle visible
}

void SoPlotter::updatePoints2D_XY(
 const SbPlottablePoints2D& aPoints
,SoStyle& aGraphicStyle
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZZ
){
  SoSeparator* pointsNode = (SoSeparator*)pointsSeparator.getValue();

  if(aGraphicStyle.visible.getValue()==TRUE) {

  std::string sid;
 {std::string sp;
  if(!inlib::p2sx(const_cast<SbPlottablePoints2D*>(&aPoints),sp)){}
  sid = "SbObject/"+sp;}

  int npoint = aPoints.getNumberOfPoints();

  std::vector<SbVec3f> points(npoint);
  for(int count=0;count<npoint;count++) {
    float xx,yy;
    aPoints.getIthPoint(count,xx,yy);
    points[count] = SbVec3f(xx,yy,aZZ);
  }

  SbBool oneNode = FALSE;
 {int nlimit = aGraphicStyle.multiNodeLimit.getValue();
  if(nlimit!=NoLimit) {
    oneNode = (npoint>nlimit?TRUE:FALSE);
  }}
  oneNode = TRUE; //FIXME : enforce one node (non-pickable points) ?

  if(oneNode) {
    int ptn;
    SbVec3f* pts = clipPoints2D(points,aBoxX,aBoxY,0,ptn);
    if(ptn) {
      SoSceneGraph* separator = new SoSceneGraph();
      separator->setString(sid.c_str());
      pointsNode->addChild(separator);

      const SbString& modeling = aGraphicStyle.modeling.getValue();

      if(modeling==SbModeling_lines){
        separator->addChild(getLineStyle(aGraphicStyle));
        separator->addChild(fStyleCache->getMaterial(aGraphicStyle.color.getValue(),aGraphicStyle.transparency.getValue()));
        SoTools_addLinesToNode(separator,ptn,pts);

      } else if(modeling==SbModeling_segments) {
        separator->addChild(getLineStyle(aGraphicStyle));
        separator->addChild(fStyleCache->getMaterial(aGraphicStyle.color.getValue(),aGraphicStyle.transparency.getValue()));
        SoTools_addSegmentsToNode(separator,ptn,pts);

      } else if(modeling==SbModeling_curve){
        separator->addChild(getLineStyle(aGraphicStyle));
        separator->addChild(fStyleCache->getMaterial(aGraphicStyle.color.getValue(),aGraphicStyle.transparency.getValue()));
        //SoTools::addLagrangePolynomialCurveToNode(separator,ptn,pts);
        //SoTools_addLinesToNode(separator,ptn,pts);
        SoTools_addNurbsCurveToNode(separator,ptn,pts);
        //SoTools::addSplineCurveToNode(separator,ptn,pts);

      } else { //Default
        separator->addChild(fStyleCache->getPoints());
        separator->addChild(fStyleCache->getMaterial(aGraphicStyle.color.getValue(),aGraphicStyle.transparency.getValue()));
        SoTools_addPointsToNode(separator,ptn,pts,
                                (SbMarkerStyle)aGraphicStyle.markerStyle.getValue(),aGraphicStyle.markerSize.getValue());
      }
      delete [] pts;
    }
  } else {
    repPoints2D_xy(pointsNode,aGraphicStyle,points,aBoxX,aBoxY,0,SbString(sid.c_str()));
  }

  } //end graphicStyle visible
}

void SoPlotter::updateBins2D_XYZ(const SbPlottableBins2D& aBins,int aIndex,SoStyle& aGraphicStyle,
                                 const SbRepBox& aBoxX,const SbRepBox& aBoxY,const SbRepBox& aBoxZ) {
  SoSeparator* binsNode = (SoSeparator*)binsSeparator.getValue();

  if(aGraphicStyle.visible.getValue()==TRUE) {

  int xnbin = aBins.getAxisNumberOfBinsX();
  int ynbin = aBins.getAxisNumberOfBinsY();

  std::string sid;
 {std::string sp;
  if(!inlib::p2sx(const_cast<SbPlottableBins2D*>(&aBins),sp)){}
  sid = "SbObject/"+sp;}

  const SbString& modeling = aGraphicStyle.modeling.getValue();
  if(modeling==SbModeling_boxes){

    bool hbe = aBins.hasEntriesPerBin();

    std::vector<SbRepBin2D> bins;
   {for(int jbin=ynbin-1;jbin>=0;jbin--) {
      for(int ibin=xnbin-1;ibin>=0;ibin--) { 
        if(hbe && (aBins.getBinNumberOfEntries(ibin,jbin)<=0)) continue;
  
        float val = aBins.getBinSumOfWeights(ibin,jbin);
  
        float xx = aBins.getBinLowerEdgeX(ibin);
        float xe = aBins.getBinUpperEdgeX(ibin);
        float yy = aBins.getBinLowerEdgeY(jbin);
        float ye = aBins.getBinUpperEdgeY(jbin);
                
        bins.push_back(SbRepBin2D(xx,xe,yy,ye,val,ibin,jbin));
      }
    }}
    int number = bins.size();
            
    float bmin;
    float bmax;
    aBins.getBinsSumOfWeightsRange(bmin,bmax);

    SbPaintingPolicy painting = (SbPaintingPolicy)aGraphicStyle.painting.getValue();
    if((painting==SbPaintingByLevel) || (painting==SbPaintingByValue) ){
      f_bins_cmaps[aIndex] = new SbByValueColormap(aGraphicStyle.colorMapping.getValue());
    } else if( (painting==SbPaintingGreyScale) ||
               (painting==SbPaintingGreyScaleInverse) ||
               (painting==SbPaintingVioletToRed) ){
      //WARNING : if multi-thread filling, bmin/bmax may be not in sync !
      float dbins = bmax-bmin;
      for(int index=0;index<number;index++) {
        bins[index].fRatio = bins[index].fVal/dbins;
      }
      if(painting==SbPaintingGreyScale) {
        f_bins_cmaps[aIndex] = new SbGreyScaleColormap(bmin,bmax,50);
      } else if(painting==SbPaintingGreyScaleInverse) {
        f_bins_cmaps[aIndex] = new SbGreyScaleInverseColormap(bmin,bmax,50);
      } else if(painting==SbPaintingVioletToRed) {
        f_bins_cmaps[aIndex] = new SbVioletToRedColormap(bmin,bmax,50);
      }
    } else {
      f_bins_cmaps[aIndex] = new SbConstColormap(aGraphicStyle.color.getValue());
    }
          
    std::string sid_bin;
   {std::string sp;
    if(!inlib::p2sx(&aBins,sp)){}
    sid_bin = "SbBin2D/"+sp;}
  
    // one node decision :
    SbBool oneNode = FALSE;
   {int nlimit = aGraphicStyle.multiNodeLimit.getValue();
    if(nlimit!=NoLimit) {
      oneNode = (number>nlimit?TRUE:FALSE);
    }}

    if(oneNode) {
      repBins2D_xyz_boxes_one(binsNode,aGraphicStyle,bins,aBoxX,aBoxY,aBoxZ,bmin,bmax,SbString(sid.c_str()));
    } else {
      repBins2D_xyz_boxes(binsNode,aGraphicStyle,painting,*(f_bins_cmaps[aIndex]),
			  bins,aBoxX,aBoxY,aBoxZ,bmin,bmax,SbString(sid_bin.c_str()));
    }

  } else if(modeling==SbModeling_curve){ //OpenPAW

    std::vector<SbRepTopFace2D> faces((xnbin-1)*(ynbin-1));
    int facei = 0;
    for(int jbin=0;jbin<(ynbin-1);jbin++) { 
      for(int ibin=0;ibin<(xnbin-1);ibin++) { 
    
        float xx_0 = aBins.getBinLowerEdgeX(ibin);
        float xe_0 = aBins.getBinUpperEdgeX(ibin);
        float xx_1 = aBins.getBinLowerEdgeX(ibin+1);
        float xe_1 = aBins.getBinUpperEdgeX(ibin+1);

        float yy_0 = aBins.getBinLowerEdgeY(jbin);
        float ye_0 = aBins.getBinUpperEdgeY(jbin);
        float yy_1 = aBins.getBinLowerEdgeY(jbin+1);
        float ye_1 = aBins.getBinUpperEdgeY(jbin+1);

        float xx = (xx_0+xe_0)/2;
        float xe = (xx_1+xe_1)/2;
        float yy = (yy_0+ye_0)/2;
        float ye = (yy_1+ye_1)/2;
 
        float val1 = aBins.getBinSumOfWeights(ibin,jbin);
        float val2 = aBins.getBinSumOfWeights(ibin+1,jbin);
        float val3 = aBins.getBinSumOfWeights(ibin+1,jbin+1);
        float val4 = aBins.getBinSumOfWeights(ibin,jbin+1);
              
        faces[facei] = SbRepTopFace2D(xx,xe,yy,ye,val1,val2,val3,val4); 
        facei++;
      }
    }

    SbPaintingPolicy painting = (SbPaintingPolicy)aGraphicStyle.painting.getValue();
  
    if((painting==SbPaintingByLevel) || (painting==SbPaintingByValue) ){
      f_bins_cmaps[aIndex] = new SbByValueColormap(aGraphicStyle.colorMapping.getValue());
    } else {
      f_bins_cmaps[aIndex] = 
        new SbConstColormap(aGraphicStyle.color.getValue());
    }
          

    if(painting==SbPaintingByLevel) { //OpenPAW.
      float bmin;
      float bmax;
      aBins.getBinsSumOfWeightsRange(bmin,bmax);
      repTopFaces2D_xyz_by_level(binsNode,aGraphicStyle,
                                 painting,*(f_bins_cmaps[aIndex]),
                                 faces,aBoxX,aBoxY,aBoxZ,
                                 bmin,bmax,SbString(sid.c_str()));

      if(aGraphicStyle.areaStyle.getValue()==SoStyle::EDGED) {
	//::printf("debug : EDGED 1\n");
        SoStyle* gs = new SoStyle;
        gs->lightModel = SbLightModel_base_color;
        gs->lineWidth = 2;
        repTopFaces2D_xyz_line(binsNode,*gs,
                               faces,aBoxX,aBoxY,aBoxZ,
                               SbString(sid.c_str()));
        gs->unref();
      }

    } else {

      SbBool oneNode = FALSE;
     {int nlimit = aGraphicStyle.multiNodeLimit.getValue();
      if(nlimit!=NoLimit) {
        oneNode = ((int)faces.size()>nlimit?TRUE:FALSE);
      }}
  
      if(oneNode) {       
        repTopFaces2D_xyz_one(binsNode,aGraphicStyle,faces,aBoxX,aBoxY,aBoxZ,SbString(sid.c_str()));
      } else {
        SbBool zlog = aBoxZ.fLog;
        if(zlog==TRUE) f_bins_cmaps[aIndex]->set_PAW_coloring();
        repTopFaces2D_xyz(binsNode,aGraphicStyle,
                          painting,*(f_bins_cmaps[aIndex]),
	  		  faces,aBoxX,aBoxY,aBoxZ,SbString(sid.c_str()));

        if(painting==SbPaintingUniform) { 
          //OpenPAW SURF3 (s_SURF_CONT).

        SbByValueColormap* cmap = new SbByValueColormap(aGraphicStyle.colorMapping.getValue());
        if(cmap->colorn()<=0) {
          delete cmap;
        } else {
          //updateBins2D_XY(aBins,aIndex,*gs,aBoxX,aBoxY,boxZ,1.1F);

          float zmin = aBoxZ.fPos;
          float dz = aBoxZ.fWidth;
          SbBool zlog = aBoxZ.fLog;

          float xmn = f_xDataAxis.getMinimumValue();
          float xmx = f_xDataAxis.getMaximumValue();
          float ymn = f_yDataAxis.getMinimumValue();
          float ymx = f_yDataAxis.getMaximumValue();

	  inlib::clist_contour listContour;
          //int nFir = 32;
          int nFir = 128;
          listContour.set_first_grid(nFir,nFir); //Default : 32,32 
          //int nSec = 256;
          int nSec = 512;
          listContour.set_secondary_grid(nSec,nSec); //Default : 256,256.

          double limits[4];
          // User limits :
          limits[0] = xmn;
          limits[1] = xmx;    
          limits[2] = ymn;    
          limits[3] = ymx;    

          SbFunc sbFunc;
          sbFunc.fFunc2D = 0;
          sbFunc.fProblem = FALSE;
          sbFunc.fBins2D = &aBins;
          sbFunc.fLimitsIn[0] = limits[0];
          sbFunc.fLimitsIn[1] = limits[1];
          sbFunc.fLimitsIn[2] = limits[2];
          sbFunc.fLimitsIn[3] = limits[3];

          // Extend the grid to have some borders in order to close contours :
          int n = nSec - 2 * 10;
          double dx = (limits[1]-limits[0]) /n;
          double dy = (limits[3]-limits[2]) /n;
          limits[0] = limits[0] - 10 * dx;
          limits[1] = limits[1] + 10 * dx;
          limits[2] = limits[2] - 10 * dy;
          limits[3] = limits[3] + 10 * dy;

          sbFunc.fLimits[0] = limits[0];
          sbFunc.fLimits[1] = limits[1];
          sbFunc.fLimits[2] = limits[2];
          sbFunc.fLimits[3] = limits[3];

          listContour.set_limits(limits);

          //int zn = numberOfLevels.getValue();
          int zn = cmap->colorn();
          if(zn<=0) zn = 1;

          std::vector<double> zs(zn+1);
          float zmax = zmin + dz;
          double zd = (zmax-zmin)/zn;
          for(int zi=0;zi<=zn;zi++) zs[zi] = zmin + zi * zd;
          listContour.set_planes(zs);

          if(zlog==TRUE)
            listContour.set_field_fcn(log_bins2D_to_contour,(void*)&sbFunc);
          else
            listContour.set_field_fcn(bins2D_to_contour,(void*)&sbFunc);

          listContour.ccontour::generate();
          if(!listContour.compact_strips ()) {
            SoDebugError::postInfo("SoPlotter::updateBins2D_XYZ"," clist_contour::compact_strips () : failure.");

            delete cmap;
          } else {
            delete f_bins_cmaps[aIndex];
            f_bins_cmaps[aIndex] = cmap; //to visualize the cmap.

  
            SoTransform* transf = new SoTransform;
            transf->scaleFactor.setValue(SbVec3f(1,1,0.01F));
            //contour back is at ZZ - 0.01F;
            transf->translation.setValue(SbVec3f(0,0,1.02F));
            binsNode->addChild(transf);

            SoStyle* gs = new SoStyle;
            gs->lightModel = SbLightModel_base_color;
            gs->transparency = 0.3F;

            float ZZ = 0.0F;

            repContour_xy_filled(binsNode,
                                 *gs,
                                 SbPaintingByLevel,*cmap,
                                 listContour,
                                 aBoxX,aBoxY,aBoxZ,ZZ,SbString(sid.c_str()));
            gs->unref();
          }

	} //Uniform

        }
      }

    }
  }

  } //end graphicStyle visible
}

void SoPlotter::updatePoints3D_XYZ(
 const SbPlottablePoints3D& aPoints
,SoStyle& aGraphicStyle
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
){
  SoSeparator* pointsNode = (SoSeparator*)pointsSeparator.getValue();

  if(aGraphicStyle.visible.getValue()==TRUE) {

  std::string sid;
 {std::string sp;
  if(!inlib::p2sx(const_cast<SbPlottablePoints3D*>(&aPoints),sp)){}
  sid = "SbObject/"+sp;}

  int npoint = aPoints.getNumberOfPoints ();

  SbBool oneNode = FALSE;
 {int nlimit = aGraphicStyle.multiNodeLimit.getValue();
  if(nlimit!=NoLimit) {
    oneNode = (npoint>nlimit?TRUE:FALSE);
  }}
  oneNode = TRUE; //FIXME : enforce one node ?

  std::vector<SbVec3f> points(npoint);
  for(int count=0;count<npoint;count++) {
    float x,y,z;
    aPoints.getIthPoint (count,x,y,z);
    points[count] = SbVec3f(x,y,z);
  }
  if(oneNode) {
    int ptn;
    SbVec3f* pts = clipPoints3D(points,aBoxX,aBoxY,aBoxZ,ptn);
    if(ptn) {
      SoSceneGraph* separator = new SoSceneGraph();
      separator->setString(sid.c_str());
      pointsNode->addChild(separator);

      const SbString& modeling = aGraphicStyle.modeling.getValue();
      if(modeling==SbModeling_lines){
        separator->addChild(getLineStyle(aGraphicStyle));
        separator->addChild(fStyleCache->getMaterial(aGraphicStyle.color.getValue(),aGraphicStyle.transparency.getValue()));
        SoTools_addLinesToNode(separator,ptn,pts);
      } else if(modeling==SbModeling_segments) {
        separator->addChild(getLineStyle(aGraphicStyle));
        separator->addChild(fStyleCache->getMaterial(aGraphicStyle.color.getValue(),aGraphicStyle.transparency.getValue()));
        SoTools_addSegmentsToNode(separator,ptn,pts);
      } else { //Default
        separator->addChild(fStyleCache->getPoints());
        separator->addChild(fStyleCache->getMaterial(aGraphicStyle.color.getValue(),aGraphicStyle.transparency.getValue()));
        SoTools_addPointsToNode(separator,ptn,pts,
          (SbMarkerStyle)aGraphicStyle.markerStyle.getValue(),aGraphicStyle.markerSize.getValue());
      }
      delete [] pts;
    }
  } else {
    repPoints3D_xyz(pointsNode,aGraphicStyle,points,aBoxX,aBoxY,aBoxZ,SbString(sid.c_str()));
  }

  } //end graphicStyle visible
}

void SoPlotter::updateFunction_XYZ(
 const SbPlottableFunction2D& aFunction
,int aIndex
,SoStyle& aGraphicStyle
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
){
  SoSeparator* functionNode = (SoSeparator*)functionSeparator.getValue();

  if(aGraphicStyle.visible.getValue()==TRUE) {

  float xmn = aFunction.getMinimumX();
  float xmx = aFunction.getMaximumX();
  if(xmx<=xmn) {
    xmn = f_xDataAxis.getMinimumValue();
    xmx = f_xDataAxis.getMaximumValue();
  }
  int nx = aFunction.getNumberOfStepsX();
  nx = nx <=0 ? 20 : nx;

  float ymn = aFunction.getMinimumY();
  float ymx = aFunction.getMaximumY();
  if(ymx<=ymn) {
    ymn = f_yDataAxis.getMinimumValue();
    ymx = f_yDataAxis.getMaximumValue();
  }
  int ny = aFunction.getNumberOfStepsY();
  ny = ny <=0 ? 20 : ny;

  float dfx = (xmx - xmn)/nx;
  float dfy = (ymx - ymn)/ny;

  //printf("debug : nx %d ny %d\n",nx,ny);
  
  SbBool problem = FALSE;

  int facei = 0;
  std::vector<SbRepTopFace2D> faces(nx*ny);
  float* vprev = new float[2*nx];
  for(int jbin=ny-1;jbin>=0;jbin--) { 
    float o1 = 0;
    float o4 = 0;
    for(int ibin=nx-1;ibin>=0;ibin--) { 
  
      float xx = xmn + ibin * dfx;
      float yy = ymn + jbin * dfy;
      float xe = xx + dfx;
      float ye = yy + dfy;
       
      // Values at the corner :    
      float val1;
      if(!aFunction.value(xx,yy,val1)) problem = TRUE;
      float val2;
      if(ibin==nx-1) {
        if(!aFunction.value(xe,yy,val2)) problem = TRUE;
      } else {
        val2 = o1;
      }
      float val3,val4;
      if(jbin==ny-1) {
        if(ibin==nx-1) {
          if(!aFunction.value(xe,ye,val3)) problem = TRUE;
        } else {
          val3 = o4;
        }
        if(!aFunction.value(xx,ye,val4)) problem = TRUE;
      } else {
        val3 = vprev[2*ibin+1];
        val4 = vprev[2*ibin];
      }
      //printf("debug : %d %d : %g %g %g %g\n",
      //   ibin,jbin,
      //   val1,val2,val3,val4);
      vprev[2*ibin] = val1;
      vprev[2*ibin+1] = val2;
      o4 = val4;
      o1 = val1;
            
      faces[facei] = SbRepTopFace2D(xx,xe,yy,ye,val1,val2,val3,val4); 
      facei++;
    }
  }
  delete [] vprev;

  if(problem==TRUE) {
    SoDebugError::postInfo("SoPlotter::updateFunction_XYZ","problem when getting some function value.");
  }

  SbPaintingPolicy painting = 
    (SbPaintingPolicy)aGraphicStyle.painting.getValue();

  if(painting==SbPaintingByValue) {
    f_function_cmaps[aIndex] = 
      new SbByValueColormap(aGraphicStyle.colorMapping.getValue());
  } else {
    f_function_cmaps[aIndex] = 
      new SbConstColormap(aGraphicStyle.color.getValue());
  }

  SbBool oneNode = FALSE;
 {int nlimit = aGraphicStyle.multiNodeLimit.getValue();
  if(nlimit!=NoLimit) {
    oneNode = ((int)faces.size()>nlimit?TRUE:FALSE);
  }}

  std::string sid;
 {std::string sp;
  if(!inlib::p2sx(&aFunction,sp)){}
  sid = "SbFunction2D/"+sp;}

  if(oneNode) {       
    repTopFaces2D_xyz_one(functionNode,aGraphicStyle,
			  faces,aBoxX,aBoxY,aBoxZ,SbString(sid.c_str()));
  } else {
    SbBool zlog = aBoxZ.fLog;
    if(zlog==TRUE) f_function_cmaps[aIndex]->set_PAW_coloring();
    repTopFaces2D_xyz(functionNode,aGraphicStyle,
                      painting,*(f_function_cmaps[aIndex]),
                      faces,aBoxX,aBoxY,aBoxZ,SbString(sid.c_str()));
  }

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float zmin = aBoxZ.fPos;
  float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

 // border X (Y=0)
 {SoSeparator* separator = new SoSeparator();
  functionNode->addChild(separator);
  separator->addChild(fStyleCache->getFilled());
  separator->addChild(fStyleCache->getNormalBindingOverall());
 {SoNormal* normal = new SoNormal;
  SbVec3f n(0,-1,0);
  normal->vector.set1Value(0,n);
  separator->addChild(normal);}
  separator->addChild(fStyleCache->getMaterial(SbColor(0.6F,0.2F,0.2F),aGraphicStyle.transparency.getValue())); //brown
  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;
  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
  separator->addChild(indexedFaceSet);
  int iindex = 0;
  SbVec3f points[4];
  int32_t coordIndex[5];

  SbBool empty = TRUE;
  SbBool problem = FALSE;

  float vprev;
 {if(!aFunction.value(xmx,ymn,vprev)) problem = TRUE;
  vprev = VerifyLog(vprev,zmin,dz,zlog);
  if(vprev<0) vprev = 0;
  if(vprev>1) vprev = 1;}

  for(int ibin=nx-1;ibin>=0;ibin--) { 

    float xx = xmn + ibin * dfx;
    float xe = xx + dfx;
    float yy = ymn;
       
    float val;
    if(!aFunction.value(xx,yy,val)) problem = TRUE;        
    val = VerifyLog(val,zmin,dz,zlog);
    if(val<0) val = 0;
    if(val>1) val = 1;

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
            
    if((xx>=0)&&(xx<=1)   &&
       (xe>=0)&&(xe<=1)   &&
       (yy>=0)&&(yy<=1) ) {

      LIST_SET(points,0,xe,yy,0);
      LIST_SET(points,1,xe,yy,vprev);
      LIST_SET(points,2,xx,yy,val);
      LIST_SET(points,3,xx,yy,0);

      coordIndex[0] = icoord + 0;
      coordIndex[1] = icoord + 1;
      coordIndex[2] = icoord + 2;
      coordIndex[3] = icoord + 3;
      coordIndex[4] = SO_END_FACE_INDEX;

      coordinate3->point.setValues(icoord,4,points);
      icoord += 4;  

      indexedFaceSet->coordIndex.setValues(iindex,5,coordIndex);
      iindex += 5;

      empty = FALSE;
    }

    vprev = val;
  }

  if(problem==TRUE) {
    SoDebugError::postInfo("SoPlotter::updateFunction_XYZ","problem when getting some function value.");
  }}

  // border Y (X=0) 
 {SoSeparator* separator = new SoSeparator();
  functionNode->addChild(separator);
  separator->addChild(fStyleCache->getFilled());
  separator->addChild(fStyleCache->getNormalBindingOverall());
 {SoNormal* normal = new SoNormal;
  SbVec3f n(-1,0,0);
  normal->vector.set1Value(0,n);
  separator->addChild(normal);}
  separator->addChild
    (fStyleCache->getMaterial(SbColor(0.6F,0.2F,0.2F), //brown
                              aGraphicStyle.transparency.getValue()));
  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;
  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
  separator->addChild(indexedFaceSet);
  int iindex = 0;
  SbVec3f points[4];
  int32_t coordIndex[5];

  SbBool empty = TRUE;
  SbBool problem = FALSE;

  float vprev;
 {if(!aFunction.value(xmn,ymx,vprev)) problem = TRUE;
  vprev = VerifyLog(vprev,zmin,dz,zlog);
  if(vprev<0) vprev = 0;
  if(vprev>1) vprev = 1;}

  for(int jbin=ny-1;jbin>=0;jbin--) { 

    float xx = xmn;
    float yy = ymn + jbin * dfy;
    float ye = yy + dfy;
     
    float val;
    if(!aFunction.value(xx,yy,val)) problem = TRUE;            
    val = VerifyLog(val,zmin,dz,zlog);
    if(val<0) val = 0;
    if(val>1) val = 1;

    xx = VerifyLog(xx,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);
            
    if((xx>=0)&&(xx<=1)   &&
       (yy>=0)&&(yy<=1)   &&
       (ye>=0)&&(ye<=1)  ) {

      LIST_SET(points,0,xx,ye,0);
      LIST_SET(points,1,xx,ye,vprev);
      LIST_SET(points,2,xx,yy,val);
      LIST_SET(points,3,xx,yy,0);

      coordIndex[0] = icoord + 0;
      coordIndex[1] = icoord + 1;
      coordIndex[2] = icoord + 2;
      coordIndex[3] = icoord + 3;
      coordIndex[4] = SO_END_FACE_INDEX;

      coordinate3->point.setValues(icoord,4,points);
      icoord += 4;  

      indexedFaceSet->coordIndex.setValues(iindex,5,coordIndex);
      iindex += 5;

      empty = FALSE;

    }

    vprev = val;
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    functionNode->addChild(separator);
  }

  if(problem==TRUE) {
    SoDebugError::postInfo("SoPlotter::updateFunction_XYZ","problem when getting some function value.");
  }}

  } //end graphicStyle visible
}

void SoPlotter::updateTitle(){
  if(fShape==XYZ) return; //FIXME

  SoTextStyle* graphicStyle = (SoTextStyle*)titleStyle.getValue();
  if(graphicStyle->visible.getValue()==FALSE) return;

  SoSeparator* titleNode = (SoSeparator*)titleSeparator.getValue();

  // Use the XY layout transform to position the title.
  // (Else we would have to compensate the 3D rotation
  //  in order to bring the title at the right position 
  //  without rotation).
  //titleNode->addChild(layout.getValue());

  SoMaterial* material = 
    fStyleCache->getMaterial(graphicStyle->color.getValue(),
                             graphicStyle->transparency.getValue());
  titleNode->addChild(material);

  float text_size = titleHeight.getValue() * graphicStyle->scale.getValue();

  SbString fontName = graphicStyle->fontName.getValue();
  if( (fontName.getLength() >=6) &&
      (fontName.getSubString(0,5)=="Text3/") ) {
    titleNode->addChild(fStyleCache->getFilled());
  } else if((fontName.getLength() >=6 ) && 
            (fontName.getSubString(0,5)=="Text2/")) {
  } else if(SoTools_IsTTF(fontName)==TRUE) {
    text_size *= ttfScale.getValue();
  } else {  // Default is Hershey.
    titleNode->addChild(
      fStyleCache->getLineStyle(graphicStyle->linePattern.getValue(),(float)graphicStyle->lineWidth.getValue()));
  }

  float XSIZ = width.getValue();
  float XMGL = leftMargin.getValue();
  float XMGR = rightMargin.getValue();
  float wData = XSIZ-XMGL-XMGR;

  float YSIZ = height.getValue();
  float YMGL = bottomMargin.getValue();
  float YMGU = topMargin.getValue();
  float hData = YSIZ-YMGL-YMGU;

  float xx = wData/2;
  float yy = hData + titleToAxis.getValue();
  SoTools_HorizontalJustification hJust = CENTER;
  SoTools_VerticalJustification vJust = BOTTOM;
  if(titleUp.getValue()==FALSE) {
    yy = -titleToAxis.getValue();
    vJust = TOP;
  }
  SbVec3f vertex;
  vertex.setValue(xx,yy,0);

  //printf("debug : %lu : %g %g : text_size : %g : %s\n",
  //  this,xx,yy,text_size,title.getValue().getString());

  SbVec3f TX(1,0,0);
  SbVec3f TY(0,1,0);
  SoTools_addAlignedStringToNode(titleNode,
                                  fontName,
                                  graphicStyle->encoding.getValue(),
                                  graphicStyle->smoothing.getValue(),
                                  title.getValue(),vertex,TX,TY,
                                  text_size,
                                  hJust,vJust);

}

void SoPlotter::updateInnerFrame_XY(){
  //printf("debug : updateInnerFrame_XY\n");

  SoStyle* graphicStyle = getInnerFrameStyle();
  if(graphicStyle->visible.getValue()==FALSE) return;

  SoSeparator* innerFrameNode = (SoSeparator*)innerFrameSeparator.getValue();

  SbVec3f points[5];
  float z = 0.5F;

  LIST_SET(points,0,0,0,z);
  LIST_SET(points,1,1,0,z);
  LIST_SET(points,2,1,1,z);
  LIST_SET(points,3,0,1,z);
  LIST_SET(points,4,0,0,z);
  innerFrameNode->addChild(getLineStyle(*graphicStyle));
  innerFrameNode->addChild(fStyleCache->getMaterial(graphicStyle->color.getValue(),graphicStyle->transparency.getValue()));

  SoTools_addLinesToNode(innerFrameNode,5,points);
}

void SoPlotter::updateInnerFrame_XYZ(){
  SoStyle* graphicStyle = getInnerFrameStyle();
  if(graphicStyle->visible.getValue()==FALSE) return;

  SoSeparator* innerFrameNode = (SoSeparator*)innerFrameSeparator.getValue();

  innerFrameNode->addChild(getLineStyle(*graphicStyle));
  innerFrameNode->addChild(fStyleCache->getMaterial(graphicStyle->color.getValue(),graphicStyle->transparency.getValue()));
  SbVec3f points[5];
  LIST_SET(points,0,0,0,0);
  LIST_SET(points,1,1,0,0);
  LIST_SET(points,2,1,1,0);
  LIST_SET(points,3,0,1,0);
  LIST_SET(points,4,0,0,0);
  SoTools_addLinesToNode(innerFrameNode,5,points);
  LIST_SET(points,0,0,0,1);
  LIST_SET(points,1,1,0,1);
  LIST_SET(points,2,1,1,1);
  LIST_SET(points,3,0,1,1);
  LIST_SET(points,4,0,0,1);
  SoTools_addLinesToNode(innerFrameNode,5,points);
  LIST_SET(points,0,0,0,0);
  LIST_SET(points,1,0,0,1);
  SoTools_addLinesToNode(innerFrameNode,2,points);
  LIST_SET(points,0,1,0,0);
  LIST_SET(points,1,1,0,1);
  SoTools_addLinesToNode(innerFrameNode,2,points);
  LIST_SET(points,0,1,1,0);
  LIST_SET(points,1,1,1,1);
  SoTools_addLinesToNode(innerFrameNode,2,points);
  LIST_SET(points,0,0,1,0);
  LIST_SET(points,1,0,1,1);
  SoTools_addLinesToNode(innerFrameNode,2,points);

}

void SoPlotter::updateColorMapping(const SbBaseColormap& aCmap){

  if(colorMappingVisible.getValue()==FALSE) return;

  unsigned int valn = aCmap.valn();
  if(!valn) return;
  unsigned int coln = aCmap.colorn();
  if(!coln) return;

 {SoTransform* transform = (SoTransform*)colorMappingTransform.getValue();
  if(fShape==XY) {
    SoTransform* from = (SoTransform*)layout.getValue();
    transform->translation = from->translation;
    transform->rotation = from->rotation;
    transform->scaleFactor = from->scaleFactor;
    transform->scaleOrientation = from->scaleOrientation;
    transform->center = from->center;

  } else {

    if(colorMappingAttached.getValue()==TRUE) {
  
      SoTransform* from = (SoTransform*)layout.getValue();
      transform->translation = from->translation;
      transform->rotation = from->rotation;
      transform->scaleFactor = from->scaleFactor;
      transform->scaleOrientation = from->scaleOrientation;
      transform->center = from->center;

     {SoRotationXYZ* rot = new SoRotationXYZ;
      rot->ref();
      rot->axis = SoRotationXYZ::X;
      rot->angle = 90.0F * FM_PI/180.0F;
      transform->combineLeft(rot);
      rot->unref();}


    } else {
     {SbMatrix matrix;
      matrix.makeIdentity();
      transform->setMatrix(matrix);}

      float XSIZ = width.getValue();
      float XMGL = leftMargin.getValue();
      //float XMGR = rightMargin.getValue();
      //float wData = XSIZ-XMGL-XMGR;

      float YSIZ = height.getValue();
      float YMGL = bottomMargin.getValue();
      //float YMGU = topMargin.getValue();
      //float hData = YSIZ-YMGL-YMGU;

      //OpenPAW
      SoTranslation* trans = new SoTranslation;
      trans->ref();
      trans->translation.setValue(-XSIZ/2+XMGL,-YSIZ/2+YMGL,-0.5F);
      transform->combineLeft(trans); //applied first
      trans->unref();
    }
  }
  } //end transform.

  float XSIZ = width.getValue();
  float XMGL = leftMargin.getValue();
  float XMGR = rightMargin.getValue();
  float wData = XSIZ-XMGL-XMGR;

  float w  = XMGR*0.4F;
  float xx = wData+XMGR*0.1F;

  float YSIZ = height.getValue();
  float YMGL = bottomMargin.getValue();
  float YMGU = topMargin.getValue();
  float hData = YSIZ-YMGL-YMGU;

  float yy = 0;
  float h = hData/float(coln);

 {// colored cells :
  SoSeparator* sep = (SoSeparator*)colorMappingCellsSeparator.getValue();
  sep->removeAllChildren();

  for(int index=0;index<coln;index++)        {
    SbVec3f points[4];
    LIST_SET(points,0,xx      ,yy     ,0);
    LIST_SET(points,1,xx + w  ,yy     ,0);
    LIST_SET(points,2,xx + w  ,yy + h ,0);
    LIST_SET(points,3,xx      ,yy + h ,0);

    sep->addChild(fStyleCache->getFilled());
    sep->addChild(fStyleCache->getNormalBindingOverall());
    sep->addChild(fStyleCache->getNormalZ());
    sep->addChild(fStyleCache->getMaterial(aCmap.color(index)));
    SoTools_addPolygonToNode(sep,4,points); 

    yy += h;
  }

  // surrounding box :
 {SbVec3f points[5];
  LIST_SET(points,0,xx      ,0     ,0);
  LIST_SET(points,1,xx + w  ,0     ,0);
  LIST_SET(points,2,xx + w  ,hData ,0);
  LIST_SET(points,3,xx      ,hData ,0);
  LIST_SET(points,4,xx      ,0     ,0);
  sep->addChild(fStyleCache->getLineStyle());
  sep->addChild(fStyleCache->getMaterial(SbColor_black));
  SoTools_addLinesToNode(sep,5,points);}

  } //end sep.

 {// right axis :
  SoTransform* transf = (SoTransform*)colorMappingAxisTransform.getValue();
  SbMatrix matrix;
  matrix.makeIdentity();
  transf->setMatrix(matrix);
  SbMatrix trans;
  if(Cast(aCmap,SbByValueColormap)) {
    if(colorMappingAxisLabeling.getValue()==CELLS) {
      if((valn+1)==coln) { // <col> <num> <col> ... <num> <col>
        trans.setTranslate(SbVec3f(xx+w,h,0));
      } else {
        trans.setTranslate(SbVec3f(xx+w,0,0));
      }
    } else {
      trans.setTranslate(SbVec3f(xx+w,0,0));
    }
  } else { //GREY_SCALE,GREY_SCALE_INVERSE,VIOLET_TO_RED
    trans.setTranslate(SbVec3f(xx+w,0,0));
  }
  transf->multLeft(trans);
  SbMatrix rotz;
  rotz.setRotate(SbRotation(SbVec3f(0,0,1),FM_PI_2));
  transf->multLeft(rotz); //Applied first.

  SoAxis* axis = (SoAxis*)colorMappingAxis.getValue();
  axis->title.setValue("");

  axis->tickUp.setValue(TRUE);
  axis->labelToAxis.setValue(0.01F);
  //axis->labelHeight.setValue(0.10F);
  //axis->ttfScale.setValue(10.0F);

  if(Cast(aCmap,SbByValueColormap)) {
    if(colorMappingAxisLabeling.getValue()==CELLS) {
      if((valn+1)==coln) { // <col> <num> <col> ... <num> <col>
        axis->width.setValue(hData-2*h);
        axis->modeling.setValue(SbTickModeling_none);
        axis->tickNumber.setValue(valn);
        for(unsigned int index=0;index<valn;index++)        {
          //FIXME : for the labels, have a "mag" logic similar to SoAxis.
          char tmp[32];
          ::sprintf(tmp,"%g",aCmap.value(index));
          axis->labels.set1Value(index,tmp);
          axis->coords.set1Value(index,h*index);
        }
      } else if((coln+1)==valn) { // <num> <col> <num> ... <col> <num>
        axis->width.setValue(hData);
        axis->modeling.setValue(SbTickModeling_none);
        axis->tickNumber.setValue(valn);
        for(unsigned int index=0;index<valn;index++)        {
          //FIXME : for the labels, have a "mag" logic similar to SoAxis.
          char tmp[32];
          ::sprintf(tmp,"%g",aCmap.value(index));
          axis->labels.set1Value(index,tmp);
          axis->coords.set1Value(index,h*index);
        }
      } else {
        SoDebugError::postInfo
          ("SoPlotter::updateColorMapping","inconsistent by value colormap.");
      }
    } else {
      axis->width.setValue(hData);
      axis->minimumValue.setValue(aCmap.value(0));
      axis->maximumValue.setValue(aCmap.value(valn-1));
    }
  } else { //GREY_SCALE,GREY_SCALE_INVERSE,VIOLET_TO_RED
    axis->width.setValue(hData);
    axis->minimumValue.setValue(aCmap.value(0));
    axis->maximumValue.setValue(aCmap.value(valn-1));
  }

 {SoTextStyle* style = axis->getLabelsStyle(); 
  style->orientationX.setValue(SbVec3f(0,-1,0));
  style->orientationY.setValue(SbVec3f(1,0,0));
  style->horizontalJustification.setValue(SoTextStyle::LEFT);
  style->verticalJustification.setValue(SoTextStyle::MIDDLE);}

 {SoTextStyle* style = axis->getMagStyle(); 
  style->horizontalJustification.setValue(SoTextStyle::CENTER);
  style->verticalJustification.setValue(SoTextStyle::BOTTOM);}

  }//end axis

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// reps xy ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoPlotter::repBins1D_xy_top_lines(
 SoSeparator* aParent
,SoStyle& aStyle
,SbPaintingPolicy aPainting
,const SbBaseColormap& aCmap
,std::vector<SbRepBin1D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZZ
,const SbString& aID
){
  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float y0 = 0;
  y0 = VerifyLog(y0,ymin,dy,ylog);
  if(y0<0) y0 = 0;
  if(y0>1) y0 = 1;

  SoSeparator* separator = new SoSeparator();

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SbBool empty = TRUE;
  SbVec3f points[5];
  int32_t coordIndex[6];
  float yp = 0;
  int xnbin = aBins.size();
  for(int index=0;index<xnbin;index++) { 
    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float y = aBins[index].fVal;

    float val = aBins[index].fVal;
    
    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    y = VerifyLog(y,ymin,dy,ylog);

    // Clipping :
    if(xe<0) continue;
    if(xx>1) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(y<0) y = 0;
    if(y>1) y = 1;

    SoSceneGraph* sep = new SoSceneGraph;
    separator->addChild(sep);

    aBins[index].fSeparator = sep;

   {char s[128];
    ::sprintf(s,"%d",index);
    sep->setInfos(s);}
   {std::string sp;
    if(!inlib::p2sx(sep->getInfos(),sp)){}
    std::string sid(aID.getString());
    sid += "/"+sp;
    sep->setString(sid.c_str());}

    SbColor sbColor;
    if(aPainting==SbPaintingByValue) {
      sbColor = aCmap.getColor(val);
    } else if( (aPainting==SbPaintingGreyScale) ||
               (aPainting==SbPaintingGreyScaleInverse) ||
               (aPainting==SbPaintingVioletToRed) ){
      sbColor = aCmap.getColor(aBins[index].fRatio);
    } else {
      sbColor = aStyle.color.getValue();
    }

    SoMaterial* material = 
      fStyleCache->getMaterial(sbColor,aStyle.transparency.getValue());
    sep->addChild(material);

    sep->addChild(getLineStyle(aStyle));

    int32_t pointn = 3;
    LIST_SET(points,0,xx,yp,aZZ);
    LIST_SET(points,1,xx,y,aZZ);
    LIST_SET(points,2,xe,y,aZZ);
    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = SO_END_LINE_INDEX;
    if(index==xnbin-1){
      pointn = 4;
      LIST_SET(points,3,xe,y0,aZZ);
      coordIndex[3] = icoord + 3;
      coordIndex[4] = SO_END_LINE_INDEX;
    }

    coordinate3->point.setValues(icoord,pointn,points);
    icoord += pointn;

    SoIndexedLineSet* lineSet = new SoIndexedLineSet;
    lineSet->coordIndex.setValues(0,pointn+1,coordIndex);
    sep->addChild(lineSet);

    empty = FALSE;

    yp = y;
  }

  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins1D_xy_top_lines_one(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepBin1D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZZ
,const SbString& aID
){
  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
  separator->addChild(material);

  separator->addChild(getLineStyle(aStyle));
          
  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float y0 = 0;
  y0 = VerifyLog(y0,ymin,dy,ylog);
  if(y0<0) y0 = 0;
  if(y0>1) y0 = 1;

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SoIndexedLineSet* lineSet = new SoIndexedLineSet;
  separator->addChild(lineSet);
  int iindex = 0;

  int32_t coordIndex[6];
  SbVec3f points[5];
  SbBool empty = TRUE;
  float yp = 0;
  int xnbin = aBins.size();
  for(int index=0;index<xnbin;index++) { 
    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float y = aBins[index].fVal;

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    y = VerifyLog(y,ymin,dy,ylog);

    // Clipping :
    if(xe<0) continue;
    if(xx>1) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(y<0) y = 0;
    if(y>1) y = 1;

    int pointn = 3;
    LIST_SET(points,0,xx,yp,aZZ);
    LIST_SET(points,1,xx,y,aZZ);
    LIST_SET(points,2,xe,y,aZZ);
    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = SO_END_LINE_INDEX;
    if(index==xnbin-1){
      pointn = 4;
      LIST_SET(points,3,xe,y0,aZZ);
      coordIndex[3] = icoord + 3;
      coordIndex[4] = SO_END_LINE_INDEX;
    }

    coordinate3->point.setValues(icoord,pointn,points);
    icoord += pointn;

    lineSet->coordIndex.setValues(iindex,pointn+1,coordIndex);
    iindex += (pointn+1);

    empty = FALSE;
    yp = y;
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins1D_xy_points(
 SoSeparator* aParent
,SoStyle& aStyle
,SbPaintingPolicy aPainting
,const SbBaseColormap& aCmap
,std::vector<SbRepBin1D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZZ
,const SbString& aID
){        
  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SoSeparator* separator = new SoSeparator();

  separator->addChild(fStyleCache->getPoints());

  //SoCoordinate3* coordinate3 = new SoCoordinate3;
  //separator->addChild(coordinate3);
  //int icoord = 0;

  SbBool empty = TRUE;
  SbVec3f points[1];
  //int32_t coordIndex[2];
  int xnbin = aBins.size();
  for(int index=0;index<xnbin;index++) { 
    float x = (aBins[index].fXmin + aBins[index].fXmax)/2;
    float y = aBins[index].fVal;
    float val = aBins[index].fVal;

    x = VerifyLog(x,xmin,dx,xlog);
    y = VerifyLog(y,ymin,dy,ylog);

    // Clipping :
    if(x<0) continue;
    if(x>1) continue;
    if(y<0) continue;
    if(y>1) continue;

    SoSceneGraph* sep = new SoSceneGraph;
    separator->addChild(sep);

    aBins[index].fSeparator = sep;

   {char s[128];
    ::sprintf(s,"%d",index);
    sep->setInfos(s);}
   {std::string sp;
    if(!inlib::p2sx(sep->getInfos(),sp)){}
    std::string sid(aID.getString());
    sid += "/"+sp;
    sep->setString(sid.c_str());}

    SbColor sbColor;
    if(aPainting==SbPaintingByValue) {
      sbColor = aCmap.getColor(val);
    } else if( (aPainting==SbPaintingGreyScale) ||
               (aPainting==SbPaintingGreyScaleInverse) ||
               (aPainting==SbPaintingVioletToRed)) {
      sbColor = aCmap.getColor(aBins[index].fRatio);
    } else {
      sbColor = aStyle.color.getValue();
    }

    SoMaterial* material = 
      fStyleCache->getMaterial(sbColor,aStyle.transparency.getValue());
    sep->addChild(material);

    LIST_SET(points,0,x,y,aZZ);

    SoTools_addPointsToNode(sep,1,points,
      (SbMarkerStyle)aStyle.markerStyle.getValue(),
      aStyle.markerSize.getValue());

    empty = FALSE;
  }

  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins1D_xy_points_one(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepBin1D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZZ
,const SbString& aID
){
  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  SoMaterial* material = 
    fStyleCache->getMaterial(aStyle.color.getValue(),
                             aStyle.transparency.getValue());
  separator->addChild(material);

  separator->addChild(fStyleCache->getPoints());
          
  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SbVec3f points[5];
  SbBool empty = TRUE;
  int xnbin = aBins.size();
  for(int index=0;index<xnbin;index++) { 
    float x = (aBins[index].fXmin + aBins[index].fXmax)/2;
    float y = aBins[index].fVal;

    x = VerifyLog(x,xmin,dx,xlog);
    y = VerifyLog(y,ymin,dy,ylog);

    // Clipping :
    if(x<0) continue;
    if(x>1) continue;
    if(y<0) continue;
    if(y>1) continue;

    LIST_SET(points,0,x,y,aZZ);

    SoTools_addPointsToNode(separator,1,points,
      (SbMarkerStyle)aStyle.markerStyle.getValue(),
      aStyle.markerSize.getValue());

    empty = FALSE;
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins1D_xy_boxes(
 SoSeparator* aParent
,SoStyle& aStyle
,SbPaintingPolicy aPainting
,const SbBaseColormap& aCmap
,std::vector<SbRepBin1D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZZ
,const SbString& aID
){
  //printf("debug : SoPlotter::repBins1D_xy_boxes : ID \"%s\" zz %g\n",
    //aID.getString(),aZZ);

  SoSeparator* separator = new SoSeparator();

  separator->addChild(fStyleCache->getFilled());
  separator->addChild(fStyleCache->getNormalBindingPerFace());
  separator->addChild(fStyleCache->getNormalZ());

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SbBool empty = TRUE;
  SbVec3f points[4];
  int32_t coordIndex[5];
  int xnbin = aBins.size();
  for(int index=0;index<xnbin;index++) { 
    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fVmin;
    float ye = aBins[index].fVal;
    if(ye<yy) {
      yy = aBins[index].fVal;
      ye = aBins[index].fVmin;
    }

    float val = aBins[index].fVal;

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);

    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    //FIXME if(ye<=yy) continue; //Else we shall have a tessellation error.

    SoSceneGraph* sep = new SoSceneGraph;
    separator->addChild(sep);

    aBins[index].fSeparator = sep;

   {char s[128];
    ::sprintf(s,"%d",index);
    sep->setInfos(s);}
   {std::string sp;
    if(!inlib::p2sx(sep->getInfos(),sp)){}
    std::string sid(aID.getString());
    sid += "/"+sp;
    sep->setString(sid.c_str());}

    SbColor sbColor;
    if(aPainting==SbPaintingByValue) {
      sbColor = aCmap.getColor(val);
    } else if( (aPainting==SbPaintingGreyScale) ||
               (aPainting==SbPaintingGreyScaleInverse) ||
               (aPainting==SbPaintingVioletToRed)) {
      sbColor = aCmap.getColor(aBins[index].fRatio);
    } else {
      sbColor = aStyle.color.getValue();
    }

    SoMaterial* material = 
      fStyleCache->getMaterial(sbColor,aStyle.transparency.getValue());
    sep->addChild(material);

    LIST_SET(points,0,xx,yy,aZZ);
    LIST_SET(points,1,xe,yy,aZZ);
    LIST_SET(points,2,xe,ye,aZZ);
    LIST_SET(points,3,xx,ye,aZZ);

    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = icoord + 3;
    coordIndex[4] = SO_END_FACE_INDEX;

    coordinate3->point.setValues(icoord,4,points);
    icoord += 4;
 
    SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
    indexedFaceSet->coordIndex.setValues(0,5,coordIndex);
    sep->addChild(indexedFaceSet);

    empty = FALSE;
  }

  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins1D_xy_boxes_one(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepBin1D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZZ
,const SbString& aID
){
  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
  separator->addChild(material);

  separator->addChild(fStyleCache->getFilled());
  separator->addChild(fStyleCache->getNormalBindingOverall());          
  separator->addChild(fStyleCache->getNormalZ());

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
  separator->addChild(indexedFaceSet);
  int iindex = 0;

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SbBool empty = TRUE;
  SbVec3f points[4];
  int32_t coordIndex[5];
  int xnbin = aBins.size();
  for(int index=0;index<xnbin;index++) { 
    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fVmin;
    float ye = aBins[index].fVal;
    if(ye<yy) {
      yy = aBins[index].fVal;
      ye = aBins[index].fVmin;
    }

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);

    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    //FIXME if(ye<=yy) continue; //Else we shall have a tessellation error.

    LIST_SET(points,0,xx,yy,aZZ);
    LIST_SET(points,1,xe,yy,aZZ);
    LIST_SET(points,2,xe,ye,aZZ);
    LIST_SET(points,3,xx,ye,aZZ);

    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = icoord + 3;
    coordIndex[4] = SO_END_FACE_INDEX;

    coordinate3->point.setValues(icoord,4,points);
    icoord += 4;  

    indexedFaceSet->coordIndex.setValues(iindex,5,coordIndex);
    iindex += 5;

    empty = FALSE;
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins1D_xy_wire_boxes(
 SoSeparator* aParent
,SoStyle& aStyle
,SbBool aBarChart
,SbPaintingPolicy aPainting
,const SbBaseColormap& aCmap
,std::vector<SbRepBin1D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZZ
,const SbString& aID
){
  //printf("debug : SoPlotter::repBins1D_xy_wire_boxes : ID \"%s\" zz %g\n",
    //aID.getString(),aZZ);

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SoSeparator* separator = new SoSeparator();

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SbBool empty = TRUE;
  int32_t coordIndex[6];
  SbVec3f points[5];
  int xnbin = aBins.size();
  for(int index=0;index<xnbin;index++) { 
    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fVmin;
    float ye = aBins[index].fVal;
    if(ye<yy) {
      yy = aBins[index].fVal;
      ye = aBins[index].fVmin;
    }

    float val = aBins[index].fVal;

    if(aBarChart==TRUE) BarChart(aStyle.barOffset.getValue(),aStyle.barWidth.getValue(),xx,xe);

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);

    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    SoSceneGraph* sep = new SoSceneGraph; 
    separator->addChild(sep);

    aBins[index].fSeparator = sep;

   {char s[128];
    ::sprintf(s,"%d",index);
    sep->setInfos(s);}
   {std::string sp;
    if(!inlib::p2sx(sep->getInfos(),sp)){}
    std::string sid(aID.getString());
    sid += "/"+sp;
    sep->setString(sid.c_str());}

    SbColor sbColor;
    if(aPainting==SbPaintingByValue) {
      sbColor = aCmap.getColor(val);
    } else if( (aPainting==SbPaintingGreyScale) ||
               (aPainting==SbPaintingGreyScaleInverse) ||
               (aPainting==SbPaintingVioletToRed)) {
      sbColor = aCmap.getColor(aBins[index].fRatio);
    } else {
      sbColor = aStyle.color.getValue();
    }

    SoMaterial* material = fStyleCache->getMaterial(sbColor,aStyle.transparency.getValue());
    sep->addChild(material);

    sep->addChild(getLineStyle(aStyle));

    LIST_SET(points,0,xx,yy,aZZ);
    LIST_SET(points,1,xe,yy,aZZ);
    LIST_SET(points,2,xe,ye,aZZ);
    LIST_SET(points,3,xx,ye,aZZ);
    LIST_SET(points,4,xx,yy,aZZ);

    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = icoord + 3;
    coordIndex[4] = icoord + 4;
    coordIndex[5] = SO_END_LINE_INDEX;

    coordinate3->point.setValues(icoord,5,points);
    icoord += 5;
 
    SoIndexedLineSet* lineSet = new SoIndexedLineSet;
    lineSet->coordIndex.setValues(0,6,coordIndex);
    sep->addChild(lineSet);

    empty = FALSE;
  }

  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins1D_xy_wire_boxes_one(
 SoSeparator* aParent
,SoStyle& aStyle
,SbBool aBarChart
,const std::vector<SbRepBin1D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZZ
,const SbString& aID
){
  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
  separator->addChild(material);

  separator->addChild(getLineStyle(aStyle));
          
  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SoIndexedLineSet* lineSet = new SoIndexedLineSet;
  separator->addChild(lineSet);
  int iindex = 0;

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SbBool empty = TRUE;
  int32_t coordIndex[6];
  SbVec3f points[5];
  int xnbin = aBins.size();
  for(int index=0;index<xnbin;index++) { 
    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fVmin;
    float ye = aBins[index].fVal;
    if(ye<yy) {
      yy = aBins[index].fVal;
      ye = aBins[index].fVmin;
    }

    if(aBarChart==TRUE) BarChart(aStyle.barOffset.getValue(),aStyle.barWidth.getValue(),xx,xe);

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);

    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    LIST_SET(points,0,xx,yy,aZZ);
    LIST_SET(points,1,xe,yy,aZZ);
    LIST_SET(points,2,xe,ye,aZZ);
    LIST_SET(points,3,xx,ye,aZZ);
    LIST_SET(points,4,xx,yy,aZZ);

    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = icoord + 3;
    coordIndex[4] = icoord + 4;
    coordIndex[5] = SO_END_LINE_INDEX;

    coordinate3->point.setValues(icoord,5,points);
    icoord += 5;
 
    lineSet->coordIndex.setValues(iindex,6,coordIndex);
    iindex += 6;

    empty = FALSE;
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins1D_xy_curve_one(SoSeparator* aParent,SoStyle& aStyle,const std::vector<SbRepBin1D>& aBins,
                                       const SbRepBox& aBoxX,const SbRepBox& aBoxY,float aZZ,const SbString& aID){
  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
  separator->addChild(material);

  separator->addChild(getLineStyle(aStyle));
          
  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  int xnbin = aBins.size();
  SbVec3f* points = new SbVec3f[xnbin?xnbin:1];
  int pointi = 0;

  for(int index=0;index<xnbin;index++) { 
    float x = (aBins[index].fXmin + aBins[index].fXmax)/2;
    float y = aBins[index].fVal;

    x = VerifyLog(x,xmin,dx,xlog);
    y = VerifyLog(y,ymin,dy,ylog);

    //FIXME : clipping should be done on the nurbs.

    LIST_SET(points,pointi,x,y,aZZ);
    pointi++;
  }

  if(pointi<=1) {
    separator->unref();
  } else {
    //SoTools_addLinesToNode(separator,pointi,points);
    SoTools_addNurbsCurveToNode(separator,pointi,points);
    aParent->addChild(separator);
  }

  delete [] points;
}

void SoPlotter::repBins1D_xy_lines_one(SoSeparator* aParent,SoStyle& aStyle,
                                       const std::vector<SbRepBin1D>& aBins,
                                       const SbRepBox& aBoxX,const SbRepBox& aBoxY,float aZZ,const SbString& aID) {
  // Draw lines connecting top middle of bins.

  //printf("debug : SoPlotter::repBins2D_lines_one\n");
  int xnbin = aBins.size();
  std::vector<SbVec3f> points(xnbin);
  for(int index=0;index<xnbin;index++) { 
    float x = (aBins[index].fXmin + aBins[index].fXmax)/2;
    float y = aBins[index].fVal;
    points[index] = SbVec3f(x,y,aZZ);
  }
  int ptn;
  SbVec3f* pts = clipPolyLine2D(points,aBoxX,aBoxY,ptn);
  if(ptn) {
    SoSceneGraph* separator = new SoSceneGraph;
    separator->setString(aID);

    separator->addChild(getLineStyle(aStyle));
    separator->addChild(fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue()));
    SoTools_addLinesToNode(separator,ptn,pts);
    delete [] pts;

    aParent->addChild(separator);
  }
}

void SoPlotter::repHatch1D_xy(SoSeparator* aParent,SoStyle& aStyle,SbBool aBarChart,
                              const std::vector<SbRepBin1D>& aBins,const SbRepBox& aBoxX,const SbRepBox& aBoxY,float aZZ){
  //printf("debug : SoPlotter::repHatch1D_xy : zz %g barchart %d sw %g\n",aZZ,aBarChart,aStyle.stripWidth.getValue());

  SoSeparator* separator = new SoSeparator;

  SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
  separator->addChild(material);

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;
          
  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SbBool empty = TRUE;

  SbVec3f points[5];
  int xnbin = aBins.size();
  for(int index=0;index<xnbin;index++) { 
    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fVmin;
    float ye = aBins[index].fVal;
    if(ye<yy) {
      yy = aBins[index].fVal;
      ye = aBins[index].fVmin;
    }

    if(aBarChart==TRUE) BarChart(aStyle.barOffset.getValue(),aStyle.barWidth.getValue(),xx,xe);

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);

    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    LIST_SET(points,0,xx,yy,aZZ);
    LIST_SET(points,1,xe,yy,aZZ);
    LIST_SET(points,2,xe,ye,aZZ);
    LIST_SET(points,3,xx,ye,aZZ);
    LIST_SET(points,4,xx,yy,aZZ);

    // Use the bin separator in order that 
    // picking a hatch picks also the bin.
    SoSeparator* binSep = aBins[index].fSeparator;
    if(binSep) {
      //We can have multiple hatching for a bins ; have a separator :
      SbHatching sbHatching;            
      sbHatching.setOffsetPoint(SbVec3f(0,0,aZZ));
      sbHatching.setAngle(aStyle.angle.getValue());
      sbHatching.setSpacing(aStyle.spacing.getValue()); 
      sbHatching.setStripWidth(aStyle.stripWidth.getValue()); 

      int res = sbHatching.checkPolyline(points,4);      
      if (res == TRUE) res = sbHatching.computePolyline(points,4);

      unsigned int numPoints = sbHatching.getNoPoints();
      unsigned int numVertices = sbHatching.getNoVertices();
      if((res==TRUE) && numPoints && numVertices) {

	SoSeparator* hatchSep = new SoSeparator(); 
	binSep->addChild(hatchSep);
	
        SbVec3f* xyz = new SbVec3f[numPoints];
        const std::vector<SbVec3f>& hatchPointsList = sbHatching.getPoints();
       {for (unsigned int a=0;a<numPoints;a++) xyz[a] = hatchPointsList[a];}
        coordinate3->point.setValues(icoord,numPoints,xyz);
        delete [] xyz;
	
	int32_t* coord = new int32_t[numVertices*3];
        for (unsigned int a=0;a<numVertices;a++) {
          int ia = 3 * a;
	  coord[ia] = icoord;
          icoord++;
	  coord[ia+1] = icoord;
          icoord++;
	  coord[ia+2] = SO_END_LINE_INDEX;
	}

	if(aStyle.stripWidth.getValue()==0) {
	  hatchSep->addChild(getLineStyle(aStyle));
          hatchSep->addChild(fStyleCache->getNormalBindingOverall());
          hatchSep->addChild(fStyleCache->getNormalZ());
    	  hatchSep->addChild(material);
          hatchSep->addChild(coordinate3);
	  SoIndexedLineSet* indexedLineSet = new SoIndexedLineSet;
	  hatchSep->addChild(indexedLineSet);
	  indexedLineSet->coordIndex.setValues(0,numVertices*3,coord);
	} else {
	  hatchSep->addChild(fStyleCache->getFilled());
          hatchSep->addChild(fStyleCache->getNormalBindingPerFace());
          hatchSep->addChild(fStyleCache->getNormalZ());
    	  hatchSep->addChild(material);
          hatchSep->addChild(coordinate3);
	  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
	  hatchSep->addChild(indexedFaceSet);
	  indexedFaceSet->coordIndex.setValues(0,numVertices*3,coord);
	}
        delete [] coord;

        empty = FALSE;
      }
    }
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repHatch1D_xy_one(
 SoSeparator* aParent
,SoStyle& aStyle
,SbBool aBarChart
,const std::vector<SbRepBin1D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZZ
,const SbString& aID
){
  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
  separator->addChild(material);

  if(aStyle.stripWidth.getValue()!=0) {
    separator->addChild(fStyleCache->getFilled());
    separator->addChild(fStyleCache->getNormalBindingPerFace());          
    separator->addChild(fStyleCache->getNormalZ());
  } else {
    separator->addChild(getLineStyle(aStyle));
  }

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;
  
  SoIndexedLineSet* lineSet = 0;
  SoIndexedFaceSet* faceSet = 0;
  if(aStyle.stripWidth.getValue()==0) {
    lineSet = new SoIndexedLineSet;
    separator->addChild(lineSet);
  } else {
    faceSet = new SoIndexedFaceSet;
    separator->addChild(faceSet);
  }
  int iindex = 0;
    
  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SbVec3f points[5];
  int xnbin = aBins.size();
  SbBool empty = TRUE;

  for(int index=0;index<xnbin;index++) {
    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fVmin;
    float ye = aBins[index].fVal;
    if(ye<yy) {
      yy = aBins[index].fVal;
      ye = aBins[index].fVmin;
    }

    if(aBarChart==TRUE) BarChart(aStyle.barOffset.getValue(),aStyle.barWidth.getValue(),xx,xe);

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);

    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    LIST_SET(points,0,xx,yy,aZZ);
    LIST_SET(points,1,xe,yy,aZZ);
    LIST_SET(points,2,xe,ye,aZZ);
    LIST_SET(points,3,xx,ye,aZZ);
    LIST_SET(points,4,xx,yy,aZZ);

    SbHatching sbHatching;            
    sbHatching.setOffsetPoint(SbVec3f(0,0,aZZ));
    sbHatching.setAngle(aStyle.angle.getValue());
    sbHatching.setSpacing(aStyle.spacing.getValue()); 
    sbHatching.setStripWidth(aStyle.stripWidth.getValue()); 
    
    int res = sbHatching.checkPolyline(points,4);      
    if (res == TRUE) res = sbHatching.computePolyline(points,4);

    unsigned int numPoints = sbHatching.getNoPoints();
    unsigned int numVertices = sbHatching.getNoVertices();
    if((res==TRUE) && numPoints && numVertices) {

      const std::vector<SbVec3f>& hatchPointsList = sbHatching.getPoints();

      SbVec3f* xyz = new SbVec3f[numPoints];
     {for (unsigned int a=0;a<numPoints;a++) xyz[a] = hatchPointsList[a];}
      coordinate3->point.setValues(icoord,numPoints,xyz);
      delete [] xyz;

      int32_t* coord = new int32_t[numVertices*3];
      for (unsigned int a=0;a<numVertices;a++) {
        int ia = 3 * a;
        coord[ia] = icoord;
        icoord++;
	coord[ia+1] = icoord;
        icoord++;
	coord[ia+2] = SO_END_LINE_INDEX;
      }

      if(aStyle.stripWidth.getValue()==0) {
	lineSet->coordIndex.setValues(iindex,numVertices*3,coord);
      } else {
	faceSet->coordIndex.setValues(iindex,numVertices*3,coord);
      }
      delete [] coord;

      iindex += numVertices*3;

      empty = FALSE;
    }
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins2D_xy_random_one(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepBin2D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aBmin
,float aBmax
,const SbString& aID
){
  //printf("debug : SoPlotter::repBins2D_xy_random_one\n");

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  //  Draw for each bins a number of random 
  // points proportiannal to bins range.

  float range = aBmax - aBmin;

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t icoord = 0;

  SbVec3f points[100];

  for(unsigned int index=0;index<aBins.size();index++) {

    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fYmin;
    float ye = aBins[index].fYmax;
    float val = aBins[index].fVal;

    // If range is 0. ; then all bins that have entries 
    // have same values. Draw one point.

    int nmin = 1;
    int nmax = 50;
    int npt = range>0. ? (int)((val - aBmin)*(nmax-nmin)/range + nmin):1;
    if(npt>0) {
      int ipt = 0;                              
      float xdbin = xe - xx;
      float ydbin = ye - yy;
      for(int count=0;count<npt;count++) {
        float xxx = xx + xdbin * (((float)rand())/RAND_MAX);
        float yyy = yy + ydbin * (((float)rand())/RAND_MAX);
        xxx = VerifyLog(xxx ,xmin,dx  ,xlog);
        yyy = VerifyLog(yyy ,ymin,dy  ,ylog);
        if((xxx>=0)&&(xxx<=1)  &&
           (yyy>=0)&&(yyy<=1)  ) { 
          LIST_SET(points,ipt,xxx,yyy,0);
          ipt++;
        }                                  
      }
      if(ipt>0) {
        int32_t pointn = ipt;
        coordinate3->point.setValues(icoord,pointn,points);
        icoord += pointn;
      }
    }

  }//end for                      

  if(icoord<=0) {
    coordinate3->unref();
  } else {
    SoSceneGraph* separator = new SoSceneGraph;
    separator->setString(aID);

    aParent->addChild(separator);

    separator->addChild(fStyleCache->getPoints());

    SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
    separator->addChild(material);

    separator->addChild(coordinate3);

    SbMarkerStyle smark = (SbMarkerStyle)aStyle.markerStyle.getValue();
    if(smark==SbMarkerDot) {
      SoPointSet* pointSet = new SoPointSet;
      pointSet->numPoints.setValue(icoord);
      separator->addChild(pointSet);
    } else {
      HEPVis_SoMarkerSet* markerSet = new HEPVis_SoMarkerSet;
      int wmark = aStyle.markerSize.getValue();
      markerSet->markerIndex.setValue(HEPVis_SoMarkerSet::getMarkerIndex(smark,wmark));
      markerSet->numPoints.setValue(icoord);
      separator->addChild(markerSet);
    }
  }
}

void SoPlotter::repBins2D_xy_box_one(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepBin2D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aBmin
,float aBmax
,const SbString& aID
){

  // Draw box of size proportionnal to bin value.                  

  //printf("debug : SoPlotter::repBins2D_xy_box_one\n");

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float range = aBmax - aBmin;
            
  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  separator->addChild(fStyleCache->getFilled());
  separator->addChild(fStyleCache->getNormalBindingOverall());
  separator->addChild(fStyleCache->getNormalZ());

  SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
  separator->addChild(material);

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
  separator->addChild(indexedFaceSet);
  int iindex = 0;

  SbVec3f line[4];
  int32_t coordIndex[5];
  for(unsigned int index=0;index<aBins.size();index++) {
    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fYmin;
    float ye = aBins[index].fYmax;
    float val = aBins[index].fVal;

    float xsize,ysize;
    if(range>0) {                  
      // If val = bmax, the box maps the cell.
      xsize = (val - aBmin) * (xe - xx) / range;
      ysize = (val - aBmin) * (ye - yy) / range;
    } else {
      //If range is 0. ; then all bins that have 
      // entries have same values. Draw box xdbin * ydbin.
      xsize = xe - xx;
      ysize = ye - yy;
    }

    xx = xx + ((xe-xx) - xsize)/2;
    xe = xx + xsize;
    yy = yy + ((ye-yy) - ysize)/2;
    ye = yy + ysize;
                                  
    xx = VerifyLog(xx ,xmin,dx  ,xlog);
    xe = VerifyLog(xe ,xmin,dx  ,xlog);
    yy = VerifyLog(yy ,ymin,dy  ,ylog);
    ye = VerifyLog(ye ,ymin,dy  ,ylog);
                                  
    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    line[0].setValue(xx,yy,0);
    line[1].setValue(xe,yy,0);
    line[2].setValue(xe,ye,0);
    line[3].setValue(xx,ye,0);

    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = icoord + 3;
    coordIndex[4] = SO_END_FACE_INDEX;

    coordinate3->point.setValues(icoord,4,line);
    icoord += 4;  

    indexedFaceSet->coordIndex.setValues(iindex,5,coordIndex);
    iindex += 5;
  }

  if(icoord<=0) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
            
}

void SoPlotter::repBins2D_xy_box(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepBin2D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aBmin
,float aBmax
,const SbString& aID
){
  // Draw box of size proportionnal to bin value.
  //printf("debug : SoPlotter::repBins2D_xy_box\n");
  SoSeparator* separator = new SoSeparator();

  separator->addChild(fStyleCache->getFilled());
  separator->addChild(fStyleCache->getNormalBindingPerFace());
  separator->addChild(fStyleCache->getNormalZ());

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SbBool empty = TRUE;
  SbVec3f points[5];
  int32_t coordIndex[6];
  float range = aBmax - aBmin;            
  for(unsigned int index=0;index<aBins.size();index++) {

    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fYmin;
    float ye = aBins[index].fYmax;
    float val = aBins[index].fVal;

    float xsize,ysize;
    if(range>0) {                  
      // If val = bmax, the box maps the cell.
      xsize = (val - aBmin) * (xe - xx) / range;
      ysize = (val - aBmin) * (ye - yy) / range;
    } else {
      //If range is 0. ; then all bins that have 
      // entries have same values. Draw box xdbin * ydbin.
      xsize = xe - xx;
      ysize = ye - yy;
    }

    xx = xx + ((xe-xx) - xsize)/2;
    xe = xx + xsize;
    yy = yy + ((ye-yy) - ysize)/2;
    ye = yy + ysize;
                                  
    xx = VerifyLog(xx ,xmin,dx  ,xlog);
    xe = VerifyLog(xe ,xmin,dx  ,xlog);
    yy = VerifyLog(yy ,ymin,dy  ,ylog);
    ye = VerifyLog(ye ,ymin,dy  ,ylog);
                                  
    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    SoSceneGraph* sep = new SoSceneGraph;
    separator->addChild(sep);

   {char s[128];
    ::sprintf(s,"%d %d",aBins[index].fI,aBins[index].fJ);
    sep->setInfos(s);}
   {std::string sp;
    if(!inlib::p2sx(sep->getInfos(),sp)){}
    std::string sid(aID.getString());
    sid += "/"+sp;
    sep->setString(sid.c_str());}

    SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
    sep->addChild(material);

    LIST_SET(points,0,xx,yy,0);
    LIST_SET(points,1,xe,yy,0);
    LIST_SET(points,2,xe,ye,0);
    LIST_SET(points,3,xx,ye,0);
    LIST_SET(points,4,xx,yy,0);

    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = icoord + 3;
    coordIndex[4] = icoord + 4;
    coordIndex[5] = SO_END_FACE_INDEX;

    coordinate3->point.setValues(icoord,5,points);
    icoord += 5;  

    SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
    indexedFaceSet->coordIndex.setValues(0,6,coordIndex);
    sep->addChild(indexedFaceSet);

    empty = FALSE;
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins2D_xy_wire_box(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepBin2D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aBmin
,float aBmax
,const SbString& aID
){
  // Draw box of size proportionnal to bin value.

  //printf("debug : SoPlotter::repBins2D_xy_wire_box\n");
  SoSeparator* separator = new SoSeparator();

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SbBool empty = TRUE;

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SbVec3f points[5];                      
  int32_t coordIndex[6];
  float range = aBmax - aBmin;
  for(unsigned int index=0;index<aBins.size();index++) {

    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fYmin;
    float ye = aBins[index].fYmax;
    float val = aBins[index].fVal;

    float xsize,ysize;
    if(range>0) {                  
      // If val = bmax, the box maps the cell.
      xsize = (val - aBmin) * (xe - xx) / range;
      ysize = (val - aBmin) * (ye - yy) / range;
    } else {
      //If range is 0. ; then all bins that have 
      // entries have same values. Draw box xdbin * ydbin.
      xsize = xe - xx;
      ysize = ye - yy;
    }

    xx = xx + ((xe-xx) - xsize)/2;
    xe = xx + xsize;
    yy = yy + ((ye-yy) - ysize)/2;
    ye = yy + ysize;
                                  
    xx = VerifyLog(xx ,xmin,dx  ,xlog);
    xe = VerifyLog(xe ,xmin,dx  ,xlog);
    yy = VerifyLog(yy ,ymin,dy  ,ylog);
    ye = VerifyLog(ye ,ymin,dy  ,ylog);
                                  
    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    SoSceneGraph* sep = new SoSceneGraph;
    separator->addChild(sep);

   {char s[128];
    ::sprintf(s,"%d %d",aBins[index].fI,aBins[index].fJ);
    sep->setInfos(s);}
   {std::string sp;
    if(!inlib::p2sx(sep->getInfos(),sp)){}
    std::string sid(aID.getString());
    sid += "/"+sp;
    sep->setString(sid.c_str());}

    SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
    sep->addChild(material);

    sep->addChild(getLineStyle(aStyle));

    LIST_SET(points,0,xx,yy,0);
    LIST_SET(points,1,xe,yy,0);
    LIST_SET(points,2,xe,ye,0);
    LIST_SET(points,3,xx,ye,0);
    LIST_SET(points,4,xx,yy,0);

    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = icoord + 3;
    coordIndex[4] = icoord + 4;
    coordIndex[5] = SO_END_LINE_INDEX;

    coordinate3->point.setValues(icoord,5,points);
    icoord += 5;

    SoIndexedLineSet* lineSet = new SoIndexedLineSet;
    lineSet->coordIndex.setValues(0,6,coordIndex);
    sep->addChild(lineSet);

    empty = FALSE;
  }            
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins2D_xy_wire_box_one(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepBin2D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aBmin
,float aBmax
,const SbString& aID
){
  // Draw wired box of size proportionnal to bin value.
  //printf("debug : SoPlotter::repBins2D_xy_wire_box_one\n");
  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
  separator->addChild(material);

  separator->addChild(getLineStyle(aStyle));

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SoIndexedLineSet* lineSet = new SoIndexedLineSet;
  separator->addChild(lineSet);
  int iindex = 0;

  SbBool empty = TRUE;

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SbVec3f points[5];                      
  int32_t coordIndex[6];
  float range = aBmax - aBmin;
  for(unsigned int index=0;index<aBins.size();index++) {

    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fYmin;
    float ye = aBins[index].fYmax;
    float val = aBins[index].fVal;

    float xsize,ysize;
    if(range>0) {                  
      // If val = bmax, the box maps the cell.
      xsize = (val - aBmin) * (xe - xx) / range;
      ysize = (val - aBmin) * (ye - yy) / range;
    } else {
      //If range is 0. ; then all bins that have 
      // entries have same values. Draw box xdbin * ydbin.
      xsize = xe - xx;
      ysize = ye - yy;
    }

    xx = xx + ((xe-xx) - xsize)/2;
    xe = xx + xsize;
    yy = yy + ((ye-yy) - ysize)/2;
    ye = yy + ysize;
                                  
    xx = VerifyLog(xx ,xmin,dx  ,xlog);
    xe = VerifyLog(xe ,xmin,dx  ,xlog);
    yy = VerifyLog(yy ,ymin,dy  ,ylog);
    ye = VerifyLog(ye ,ymin,dy  ,ylog);
                                  
    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    LIST_SET(points,0,xx,yy,0);
    LIST_SET(points,1,xe,yy,0);
    LIST_SET(points,2,xe,ye,0);
    LIST_SET(points,3,xx,ye,0);
    LIST_SET(points,4,xx,yy,0);

    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = icoord + 3;
    coordIndex[4] = icoord + 4;
    coordIndex[5] = SO_END_LINE_INDEX;

    coordinate3->point.setValues(icoord,5,points);
    icoord += 5;

    lineSet->coordIndex.setValues(iindex,6,coordIndex);
    iindex += 6;

    empty = FALSE;
  }            
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins2D_xy_solid(
 SoSeparator* aParent
,SoStyle& aStyle
,SbPaintingPolicy aPainting
,const SbBaseColormap& aCmap
,const std::vector<SbRepBin2D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,const SbString& aID
){

  // Draw box of bin size colored according a color map.
  //printf("debug : SoPlotter::repBins2D_xy_solid\n");

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  //float zmin = aBoxZ.fPos;
  //float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  SoSeparator* separator = new SoSeparator();

  separator->addChild(fStyleCache->getFilled());
  separator->addChild(fStyleCache->getNormalBindingPerFace());
  separator->addChild(fStyleCache->getNormalZ());

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SbBool empty = TRUE;
  SbVec3f points[4];
  int32_t coordIndex[5];                      
  for(unsigned int index=0;index<aBins.size();index++) {

    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fYmin;
    float ye = aBins[index].fYmax;

    xx = VerifyLog(xx ,xmin,dx  ,xlog);
    xe = VerifyLog(xe ,xmin,dx  ,xlog);
    yy = VerifyLog(yy ,ymin,dy  ,ylog);
    ye = VerifyLog(ye ,ymin,dy  ,ylog);
                                  
    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    SoSceneGraph* sep = new SoSceneGraph;
    separator->addChild(sep);

   {char s[128];
    ::sprintf(s,"%d %d",aBins[index].fI,aBins[index].fJ);
    sep->setInfos(s);}
   {std::string sp;
    if(!inlib::p2sx(sep->getInfos(),sp)){}
    std::string sid(aID.getString());
    sid += "/"+sp;
    sep->setString(sid.c_str());}

    if(aPainting==SbPaintingByValue) {
      float val = aBins[index].fVal;
      val = (zlog==TRUE?TakeLog(val):val);
      SoMaterial* material = new SoMaterial;
      material->diffuseColor.setValue(aCmap.getColor(val));
      material->transparency.setValue(aStyle.transparency.getValue());
      sep->addChild(material);
    } else if( (aPainting==SbPaintingGreyScale) ||
               (aPainting==SbPaintingGreyScaleInverse) ||
	       (aPainting==SbPaintingVioletToRed) ){
      SoMaterial* material = new SoMaterial;
      material->diffuseColor.setValue(aCmap.getColor(aBins[index].fRatio));
      material->transparency.setValue(aStyle.transparency.getValue());
      sep->addChild(material);
    } else {
      SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
      sep->addChild(material);
    }

    LIST_SET(points,0,xx ,yy ,0);
    LIST_SET(points,1,xe ,yy ,0);
    LIST_SET(points,2,xe ,ye ,0);
    LIST_SET(points,3,xx ,ye ,0);

    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = icoord + 3;
    coordIndex[4] = SO_END_FACE_INDEX;

    coordinate3->point.setValues(icoord,4,points);
    icoord += 4;  

    SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
    indexedFaceSet->coordIndex.setValues(0,5,coordIndex);
    sep->addChild(indexedFaceSet);

    empty = FALSE;
  }
            
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins2D_xy_solid_one(
 SoSeparator* aParent
,SoStyle& aStyle
,SbPaintingPolicy aPainting
,const SbBaseColormap& aCmap
,const std::vector<SbRepBin2D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,const SbString& aID
){
  // Draw box of bin size colored according a color map.
  //printf("debug : SoPlotter::repBins2D_xy_solid_one\n");

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  //float zmin = aBoxZ.fPos;
  //float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  separator->addChild(fStyleCache->getFilled());
  separator->addChild(fStyleCache->getNormalBindingOverall());
  separator->addChild(fStyleCache->getNormalZ());

  float transparency = aStyle.transparency.getValue();

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

#define MAT_BINDING

#ifdef MAT_BINDING
  separator->addChild(fStyleCache->getMaterialBindingPerFaceIndexed());
  SoMaterial* soMaterial = new SoMaterial;
  separator->addChild(soMaterial);

  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
  separator->addChild(indexedFaceSet);
  int icindex = 0;
  int imindex = 0;
#endif

  SbBool empty = TRUE;
  SbVec3f points[4];
  int32_t coordIndex[5];                      
  SbVec3f zunit(0,0,1);
  for(unsigned int index=0;index<aBins.size();index++) {

    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fYmin;
    float ye = aBins[index].fYmax;

    xx = VerifyLog(xx ,xmin,dx  ,xlog);
    xe = VerifyLog(xe ,xmin,dx  ,xlog);
    yy = VerifyLog(yy ,ymin,dy  ,ylog);
    ye = VerifyLog(ye ,ymin,dy  ,ylog);
                                  
    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    SbColor sbColor;
    if(aPainting==SbPaintingByValue) {
      float val = aBins[index].fVal;
      val = (zlog==TRUE?TakeLog(val):val);
      sbColor = aCmap.getColor(val);
    } else if( (aPainting==SbPaintingGreyScale) ||
               (aPainting==SbPaintingGreyScaleInverse) ||
               (aPainting==SbPaintingVioletToRed)) {
      sbColor = aCmap.getColor(aBins[index].fRatio);
    } else {
      sbColor = aStyle.color.getValue();
    }

#ifdef MAT_BINDING
    int32_t imat = 0;
    if(imindex==0) {
      soMaterial->diffuseColor.set1Value(0,sbColor);
      soMaterial->transparency.set1Value(0,transparency);
    } else {
      bool found = false;
      int num = soMaterial->diffuseColor.getNum();
      for(int i=0;i<num;i++) {
        if(sbColor==soMaterial->diffuseColor[i]) {
          imat = i;
          found = true;
        }
      }
      if(!found) {
        imat = num;
        soMaterial->diffuseColor.set1Value(num,sbColor);
        soMaterial->transparency.set1Value(num,transparency);
      }
    }
#else
    SoMaterial* material = fStyleCache->getMaterial(sbColor,transparency);
    separator->addChild(material);
#endif

    LIST_SET(points,0,xx ,yy ,0);
    LIST_SET(points,1,xe ,yy ,0);
    LIST_SET(points,2,xe ,ye ,0);
    LIST_SET(points,3,xx ,ye ,0);

    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = icoord + 2;
    coordIndex[3] = icoord + 3;
    coordIndex[4] = SO_END_FACE_INDEX;

    coordinate3->point.setValues(icoord,4,points);
    icoord += 4;  

#ifdef MAT_BINDING
    indexedFaceSet->coordIndex.setValues(icindex,5,coordIndex);
    icindex += 5;
    indexedFaceSet->materialIndex.set1Value(imindex,imat);
    imindex += 1;
#else
    SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
    indexedFaceSet->coordIndex.setValues(0,5,coordIndex);
    separator->addChild(indexedFaceSet);
#endif

    empty = FALSE;
  }
            
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repBins2D_xy_text_one(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepBin2D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbString& aID
){
  //Draw box and text for number of entries.          
  //printf("debug : SoPlotter::repBins2D_xy_text_one\n");

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SbVec3f points[5];
  int32_t coordIndex[6];
            
  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  separator->addChild(getLineStyle(aStyle));

  SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
  separator->addChild(material);

  SbBool drawWireBoxe = FALSE;

  SoCoordinate3* coordinate3 = 0;
  int icoord = 0;

  SoIndexedLineSet* lineSet = 0;
  int iindex = 0;
  if(drawWireBoxe==TRUE) {
    coordinate3 = new SoCoordinate3;
    separator->addChild(coordinate3);

    lineSet = new SoIndexedLineSet;
    separator->addChild(lineSet);
  }

  SbBool empty = TRUE;
  for(unsigned int index=0;index<aBins.size();index++) {

    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fYmin;
    float ye = aBins[index].fYmax;
    float val = aBins[index].fVal;

    int ival = (int)val;
    if(ival==0) continue;

    xx = VerifyLog(xx ,xmin,dx  ,xlog);
    xe = VerifyLog(xe ,xmin,dx  ,xlog);
    yy = VerifyLog(yy ,ymin,dy  ,ylog);
    ye = VerifyLog(ye ,ymin,dy  ,ylog);
                
    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    char sval[32];
    ::sprintf (sval,"%d",ival);
    SbString sbval(sval);
    int charn = sbval.getLength();
    if(charn<=0) continue;

    float dx = xe-xx;
    float dy = ye-yy;
                  
    if(drawWireBoxe==TRUE) {

      LIST_SET(points,0,xx,yy,0);
      LIST_SET(points,1,xe,yy,0);
      LIST_SET(points,2,xe,ye,0);
      LIST_SET(points,3,xx,ye,0);
      LIST_SET(points,4,xx,yy,0);

      coordIndex[0] = icoord + 0;
      coordIndex[1] = icoord + 1;
      coordIndex[2] = icoord + 2;
      coordIndex[3] = icoord + 3;
      coordIndex[4] = icoord + 4;
      coordIndex[5] = SO_END_LINE_INDEX;

      coordinate3->point.setValues(icoord,5,points);
      icoord += 5;
 
      lineSet->coordIndex.setValues(iindex,6,coordIndex);
      iindex += 6;
    }

    float x = xx + 0.1F * dx;
    float y = yy + 0.1F * dy;
    float z = 0;
    float w = dx * 0.5F;
    float h = dy * 0.5F;

    SoSeparator* sepText = new SoSeparator;
    separator->addChild(sepText);

    SoTransform* transform = new SoTransform;
    transform->scaleFactor.setValue(SbVec3f(w/charn,h,1));
    transform->translation.setValue(x,y,z);
    sepText->addChild(transform);

    SoTextHershey* text = new SoTextHershey;
    text->string.setValue(sbval);
    sepText->addChild (text);

    empty = FALSE;
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }            
}

void SoPlotter::repBins2D_xy_text(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepBin2D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbString& aID
){
  //Draw box and text for number of entries.          
  //printf("debug : SoPlotter::repBins2D_xy_text\n");

  SoSeparator* separator = new SoSeparator();

  SbBool drawWireBoxe = FALSE;

  SoCoordinate3* coordinate3 = 0;
  int icoord = 0;

  if(drawWireBoxe==TRUE) {
    coordinate3 = new SoCoordinate3;
    separator->addChild(coordinate3);
  }

  SbBool empty = TRUE;
  int32_t coordIndex[6];

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  SbVec3f points[5];
            
  for(unsigned int index=0;index<aBins.size();index++) {

    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fYmin;
    float ye = aBins[index].fYmax;
    float val = aBins[index].fVal;

    int ival = (int)val;
    if(ival==0) continue;

    xx = VerifyLog(xx ,xmin,dx  ,xlog);
    xe = VerifyLog(xe ,xmin,dx  ,xlog);
    yy = VerifyLog(yy ,ymin,dy  ,ylog);
    ye = VerifyLog(ye ,ymin,dy  ,ylog);
                
    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    char sval[32];
    ::sprintf (sval,"%d",ival);
    SbString sbval(sval);
    int charn = sbval.getLength();
    if(charn<=0) continue;

    SoSceneGraph* sep = new SoSceneGraph;
    separator->addChild(sep);

   {char s[128];
    ::sprintf(s,"%d %d",aBins[index].fI,aBins[index].fJ);
    sep->setInfos(s);}
   {std::string sp;
    if(!inlib::p2sx(sep->getInfos(),sp)){}
    std::string sid(aID.getString());
    sid += "/"+sp;
    sep->setString(sid.c_str());}

    SoMaterial* material = 
      fStyleCache->getMaterial(aStyle.color.getValue(),
                               aStyle.transparency.getValue());
    sep->addChild(material);

    sep->addChild(getLineStyle(aStyle));

    float dx = xe-xx;
    float dy = ye-yy;
                  
    if(drawWireBoxe==TRUE) {
      LIST_SET(points,0,xx,yy,0);
      LIST_SET(points,1,xe,yy,0);
      LIST_SET(points,2,xe,ye,0);
      LIST_SET(points,3,xx,ye,0);
      LIST_SET(points,4,xx,yy,0);
      coordIndex[0] = icoord + 0;
      coordIndex[1] = icoord + 1;
      coordIndex[2] = icoord + 2;
      coordIndex[3] = icoord + 3;
      coordIndex[4] = icoord + 4;
      coordIndex[5] = SO_END_LINE_INDEX;
  
      coordinate3->point.setValues(icoord,5,points);
      icoord += 5;
 
      SoIndexedLineSet* lineSet = new SoIndexedLineSet;
      lineSet->coordIndex.setValues(0,6,coordIndex);
      sep->addChild(lineSet);
    }

    float x = xx + 0.1F * dx;
    float y = yy + 0.1F * dy;
    float z = 0;
    float w = dx * 0.5F;
    float h = dy * 0.5F;

    SoTransform* transform = new SoTransform;
    transform->scaleFactor.setValue(SbVec3f(w/charn,h,1));
    transform->translation.setValue(x,y,z);
    sep->addChild(transform);

    SoTextHershey* text = new SoTextHershey;
    text->string.setValue(sbval);
    sep->addChild (text);

    empty = FALSE;
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }            
}

void SoPlotter::repContour_xy(
 SoSeparator* aParent
,SoStyle& aStyle
,SbPaintingPolicy aPainting
,const SbBaseColormap& aCmap
,inlib::clist_contour& aContour
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,float aZZ
,const SbString& aID
){
  //printf("debug : repContour_xy\n");

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float zmin = aBoxZ.fPos;
  float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SbBool empty = TRUE;

  for (unsigned int i=0;i<aContour.get_number_of_planes();i++)  {
    inlib::cline_strip_list* pStripList = aContour.get_lines(i);
    if(!pStripList) {
      SoDebugError::postInfo("SoPlotter::repContour_xy","problem with contour.");
      separator->unref();
      return;
    }

    SoSeparator* sep = new SoSeparator();
    separator->addChild(sep);

    //If zlog true, zz is already in log.
    float val = (float)aContour.get_plane(i);
    float zz = val;

    zz = VerifyLog(zz,zmin,dz,FALSE);
    if(zz>1) continue;
    if(zz<0) continue;

    SbColor sbColor;
    if(aPainting==SbPaintingByValue) {
      sbColor = aCmap.getColor(val);
    } else if(aPainting==SbPaintingByLevel) {
      int icol = aCmap.colorn() ? (i % aCmap.colorn()) :0;
      sbColor = aCmap.color(icol);
    } else {
      sbColor = aStyle.color.getValue();
    }

    SoMaterial* material = fStyleCache->getMaterial(sbColor,aStyle.transparency.getValue());
    sep->addChild(material);

    sep->addChild(getLineStyle(aStyle));

    SoIndexedLineSet* lineSet = new SoIndexedLineSet;
    sep->addChild(lineSet);
    int iindex = 0;

    for (inlib::cline_strip_list::iterator pos=pStripList->begin();pos != pStripList->end() ; pos++) {
      inlib::cline_strip* pStrip = (*pos);
      if(!pStrip) {
        SoDebugError::postInfo
        ("SoPlotter::repContour_xy","problem with contour.");
        separator->unref();
        return;
      }
      if (pStrip->empty()) continue;
    
      //// putting point at start and end of strip
      //// retreiving index
      //unsigned int index=pStrip->front();
      //double xb=aContour.get_xi(index);
      //double yb=aContour.get_yi(index);
      //// retreiving index
      ////glColor4f(0,0,1,.8f);
      //index=pStrip->back();
      //double xe=aContour.get_xi(index);
      //double ye=aContour.get_yi(index);

      SbVec3f* points = new SbVec3f[pStrip->size()];
      int pointi = 0;
      int32_t* coordIndex = new int32_t[pStrip->size()+1];
      for (inlib::cline_strip::iterator pos2=pStrip->begin(); pos2 != pStrip->end() ; pos2++) {
        // retreiving index
        unsigned int index=(*pos2);
        float xx = (float)aContour.get_xi(index);
        float yy = (float)aContour.get_yi(index);
        xx = VerifyLog(xx,xmin,dx,xlog);
        yy = VerifyLog(yy,ymin,dy,ylog);
        if( 
            (xx<0) || (xx>1) ||
            (yy<0) || (yy>1) 
          ) {
          // Throw away this strip :
        }
        LIST_SET(points,pointi,xx,yy,aZZ+zz);
        coordIndex[pointi] = icoord + pointi;
        pointi++;
      }
      coordIndex[pointi] = SO_END_LINE_INDEX;

      if(pointi) {
        coordinate3->point.setValues(icoord,pointi,points);
        icoord += pointi;
        lineSet->coordIndex.setValues(iindex,pointi+1,coordIndex);
        iindex += pointi+1;
        empty = FALSE;
      }

      delete [] points;
      delete [] coordIndex;

    }

  }

  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }

}

void SoPlotter::repContour_xy_filled(
 SoSeparator* aParent
,SoStyle& aStyle
,SbPaintingPolicy aPainting
,const SbBaseColormap& aCmap
,inlib::clist_contour& aContour
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,float aZZ
,const SbString& aID
){
  //printf("debug : repContour_xy_filled\n");

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float zmin = aBoxZ.fPos;
  float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  SoSceneGraph* separator = new SoSceneGraph();
  separator->setString(aID);

  if(aStyle.lightModel.getValue()==SbLightModel_base_color) { //OpenPAW
    separator->addChild(fStyleCache->getLightModelBaseColor());
  }

  separator->addChild(fStyleCache->getFilled());
  separator->addChild(fStyleCache->getNormalBindingOverall());
  separator->addChild(fStyleCache->getNormalZ());

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  {// Draw background :
    SoSeparator* sep = new SoSeparator;
    separator->addChild(sep);

    float zz = aZZ - 0.01F;

    SbColor sbColor;
    if(aCmap.colorn()) {
      sbColor = aCmap.color(0);
    } else {
      sbColor = aStyle.color.getValue();
    }

    SoMaterial* material = fStyleCache->getMaterial(sbColor,aStyle.transparency.getValue());
    sep->addChild(material);

    float xx = xmin;
    float xe = xmin+dx; 
    float yy = ymin;
    float ye = ymin+dy; 

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);
                          
    SbVec3f points[4];
    LIST_SET(points,0,xx,yy,zz);
    LIST_SET(points,1,xe,yy,zz);
    LIST_SET(points,2,xe,ye,zz);
    LIST_SET(points,3,xx,ye,zz);
    coordinate3->point.setValues(icoord,4,points);
  
    int32_t coordIndex[5];
    coordIndex[0] = icoord;
    coordIndex[1] = icoord+1;
    coordIndex[2] = icoord+2;
    coordIndex[3] = icoord+3;
    coordIndex[4] = SO_END_FACE_INDEX;
    icoord += 4;

    SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
    indexedFaceSet->coordIndex.setValues(0,5,coordIndex);
    sep->addChild(indexedFaceSet);
  } // End background.

  SbBool empty = TRUE;
  for (unsigned int i=0;i<aContour.get_number_of_planes();i++)  {

    inlib::cline_strip_list* pStripList = aContour.get_lines(i);
    if(!pStripList) {
      SoDebugError::postInfo("SoPlotter::repContour_xy_filled","problem with contour.");
      separator->unref();
      return;
    }

    SoSeparator* sep = new SoSeparator;
    separator->addChild(sep);

    //If zlog true, zz is already in log.
    float val = (float)aContour.get_plane(i);
    float zz = val;

    zz = VerifyLog(zz,zmin,dz,FALSE);
    if(zz>1) continue;
    if(zz<0) continue;

    std::vector< std::vector<SbVec3f> > contourVector;

    for (inlib::cline_strip_list::iterator pos=pStripList->begin();pos != pStripList->end() ; pos++) {
      inlib::cline_strip* pStrip = (*pos);
      if(pStrip->size() >2) {
        std::vector<SbVec3f> v;
        for (inlib::cline_strip::iterator pos2=pStrip->begin();pos2 != pStrip->end();pos2++) {
          unsigned int index=(*pos2);
          float xx = (float)aContour.get_xi(index);
          float yy = (float)aContour.get_yi(index);
          xx = VerifyLog(xx,xmin,dx,xlog);
          yy = VerifyLog(yy,ymin,dy,ylog);
          v.push_back(SbVec3f(xx,yy,aZZ+zz));
        }
        contourVector.push_back(v);
      }
    }

    SbTessContour tessContour;
    std::vector<SbTessTriangle> tris = tessContour.getFilledArea(contourVector);

    if(!tris.size()) continue;

    SbColor sbColor;
    if(aPainting==SbPaintingByValue) {
      sbColor = aCmap.getColor(val);
    } else if(aPainting==SbPaintingByLevel) {
      int icol = aCmap.colorn() ? (i % aCmap.colorn()) :0;
      sbColor = aCmap.color(icol);
    } else {
      sbColor = aStyle.color.getValue();
    }

    SoMaterial* material = fStyleCache->getMaterial(sbColor,aStyle.transparency.getValue());
    sep->addChild(material);

    SbVec3f* points = new SbVec3f[tris.size()*3];
    int pointi = 0;
    int32_t* coordIndex = new int32_t[tris.size()*4];
    int coordi = 0;
    for(unsigned itri=0;itri<tris.size();itri++) {
      SbTessTriangle& tri = tris[itri];

     {float xx = (float)tri.pointA[0];
      float yy = (float)tri.pointA[1];
      float zz = (float)tri.pointA[2];
      LIST_SET(points,pointi,xx,yy,zz);
      coordIndex[coordi] = icoord + pointi;
      coordi++;
      pointi++;}

     {float xx = (float)tri.pointB[0];
      float yy = (float)tri.pointB[1];
      float zz = (float)tri.pointB[2];
      LIST_SET(points,pointi,xx,yy,zz);
      coordIndex[coordi] = icoord + pointi;
      coordi++;
      pointi++;}

     {float xx = (float)tri.pointC[0];
      float yy = (float)tri.pointC[1];
      float zz = (float)tri.pointC[2];
      LIST_SET(points,pointi,xx,yy,zz);
      coordIndex[coordi] = icoord + pointi;
      coordi++;
      pointi++;}

      coordIndex[coordi] = SO_END_FACE_INDEX;
      coordi++;
    }

    coordinate3->point.setValues(icoord,pointi,points);
    icoord += pointi;

    SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
    sep->addChild(indexedFaceSet);
    indexedFaceSet->coordIndex.setValues(0,coordi,coordIndex);

    delete [] points;
    delete [] coordIndex;

    empty = FALSE;

  }

  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repErrorsI_xy(SoSeparator* aParent,SoStyle& aStyle,const std::vector<SbRepBin1D>& aBins,
                              const SbRepBox& aBoxX,const SbRepBox& aBoxY,
                              const std::vector<float>& aBars,float aZ,const SbString& aID) {
  SoSceneGraph* separator = new SoSceneGraph();
  separator->setString(aID);

  separator->addChild(getLineStyle(aStyle));

  SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
  separator->addChild(material);
            
  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SoIndexedLineSet* lineSet = new SoIndexedLineSet;
  separator->addChild(lineSet);
  int iindex = 0;

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  int32_t coordIndex[7];
  SbBool empty = TRUE;
  int xnbin = aBins.size();
  for(int index=0;index<xnbin;index++) { 
          
    //Need all bins modeled for fitting.
                  
    float val = aBins[index].fVal;
    float bar_height = aBars[index];
                          
    float bar_min = val - bar_height/2;
    float bar_max = val + bar_height/2;
                          
    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    val = VerifyLog(val,ymin,dy,ylog);
          
    bar_min = VerifyLog(bar_min,ymin,dy,ylog);
    bar_max = VerifyLog(bar_max,ymin,dy,ylog);

    if(xe<0) continue;
    if(xx>1) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(val<0) val = 0;
    if(val>1) val = 1;

    float ex = (xe+xx)/2;
    //if( (ex >=0)&&(ex <=1) ) { //FIXME : have to clip

    float edx = 0.3F * (xe-xx);
    int ipt = 0;
    SbVec3f points[6];
    if(bar_min >1)  {
      // do nothing
    } else  if(bar_max <0)  {
      // do nothing
    } else  if(bar_min <0) { 
      if(bar_max >1) { 
        LIST_SET(points,ipt,ex,0,aZ);ipt++;
        LIST_SET(points,ipt,ex,1,aZ);ipt++;
      } else { 
        LIST_SET(points,ipt,ex,0,aZ);ipt++;
        LIST_SET(points,ipt,ex,bar_max,aZ);ipt++;
        LIST_SET(points,ipt,ex-edx,bar_max,aZ);ipt++;
        LIST_SET(points,ipt,ex+edx,bar_max,aZ);ipt++;
      }
    } else  if(bar_max >1) { 
      LIST_SET(points,ipt,ex-edx,bar_min,aZ);ipt++;
      LIST_SET(points,ipt,ex+edx,bar_min,aZ);ipt++;
      LIST_SET(points,ipt,ex,bar_min,aZ);ipt++;
      LIST_SET(points,ipt,ex,1,aZ);ipt++;
    } else {
      LIST_SET(points,ipt,ex-edx,bar_min,aZ);ipt++;
      LIST_SET(points,ipt,ex+edx,bar_min,aZ);ipt++;
      LIST_SET(points,ipt,ex    ,bar_min,aZ);ipt++;
      LIST_SET(points,ipt,ex    ,bar_max,aZ);ipt++;
      LIST_SET(points,ipt,ex-edx,bar_max,aZ);ipt++;
      LIST_SET(points,ipt,ex+edx,bar_max,aZ);ipt++;
    }

    if(ipt<=0) continue;

    for(int i=0;i<ipt;i++) coordIndex[i] = icoord + i;
    coordIndex[ipt] = SO_END_LINE_INDEX;

    coordinate3->point.setValues(icoord,ipt,points);
    icoord += ipt;

    lineSet->coordIndex.setValues(iindex,ipt+1,coordIndex);
    iindex += (ipt+1);

    empty = FALSE;
  }          
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repErrorsPlus_xy(SoSeparator* aParent,SoStyle& aStyle,const std::vector<SbRepBin1D>& aBins,
                                 const SbRepBox& aBoxX,const SbRepBox& aBoxY,
                                 const std::vector<float>& aBars,float aZ,const SbString& aID) {
  SoSceneGraph* separator = new SoSceneGraph();
  separator->setString(aID);

  separator->addChild(getLineStyle(aStyle));

  SoMaterial* material = fStyleCache->getMaterial(aStyle.color.getValue(),aStyle.transparency.getValue());
  separator->addChild(material);
            
  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SoIndexedLineSet* lineSet = new SoIndexedLineSet;
  separator->addChild(lineSet);
  int iindex = 0;

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  int32_t coordIndex[7];
  SbBool empty = TRUE;
  int xnbin = aBins.size();
  for(int index=0;index<xnbin;index++) { 
          
    //Need all bins modeled for fitting.
    //if(binsloop->getBinNumberOfEntries(index)<=0) continue;
                  
    float val = aBins[index].fVal;
    float bar_height = aBars[index];
                          
    float bar_min = val - bar_height/2;
    float bar_max = val + bar_height/2;
                          
    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    val = VerifyLog(val,ymin,dy,ylog);
          
    bar_min = VerifyLog(bar_min,ymin,dy,ylog);
    bar_max = VerifyLog(bar_max,ymin,dy,ylog);

    if(xe<0) continue;
    if(xx>1) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(val<0) val = 0;
    if(val>1) val = 1;

    float ex = (xe+xx)/2;
    //if( (ex >=0)&&(ex <=1) ) //FIXME : have to clip
    //float edx = 0.3F * (xe-xx);

    int ipt = 0;
    SbVec3f points[6];
    if(bar_min >1)  {
      // do nothing
    } else  if(bar_max <0)  {
      // do nothing
    } else  if(bar_min <0) { 
      if(bar_max >1) { 
        LIST_SET(points,ipt,ex,0,aZ);ipt++;
        LIST_SET(points,ipt,ex,1,aZ);ipt++;
      } else { 
        LIST_SET(points,ipt,ex,0,aZ);ipt++;
        LIST_SET(points,ipt,ex,bar_max,aZ);ipt++;
      }
    } else  if(bar_max>1) { 
      LIST_SET(points,ipt,ex,bar_min,aZ);ipt++;
      LIST_SET(points,ipt,ex,1,aZ);ipt++;
    } else {
      LIST_SET(points,ipt,ex    ,bar_min,aZ);ipt++;
      LIST_SET(points,ipt,ex    ,bar_max,aZ);ipt++;
    }

    if(ipt<=0) continue;

    for(int i=0;i<ipt;i++) coordIndex[i] = icoord + i;
    coordIndex[ipt] = SO_END_LINE_INDEX;

    coordinate3->point.setValues(icoord,ipt,points);
    icoord += ipt;

    lineSet->coordIndex.setValues(iindex,ipt+1,coordIndex);
    iindex += (ipt+1);

    // top of bin :
    ipt = 0;
    LIST_SET(points,ipt,xx,val,aZ);ipt++;
    LIST_SET(points,ipt,xe,val,aZ);ipt++;
    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 1;
    coordIndex[2] = SO_END_LINE_INDEX;

    coordinate3->point.setValues(icoord,ipt,points);
    icoord += ipt;
    lineSet->coordIndex.setValues(iindex,ipt+1,coordIndex);
    iindex += (ipt+1);

    empty = FALSE;
  }          
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repPoints2D_xy(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbVec3f>& aPoints
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,float aZ
,const SbString& aID
){
  SoSceneGraph* separator = new SoSceneGraph();
  separator->setString(aID);

  SoMaterial* material = 
    fStyleCache->getMaterial(aStyle.color.getValue(),
                             aStyle.transparency.getValue());
  int npoint = aPoints.size();

  SbVec3f points[1];

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  for(int index=0;index<npoint;index++) {
    float xx = aPoints[index][0];
    float yy = aPoints[index][1];

    xx = VerifyLog(xx,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);

    if((xx>=0)&&(xx<=1)&&(yy>=0)&&(yy<=1)) {
      SoSeparator* sep = new SoSeparator();
      separator->addChild(sep);
      LIST_SET(points,0,xx,yy,aZ);
      sep->addChild(fStyleCache->getPoints());
      sep->addChild(material);

      SoTools_addPointsToNode(sep,1,points,
        (SbMarkerStyle)aStyle.markerStyle.getValue(),
        aStyle.markerSize.getValue());
    }
  }

  if(separator->getNumChildren()) {
    aParent->addChild(separator);
  } else {
    separator->unref();
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// reps xyz //////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoPlotter::repBins2D_xyz_boxes(
 SoSeparator* aParent
,SoStyle& aStyle
,SbPaintingPolicy aPainting
,const SbBaseColormap& aCmap
,const std::vector<SbRepBin2D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,float aBmin
,float aBmax
,const SbString& aID
){
  //printf("debug : SoPlotter::repBins2D\n");

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float zmin = aBoxZ.fPos;
  float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  for(unsigned int index=0;index<aBins.size();index++) {

    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fYmin;
    float ye = aBins[index].fYmax;
    float zz,ze;
    float val = aBins[index].fVal;
    zz = aBmin;
    ze = val;

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);
    zz = VerifyLog(zz,zmin,dz,zlog);
    ze = VerifyLog(ze,zmin,dz,zlog);
                          
    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    if(zz>1) continue;
    if(ze<0) continue;
    if(zz<0) zz = 0;
    if(ze>1) ze = 1;
    if(yy>=ye) continue;
    if(xx>=xe) continue;
    if(zz>=ze) continue;

    SoSceneGraph* sep = new SoSceneGraph;
    aParent->addChild(sep);

   {char s[128];
    ::sprintf(s,"%d %d",aBins[index].fI,aBins[index].fJ);
    sep->setInfos(s);}
   {std::string sp;
    if(!inlib::p2sx(sep->getInfos(),sp)){}
    std::string sid(aID.getString());
    sid += "/"+sp;
    sep->setString(sid.c_str());}

    if(aStyle.lightModel.getValue()==SbLightModel_base_color) { //OpenPAW
      sep->addChild(fStyleCache->getLightModelBaseColor());
    }
          
    float sx = xe-xx;
    float sy = ye-yy;
    float sz = ze-zz;

    //FIXME : something better (SoPolyhedron) ?

    if(aPainting==SbPaintingByLevel) {

      int ncol = aCmap.colorn();
      if(ncol<=0) {
        //FIXME
      } else {

        SoSeparator* sp  = new SoSeparator();
        sep->addChild(sp);

        float zmx = aBmax;
        zmx = VerifyLog(zmx,zmin,dz,zlog);

        float dval = (aBmax-aBmin)/ncol;
        float d_z = (zmx-zz)/ncol;
        float d_z_prev = 0;

        sp->addChild(fStyleCache->getFilled());

        SoTransform* transform = new SoTransform;
        transform->translation.setValue(xx+sx/2,yy+sy/2,0);
        sp->addChild(transform);

        float cook = 0.98F;

        float a_z = 0;
        bool have_to_break = false;
        for(unsigned int i=0;i<ncol;i++){
          float v = aBmin + i * dval;
          if((v+dval)>=val) {
            v = val;
            d_z = ze-a_z;
            have_to_break = true;
          }

          if(i) {
            SoTransform* transform = new SoTransform;
            transform->translation.setValue(0,0,d_z_prev);
            sp->addChild(transform);
          }
  
          SbColor sbColor = aCmap.color(i);
          
          SoMaterial* material = fStyleCache->getMaterial(sbColor,aStyle.transparency.getValue());
          sp->addChild(material);
  
          SoSeparator* s_p  = new SoSeparator();
          sp->addChild(s_p);

          SoTransform* transform = new SoTransform;
          transform->translation.setValue(0,0,d_z/2);
          s_p->addChild(transform);

          SoCube* cube = new SoCube;
          cube->width.setValue(cook*sx);
          cube->height.setValue(cook*sy);
          cube->depth.setValue(d_z);
          s_p->addChild(cube);

          a_z += d_z;  
          d_z_prev = d_z;
          if(have_to_break) break;
        }

        if(aStyle.areaStyle.getValue()==SoStyle::EDGED) { //OpenPAW.
	  //::printf("debug : EDGED 2\n");

          SoTransform* transform = new SoTransform;
          transform->translation.setValue(xx+sx/2,yy+sy/2,sz/2);
          sep->addChild(transform);

          SoTools_addBoxCoords(*sep,cook*sx,cook*sy,sz); 

          sep->addChild(getLineStyle(aStyle));
          sep->addChild(fStyleCache->getMaterial(SbColor_black,0));

         {SoLineSet* lineSet = new SoLineSet;
          lineSet->numVertices.set1Value(0,4);
          lineSet->numVertices.set1Value(1,4);
          lineSet->numVertices.set1Value(2,4);
          lineSet->numVertices.set1Value(3,4);
          lineSet->numVertices.set1Value(4,4);
          lineSet->numVertices.set1Value(5,4);
          sep->addChild(lineSet);}

        }
      }
    } else {
      SoTransform* transform = new SoTransform;
      transform->translation.setValue(xx+sx/2,yy+sy/2,sz/2);
      sep->addChild(transform);

      SbColor sbColor;
      if(aPainting==SbPaintingByValue) {
        sbColor = aCmap.getColor(val);
      } else if( (aPainting==SbPaintingGreyScale) ||
                 (aPainting==SbPaintingGreyScaleInverse) ||
                 (aPainting==SbPaintingVioletToRed) ){
        sbColor = aCmap.getColor(aBins[index].fRatio);
      } else {
        sbColor = aStyle.color.getValue();
      }
                          
      if(aStyle.areaStyle.getValue()==SoStyle::EDGED) { //OpenPAW.
	//::printf("debug : EDGED 3\n");

        float cook = 0.98F;
        SoTools_addBoxCoords(*sep,cook*sx,cook*sy,sz); 

        SoMaterial* material = fStyleCache->getMaterial(sbColor,aStyle.transparency.getValue());
        sep->addChild(material);

        sep->addChild(fStyleCache->getFilled());
        sep->addChild(fStyleCache->getNormalBindingPerFace());

        SoFaceSet* faceSet = new SoFaceSet;
        faceSet->numVertices.set1Value(0,4);
        faceSet->numVertices.set1Value(1,4);
        faceSet->numVertices.set1Value(2,4);
        faceSet->numVertices.set1Value(3,4);
        faceSet->numVertices.set1Value(4,4);
        faceSet->numVertices.set1Value(5,4);
        sep->addChild(faceSet);

        sep->addChild(getLineStyle(aStyle));
        sep->addChild(fStyleCache->getMaterial(SbColor_black,0));

        sep->addChild(faceSet);

	//{SoLineSet* lineSet = new SoLineSet;
        //lineSet->numVertices.set1Value(0,4);
        //lineSet->numVertices.set1Value(1,4);
        //lineSet->numVertices.set1Value(2,4);
        //lineSet->numVertices.set1Value(3,4);
        //lineSet->numVertices.set1Value(4,4);
        //lineSet->numVertices.set1Value(5,4);
        //sep->addChild(lineSet);}

      } else {
        SoMaterial* material = fStyleCache->getMaterial(sbColor,aStyle.transparency.getValue());
        sep->addChild(material);

        SoCube* cube = new SoCube;
        cube->width.setValue(sx);
        cube->height.setValue(sy);
        cube->depth.setValue(sz);
        sep->addChild(cube);

      //SoPolyhedron* soPol = 
      //  new SoPolyhedron(SbPolyhedronBox(sx/2,sy/2,sz/2));
      //soPol->solid.setValue(TRUE);
      //sep->addChild(soPol);

      }

    }
  }
}

void SoPlotter::repBins2D_xyz_boxes_one(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepBin2D>& aBins
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,float aBmin
,float //aBmax
,const SbString& aID
){
  //printf("debug : SoPlotter::repBins2D_one\n");

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float zmin = aBoxZ.fPos;
  float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  SoSceneGraph* separator = new SoSceneGraph;
  separator->setString(aID);

  if(aStyle.lightModel.getValue()==SbLightModel_base_color) { //OpenPAW
    separator->addChild(fStyleCache->getLightModelBaseColor());
  }
  separator->addChild(fStyleCache->getFilled());
  separator->addChild(fStyleCache->getNormalBindingPerFace());

  //PaintingByLevel

  SoMaterial* material = 
    fStyleCache->getMaterial(aStyle.color.getValue(),
                             aStyle.transparency.getValue());
  separator->addChild(material);

  SoNormal* normal = new SoNormal;
  separator->addChild(normal);
  int inormal = 0;

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
  separator->addChild(indexedFaceSet);
  int iindex = 0;          

  SbVec3f line[8];
  SbVec3f normals[6];
  int32_t coordIndex[30];
  SbBool empty = TRUE;
  for(unsigned int index=0;index<aBins.size();index++) {

    float xx = aBins[index].fXmin;
    float xe = aBins[index].fXmax;
    float yy = aBins[index].fYmin;
    float ye = aBins[index].fYmax;
    float zz,ze;
   {float val = aBins[index].fVal;
    zz = aBmin;
    ze = val;}

    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);
    zz = VerifyLog(zz,zmin,dz,zlog);
    ze = VerifyLog(ze,zmin,dz,zlog);
                          
    // Clipping :
    if(xx>1) continue;
    if(xe<0) continue;
    if(xx<0) xx = 0;
    if(xe>1) xe = 1;
    if(yy>1) continue;
    if(ye<0) continue;
    if(yy<0) yy = 0;
    if(ye>1) ye = 1;

    if(zz>1) continue;
    if(ze<0) continue;
    if(zz<0) zz = 0;
    if(ze>1) ze = 1;
    if(yy>=ye) continue;
    if(xx>=xe) continue;
    if(zz>=ze) continue;

    line[0].setValue(xx,yy,zz);
    line[1].setValue(xe,yy,zz);
    line[2].setValue(xx,ye,zz);
    line[3].setValue(xe,ye,zz);
    line[4].setValue(xx,yy,ze);
    line[5].setValue(xe,yy,ze);
    line[6].setValue(xx,ye,ze);
    line[7].setValue(xe,ye,ze);

    // z back :
    normals[0].setValue(0,0,-1);
    coordIndex[0] = icoord + 0;
    coordIndex[1] = icoord + 2;
    coordIndex[2] = icoord + 3;
    coordIndex[3] = icoord + 1;
    coordIndex[4] = SO_END_FACE_INDEX;
    // y front :
    normals[1].setValue(0,1,0);
    coordIndex[5] = icoord + 2;
    coordIndex[6] = icoord + 6;
    coordIndex[7] = icoord + 7;
    coordIndex[8] = icoord + 3;
    coordIndex[9] = SO_END_FACE_INDEX;
    // x front :
    normals[2].setValue(1,0,0);
    coordIndex[10] = icoord + 1;
    coordIndex[11] = icoord + 3;
    coordIndex[12] = icoord + 7;
    coordIndex[13] = icoord + 5;
    coordIndex[14] = SO_END_FACE_INDEX;
    // y back :
    normals[3].setValue(0,-1,0);
    coordIndex[15] = icoord + 0;
    coordIndex[16] = icoord + 1;
    coordIndex[17] = icoord + 5;
    coordIndex[18] = icoord + 4;
    coordIndex[19] = SO_END_FACE_INDEX;
    // x back :
    normals[4].setValue(-1,0,0);
    coordIndex[20] = icoord + 0;
    coordIndex[21] = icoord + 4;
    coordIndex[22] = icoord + 6;
    coordIndex[23] = icoord + 2;
    coordIndex[24] = SO_END_FACE_INDEX;
    // z front :
    normals[5].setValue(0,0,1);
    coordIndex[25] = icoord + 4;
    coordIndex[26] = icoord + 5;
    coordIndex[27] = icoord + 7;
    coordIndex[28] = icoord + 6;
    coordIndex[29] = SO_END_FACE_INDEX;

    coordinate3->point.setValues(icoord,8,line);
    icoord += 8;

    normal->vector.setValues(inormal,6,normals);
    inormal += 6;

    indexedFaceSet->coordIndex.setValues(iindex,30,coordIndex);
    iindex += 30;
    empty = FALSE;
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repPoints3D_xyz(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbVec3f>& aPoints
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,const SbString& aID
){
  SoSceneGraph* separator = new SoSceneGraph();
  separator->setString(aID);

  SoMaterial* material = 
    fStyleCache->getMaterial(aStyle.color.getValue(),
                             aStyle.transparency.getValue());
  int npoint = aPoints.size();

  SbVec3f points[1];

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float zmin = aBoxZ.fPos;
  float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  for(int index=0;index<npoint;index++) {
    float xx = aPoints[index][0];
    float yy = aPoints[index][1];
    float zz = aPoints[index][2];

    xx = VerifyLog(xx,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    zz = VerifyLog(zz,zmin,dz,zlog);

    if( (xx>=0)&&(xx<=1) && 
        (yy>=0)&&(yy<=1) &&
        (zz>=0)&&(zz<=1)
        ) {
      //FIXME : picking.
      SoSeparator* sep  = new SoSeparator();
      separator->addChild(sep);
      LIST_SET(points,0,xx,yy,zz);
      sep->addChild(fStyleCache->getPoints());
      sep->addChild(material);
      SoTools_addPointsToNode(sep,1,points,
        (SbMarkerStyle)aStyle.markerStyle.getValue(),
        aStyle.markerSize.getValue());
    }
  }

  if(separator->getNumChildren()) {
    aParent->addChild(separator);
  } else {
    separator->unref();
  }
}

void SoPlotter::repTopFaces2D_xyz(
 SoSeparator* aParent
,SoStyle& aStyle
,SbPaintingPolicy aPainting
,const SbBaseColormap& aCmap
,const std::vector<SbRepTopFace2D>& aTopFaces
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,const SbString& aID
){
  //printf("debug : SoPlotter::repTopFaces2D_xyz\n");

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float zmin = aBoxZ.fPos;
  float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  SbBool empty = TRUE;
        
  SoSceneGraph* separator = new SoSceneGraph();
  separator->setString(aID);

  if(aStyle.lightModel.getValue()==SbLightModel_base_color) { //OpenPAW
    separator->addChild(fStyleCache->getLightModelBaseColor());
  }

  separator->addChild(fStyleCache->getNormalBindingPerFaceIndexed());

  SoNormal* normal = new SoNormal;
  separator->addChild(normal);
  int inormal = 0;

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SbVec3f line[3];
  int32_t coordIndex[4];

  bool use_same = true;
  float epsilon = 0.08f;

  int number = aTopFaces.size();
  for(int index=0;index<number;index++) {
    float xx = aTopFaces[index].fXmin;
    float xe = aTopFaces[index].fXmax;
    float yy = aTopFaces[index].fYmin;
    float ye = aTopFaces[index].fYmax;
    float val1 = aTopFaces[index].fVal1;
    float val2 = aTopFaces[index].fVal2;
    float val3 = aTopFaces[index].fVal3;
    float val4 = aTopFaces[index].fVal4;

    float val = val1;

    val1 = VerifyLog(val1,zmin,dz,zlog);
    val2 = VerifyLog(val2,zmin,dz,zlog);
    val3 = VerifyLog(val3,zmin,dz,zlog);
    val4 = VerifyLog(val4,zmin,dz,zlog);
    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);
            
    if(val1<0) val1 = 0;
    if(val1>1) val1 = 1;

    if(val2<0) val2 = 0;
    if(val2>1) val2 = 1;

    if(val3<0) val3 = 0;
    if(val3>1) val3 = 1;

    if(val4<0) val4 = 0;
    if(val4>1) val4 = 1;

    if((xx>=0)&&(xx<=1)   &&
       (xe>=0)&&(xe<=1)   &&
       (yy>=0)&&(yy<=1)   &&
       (ye>=0)&&(ye<=1)  ) {

      SbColor sbColor;
      if(aPainting==SbPaintingByValue) {
        float v = (zlog==TRUE?TakeLog(val):val);
        sbColor = aCmap.getColor(v);
      } else if(aPainting==SbPaintingByLevel) {
        // Should not have to pass here !
      } else {
        sbColor = aStyle.color.getValue();
      }

      SoSeparator* sep = new SoSeparator;
      separator->addChild(sep);

      SoMaterial* material = fStyleCache->getMaterial(sbColor,aStyle.transparency.getValue());
      sep->addChild(material);
      sep->addChild(fStyleCache->getFilled());

      SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
      sep->addChild(indexedFaceSet);

      SoIndexedLineSet* indexedLineSet = 0;
      if(aStyle.areaStyle.getValue()==SoStyle::EDGED) { //OpenPAW.
	//::printf("debug : EDGED 4\n");
        //Do not reuse the face set to avoid drawing the v2-v4 line.
        sep->addChild(getLineStyle(aStyle));
        sep->addChild(fStyleCache->getMaterial(SbColor_black,0));
        //sep->addChild(indexedFaceSet);
        indexedLineSet = new SoIndexedLineSet;
        sep->addChild(indexedLineSet);
      }

      int i_coord_face = 0;
      int i_coord_line = 0;
      int i_ni = 0;

      //////////////////////////////////////
      //////////////////////////////////////
     {line[0].setValue(xx,ye,val4);
      line[1].setValue(xx,yy,val1);
      line[2].setValue(xe,yy,val2);
      coordinate3->point.setValues(icoord,3,line);
      coordIndex[0] = icoord + 0;
      coordIndex[1] = icoord + 1;
      coordIndex[2] = icoord + 2;
      coordIndex[3] = SO_END_FACE_INDEX;
      indexedFaceSet->coordIndex.setValues(i_coord_face,4,coordIndex);
      icoord += 3;
      i_coord_face += 4;

      if(indexedLineSet) {
        if(use_same) {
          indexedLineSet->coordIndex.setValues(i_coord_line,4,coordIndex);
          i_coord_line += 4;
        } else {
          line[0].setValue(xx,ye,val4+epsilon);
          line[1].setValue(xx,yy,val1+epsilon);
          line[2].setValue(xe,yy,val2+epsilon);
          coordinate3->point.setValues(icoord,3,line);
          coordIndex[0] = icoord + 0;
          coordIndex[1] = icoord + 1;
          coordIndex[2] = icoord + 2;
          coordIndex[3] = SO_END_LINE_INDEX;
          indexedLineSet->coordIndex.setValues(i_coord_line,4,coordIndex);
          icoord += 3;
          i_coord_line += 4;
	}
      }
      
      SbPlane plane1(SbVec3f(xx,ye,val4),SbVec3f(xx,yy,val1),SbVec3f(xe,yy,val2));
      SbVec3f norm1 = plane1.getNormal();

      indexedFaceSet->normalIndex.set1Value(i_ni,inormal);
      i_ni++;

      normal->vector.set1Value(inormal,norm1);
      inormal++;}

      //////////////////////////////////////
      //////////////////////////////////////
     {line[0].setValue(xe,yy,val2);
      line[1].setValue(xe,ye,val3);
      line[2].setValue(xx,ye,val4);
      coordinate3->point.setValues(icoord,3,line);
      coordIndex[0] = icoord + 0;
      coordIndex[1] = icoord + 1;
      coordIndex[2] = icoord + 2;
      coordIndex[3] = SO_END_FACE_INDEX;
      indexedFaceSet->coordIndex.setValues(i_coord_face,4,coordIndex);
      icoord += 3;
      i_coord_face += 4;

      if(indexedLineSet) {
        if(use_same) {
          indexedLineSet->coordIndex.setValues(i_coord_line,4,coordIndex);
          i_coord_line += 4;
        } else {
          line[0].setValue(xe,yy,val2+epsilon);
          line[1].setValue(xe,ye,val3+epsilon);
          line[2].setValue(xx,ye,val4+epsilon);
          coordinate3->point.setValues(icoord,3,line);
          coordIndex[0] = icoord + 0;
          coordIndex[1] = icoord + 1;
          coordIndex[2] = icoord + 2;
          coordIndex[3] = SO_END_LINE_INDEX;
          indexedLineSet->coordIndex.setValues(i_coord_line,4,coordIndex);
          icoord += 3;
          i_coord_line += 4;
	}
      }
      
      SbPlane plane2(SbVec3f(xe,yy,val2),SbVec3f(xe,ye,val3),SbVec3f(xx,ye,val4));
      SbVec3f norm2 = plane2.getNormal();

      indexedFaceSet->normalIndex.set1Value(i_ni,inormal);
      i_ni++;

      normal->vector.set1Value(inormal,norm2);
      inormal++;}

      empty = FALSE;

    }              
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repTopFaces2D_xyz_line(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepTopFace2D>& aTopFaces
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,const SbString& aID
){
  //printf("debug : SoPlotter::repTopFaces2D_xyz\n");

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float zmin = aBoxZ.fPos;
  float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  SbBool empty = TRUE;
        
  SoSceneGraph* separator = new SoSceneGraph();
  separator->setString(aID);

  if(aStyle.lightModel.getValue()==SbLightModel_base_color) { //OpenPAW
    separator->addChild(fStyleCache->getLightModelBaseColor());
  }

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SbVec3f line[3];
  int32_t coordIndex[4];

  int number = aTopFaces.size();
  for(int index=0;index<number;index++) {
    float xx = aTopFaces[index].fXmin;
    float xe = aTopFaces[index].fXmax;
    float yy = aTopFaces[index].fYmin;
    float ye = aTopFaces[index].fYmax;
    float val1 = aTopFaces[index].fVal1;
    float val2 = aTopFaces[index].fVal2;
    float val3 = aTopFaces[index].fVal3;
    float val4 = aTopFaces[index].fVal4;

    float val = val1;

    val1 = VerifyLog(val1,zmin,dz,zlog);
    val2 = VerifyLog(val2,zmin,dz,zlog);
    val3 = VerifyLog(val3,zmin,dz,zlog);
    val4 = VerifyLog(val4,zmin,dz,zlog);
    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);
            
    if(val1<0) val1 = 0;
    if(val1>1) val1 = 1;

    if(val2<0) val2 = 0;
    if(val2>1) val2 = 1;

    if(val3<0) val3 = 0;
    if(val3>1) val3 = 1;

    if(val4<0) val4 = 0;
    if(val4>1) val4 = 1;

    if((xx>=0)&&(xx<=1)   &&
       (xe>=0)&&(xe<=1)   &&
       (yy>=0)&&(yy<=1)   &&
       (ye>=0)&&(ye<=1)  ) {

      SoSeparator* sep = new SoSeparator;
      separator->addChild(sep);

      SoIndexedLineSet* indexedLineSet = 0;
      sep->addChild(getLineStyle(aStyle));
      sep->addChild(fStyleCache->getMaterial(SbColor_black,0));
      indexedLineSet = new SoIndexedLineSet;
      sep->addChild(indexedLineSet);

      int i_coord = 0;
      int i_ni = 0;

      //////////////////////////////////////
      //////////////////////////////////////
      line[0].setValue(xx,ye,val4);
      line[1].setValue(xx,yy,val1);
      line[2].setValue(xe,yy,val2);
      coordinate3->point.setValues(icoord,3,line);
      coordIndex[0] = icoord + 0;
      coordIndex[1] = icoord + 1;
      coordIndex[2] = icoord + 2;
      coordIndex[3] = SO_END_LINE_INDEX;
      indexedLineSet->coordIndex.setValues(i_coord,4,coordIndex);
      icoord += 3;
      i_coord += 4;

      //////////////////////////////////////
      //////////////////////////////////////
      line[0].setValue(xe,yy,val2);
      line[1].setValue(xe,ye,val3);
      line[2].setValue(xx,ye,val4);
      coordinate3->point.setValues(icoord,3,line);
      coordIndex[0] = icoord + 0;
      coordIndex[1] = icoord + 1;
      coordIndex[2] = icoord + 2;
      coordIndex[3] = SO_END_LINE_INDEX;
      indexedLineSet->coordIndex.setValues(i_coord,4,coordIndex);
      icoord += 3;
      i_coord += 4;

      empty = FALSE;

    }              
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repTopFaces2D_xyz_by_level(
 SoSeparator* aParent
,SoStyle& aStyle
,SbPaintingPolicy aPainting
,const SbBaseColormap& aCmap
,const std::vector<SbRepTopFace2D>& aTopFaces
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,float aBmin
,float aBmax
,const SbString& aID
){
  //printf("debug : SoPlotter::repTopFaces2D_xyz_by_level\n");

  int ncol = aCmap.colorn();
  if(ncol<=0) return;

  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float zmin = aBoxZ.fPos;
  float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  float zz = aBmin;
  zz = VerifyLog(zz,zmin,dz,zlog);
  float zmx = aBmax;
  zmx = VerifyLog(zmx,zmin,dz,zlog);

  SbBool empty = TRUE;
        
  SoSceneGraph* separator = new SoSceneGraph();
  separator->setString(aID);

  if(aStyle.lightModel.getValue()==SbLightModel_base_color) { //OpenPAW
    separator->addChild(fStyleCache->getLightModelBaseColor());
  }

  separator->addChild(fStyleCache->getNormalBindingPerFaceIndexed());

  SoNormal* normal = new SoNormal;
  separator->addChild(normal);
  int inormal = 0;

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  float d_z = (zmx-zz)/ncol;

  int number = aTopFaces.size();

  for(int icol=0;icol<ncol;icol++) {

    SoSeparator* sep = new SoSeparator;

    SbBool sep_empty = TRUE;

    SoMaterial* material = 
      fStyleCache->getMaterial
        (aCmap.color(icol),aStyle.transparency.getValue());
    sep->addChild(material);
    sep->addChild(fStyleCache->getFilled());

    SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
    sep->addChild(indexedFaceSet);

    int i_coord = 0;
    int i_ni = 0;

    for(int index=0;index<number;index++) {
      float xx = aTopFaces[index].fXmin;
      float xe = aTopFaces[index].fXmax;
      float yy = aTopFaces[index].fYmin;
      float ye = aTopFaces[index].fYmax;
      float val1 = aTopFaces[index].fVal1;
      float val2 = aTopFaces[index].fVal2;
      float val3 = aTopFaces[index].fVal3;
      float val4 = aTopFaces[index].fVal4;
  
      float val = val1;
  
      val1 = VerifyLog(val1,zmin,dz,zlog);
      val2 = VerifyLog(val2,zmin,dz,zlog);
      val3 = VerifyLog(val3,zmin,dz,zlog);
      val4 = VerifyLog(val4,zmin,dz,zlog);
      xx = VerifyLog(xx,xmin,dx,xlog);
      xe = VerifyLog(xe,xmin,dx,xlog);
      yy = VerifyLog(yy,ymin,dy,ylog);
      ye = VerifyLog(ye,ymin,dy,ylog);
              
      if(val1<0) val1 = 0;
      if(val1>1) val1 = 1;
  
      if(val2<0) val2 = 0;
      if(val2>1) val2 = 1;
  
      if(val3<0) val3 = 0;
      if(val3>1) val3 = 1;
  
      if(val4<0) val4 = 0;
      if(val4>1) val4 = 1;
  
      if((xx>=0)&&(xx<=1)   &&
         (xe>=0)&&(xe<=1)   &&
         (yy>=0)&&(yy<=1)   &&
         (ye>=0)&&(ye<=1)  ) {
  
        //////////////////////////////////////
        //////////////////////////////////////
       {SbClip clipper;
        clipper.addVertex(SbVec3f(xx,ye,val4));
        clipper.addVertex(SbVec3f(xx,yy,val1));
        clipper.addVertex(SbVec3f(xe,yy,val2));
        //val[n] had been z normalized.
        float z1 = zz+d_z*icol;
        float z2 = z1+d_z;
        SbPlane plane_z_bot(SbVec3f(0,0,1),SbVec3f(0,0,z1));
        SbPlane plane_z_top(SbVec3f(0,0,-1),SbVec3f(0,0,z2));
        clipper.clip(plane_z_bot);
        clipper.clip(plane_z_top);
      
        int npt = clipper.getNumVertices();
        if(npt>0) {
          SbVec3f* line = new SbVec3f[npt];
          int coordIndexn = npt+1;
          int32_t* coordIndex = new int32_t[coordIndexn];
          for(int index=0;index<npt;index++) {
            clipper.getVertex(index,line[index]);
            coordIndex[index] = icoord + index;
          }
          coordIndex[npt] = SO_END_FACE_INDEX;
          coordinate3->point.setValues(icoord,npt,line);
          icoord += npt;  
          delete [] line;
  
          indexedFaceSet->coordIndex.setValues(i_coord,coordIndexn,coordIndex);
          i_coord += coordIndexn;
          delete [] coordIndex;
      
          SbPlane plane1(SbVec3f(xx,ye,val4),SbVec3f(xx,yy,val1),SbVec3f(xe,yy,val2));
          SbVec3f norm1 = plane1.getNormal();
      
          indexedFaceSet->normalIndex.set1Value(i_ni,inormal);
          i_ni++;
      
          normal->vector.set1Value(inormal,norm1);
          inormal++;
      
          empty = FALSE;
          sep_empty = FALSE;
        }}

        //////////////////////////////////////
        //////////////////////////////////////
       {SbClip clipper;
        clipper.addVertex(SbVec3f(xe,yy,val2));
        clipper.addVertex(SbVec3f(xe,ye,val3));
        clipper.addVertex(SbVec3f(xx,ye,val4));
        //val[n] had been z normalized.
        float z1 = zz+d_z*icol;
        float z2 = z1+d_z;
        SbPlane plane_z_bot(SbVec3f(0,0,1),SbVec3f(0,0,z1));
        SbPlane plane_z_top(SbVec3f(0,0,-1),SbVec3f(0,0,z2));
        clipper.clip(plane_z_bot);
        clipper.clip(plane_z_top);
  
        int npt = clipper.getNumVertices();
        if(npt>0) {
          SbVec3f* line = new SbVec3f[npt];
          int coordIndexn = npt+1;
          int32_t* coordIndex = new int32_t[coordIndexn];
          for(int index=0;index<npt;index++) {
            clipper.getVertex(index,line[index]);
            coordIndex[index] = icoord + index;
          }
          coordIndex[npt] = SO_END_FACE_INDEX;
          coordinate3->point.setValues(icoord,npt,line);
          icoord += npt;  
          delete [] line;
      
          indexedFaceSet->coordIndex.setValues(i_coord,coordIndexn,coordIndex);
          i_coord += coordIndexn;
          delete [] coordIndex;
      
          SbPlane plane2(SbVec3f(xe,yy,val2),
                         SbVec3f(xe,ye,val3),
                         SbVec3f(xx,ye,val4));
          SbVec3f norm2 = plane2.getNormal();
      
          indexedFaceSet->normalIndex.set1Value(i_ni,inormal);
          i_ni++;
      
          normal->vector.set1Value(inormal,norm2);
          inormal++;  
      
          empty = FALSE;
          sep_empty = FALSE;
        }}
        
      }              

    } //index faces

    if(sep_empty==TRUE) {
      sep->unref();
    } else {
      separator->addChild(sep);
    }

  } //icol

  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

void SoPlotter::repTopFaces2D_xyz_one(
 SoSeparator* aParent
,SoStyle& aStyle
,const std::vector<SbRepTopFace2D>& aTopFaces
,const SbRepBox& aBoxX
,const SbRepBox& aBoxY
,const SbRepBox& aBoxZ
,const SbString& aID
){
  float xmin = aBoxX.fPos;
  float dx = aBoxX.fWidth;
  SbBool xlog = aBoxX.fLog;

  float ymin = aBoxY.fPos;
  float dy = aBoxY.fWidth;
  SbBool ylog = aBoxY.fLog;

  float zmin = aBoxZ.fPos;
  float dz = aBoxZ.fWidth;
  SbBool zlog = aBoxZ.fLog;

  SbBool empty = TRUE;
        
  SoSceneGraph* separator = new SoSceneGraph();
  separator->setString(aID);

  if(aStyle.lightModel.getValue()==SbLightModel_base_color) { //OpenPAW
    separator->addChild(fStyleCache->getLightModelBaseColor());
  }
  separator->addChild(fStyleCache->getFilled());
  separator->addChild(fStyleCache->getNormalBindingPerFace());
        
  SoMaterial* material = 
    fStyleCache->getMaterial(aStyle.color.getValue(),
                             aStyle.transparency.getValue());
  separator->addChild(material);

  SoNormal* normal = new SoNormal;
  separator->addChild(normal);
  int inormal = 0;

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;

  SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
  separator->addChild(indexedFaceSet);
  int iindex = 0;

  SbVec3f line[3];
  int32_t coordIndex[4];
  int number = aTopFaces.size();
  for(int index=0;index<number;index++) {
    float xx = aTopFaces[index].fXmin;
    float xe = aTopFaces[index].fXmax;
    float yy = aTopFaces[index].fYmin;
    float ye = aTopFaces[index].fYmax;
    float val1 = aTopFaces[index].fVal1;
    float val2 = aTopFaces[index].fVal2;
    float val3 = aTopFaces[index].fVal3;
    float val4 = aTopFaces[index].fVal4;

    val1 = VerifyLog(val1,zmin,dz,zlog);
    val2 = VerifyLog(val2,zmin,dz,zlog);
    val3 = VerifyLog(val3,zmin,dz,zlog);
    val4 = VerifyLog(val4,zmin,dz,zlog);
    xx = VerifyLog(xx,xmin,dx,xlog);
    xe = VerifyLog(xe,xmin,dx,xlog);
    yy = VerifyLog(yy,ymin,dy,ylog);
    ye = VerifyLog(ye,ymin,dy,ylog);
            
    if(val1<0) val1 = 0;
    if(val1>1) val1 = 1;

    if(val2<0) val2 = 0;
    if(val2>1) val2 = 1;

    if(val3<0) val3 = 0;
    if(val3>1) val3 = 1;

    if(val4<0) val4 = 0;
    if(val4>1) val4 = 1;

    if((xx>=0)&&(xx<=1)   &&
       (xe>=0)&&(xe<=1)   &&
       (yy>=0)&&(yy<=1)   &&
       (ye>=0)&&(ye<=1)  ) {
      SbPlane plane1(SbVec3f(xx,yy,val1),
                     SbVec3f(xe,yy,val2),
                     SbVec3f(xx,ye,val4));
      SbVec3f norm1 = plane1.getNormal();

      line[0].setValue(xx,yy,val1);
      line[1].setValue(xe,yy,val2);
      line[2].setValue(xx,ye,val4);

      coordIndex[0] = icoord + 0;
      coordIndex[1] = icoord + 1;
      coordIndex[2] = icoord + 2;
      coordIndex[3] = SO_END_FACE_INDEX;

      coordinate3->point.setValues(icoord,3,line);
      icoord += 3;  

      normal->vector.set1Value(inormal,norm1);
      inormal++;  

      indexedFaceSet->coordIndex.setValues(iindex,4,coordIndex);
      iindex += 4;

      SbPlane plane2(SbVec3f(xe,yy,val2),
                     SbVec3f(xe,ye,val3),
                     SbVec3f(xx,ye,val4));

      SbVec3f norm2 = plane2.getNormal();

      line[0].setValue(xe,yy,val2);
      line[1].setValue(xe,ye,val3);
      line[2].setValue(xx,ye,val4);

      coordIndex[0] = icoord + 0;
      coordIndex[1] = icoord + 1;
      coordIndex[2] = icoord + 2;
      coordIndex[3] = SO_END_FACE_INDEX;

      coordinate3->point.setValues(icoord,3,line);
      icoord += 3;  

      normal->vector.set1Value(inormal,norm2);
      inormal++;  

      indexedFaceSet->coordIndex.setValues(iindex,4,coordIndex);
      iindex += 4;

      empty = FALSE;
    }              
  }
  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent->addChild(separator);
  }
}

//////////////////////////////////////////////////////////////////////////////
/// Style helpers ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoPlotter::resetStyles(){
  float xfac = 1.0F/20.0F; //0.05
  float yfac = 1.0F/20.0F; //0.05

  // Take PAW defaults :
  float XSIZ = 20 * xfac;    //1     //page width
  float YSIZ = 20 * yfac;    //1     //page height
  float XMGL = 2 * xfac;     //0.1   //left x margin (to data frame).
  float XMGR = 2 * xfac;     //0.1   //right y margin (to data frame).
  float YMGL = 2 * yfac;     //0.1   //low y margin (to data frame).
  float YMGU = 2 * yfac;     //0.1   //up y margin (to data frame).
  // Axes :
  //float VSIZ = 0.28F * yfac; //0.014 //tick label character size.
  //float XVAL = 0.4F * xfac;  //0.02 //x distance of y tick label to data frame.
  //float YVAL = 0.4F * yfac;  //0.02 //y distance of x tick label to data frame.
  //float XTIC = 0.3F * yfac;  //0.015 //y length of X axis ticks.
  //float YTIC = 0.3F * xfac;  //0.015 //x length of Y axis ticks.
  //float XLAB = 1.4F * xfac;  //0.07  //x distance of y title to data frame.
  //float YLAB = 0.8F * yfac;  //0.04  //y distance of x title to data frame.
  //float ASIZ = 0.28F * yfac; //0.014 // axis title (label) character size.

  float YHTI = 1.2F * yfac;  //0.06  //y distance of title to x axis.
  float TSIZ = 0.28F * yfac; //0.014 //title character size

  // Data area :
  //float wData = XSIZ-XMGL-XMGR;
  //float hData = YSIZ-YMGL-YMGU;

  width.setValue(XSIZ);
  height.setValue(YSIZ);
  leftMargin.setValue(XMGL);
  rightMargin.setValue(XMGR);
  bottomMargin.setValue(YMGL);
  topMargin.setValue(YMGU);
  depth.setValue(1);
  titleUp.setValue(TRUE);
  titleToAxis.setValue(YHTI);
  titleHeight.setValue(TSIZ);
  titleAutomated.setValue(TRUE);
  //title.setValue("");
  //superposeBins.setValue(FALSE);
  colorMappingVisible.setValue(FALSE);
  
  // Is it part of the style ?
  //xAxisLogScale.setValue(FALSE);
  //yAxisLogScale.setValue(FALSE);
  //zAxisLogScale.setValue(FALSE);
  //xAxisAutomated.setValue(TRUE);
  //xAxisMinimum.setValue(0);
  //xAxisMaximum.setValue(1);
  //yAxisAutomated.setValue(TRUE);
  //yAxisMinimum.setValue(0);
  //yAxisMaximum.setValue(1);
  //zAxisAutomated.setValue(TRUE);
  //zAxisMinimum.setValue(0);
  //zAxisMaximum.setValue(1);

  numberOfLevels.setValue(10);
  topAxisVisible.setValue(FALSE);
  rightAxisVisible.setValue(FALSE);

  shapeAutomated.setValue(TRUE);

  setPAW_AxesTextSize(XSIZ,YSIZ);

  setPart("wallStyle",fDefault__wallStyle->copy());
  setPart("gridStyle",fDefault__gridStyle->copy());
  setPart("innerFrameStyle",fDefault__innerFrameStyle->copy());
  setPart("infosStyle",fDefault__infosStyle->copy());
  setPart("titleStyle",fDefault__titleStyle->copy());

  SoGroup* group = 0;
  group = getContainerNode("binsStyle");
  group->removeAllChildren();
  group = getContainerNode("pointsStyle");
  group->removeAllChildren();
  group = getContainerNode("functionStyle");
  group->removeAllChildren();
  group = getContainerNode("rightHatchStyle");
  group->removeAllChildren();
  group = getContainerNode("leftHatchStyle");
  group->removeAllChildren();
  group = getContainerNode("errorsStyle");
  group->removeAllChildren();
  group = getContainerNode("legendStyle");
  group->removeAllChildren();

  // Reset axes ; but not their title which are not automatic.
 {SbString s = xAxisNode->title.getValue();
  xAxisNode->reset();
  xAxisNode->title.setValue(s);}

 {SbString s = yAxisNode->title.getValue();
  yAxisNode->reset();
  yAxisNode->title.setValue(s);}

 {SbString s = zAxisNode->title.getValue();
  zAxisNode->reset();
  zAxisNode->title.setValue(s);}

  f_shape_Update = TRUE;
}

void SoPlotter::setAxesModeling(const SbString& aValue){
  xAxisNode->modeling.setValue(aValue);
  yAxisNode->modeling.setValue(aValue);
  zAxisNode->modeling.setValue(aValue);
}

void SoPlotter::setAxesColor(const SbColor& aColor){
  xAxisNode->getLineStyle()->color.setValue(aColor);
  xAxisNode->getTicksStyle()->color.setValue(aColor);
  xAxisNode->getLabelsStyle()->color.setValue(aColor);
  xAxisNode->getTitleStyle()->color.setValue(aColor);
  xAxisNode->getMagStyle()->color.setValue(aColor);

  yAxisNode->getLineStyle()->color.setValue(aColor);
  yAxisNode->getTicksStyle()->color.setValue(aColor);
  yAxisNode->getLabelsStyle()->color.setValue(aColor);
  yAxisNode->getTitleStyle()->color.setValue(aColor);
  yAxisNode->getMagStyle()->color.setValue(aColor);

  zAxisNode->getLineStyle()->color.setValue(aColor);
  zAxisNode->getTicksStyle()->color.setValue(aColor);
  zAxisNode->getLabelsStyle()->color.setValue(aColor);
  zAxisNode->getTitleStyle()->color.setValue(aColor);
  zAxisNode->getMagStyle()->color.setValue(aColor);
}

void SoPlotter::setAxesTextScale(float aValue){
  xAxisNode->getLabelsStyle()->scale.setValue(aValue);
  xAxisNode->getTitleStyle()->scale.setValue(aValue);
  xAxisNode->getMagStyle()->scale.setValue(aValue);

  yAxisNode->getLabelsStyle()->scale.setValue(aValue);
  yAxisNode->getTitleStyle()->scale.setValue(aValue);
  yAxisNode->getMagStyle()->scale.setValue(aValue);

  zAxisNode->getLabelsStyle()->scale.setValue(aValue);
  zAxisNode->getTitleStyle()->scale.setValue(aValue);
  zAxisNode->getMagStyle()->scale.setValue(aValue);
}

void SoPlotter::setAxesLinePattern(unsigned short aValue){
  xAxisNode->getLineStyle()->pattern.setValue(aValue);
  yAxisNode->getLineStyle()->pattern.setValue(aValue);
  zAxisNode->getLineStyle()->pattern.setValue(aValue);
}

void SoPlotter::setAxesLineWidth(int aValue){
  xAxisNode->getLineStyle()->width.setValue(aValue);
  yAxisNode->getLineStyle()->width.setValue(aValue);
  zAxisNode->getLineStyle()->width.setValue(aValue);

  xAxisNode->getTicksStyle()->width.setValue(aValue);
  yAxisNode->getTicksStyle()->width.setValue(aValue);
  zAxisNode->getTicksStyle()->width.setValue(aValue);
}

void SoPlotter::setAxesTickLength(float aValue){
  xAxisNode->tickLength.setValue(aValue);
  yAxisNode->tickLength.setValue(aValue);
  zAxisNode->tickLength.setValue(aValue);
}

void SoPlotter::setAxesTitleHeight(float aValue){
  xAxisNode->titleHeight.setValue(aValue);
  yAxisNode->titleHeight.setValue(aValue);
  zAxisNode->titleHeight.setValue(aValue);
}

void SoPlotter::setAxesLabelHeight(float aValue){
  xAxisNode->labelHeight.setValue(aValue);
  yAxisNode->labelHeight.setValue(aValue);
  zAxisNode->labelHeight.setValue(aValue);
}

void SoPlotter::setFont(const SbString& aFont,SbBool aSmoothing,SbBool aHinting){
 {SoTextStyle* style = getXAxis()->getLabelsStyle();
  style->fontName.setValue(aFont);
  style->smoothing.setValue(aSmoothing);
  style->hinting.setValue(aHinting);}
 {SoTextStyle* style = getXAxis()->getTitleStyle();
  style->fontName.setValue(aFont);
  style->smoothing.setValue(aSmoothing);
  style->hinting.setValue(aHinting);}

 {SoTextStyle* style = getYAxis()->getLabelsStyle();
  style->fontName.setValue(aFont);
  style->smoothing.setValue(aSmoothing);
  style->hinting.setValue(aHinting);}
 {SoTextStyle* style = getYAxis()->getTitleStyle();
  style->fontName.setValue(aFont);
  style->smoothing.setValue(aSmoothing);
  style->hinting.setValue(aHinting);}

 {SoTextStyle* style = getZAxis()->getLabelsStyle();
  style->fontName.setValue(aFont);
  style->smoothing.setValue(aSmoothing);
  style->hinting.setValue(aHinting);}
 {SoTextStyle* style = getZAxis()->getTitleStyle();
  style->fontName.setValue(aFont);
  style->smoothing.setValue(aSmoothing);
  style->hinting.setValue(aHinting);}

  getTitleStyle()->fontName.setValue(aFont);
  getTitleStyle()->smoothing.setValue(aSmoothing);
  getTitleStyle()->hinting.setValue(aHinting);

  //plotter->get_infosStyle()->fontName.setValue(aFont);
  //plotter->get_infosStyle()->smoothing.setValue(aSmoothing);
  //plotter->get_infosStyle()->hinting.setValue(aHinting);
}

void SoPlotter::setPAW_AxesTextSize(float aXSIZ,float aYSIZ){
  float xfac = aXSIZ/20.0F;
  float yfac = aYSIZ/20.0F;

  // Take PAW defaults :
  float VSIZ = 0.28F * yfac; //0.014 //tick label character size.
  float XVAL = 0.4F * xfac;  //0.02 //x distance of y tick label to data frame.
  float YVAL = 0.4F * yfac;  //0.02 //y distance of x tick label to data frame.
  float XTIC = 0.3F * yfac;  //0.015 //y length of X axis ticks.
  float YTIC = 0.3F * xfac;  //0.015 //x length of Y axis ticks.
  float XLAB = 1.4F * xfac;  //0.07  //x distance of y title to data frame.
  float YLAB = 0.8F * yfac;  //0.04  //y distance of x title to data frame.
  float ASIZ = 0.28F * yfac; //0.014 // axis title (label) character size.
  //float YHTI = 1.2F * yfac;  //0.06  //y distance of title to x axis.
  //float TSIZ = 0.28F * yfac; //0.014 //title character size

  // Initialize axes :
  if(fShape==XY) {
    xAxisNode->tickLength.setValue(XTIC);
    xAxisNode->labelToAxis.setValue(YVAL);
    xAxisNode->labelHeight.setValue(VSIZ);
    xAxisNode->titleToAxis.setValue(YLAB);
    xAxisNode->titleHeight.setValue(ASIZ);

    yAxisNode->tickLength.setValue(YTIC);
    yAxisNode->labelToAxis.setValue(XVAL);
    yAxisNode->labelHeight.setValue(VSIZ);
    yAxisNode->titleToAxis.setValue(XLAB);
    yAxisNode->titleHeight.setValue(ASIZ);
  } else { //XYZ    
    xAxisNode->tickLength.setValue(XTIC);
    xAxisNode->labelToAxis.setValue(YVAL);
    xAxisNode->labelHeight.setValue(VSIZ);
    xAxisNode->titleToAxis.setValue(YLAB);
    xAxisNode->titleHeight.setValue(ASIZ);
  
    yAxisNode->tickLength.setValue(XTIC);
    yAxisNode->labelToAxis.setValue(YVAL);
    yAxisNode->labelHeight.setValue(VSIZ);
    yAxisNode->titleToAxis.setValue(YLAB);
    yAxisNode->titleHeight.setValue(ASIZ);
  
    zAxisNode->tickLength.setValue(YTIC);
    zAxisNode->labelToAxis.setValue(XVAL);
    zAxisNode->labelHeight.setValue(VSIZ);
    zAxisNode->titleToAxis.setValue(XLAB);
    zAxisNode->titleHeight.setValue(ASIZ);
  }  

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoPlotter::addPlottablePrimitive(SbPlottablePrimitive* aObject){
  f_primitives.push_back(aObject);
  f_primitives_Update = TRUE;
}
void SoPlotter::clearPlottablePrimitives(){
  std::vector<SbPlottablePrimitive*>::const_iterator it;
  for(it=f_primitives.begin();it!=f_primitives.end();++it) delete *it;
  f_primitives.clear();
  f_primitives_Update = TRUE;
}

// for OpenPAW /GRAPHICS/PRIMITIVES/TEXT
// for OpenPAW /GRAPHICS/PRIMITIVES/ITX

void SoPlotter::updateText(SoSeparator& aSep,const SbPlottableText& aObj){

  float z = XY_DEPTH_DATA*1.1F;

  SbVec3f pos;
  axis2dataFrame(SbVec3f(aObj.fX,aObj.fY,z),pos); //FIXME return FALSE

  xx2yy(pos,pos);

  //printf("debug : SoPlotter::updateText+1 : %g %g : %g %g\n",
  //  aObj.fX,aObj.fY,pos[0],pos[1]);

  SoSeparator* sep = new SoSeparator;

  sep->addChild(fStyleCache->getMaterial(SbColor_black));

  SoTranslation* soTranslation = new SoTranslation;
  soTranslation->translation.setValue(pos);
  sep->addChild(soTranslation);

  SoRotationXYZ* soRotation = new SoRotationXYZ;
  soRotation->axis.setValue(SoRotationXYZ::Z);
  soRotation->angle.setValue(aObj.fANGLE*FM_PI/180.0F);
  sep->addChild(soRotation);

  //SIZE is in page coordinate YSIZ.
  float YSIZ = height.getValue();
  if(YSIZ<=0) YSIZ = 1;

  if(SoTools_IsTTF(aObj.fFONT)==TRUE) {
    SoTextTTF* text = new SoTextTTF;
    text->fontName.setValue
      (aObj.fFONT.getSubString(4,aObj.fFONT.getLength()-1));
    text->encoding.setValue("PAW");
    // Majic factor to have same aspect than SoTextHershey.
    float majic = 43.0F; //To match PAW look and feel.
    //float majic = scale * 290.0F; //To match PAW look and feel.
    text->size.setValue(SbFROUND(aObj.fSIZE * majic)); // around 10
    text->string.setValue(aObj.fTEXT);
    text->smoothing.setValue(aObj.fSMOOTHING);
    text->hinting.setValue(aObj.fHINTING);
    //text->rotated.setValue(aRotated);
    //text->horizontalJustification.setValue(aHJust);
    //text->verticalJustification.setValue(aVJust);
    sep->addChild(text);
  } else {
    sep->addChild(fStyleCache->getLineStyle());

    float cooking = 1.1F;
    SoTextHershey* soTextHershey = new SoTextHershey;
    soTextHershey->height.setValue(cooking*aObj.fSIZE);
    soTextHershey->encoding.setValue("PAW");
    soTextHershey->string.setValue(aObj.fTEXT);
    if(aObj.fJUST=='R') {
      soTextHershey->horizontalJustification.setValue(SoTextHershey::RIGHT);
    } else if(aObj.fJUST=='C') {
      soTextHershey->horizontalJustification.setValue(SoTextHershey::CENTER);
    } else {
      soTextHershey->horizontalJustification.setValue(SoTextHershey::LEFT);
    }

    sep->addChild(soTextHershey);
  }

  aSep.addChild(sep);
}

// for OpenPAW /GRAPHICS/PRIMITIVES/BOX

inline void PAW_hatch(int aHTYP,
SbHatchingPolicy& aPolicy
,float& a_spacing
,float& a_angle_right
,float& a_angle_left
) {
  // PAW hatching encoding (paw.pdf 1.14(1992) p 174) :

  int code = aHTYP;
  if(code==0) {
    aPolicy = SbHatchingNone;
    return;
  }

  // From PAW FAQ web page.
  // special code from code [1,25]
  if(code==1) {
    aPolicy = SbHatchingLeftAndRight;
    a_spacing = 0.04F;
    a_angle_right = 3.0F*FM_PI/4.0F;
    a_angle_left = FM_PI/4.0F;
    return;
  } else if(code==2) {
    aPolicy = SbHatchingLeftAndRight;
    a_spacing = 0.08F;
    a_angle_right = 3.0F*FM_PI/4.0F;
    a_angle_left = FM_PI/4.0F;
    return;
  } else if(code==3) {
    aPolicy = SbHatchingLeftAndRight;
    a_spacing = 1.6*0.07F; //cooking
    a_angle_right = 3.0F*FM_PI/4.0F;
    a_angle_left = FM_PI/4.0F;
    return;
  } else if(code==4) {
    code = 354;
  } else if(code==5) {
    code = 345;
  } else if(code==6) {
    code = 359;
  } else if(code==7) {
    code = 350;
  } else if(code<=25) {
    //FIXME : seems to be done with patterns.
    aPolicy = SbHatchingNone;
    return;
  } else if(code<=99) {
    //FIXME
    aPolicy = SbHatchingNone;
    return;
  }

  //code >=100

  // code = ijk

  int i = code / 100;
  int j = (code - i * 100)/10;
  int k = code - i * 100 - j * 10;

  // j-hatching on rightHatchStyle :
  // k-hatching on leftHatchStyle :

  if((j==5)&&(k==5))
    aPolicy = SbHatchingNone;
  else if((j!=5)&&(k==5))
    aPolicy = SbHatchingRight;
  else if((j==5)&&(k!=5))
    aPolicy = SbHatchingLeft;
  else if((j!=5)&&(k!=5))
    aPolicy = SbHatchingLeftAndRight;

  // In CERN/PAW, hatch spacing is fixed for the page (not for the region) !
  //int NX = 0;
  //int NY = 0;
  //if(fZONE.SUPER()) {
  //  NX = -fZONE.NX();
  //  NY = -fZONE.NY();
  //} else {
  //  NX = fZONE.NX();
  //  NY = fZONE.NY();
  //}

  int NY = 1;

  //a_spacing = NY * i * 0.004F;
  a_spacing = NY * i * 0.07F; //cooking

  if(j!=5) {
    float angle = (j==4?45:j*10);
    angle = 180.0F - angle;
    angle *= FM_PI / 180.0F;

    a_angle_right = angle;
  }

  if(k!=5) {
    float angle = (k==4?45:k*10);
    angle *= FM_PI / 180.0F;

    a_angle_left = angle;
  }

}


void SoPlotter::rep_box_hatch(
 SoSeparator& aParent
,float a_spacing
,float a_angle
,float a_stripWidth
,float xx
,float yy
,float xe
,float ye
,float aZZ
){

  //printf("debug : SoPlotter::repHatch1D_xy : zz %g\n",aZZ);
  SoSeparator* separator = new SoSeparator;

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  separator->addChild(coordinate3);
  int icoord = 0;
          
  SbBool empty = TRUE;

  SbVec3f points[5];

  LIST_SET(points,0,xx,yy,aZZ);
  LIST_SET(points,1,xe,yy,aZZ);
  LIST_SET(points,2,xe,ye,aZZ);
  LIST_SET(points,3,xx,ye,aZZ);
  LIST_SET(points,4,xx,yy,aZZ);

  //We can have multiple hatching for a bins ; have a separator :
  SbHatching sbHatching;            
  sbHatching.setOffsetPoint(SbVec3f(0,0,aZZ));
  sbHatching.setAngle(a_angle);
  sbHatching.setSpacing(a_spacing); 
  sbHatching.setStripWidth(a_stripWidth); 

  int res = sbHatching.checkPolyline(points,4);      
  if (res == TRUE) res = sbHatching.computePolyline(points,4);

  unsigned int numPoints = sbHatching.getNoPoints();
  unsigned int numVertices = sbHatching.getNoVertices();
  if((res==TRUE) && numPoints && numVertices) {

    SoSeparator* hatchSep = new SoSeparator(); 
    separator->addChild(hatchSep);
	
    SbVec3f* xyz = new SbVec3f[numPoints];
    const std::vector<SbVec3f>& hatchPointsList = sbHatching.getPoints();
    for (unsigned int a=0;a<numPoints;a++) xyz[a] = hatchPointsList[a];
    coordinate3->point.setValues(icoord,numPoints,xyz);
    delete [] xyz;
	
    int32_t* coord = new int32_t[numVertices*3];
    for (unsigned int a=0;a<numVertices;a++) {
      int ia = 3 * a;
      coord[ia] = icoord;
      icoord++;
      coord[ia+1] = icoord;
      icoord++;
      coord[ia+2] = SO_END_LINE_INDEX;
    }

    if(a_stripWidth==0) {
      hatchSep->addChild(fStyleCache->getLineStyle());
      hatchSep->addChild(fStyleCache->getNormalBindingOverall());
      hatchSep->addChild(fStyleCache->getNormalZ());
      //hatchSep->addChild(material);
      hatchSep->addChild(coordinate3);
      SoIndexedLineSet* indexedLineSet = new SoIndexedLineSet;
      hatchSep->addChild(indexedLineSet);
      indexedLineSet->coordIndex.setValues(0,numVertices*3,coord);
    } else {
      hatchSep->addChild(fStyleCache->getFilled());
      hatchSep->addChild(fStyleCache->getNormalBindingPerFace());
      hatchSep->addChild(fStyleCache->getNormalZ());
      //hatchSep->addChild(material);
      hatchSep->addChild(coordinate3);
      SoIndexedFaceSet* indexedFaceSet = new SoIndexedFaceSet;
      hatchSep->addChild(indexedFaceSet);
      indexedFaceSet->coordIndex.setValues(0,numVertices*3,coord);
    }
    delete [] coord;

    empty = FALSE;
  }

  if(empty==TRUE) {
    separator->unref();
  } else {
    aParent.addChild(separator);
  }
}

void SoPlotter::updateBox(SoSeparator& aSep,const SbPlottableBox& aObj){

  float z = XY_DEPTH_DATA*1.1F;

  SbVec3f pos1;
  axis2dataFrame(SbVec3f(aObj.fX1,aObj.fY1,z),pos1);
  xx2yy(pos1,pos1);
  SbVec3f pos2;
  axis2dataFrame(SbVec3f(aObj.fX2,aObj.fY2,z),pos2);
  xx2yy(pos2,pos2);

  z = pos1[2];

  //printf("debug : SoPlotter::updateText+1 : %g %g : %g %g\n",
  //  aObj.fX,aObj.fY,pos[0],pos[1]);

  if(aObj.fFAIS==SbPlottableBox::HOLLOW) {
  
    SoSeparator* sep = new SoSeparator;

    sep->addChild(fStyleCache->getLineStyle());
    sep->addChild(fStyleCache->getMaterial(aObj.fPLCI));
          
    SbVec3f points[5];
    LIST_SET(points,0,pos1[0],pos1[1],z);
    LIST_SET(points,1,pos2[0],pos1[1],z);
    LIST_SET(points,2,pos2[0],pos2[1],z);
    LIST_SET(points,3,pos1[0],pos2[1],z);
    points[4] = points[0];
    SoTools_addLinesToNode(sep,5,points);

    aSep.addChild(sep);

  } else if(aObj.fFAIS==SbPlottableBox::SOLID) {
  
    SoSeparator* sep = new SoSeparator;

    sep->addChild(fStyleCache->getFilled());
    sep->addChild(fStyleCache->getNormalBindingOverall());
    sep->addChild(fStyleCache->getNormalZ());
    sep->addChild(fStyleCache->getMaterial(aObj.fFACI));
          
    SbVec3f points[4];
    LIST_SET(points,0,pos1[0],pos1[1],z);
    LIST_SET(points,1,pos2[0],pos1[1],z);
    LIST_SET(points,2,pos2[0],pos2[1],z);
    LIST_SET(points,3,pos1[0],pos2[1],z);

    SoTools_addPolygonToNode(sep,4,points); 

    aSep.addChild(sep);

  } else if(aObj.fFAIS==SbPlottableBox::HATCHED) {
  
   {SoSeparator* sep = new SoSeparator;

    sep->addChild(fStyleCache->getMaterial(aObj.fFACI));          

    SbHatchingPolicy hatching;
    float spacing;
    float angle_right;
    float angle_left;
    PAW_hatch(aObj.fFASI,hatching,spacing,angle_right,angle_left);
    float stripWidth = 0;

    if((hatching==SbHatchingRight)||((hatching==SbHatchingLeftAndRight))) {
      rep_box_hatch(*sep,
                    spacing,angle_right,stripWidth,
                    pos1[0],pos1[1],pos2[0],pos2[1],z);
    }
    if((hatching==SbHatchingLeft)||((hatching==SbHatchingLeftAndRight))) {
      rep_box_hatch(*sep,
                    spacing,angle_left,stripWidth,
                    pos1[0],pos1[1],pos2[0],pos2[1],z);
    }
    if(hatching==SbHatchingNone) {
      sep->addChild(fStyleCache->getFilled());
      sep->addChild(fStyleCache->getNormalBindingOverall());
      sep->addChild(fStyleCache->getNormalZ());
      SbVec3f points[4];
      LIST_SET(points,0,pos1[0],pos1[1],z);
      LIST_SET(points,1,pos2[0],pos1[1],z);
      LIST_SET(points,2,pos2[0],pos2[1],z);
      LIST_SET(points,3,pos1[0],pos2[1],z);
      SoTools_addPolygonToNode(sep,4,points); 
    }

    aSep.addChild(sep);}

    if(aObj.fBORD==TRUE) {
      SoSeparator* sep = new SoSeparator;
      sep->addChild(fStyleCache->getLineStyle());
      sep->addChild(fStyleCache->getMaterial(aObj.fPLCI));
          
      z *= 1.01F;
      SbVec3f points[5];
      LIST_SET(points,0,pos1[0],pos1[1],z);
      LIST_SET(points,1,pos2[0],pos1[1],z);
      LIST_SET(points,2,pos2[0],pos2[1],z);
      LIST_SET(points,3,pos1[0],pos2[1],z);
      points[4] = points[0];
      SoTools_addLinesToNode(sep,5,points);
      aSep.addChild(sep);
    }

  } else if(aObj.fFAIS==SbPlottableBox::PATTERN) {
    SoDebugError::postInfo
      ("SoPlotter::updateBox","FAIS PATTERN not yet handled.");  
  }

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

float VerifyLog(float aVal,float aMin,float aDx,SbBool aLog){
 //NOTE : we use FLT_MAX ; -HUGE_VAL does not work on some system (exa:ULTRIX).
 if(aLog==TRUE) {
    if(aVal>0.0F) {
      return (FLOG10(aVal) - aMin)/aDx;
    } else { // Return a negative large number :
      //return -FLT_MAX; 
      return -100;
    }
  } else {
    // Simple protection against value that could exceed a float :
    if(aVal>(aMin+100.F * aDx)) return 100; 
    if(aVal<aMin-100.F * aDx) return -100;
    // Rescale :
    return (aVal - aMin)/aDx;
  }
}

float VerifyLogInv(float aVal,float aMin,float aDx,SbBool aLog){
 if(aLog==TRUE) {
    return FPOW(10,aVal*aDx+aMin);
  } else {
    return aVal*aDx+aMin;
  }
}

void getHeight(
 int aBinsi
,const std::vector<SbPlottableBins1D*>& aBinss
,const SbPList& aBinssSwMnMx //(bim,bmax) of each SbPlottableBins1D
,int aIndex                  //bin of the aBinsi SbPlottableBins1D
,float& aMin
,float& aMax
){
  // Get total height of a bin when superposing bins.
  aMin = 0;
  aMax = aBinss[0]->getBinSumOfWeights(aIndex);
  for(int ibins=1;ibins<=aBinsi;ibins++) {
    float bmin = (*((SbVec2f*)aBinssSwMnMx[ibins]))[0];
    float delta = aBinss[ibins]->getBinSumOfWeights(aIndex) - bmin;
    aMin = aMax; //previous max.
    aMax += delta;
  }   
}

void BarChart(float aBarOffset,float aBarWidth,float& aBegin,float& aEnd){
  float xe = (aEnd - aBegin)*aBarOffset;
  float xw = (aEnd - aBegin)*aBarWidth;
  aEnd = aBegin + xe + xw;
  aBegin = aBegin + xe;
}

bool bins2DToFunc(const SbPlottableBins2D& aBins,float aX,float aY,float& a_value){
  int xn = aBins.getAxisNumberOfBinsX();
  float xmn = aBins.getAxisMinimumX();
  float xmx = aBins.getAxisMaximumX();
  int yn = aBins.getAxisNumberOfBinsY();
  float ymn = aBins.getAxisMinimumY();
  float ymx = aBins.getAxisMaximumY();

  float dx = (xmx-xmn)/xn;
  float dy = (ymx-ymn)/yn;
  int ibin = (int)((aX-xmn)/dx);
  int jbin = (int)((aY-ymn)/dy);

  if((ibin<0)||(ibin>=xn)) {a_value=0;return false;}
  if((jbin<0)||(jbin>=yn)) {a_value=0;return false;}

  float xx_0 = aBins.getBinLowerEdgeX(ibin);
  //float xe_0 = aBins.getBinUpperEdgeX(ibin);
  float xx_1 = aBins.getBinLowerEdgeX(ibin+1);
  //float xe_1 = aBins.getBinUpperEdgeX(ibin+1);

  float yy_0 = aBins.getBinLowerEdgeY(jbin);
  //float ye_0 = aBins.getBinUpperEdgeY(jbin);
  float yy_1 = aBins.getBinLowerEdgeY(jbin+1);
  //float ye_1 = aBins.getBinUpperEdgeY(jbin+1);

  float val1 = aBins.getBinSumOfWeights(ibin,jbin);
  float val2 = aBins.getBinSumOfWeights(ibin+1,jbin);
  //float val3 = aBins.getBinSumOfWeights(ibin+1,jbin+1);
  float val4 = aBins.getBinSumOfWeights(ibin,jbin+1);

  // Interpolate :
  SbVec3f p1(xx_0,yy_0,val1);
  SbVec3f p2(xx_1,yy_0,val2);
  //SbVec3f p3(xx_1,yy_1,val3);
  SbVec3f p4(xx_0,yy_1,val4);

  //FIXME : case of (x,y) in (p2,p3,p4)

  SbPlane plane(p1,p2,p4);
  SbVec3f pt;
  SbLine line(SbVec3f(aX,aY,0),SbVec3f(aX,aY,10));
  plane.intersect(line,pt);

  a_value = pt[2];
  return true;
}

SbBool SoPlotter::xx2yy(const SbVec3f& aPos,SbVec3f& aOut) const {
  // aPos is in data frame NDC coordinates.
 {float XSIZ = width.getValue();
  float XMGL = leftMargin.getValue();
  float XMGR = rightMargin.getValue();
  float wData = XSIZ-XMGL-XMGR;
  aOut[0] = wData*aPos[0];}

 {float YSIZ = height.getValue();
  float YMGL = bottomMargin.getValue();
  float YMGU = topMargin.getValue();
  float hData = YSIZ-YMGL-YMGU;
  aOut[1] = hData*aPos[1];}

 {float ZSIZ = depth.getValue();
  float ZMGD = downMargin.getValue();
  float ZMGU = upMargin.getValue();
  float dData = ZSIZ-ZMGD-ZMGU;
  aOut[2] = dData*aPos[2];}

  return TRUE;
}

SbBool SoPlotter::dataFrame2vp(const SbVec3f& aPos,SbVec3f& aVP) const {
  // aPos is in data frame NDC coordinates.
  // aVP is in viewport/screen coordinates (in [0,1]).
 {float XSIZ = width.getValue();
  float XMGL = leftMargin.getValue();
  float XMGR = rightMargin.getValue();
  float wData = XSIZ-XMGL-XMGR;
  if(XSIZ==0.0F) {
    SoDebugError::postInfo("SoPlotter::dataFrame2vp","XSIZ is 0");
    return FALSE;
  }
  aVP[0] = (wData*aPos[0] + XMGL)/XSIZ;}

 {float YSIZ = height.getValue();
  float YMGL = bottomMargin.getValue();
  float YMGU = topMargin.getValue();
  float hData = YSIZ-YMGL-YMGU;
  if(YSIZ==0.0F) {
    SoDebugError::postInfo("SoPlotter::dataFrame2vp","YSIZ is 0");
    return FALSE;
  }
  aVP[1] = (hData*aPos[1] + YMGL)/YSIZ;}

 {float ZSIZ = depth.getValue();
  float ZMGD = downMargin.getValue();
  float ZMGU = upMargin.getValue();
  float dData = ZSIZ-ZMGD-ZMGU;
  if(ZSIZ==0.0F) {
    SoDebugError::postInfo("SoPlotter::dataFrame2vp","ZSIZ is 0");
    return FALSE;
  }
  aVP[2] = (dData*aPos[2] + ZMGD)/ZSIZ;}

  return TRUE;
}

SbBool SoPlotter::vp2dataFrame(const SbVec3f& aVP,SbVec3f& aPos) const {
  // aVP is in viewport/screen coordinates (in [0,1]).
  // aPos is in data frame NDC coordinates.

 {float XSIZ = width.getValue();
  float XMGL = leftMargin.getValue();
  float XMGR = rightMargin.getValue();
  float wData = XSIZ-XMGL-XMGR;
  if(wData==0.0F) {
    SoDebugError::postInfo("SoPlotter::vp2dataFrame","wData is 0");
    return FALSE;
  }
  aPos[0] = (aVP[0]*XSIZ - XMGL)/wData;}

 {float YSIZ = height.getValue();
  float YMGL = bottomMargin.getValue();
  float YMGU = topMargin.getValue();
  float hData = YSIZ-YMGL-YMGU;
  if(hData==0.0F) {
    SoDebugError::postInfo("SoPlotter::vp2dataFrame","hData is 0");
    return FALSE;
  }
  aPos[1] = (aVP[1]*YSIZ - YMGL)/hData;}

 {float ZSIZ = depth.getValue();
  float ZMGD = downMargin.getValue();
  float ZMGU = upMargin.getValue();
  float dData = ZSIZ-ZMGD-ZMGU;
  if(dData==0.0F) {
    SoDebugError::postInfo("SoPlotter::vp2dataFrame","dData is 0");
    return FALSE;
  }
  aPos[2] = (aVP[2]*ZSIZ - ZMGD)/dData;}

  return TRUE;
}

SbBool SoPlotter::dataFrame2axis(const SbVec3f& aDF,SbVec3f& aPos) const {
  // aDF is in data area coordinates. In [0,1][0,1][0,1].
  // aPos is in axes coordinates.

  // Assume that axes min,max,logScale are up to date.

 {float mn = xAxisNode->minimumValue.getValue();
  float mx = xAxisNode->maximumValue.getValue();
  SbBool lg = xAxisNode->logScale.getValue();
  if(lg==TRUE) {
    mn = FPOW(10,mn);
    mx = FPOW(10,mx);
  }
  aPos[0] = VerifyLogInv(aDF[0],mn,mx-mn,lg);}

 {float mn = yAxisNode->minimumValue.getValue();
  float mx = yAxisNode->maximumValue.getValue();
  SbBool lg = yAxisNode->logScale.getValue();
  if(lg==TRUE) {
    mn = FPOW(10,mn);
    mx = FPOW(10,mx);
  }
  aPos[1] = VerifyLogInv(aDF[1],mn,mx-mn,lg);}

 {float mn = zAxisNode->minimumValue.getValue();
  float mx = zAxisNode->maximumValue.getValue();
  SbBool lg = zAxisNode->logScale.getValue();
  if(lg==TRUE) {
    mn = FPOW(10,mn);
    mx = FPOW(10,mx);
  }
  aPos[2] = VerifyLogInv(aDF[2],mn,mx-mn,lg);}

  return TRUE;
}

SbBool SoPlotter::axis2dataFrame(const SbVec3f& aPos,SbVec3f& aDF) const {
  // aPos is in axes coordinates.
  // aDF in data area coodinate. In [0,1][0,1][0,1].

  // Assume that axes min,max,logScale are up to date.

 {float mn = xAxisNode->minimumValue.getValue();
  float mx = xAxisNode->maximumValue.getValue();
  if(mx==mn) {
    SoDebugError::postInfo
      ("SoPlotter::axis2dataFrame","x : mn (%g) == mx (%g)",mn,mx);
    return FALSE;
  }
  SbBool lg = xAxisNode->logScale.getValue();
  if(lg==TRUE) {
    if(mn<=0) {
      SoDebugError::postInfo
        ("SoPlotter::axis2dataFrame","x log but mn (%g) <=0",mn);
      return FALSE;
    }
    if(mx<=0) {
      SoDebugError::postInfo
        ("SoPlotter::axis2dataFrame","x log but mx (%g) <=0",mx);
      return FALSE;
    }
    mn = FLOG10(mn);
    mx = FLOG10(mx);
  }
  aDF[0] = VerifyLog(aPos[0],mn,mx-mn,lg);}

 {float mn = yAxisNode->minimumValue.getValue();
  float mx = yAxisNode->maximumValue.getValue();
  if(mx==mn) {
    SoDebugError::postInfo
      ("SoPlotter::axis2dataFrame","y : mn (%g) == mx (%g)",mn,mx);
    return FALSE;
  }
  SbBool lg = yAxisNode->logScale.getValue();
  if(lg==TRUE) {
    if(mn<=0) {
      SoDebugError::postInfo
        ("SoPlotter::axis2dataFrame","y log but mn (%g) <=0",mn);
      return FALSE;
    }
    if(mx<=0) {
      SoDebugError::postInfo
        ("SoPlotter::axis2dataFrame","y log but mx (%g) <=0",mx);
      return FALSE;
    }
    mn = FLOG10(mn);
    mx = FLOG10(mx);
  }
  aDF[1] = VerifyLog(aPos[1],mn,mx-mn,lg);}

 {float mn = zAxisNode->minimumValue.getValue();
  float mx = zAxisNode->maximumValue.getValue();
  if(mx==mn) {
    SoDebugError::postInfo
      ("SoPlotter::axis2dataFrame","z : mn (%g) == mx (%g)",mn,mx);
    return FALSE;
  }
  SbBool lg = zAxisNode->logScale.getValue();
  if(lg==TRUE) {
    if(mn<=0) {
      SoDebugError::postInfo
        ("SoPlotter::axis2dataFrame","z log but mn (%g) <=0",mn);
      return FALSE;
    }
    if(mx<=0) {
      SoDebugError::postInfo
        ("SoPlotter::axis2dataFrame","z log but mx (%g) <=0",mx);
      return FALSE;
    }
    mn = FLOG10(mn);
    mx = FLOG10(mx);
  }
  aDF[2] = VerifyLog(aPos[2],mn,mx-mn,lg);}

  return TRUE;
}

SbBool SoPlotter::axis2vp(const SbVec3f& aPos,SbVec3f& aVP) const {
  // aPos is in axes coordinates.
  // aVP is in viewport/screen coordinates (in [0,1]).

  SbVec3f d; // In data area coodinate. In [0,1][0,1][0,1].
  if(axis2dataFrame(aPos,d)==FALSE) return FALSE;

  return dataFrame2vp(d,aVP);
}

SbBool SoPlotter::vp2axis(const SbVec3f& aVP,SbVec3f& aPos) const {
  // aVP is in viewport/screen coordinates (in [0,1]).
  // aPos is in axes coordinates.

  SbVec3f d; // In data area coodinate. In [0,1][0,1][0,1].
  if(vp2dataFrame(aVP,d)==FALSE) return FALSE;

  return dataFrame2axis(d,aPos);
}

void SoPlotter::generateAlternateRep(SoAction*) {
  alternateRep.setValue(topSeparator.getValue());
}
void SoPlotter::clearAlternateRep() {alternateRep.setValue(NULL);}

void SoPlotter::clear() { //used in SoPlotterRegion.

  setPlottableObject(0);
  clearPlottablePrimitives();

  wallEnforced.setValue(FALSE);
  gridEnforced.setValue(FALSE);
  primitivesEnforced.setValue(FALSE);
  innerFrameEnforced.setValue(FALSE);

  xAxisEnforced.setValue(FALSE);
  yAxisEnforced.setValue(FALSE);
  zAxisEnforced.setValue(FALSE);

  getEtcSeparator()->removeAllChildren();
  getEtcDataSeparator()->removeAllChildren();
}

//SbBool SoPlotter::hasMutex() const {return fMutex?TRUE:FALSE;}
//
//void SoPlotter::setMutex(HEPVis::SbMutex* aMutex) {
//  delete fMutex;
//  fMutex = aMutex;
//}
//
//SbBool SoPlotter::lock() const {
//  if(!fMutex) return TRUE; //we consider that thread safety not required.
//  return fMutex->lock();
//}
//
//SbBool SoPlotter::unlock() const {
//  if(!fMutex) return TRUE; //we consider that thread safety not required.
//  return fMutex->unlock();
//}
