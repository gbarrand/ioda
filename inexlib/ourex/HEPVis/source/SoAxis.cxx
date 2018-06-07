/*
 *  SoAxis.cxx
 *  
 *
 *  Created by Laurent Garnier on Fri Dec 05 2003.
 *
 */ 

// this :  
#include <HEPVis/nodekits/SoAxis.h>

// Inventor :
#include <Inventor/SbLinear.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoWriteAction.h>

// HEPVis :
#include <HEPVis/SbMath.h>
#include <HEPVis/SbAxisHPLOT.h>
#include <HEPVis/misc/SoStyleCache.h>
#include <HEPVis/nodes/SoLineStyle.h>
#include <HEPVis/nodes/SoTextStyle.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

#include <HEPVis/misc/SoTools2.h>

#include <cstring> //strcpy
#include <cstdio> //sscanf
#include <vector>
#include <string>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static float CalculateTicksHippo(float,float&);

#define GET_LINE_STYLE(aStyle) \
  fStyleCache->getLineStyle(aStyle->linePattern.getValue(),(float)aStyle->lineWidth.getValue())

//#define DEBUG

SO_KIT_SOURCE(SoAxis)

void SoAxis::initClass() {
  SO_KIT_INIT_CLASS(SoAxis,SoBaseKit,"BaseKit");
}

SoAxis::SoAxis()
:fOld_modeling("")
,fOld_width(0)
,fOld_minimumValue(0)
,fOld_maximumValue(0)
,fOld_divisions(0)
,fOld_logScale(FALSE)
,fOld_tickUp(FALSE)
,fOld_tickLength(0)
,fOld_labelToAxis(0)
,fOld_labelHeight(0)
,fOld_title("")
,fOld_titleToAxis(0)
,fOld_titleHeight(0)
,fOld_titleToAxisJustification(-1)
,fOld_ttfScale(0)
,fOld_tickNumber(0)
,fOld_labelsEnforced(FALSE)
,fOld_labels(0)
,fOld_values(0)
,fOld_coords(0)
,fOld_subCoords(0)
,fStyleCache(0)
{
  SO_KIT_CONSTRUCTOR(SoAxis);

  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0, FALSE);

  SO_KIT_ADD_CATALOG_ENTRY(lineSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(ticksSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(labelsSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(titleSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(magSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);

  // Styles :
  // must not be put under topSeparator because of alternate reps.
  SO_KIT_ADD_CATALOG_ENTRY(styleSeparator,SoSeparator,FALSE,this,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lineStyle,SoLineStyle,FALSE,styleSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(ticksStyle,SoLineStyle,FALSE,styleSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(labelsStyle,SoTextStyle,FALSE,styleSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(titleStyle,SoTextStyle,FALSE,styleSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(magStyle,SoTextStyle,FALSE,styleSeparator,\0,TRUE);

  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SO_NODE_ADD_FIELD(verbose,(FALSE));
  SO_NODE_ADD_FIELD(modeling,(SbTickModeling_hippo));
  // line of axis goes from (0,0,0) to (width,0,0)
  SO_NODE_ADD_FIELD(width,(1));
  SO_NODE_ADD_FIELD(minimumValue,(0));
  SO_NODE_ADD_FIELD(maximumValue,(1));
  SO_NODE_ADD_FIELD(divisions,(510));
  SO_NODE_ADD_FIELD(logScale,(FALSE));
  SO_NODE_ADD_FIELD(tickUp,(TRUE));

  SO_NODE_ADD_FIELD(timeLabels,(FALSE));
  SO_NODE_ADD_FIELD(timeFormat,("%H:%M:%S"));
  SO_NODE_ADD_FIELD(timeOffset,(0));     //UTC_time_1970_01_01__00_00_00
  SO_NODE_ADD_FIELD(timeOffsetIsGMT,(FALSE));

  // Take PAW default : 
  float YSIZ = 20; //page height
  float YMGL = 2;  //low y margin (to data frame).
  float YMGU = 2;  //up y margin (to data frame).
  float VSIZ = 0.28F; //tick label character size.
  float YVAL = 0.4F;  //y distance of x tick label to data frame.
  float XTIC = 0.3F;  //y length of X axis ticks.
  float YLAB = 0.8F; //y distance of x title to data frame.
  float ASIZ = 0.28F; // axis title (label) character size.

  float hData = YSIZ-YMGL-YMGU;

  // To map data space to width :
  float to1 = width.getValue()/hData;

  float vsiz = VSIZ * to1; //0.0175F
  float yval = YVAL * to1; //0.025F
  float xtic = XTIC * to1; //0.01875F
  float ylab = YLAB * to1; //0.05F
  float asiz = ASIZ * to1; //0.0175F

  SO_NODE_ADD_FIELD(tickLength,(xtic)); 
  SO_NODE_ADD_FIELD(labelToAxis,(yval));
  SO_NODE_ADD_FIELD(labelHeight,(vsiz));

  // The SoAxis title is the PAW axis label.
  // It is right justified at the end of axis 
  // (at end right for XY_X, at end top for XY_Y)
  SO_NODE_ADD_FIELD(title,(""));
  SO_NODE_ADD_FIELD(titleToAxis,(ylab));
  SO_NODE_ADD_FIELD(titleHeight,(asiz));
  SO_NODE_ADD_FIELD(titleToAxisJustification,(RIGHT));

  SO_NODE_ADD_FIELD(ttfScale,(1));

  SO_NODE_DEFINE_ENUM_VALUE(Justification,LEFT);
  SO_NODE_DEFINE_ENUM_VALUE(Justification,CENTER);
  SO_NODE_DEFINE_ENUM_VALUE(Justification,RIGHT);
  SO_NODE_SET_SF_ENUM_TYPE(titleToAxisJustification,Justification);

  // If modeling is hippo or hplot, 
  // labelsEnforced TRUE let labels be an Input field.
  SO_NODE_ADD_FIELD(labelsEnforced,(FALSE));

  // Note : if modeling is none,the below are input fields.
  //        If modeling is hippo or hplot, the below are output fields.
  SO_NODE_ADD_FIELD(tickNumber,(0)); // Output field.
  SO_NODE_ADD_FIELD(labels,("")); // Output field.
  //Values of ticks (is in [minimumValue,maximumValue])
  SO_NODE_ADD_FIELD(values,(0)); // Output field.
  //Coords of ticks (in [0,width]
  SO_NODE_ADD_FIELD(coords,(0)); // Output field.
  SO_NODE_ADD_FIELD(subCoords,(0)); // Output field.
  SO_NODE_ADD_FIELD(magnitude,(0)); // Output field.

  // Setup styles :
  ((SoLineStyle*)lineStyle.getValue())->setFromString("color black");
  ((SoLineStyle*)ticksStyle.getValue())->setFromString("color black");
  ((SoTextStyle*)labelsStyle.getValue())->setFromString(
    "color black\nfontName Hershey\nencoding PAW");
  ((SoTextStyle*)titleStyle.getValue())->setFromString(
    "color black\nfontName Hershey\nencoding PAW");
  ((SoTextStyle*)magStyle.getValue())->setFromString(
    "color black\nfontName Hershey\nencoding PAW");

  fOld_lineStyle.setTransparency(-1);
  fOld_ticksStyle.setTransparency(-1);
  fOld_labelsStyle.setTransparency(-1);
  fOld_titleStyle.setTransparency(-1);
  fOld_magStyle.setTransparency(-1);

  fOld_labels = new SoMFString;
  fOld_values = new SoMFFloat;
  fOld_coords = new SoMFFloat;
  fOld_subCoords = new SoMFFloat;

  fStyleCache = new SoStyleCache;
  fStyleCache->ref();
}

SoAxis::~SoAxis() {
  delete fOld_labels;
  delete fOld_values;
  delete fOld_coords;
  delete fOld_subCoords;

  fStyleCache->unref();
}

void SoAxis::reset() {
  modeling.setValue(SbTickModeling_hippo);
  width.setValue(1);
  minimumValue.setValue(0);
  maximumValue.setValue(1);
  divisions.setValue(510);
  logScale.setValue(FALSE);
  tickUp.setValue(TRUE);

  // Take PAW default : 
  float YSIZ = 20; //page height
  float YMGL = 2;  //low y margin (to data frame).
  float YMGU = 2;  //up y margin (to data frame).
  float VSIZ = 0.28F; //tick label character size.
  float YVAL = 0.4F;  //y distance of x tick label to data frame.
  float XTIC = 0.3F;  //y length of X axis ticks.
  float YLAB = 0.8F; //y distance of x title to data frame.
  float ASIZ = 0.28F; // axis title (label) character size.

  float hData = YSIZ-YMGL-YMGU;

  // To map data space to width :
  float to1 = width.getValue()/hData;

  float vsiz = VSIZ * to1; //0.0175F
  float yval = YVAL * to1; //0.025F
  float xtic = XTIC * to1; //0.01875F
  float ylab = YLAB * to1; //0.05F
  float asiz = ASIZ * to1; //0.0175F

  tickLength.setValue(xtic); 
  labelToAxis.setValue(yval);
  labelHeight.setValue(vsiz);

  // The SoAxis title is the PAW axis label.
  // It is right justified at the end of axis 
  // (at end right for XY_X, at end top for XY_Y)
  title.setValue("");
  titleToAxis.setValue(ylab);
  titleHeight.setValue(asiz);
  titleToAxisJustification.setValue(RIGHT);

  // Setup styles :
  ((SoLineStyle*)lineStyle.getValue())->setFromString("color black");
  ((SoLineStyle*)ticksStyle.getValue())->setFromString("color black");
  ((SoTextStyle*)labelsStyle.getValue())->setFromString(
    "color black\nfontName Hershey\nencoding PAW");
  ((SoTextStyle*)titleStyle.getValue())->setFromString(
    "color black\nfontName Hershey\nencoding PAW");
  ((SoTextStyle*)magStyle.getValue())->setFromString(
    "color black\nfontName Hershey\nencoding PAW");
}

void SoAxis::setVisible(SbBool aValue) { //used with SoPlotterRegion overlays.
  ((SoLineStyle*)lineStyle.getValue())->visible.setValue(aValue);
  ((SoLineStyle*)ticksStyle.getValue())->visible.setValue(aValue);
  ((SoTextStyle*)labelsStyle.getValue())->visible.setValue(aValue);
  ((SoTextStyle*)titleStyle.getValue())->visible.setValue(aValue);
  ((SoTextStyle*)magStyle.getValue())->visible.setValue(aValue);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoAxis::doAction(SoAction* aAction) {
  updateChildren();
  SO_ALTERNATEREP_DO_ACTION(aAction)
}
#define ACTION(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::doAction(action);\
  aSuper::aMethod(action);\
}
ACTION(SoAxis,SoBaseKit,SoCallbackAction,callback)
ACTION(SoAxis,SoBaseKit,SoGLRenderAction,GLRender)
ACTION(SoAxis,SoBaseKit,SoGetBoundingBoxAction,getBoundingBox)
ACTION(SoAxis,SoBaseKit,SoGetMatrixAction,getMatrix)
ACTION(SoAxis,SoBaseKit,SoHandleEventAction,handleEvent)
ACTION(SoAxis,SoBaseKit,SoRayPickAction,rayPick)
ACTION(SoAxis,SoBaseKit,SoSearchAction,search)
//ACTION(SoAxis,SoBaseKit,SoWriteAction,write)
ACTION(SoAxis,SoBaseKit,SoPickAction,pick)

void SoAxis::write(SoWriteAction* aAction) {
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    //NOTE : the below logic loose the fact
    //       that we had a SoAxis. 
    //       We may also introduce a coworking SoAltAxis
    //       that has no polyhedron field, and arrange
    //       that the SoAlternateRepAction swaps 
    //       from SoAxis to SoAltAxis.
    altRep->write(aAction);
  } else {
    SoAxis::doAction(aAction);
    SoBaseKit::write(aAction);
  }
}

SoLineStyle* SoAxis::getLineStyle() {
  return (SoLineStyle*)lineStyle.getValue();
}

SoLineStyle* SoAxis::getTicksStyle() {
  return (SoLineStyle*)ticksStyle.getValue();
}

SoTextStyle* SoAxis::getLabelsStyle() {
  return (SoTextStyle*)labelsStyle.getValue();
}

SoTextStyle* SoAxis::getTitleStyle() {
  return (SoTextStyle*)titleStyle.getValue();
}

SoTextStyle* SoAxis::getMagStyle() {return (SoTextStyle*)magStyle.getValue();}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoAxis::updateChildren() {
  SbBool update_line = FALSE;
  SbBool update_ticks = FALSE;
  SbBool update_labels = FALSE;
  SbBool update_title = FALSE;
  SbBool update_mag = FALSE;
  SbBool update_computeTicks = FALSE;

  if(fOld_modeling!=modeling.getValue()) {
    fOld_modeling = modeling.getValue();
    update_ticks = TRUE;
    update_labels = TRUE;
    update_title = TRUE;
    update_mag = TRUE;
    update_computeTicks = TRUE;
  }
  if(fOld_width!=width.getValue()) {
    fOld_width = width.getValue();
    update_line = TRUE;
    update_ticks = TRUE;
    update_labels = TRUE;
    update_title = TRUE;
    update_mag = TRUE;
    update_computeTicks = TRUE;
  }
  if(fOld_minimumValue!=minimumValue.getValue()) {
    fOld_minimumValue = minimumValue.getValue();
    update_line = TRUE;
    update_ticks = TRUE;
    update_labels = TRUE;
    update_mag = TRUE;
    update_computeTicks = TRUE;
  }
  if(fOld_maximumValue!=maximumValue.getValue()) {
    fOld_maximumValue = maximumValue.getValue();
    update_ticks = TRUE;
    update_labels = TRUE;
    update_mag = TRUE;
    update_computeTicks = TRUE;
  }
  if(fOld_divisions!=divisions.getValue()) {
    fOld_divisions = divisions.getValue();
    update_ticks = TRUE;
    update_labels = TRUE;
    update_mag = TRUE;
    update_computeTicks = TRUE;
  }
  if(fOld_logScale!=logScale.getValue()) {
    fOld_logScale = logScale.getValue();
    update_ticks = TRUE;
    update_labels = TRUE;
    update_mag = TRUE;
    update_computeTicks = TRUE;
  }

#define CHECK_IF(a__field,a__flag) \
  if(fOld_##a__field!=a__field.getValue()) {\
    fOld_##a__field = a__field.getValue();\
    a__flag = TRUE;\
  }

  CHECK_IF(tickUp,update_ticks)
  CHECK_IF(tickLength,update_ticks)

  CHECK_IF(labelsEnforced,update_labels)
  CHECK_IF(labelToAxis,update_labels)
  CHECK_IF(labelHeight,update_labels)

  CHECK_IF(title,update_title)
  CHECK_IF(titleToAxis,update_title)
  CHECK_IF(titleHeight,update_title)
  CHECK_IF(titleToAxisJustification,update_title)

#define CHECK_IF_MF(a__field,a__flag) \
  if(*fOld_##a__field!=a__field) {\
    *fOld_##a__field = a__field;\
    a__flag = TRUE;\
  }

  if(modeling.getValue()==SbTickModeling_none) {
    CHECK_IF(tickNumber,update_computeTicks)
    CHECK_IF_MF(values,update_computeTicks)
    CHECK_IF_MF(coords,update_computeTicks)
    CHECK_IF_MF(subCoords,update_computeTicks)
    CHECK_IF_MF(labels,update_computeTicks)
  }

 {SoLineStyle* soStyle = getLineStyle();
  if(fOld_lineStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    fOld_lineStyle = soStyle->getSbStyle();
    update_line = TRUE;
  }}
 {SoLineStyle* soStyle = getTicksStyle();
  if(fOld_ticksStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    fOld_ticksStyle = soStyle->getSbStyle();
    update_ticks = TRUE;
  }}
 {SoTextStyle* soStyle = getLabelsStyle();
  if(fOld_labelsStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    fOld_labelsStyle = soStyle->getSbStyle();
    update_labels = TRUE;
  }}
 {SoTextStyle* soStyle = getTitleStyle();
  if(fOld_titleStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    fOld_titleStyle = soStyle->getSbStyle();
    update_title = TRUE;
  }}
 {SoTextStyle* soStyle = getMagStyle();
  if(fOld_magStyle.isEqual(soStyle->getSbStyle())==FALSE) {
    fOld_magStyle = soStyle->getSbStyle();
    update_mag = TRUE;
  }}

  if(fOld_ttfScale!=ttfScale.getValue()) {
    fOld_ttfScale = ttfScale.getValue();
   {SoTextStyle* style = (SoTextStyle*)labelsStyle.getValue();
    if(SoTools_IsTTF(style->fontName.getValue())==TRUE) {
      update_labels = TRUE;
    }}
   {SoTextStyle* style = (SoTextStyle*)magStyle.getValue();
    if(SoTools_IsTTF(style->fontName.getValue())==TRUE) {
      update_mag = TRUE;
    }}
   {SoTextStyle* style = (SoTextStyle*)titleStyle.getValue();
    if(SoTools_IsTTF(style->fontName.getValue())==TRUE) {
      update_title = TRUE;
    }}
  }

#ifdef DEBUG
  printf("SoAxis::updateChildren %ld : width %g : min %g max %g\n",
    this,width.getValue(),minimumValue.getValue(),maximumValue.getValue());
#endif
  if(width.getValue()<=0) {
    SoSeparator* soLineSep = (SoSeparator*)lineSeparator.getValue();
    soLineSep->removeAllChildren();
    SoSeparator* soTicksSep = (SoSeparator*)ticksSeparator.getValue();
    soTicksSep->removeAllChildren();
    SoSeparator* soLabelsSep = (SoSeparator*)labelsSeparator.getValue();
    soLabelsSep->removeAllChildren();
    SoSeparator* soMagSep = (SoSeparator*)magSeparator.getValue();
    soMagSep->removeAllChildren();
    SoSeparator* soTitleSep = (SoSeparator*)titleSeparator.getValue();
    soTitleSep->removeAllChildren();
    return;
  }

  if(verbose.getValue()==TRUE) {
    if(update_line) 
      SoDebugError::postInfo("SoAxis::updateChildren","update line");
    if(update_ticks) 
      SoDebugError::postInfo("SoAxis::updateChildren","update ticks");
    if(update_labels) 
      SoDebugError::postInfo("SoAxis::updateChildren","update labels");
    if(update_title) 
      SoDebugError::postInfo("SoAxis::updateChildren","update title");
    if(update_mag) 
      SoDebugError::postInfo("SoAxis::updateChildren","update mag");
    if(update_computeTicks) 
      SoDebugError::postInfo("SoAxis::updateChildren","update computeTicks");
  }

  if ((update_line==TRUE) || 
      (update_ticks==TRUE) || 
      (update_labels==TRUE) ) { 

     if(update_line==TRUE) {
       update_line = FALSE;

       //printf("debug : SoAxis::updateChildren : update_lines\n");

       SoSeparator* soLineSep = (SoSeparator*)lineSeparator.getValue();
       soLineSep->removeAllChildren();

       SoLineStyle* style = (SoLineStyle*)lineStyle.getValue();
       if(style->visible.getValue()==TRUE) {

         soLineSep->addChild(fStyleCache->getMaterial 
              (style->color.getValue(),(float)style->transparency.getValue()));

         soLineSep->addChild(fStyleCache->getLineStyle 
              (style->pattern.getValue(),(float)style->width.getValue()));
  
         SbVec3f points[2];
         points[0].setValue(0,0,0);
         points[1].setValue(width.getValue(),0,0);
  
         SoCoordinate3* coordinate3 = new SoCoordinate3;
         int32_t pointn = 2;
         coordinate3->point.setValues(0,pointn,points);
         soLineSep->addChild(coordinate3);
  
         SoLineSet* lineSet = new SoLineSet;
         lineSet->numVertices.set1Value(0,pointn);
         soLineSep->addChild(lineSet);

       }
     }

     if(update_ticks==TRUE) {
       update_ticks = FALSE;

       //printf("debug : SoAxis::updateChildren : update_ticks\n");

       if(update_computeTicks==TRUE) {
         update_computeTicks = FALSE;
         //printf("debug : SoAxis::updateChildren : update_computeTicks %s\n",
         //  modeling.getValue().getString());
         if(modeling.getValue()==SbTickModeling_none) {
         } else if(modeling.getValue()==SbTickModeling_hplot) {
           computeTicksHPLOT();
         } else {
           computeTicksHippo();
         }
       }

       SoSeparator* soTicksSep = (SoSeparator*)ticksSeparator.getValue();
       soTicksSep->removeAllChildren();

       SoLineStyle* style = (SoLineStyle*)ticksStyle.getValue(); 

       if(style->visible.getValue()==TRUE) {

         int number = tickNumber.getValue()*2;
         number += fSubTicks.size()*2;       
         SbVec3f* points = new SbVec3f[number];
         
         SoLineSet* lineSet = new SoLineSet;
  
         int countPoints = 0;
         int numVertices = 0;
         
         // Ticks :
#ifdef DEBUG
         printf(" \nTicks : ");
#endif
         if(modeling.getValue()==SbTickModeling_hplot) {
  
           for(unsigned int index=0;index<fSubTicks.size();index++) {
             points[countPoints] = fSubTicks[index].first;
             points[countPoints+1] = fSubTicks[index].second;

	     /*
             printf("debug : SoAxis : first : %g %g %g second : %g %g %g\n",
               fSubTicks[index].first[0],
               fSubTicks[index].first[1],
   	       fSubTicks[index].first[2],
               fSubTicks[index].second[0],
               fSubTicks[index].second[1],
               fSubTicks[index].second[2]);
	     */

             if(tickUp.getValue()==FALSE) {
               points[countPoints][1] *= -1;
               points[countPoints+1][1] *= -1;
             }
             countPoints +=2;
             lineSet->numVertices.set1Value(numVertices,2);
             numVertices ++;
           }
    
         } else {
  
           float yy = tickUp.getValue()==TRUE ? 
             tickLength.getValue() : -tickLength.getValue();
           for(int index=0;index<tickNumber.getValue();index++) {
#ifdef DEBUG
             printf("%f,",values[index]);
#endif
             float xx = coords[index];
  
             points[countPoints].setValue((xx),(0),(0));
             points[countPoints+1].setValue((xx),(yy),(0));
             countPoints +=2;
             lineSet->numVertices.set1Value(numVertices,2);
             numVertices ++;
           }
  
         }
         
         if(countPoints) {
  
           soTicksSep->addChild(fStyleCache->getMaterial 
              (style->color.getValue(),(float)style->transparency.getValue()));
  
           soTicksSep->addChild(fStyleCache->getLineStyle 
              (style->pattern.getValue(),(float)style->width.getValue()));
  
           SoCoordinate3* coordinate3 = new SoCoordinate3;
           int32_t pointn = countPoints;
           coordinate3->point.setValues(0,pointn,points);
           soTicksSep->addChild(coordinate3);
  
           soTicksSep->addChild(lineSet);
         } else {
           lineSet->unref();
         }
  
         delete [] points;

       } //end visible.
     }

     if(update_labels == TRUE) {
       update_labels = FALSE;

       //printf("debug : SoAxis::updateChildren : update_labels\n");

       SoSeparator* soLabelsSep = (SoSeparator*)labelsSeparator.getValue();
       soLabelsSep->removeAllChildren();

       SoTextStyle* style = (SoTextStyle*)labelsStyle.getValue();

       if((style->visible.getValue()==TRUE) && (tickNumber.getValue()>0) ) {

         soLabelsSep->addChild(fStyleCache->getMaterial 
            (style->color.getValue(),(float)style->transparency.getValue()));

         SbString fontName = style->fontName.getValue();

         float text_size = labelHeight.getValue() * style->scale.getValue();

/*FIXME  // Adjust text_size to be sure that the labels do not overlap.
        {int number = tickNumber.getValue();
         if(number>=2) {
           float min_delta_tick = coords[number-1]-coords[0];
           float xx_prev = coords[0];
           for(unsigned int index=1;index<number;index++) {
             float xx = coords[index];
             min_delta_tick = SbMinimum(min_delta_tick,xx-xx_prev);
             xx_prev = xx;
           }
           text_size = SbMinimum(text_size,min_delta_tick);
         }}
*/

#ifdef DEBUG
         printf ("\n  dessin des labels \n");
#endif
         // tick labels
         if( (fontName.getLength() >=6) &&
             (fontName.getSubString(0,5)=="Text3/") ) {

           soLabelsSep->addChild(fStyleCache->getFilled());

         } else if((fontName.getLength() >=6 ) && 
                   (fontName.getSubString(0,5)=="Text2/")) {
         } else if(SoTools_IsTTF(fontName)==TRUE) {

           text_size *= ttfScale.getValue();

         } else {  // Default is Hershey.
           soLabelsSep->addChild(GET_LINE_STYLE(style));
         }

         for(int count=0;count<tickNumber.getValue();count++) { 
           float xx = coords[count];
           SbVec3f vec(xx,-labelToAxis.getValue(),0);
#ifdef DEBUG
           printf("SoAxis :: dessin du titre \n");
           printf(": %s : size (%g) %g : pos %g %g %g\n",
             labels[count].getString(),text_size,text_size,
             vec[0],vec[1],vec[2]);
#endif

           vec += style->translation.getValue();

           SoTools_addAlignedStringToNode(soLabelsSep,
                                  fontName,
                                  style->encoding.getValue(),
                                  style->smoothing.getValue(),
                                  labels[count],
                                  vec,
                                  style->orientationX.getValue(),
                                  style->orientationY.getValue(),
                                  text_size,
           (SoTools_HorizontalJustification)
             style->horizontalJustification.getValue(),
           (SoTools_VerticalJustification)
             style->verticalJustification.getValue());
         }
       }
     }
  }

  if(update_mag == TRUE) {
    update_mag = FALSE;

    SoSeparator* soMagSep = (SoSeparator*)magSeparator.getValue();
    soMagSep->removeAllChildren();
   
#ifdef DEBUG
    printf("SoAxis ::   dessin de la magnitude\n");
#endif

    // magnitude :
    int mag = magnitude.getValue ();
    if(mag!=0) { 

      SoTextStyle* style = (SoTextStyle*)magStyle.getValue();

      soMagSep->addChild(fStyleCache->getMaterial 
        (style->color.getValue(),(float)style->transparency.getValue()));

      char string[64];
      if(mag>=0)
        ::sprintf(string,"x10+%d",mag);
      else
        ::sprintf(string,"x10-%d",::abs(mag));
      
      SbVec3f vec(width.getValue()*1.01F,0,0);
      vec += style->translation.getValue();

      float text_size = labelHeight.getValue()*0.8F*style->scale.getValue();

      SbString fontName = style->fontName.getValue();
      if ( (fontName.getLength() >=6) &&
           (fontName.getSubString(0,5)=="Text3/") ) {
        soMagSep->addChild(fStyleCache->getFilled());
      } else if((fontName.getLength() >=6 ) && 
                (fontName.getSubString(0,5)=="Text2/")) {
      } else if(SoTools_IsTTF(fontName)==TRUE) {
        text_size *= ttfScale.getValue();
      } else {  // Default is Hershey.
        soMagSep->addChild(GET_LINE_STYLE(style));
      }

      SoTools_addAlignedStringToNode(soMagSep,
                             style->fontName.getValue(),
                             style->encoding.getValue(),
                             style->smoothing.getValue(),
                             string,
                             vec,
                             style->orientationX.getValue(),
                             style->orientationY.getValue(),
                             text_size,
     (SoTools_HorizontalJustification)
       style->horizontalJustification.getValue(),
     (SoTools_VerticalJustification)
       style->verticalJustification.getValue());
    }
  }

  if(update_title == TRUE) {
    update_title = FALSE;

    SoSeparator* soTitleSep = (SoSeparator*)titleSeparator.getValue();
    soTitleSep->removeAllChildren();

    SoTextStyle* style = (SoTextStyle*)titleStyle.getValue();

#ifdef DEBUG
    printf("SoAxis :: dessin du titre \n");
#endif

    if((title.getValue()!="") && (style->visible.getValue()==TRUE) ) { 

#ifdef DEBUG
      printf(": %s : height %g\n",
        title.getValue().getString(),
        titleHeight.getValue());
#endif

      soTitleSep->addChild(fStyleCache->getMaterial 
        (style->color.getValue(),(float)style->transparency.getValue()));

      float text_size = titleHeight.getValue()*style->scale.getValue();

      SbString fontName = style->fontName.getValue();
      if( (fontName.getLength() >=6) &&
          (fontName.getSubString(0,5)=="Text3/") ) {
        soTitleSep->addChild(fStyleCache->getFilled());
      } else if((fontName.getLength() >=6 ) && 
                (fontName.getSubString(0,5)=="Text2/")) {
      } else if(SoTools_IsTTF(fontName)==TRUE) {
        text_size *= ttfScale.getValue();
      } else {  // Default is Hershey.
        soTitleSep->addChild(GET_LINE_STYLE(style));
      }

      float xx = 0; //LEFT
      if(titleToAxisJustification.getValue()==CENTER) {
         xx = width.getValue()/2;
      } else if(titleToAxisJustification.getValue()==RIGHT) {
         xx = width.getValue();
      }

      SbVec3f vec(xx,-titleToAxis.getValue(),0);
      vec += style->translation.getValue();

      SoTools_addAlignedStringToNode(soTitleSep,
                               fontName,
                               style->encoding.getValue(),
                               style->smoothing.getValue(),
                               title.getValue(),
                               vec,
                               style->orientationX.getValue(),
                               style->orientationY.getValue(),
                               text_size,
      (SoTools_HorizontalJustification)
        style->horizontalJustification.getValue(),
      (SoTools_VerticalJustification)
        style->verticalJustification.getValue(),style->rotated.getValue());
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
/// Hippodraw tick modeling //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoAxis::computeTicksHippo() {
  //  Controlled by :
  //    minimumValue SoField
  //    maximumValue SoField
  //    aLogScale
  //  Produces SoFields :
  //    values
  //    coords
  //    subCoords
  //    labels (if labelsEnforced is FALSE)
  //    tickNumber

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoAxis::computeTicksHippo","%lu",(unsigned long)this);
  }

  float mn = minimumValue.getValue();
  float mx = maximumValue.getValue();

  SbBool aLogScale = logScale.getValue();
  if(aLogScale==TRUE) {
    if((mn<=0) || (mx<=0) ) aLogScale = FALSE;
  }

  float magxxx,y,yr,startTick,tickSize;
  float pmag;
  char pstr[10] = "";
  char tmp[10];

  int tick_number = 0;
  std::vector<float> tick_values;
  std::vector<std::string> tick_labels;

/* need include <float.h> which does not exist on some system
#define NUM_FUZZ DBL_EPSILON*4
*/
#define NUM_FUZZ 0.01

  if (mn >= mx) {
    if(tickNumber.getValue()!=0) {
      tickNumber.setValue(0);
      values.deleteValues(0);
      coords.deleteValues(0);
      subCoords.deleteValues(0);
      labels.deleteValues(0);
    }
    if(magnitude.getValue()!=0) magnitude.setValue(0);
    fSubTicks.clear();
    return;
  }

  if (aLogScale==FALSE) {

    tickSize  = CalculateTicksHippo(mx-mn,magxxx);
    startTick = FCEIL( mn / tickSize) * tickSize;

    if (FFABS(magxxx) <= 3.0)
      pmag = 0.0;
    else 
      pmag = startTick != 0.0 ? FFLOOR(FLOG10(FFABS(startTick))) : magxxx;
    
    sprintf(pstr,"%%1.%df",(int)SbMaximum((pmag-magxxx),0.0));
    
    y = startTick;
    while (y <= mx*(1.0+NUM_FUZZ)) {
      
      yr = FFLOOR(y/FPOW(10.0,magxxx) + 0.5);
      
      sprintf(tmp,pstr,yr*FPOW(10.0,magxxx-pmag));

     {float val = yr * FPOW(10.0,magxxx);
      if((val>=mn)&&(val<=mx)) { //G.Barrand : add this test.
        tick_values.push_back(val);
        tick_labels.push_back(tmp);
        tick_number++;
      }}

      y += tickSize;
    }
    if (FFABS(magxxx) <= 3.0) magxxx = 0.0;

  }  else {

    if (mn <= 0) {
      if(tickNumber.getValue()!=0) {
        tickNumber.setValue(0);
        values.deleteValues(0);
        coords.deleteValues(0);
        subCoords.deleteValues(0);
        labels.deleteValues(0);
      }
      if(magnitude.getValue()!=0) magnitude.setValue(0);
      fSubTicks.clear();
      return;
    }

    int nLogTicks;
    float logTicks[5];
    float magStep;

    float maghigh = FCEIL(FLOG10(mx));
    float maglow  = FFLOOR(FLOG10(mn));
    float magrng  = maghigh - maglow;
    
    if (magrng <=3) {
      nLogTicks   = 3;
      logTicks[0] = 1.0;
      logTicks[1] = 2.0;
      logTicks[2] = 5.0;
      magStep     = 1.0;
    } else {
      nLogTicks   = 1;
      logTicks[0] = 1.0;
      magStep     = magrng <= 7 ? 1.0F : 2.0F;
    }
    
    pmag = (nLogTicks == 3 && (FFABS(maglow)>3 || FFABS(maghigh)>3)) ? 
      maglow : 0;
    
    magxxx = maglow;
    int i = 0;
    while ((y=logTicks[i]*FPOW(10,magxxx)) < mx*(1+NUM_FUZZ)) {
      if (y >= mn) {

        /*
         * be careful: there is a bug in the NeXT (s)printf 
         *   routine when you do, eg. printf("%1.0g",0.01);
         */
        if ((magxxx-pmag) > 4 || (magxxx-pmag) < -3)
          strcpy(pstr,"%1.0e");
        else
          sprintf(pstr,"%%1.%df",(int)((magxxx-pmag)>0?0.:-(magxxx-pmag)));
        sprintf(tmp,pstr,y*FPOW(10.0,-pmag));

       {float val = FLOG10(y);
        if((val>=FLOG10(mn))&&(val<=FLOG10(mx))) { //G.Barrand : add this test.
          tick_values.push_back(val);
          tick_labels.push_back(tmp);
          tick_number++;
        }}

      }
      
      i++;
      if (i>=nLogTicks)        {
        i = 0;
        magxxx += magStep;
      }
    }

    mn = FLOG10(mn);
    mx = FLOG10(mx);
  }

  float range = mx - mn;         

  // it is assumes that tick_values are ordered min to max.
  values.setNum(tick_number);
  coords.setNum(tick_number);
  tickNumber.setValue(tick_number);
  for(int index=0;index<tick_number;index++) {
    float val = tick_values[index];
    float coord = width.getValue() * (val-mn)/range;
    values.set1Value(index,val);
    coords.set1Value(index,coord);
  }

  if(labelsEnforced.getValue()==TRUE) {
    int n = labels.getNum();
    if(tick_number>n) {
      for(int index=n;index<tick_number;index++) {
        labels.set1Value(index,"");
      }
    }
  } else {
    labels.setNum(tick_number); 
    for(int index=0;index<tick_number;index++) {
      labels.set1Value(index,tick_labels[index].c_str());
    }
  }

  magnitude.setValue((int)pmag);

  subCoords.deleteValues(0);
  fSubTicks.clear();
}

static float CalculateTicksHippo(float aSize,float& aMagnitude) {
#define MIN_TICKS 4

  if (aSize <= 0.0) {
    //printf ("CalculateTicks : bad value \n");
    aSize = FFABS(aSize);
    if (aSize == 0.0) aSize = 1.0;
  }
  
  aMagnitude = FFLOOR(FLOG10(aSize));
  if (aSize/FPOW(10.0,aMagnitude) < MIN_TICKS) (aMagnitude)--;
  
  /* now fit the max number of ticks into this range */
  
  float  tickSize;
  int tickIndex;
  static float goodTicks[] = {10.0, 5.0, 4.0, 2.0, 1.0};
  for (tickIndex = 0;
   aSize / (tickSize=goodTicks[tickIndex]*FPOW(10.0,aMagnitude) ) < MIN_TICKS;
       tickIndex++);
  
  if (tickIndex == 0) (aMagnitude)++;
     
  return tickSize;
}

//////////////////////////////////////////////////////////////////////////////
/// HPLOT tick modeling //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoAxis::computeTicksHPLOT() {
  //  Controlled by :
  //    minimumValue SoField
  //    maximumValue SoField
  //    divisions SoField
  //    aLogScale
  //  Produces SoFields :
  //    values
  //    coords
  //    labels (if labelsEnforced is FALSE)
  //    tickNumber

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoAxis::computeTicksHPLOT","%lu",(unsigned long)this);
  }

  float mn = minimumValue.getValue();
  float mx = maximumValue.getValue();

  SbBool aLogScale = logScale.getValue();
  if(aLogScale==TRUE) {
    if((mn<=0) || (mx<=0) ) aLogScale = FALSE;
  }
     
  // Use SbAxisHPLOT to get the ticks and subticks positions
  // and the labels text.

  double xmin = 0;
  double ymin = 0;
  double xmax = width.getValue();
  double ymax = 0;

  double gridlength = 0;
  std::string chopt;
  if(aLogScale==TRUE) chopt += "G";

  std::vector< std::pair<SbVec3f,SbVec3f> > linesGrid;
  std::vector<SbAxisHPLOT_Text> texts;

  SbAxisHPLOT sbAxisHPLOT;

  chopt += "S";
  sbAxisHPLOT.setTickSize(tickLength.getValue()/width.getValue());

  if(timeLabels.getValue()==TRUE) {
    chopt += "t";
    sbAxisHPLOT.setTimeFormat(timeFormat.getValue().getString());
    sbAxisHPLOT.setTimeOffset(timeOffset.getValue(),
                              timeOffsetIsGMT.getValue());
  }

  // Get ticks :
 {double wmin = mn;
  double wmax = mx;
  int ndiv = divisions.getValue();
  sbAxisHPLOT.setTitle("");
  sbAxisHPLOT.paint(xmin,ymin,xmax,ymax,
                    wmin,wmax,ndiv, //Modified
                    chopt,gridlength,false,
                    fSubTicks,linesGrid,texts);}

  if(aLogScale==TRUE) {
    mn = FLOG10(mn);
    mx = FLOG10(mx);
  }

  float range = mx - mn;         

  int tick_number = texts.size();

  // HPLOT stores the magnitude on the last label :
  magnitude.setValue(0);
  if(tick_number) {
    int pmag;
    if(::sscanf(texts[tick_number-1].fString.c_str(),"x10^%d!",&pmag)==1) {
      magnitude.setValue(pmag);
      tick_number--;
    }
  }

  values.setNum(tick_number);
  coords.setNum(tick_number);
  tickNumber.setValue(tick_number);

  for(int index=0;index<tick_number;index++) {
    float coord = (float)texts[index].fX;
    //NOTE : are we sure that val is in [mn,mx]
    float val = (coord/width.getValue()) * range + mn;
    coords.set1Value(index,coord);
    values.set1Value(index,val);
  }

  if(labelsEnforced.getValue()==TRUE) {
    int n = labels.getNum();
    if(tick_number>n) {
      for(int index=n;index<tick_number;index++) {
        labels.set1Value(index,"");
      }
    }
  } else {
    labels.setNum(tick_number);
    for(int index=0;index<tick_number;index++) {
      labels.set1Value(index,texts[index].fString.c_str());
    }
  }
 
 {int subn = 0;
  subCoords.setNum(fSubTicks.size());
  for(unsigned int index=0;index<fSubTicks.size();index++) {
    float coord = fSubTicks[index].first[0];
    bool found = false;
    for(int i=0;i<tick_number;i++) {
      if((float)texts[i].fX==coord) {
        found = true;
        break;
      }
    }
    if(!found) { //not a main tick
      subCoords.set1Value(subn,coord);
      subn++;
    }
  }
  subCoords.setNum(subn);}
}

void SoAxis::generateAlternateRep(SoAction*) {
  alternateRep.setValue(topSeparator.getValue());
}

void SoAxis::clearAlternateRep() {alternateRep.setValue(NULL);}
