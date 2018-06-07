// this :
#include <HEPVis/nodekits/SoPage.h>

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
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
#include <HEPVis/SbColors.h>
#include <HEPVis/nodes/SoViewportRegion.h>
#include <HEPVis/nodes/SoImageWriter.h>
#include <HEPVis/nodes/SoTextStyle.h>
#include <HEPVis/nodekits/SoRegion.h>
#include <HEPVis/nodekits/SoTextRegion.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

SO_KIT_SOURCE(SoPage) 

void SoPage::initClass(){
  SO_KIT_INIT_CLASS(SoPage,SoCanvas,"SoCanvas");
}

SoPage::SoPage()
:fDisplayHeight(0)
,fOldWidth(0)
,fOldHeight(0)
,fOld_title(0)
,fOld_titleVisible(TRUE)
,fOld_titleHeight(0)
,fOld_titleGeometryAutomated(TRUE)
,fOld_regionsResizePolicy(-1)
,fOld_numberOfRegions(0)
,fOld_pageWidth(0)
,fOld_pageHeight(0)
,fOld_leftMargin(0)
,fOld_rightMargin(0)
,fOld_bottomMargin(0)
,fOld_topMargin(0)
,fOld_horizontalSpacing(0)
,fOld_verticalSpacing(0)
,fUpdate(FALSE)
{
  SO_KIT_CONSTRUCTOR(SoPage);
  SO_KIT_ADD_CATALOG_ENTRY(pageSeparator,SoSeparator,FALSE,this,\0, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(titleRegion,SoTextRegion,FALSE,pageSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(imageWriter,SoImageWriter,FALSE,pageSeparator,\0,TRUE);
  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(title,("HEPVis/SoPage"));
  SO_NODE_ADD_FIELD(titleVisible,(TRUE));
  SO_NODE_ADD_FIELD(titleHeight,(0.07F));
  SO_NODE_ADD_FIELD(titleGeometryAutomated,(TRUE));

  SO_NODE_ADD_FIELD(regionsResizePolicy,(LEAVE_ALONE));

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  // PAW_GRID parameters :
  float xfac = 1.0F/20.0F; //0.05
  float yfac = 1.0F/20.0F; //0.05
  // Take PAW defaults :
  float XSIZ = 20 * xfac;    //1     //page width
  float YSIZ = 20 * yfac;    //1     //page height
  float XMGL = 2 * xfac;     //0.1   //left x margin (to data frame).
  float XMGR = 2 * xfac;     //0.1   //right y margin (to data frame).
  float YMGL = 2 * yfac;     //0.1   //low y margin (to data frame).
  float YMGU = 2 * yfac;     //0.1   //up y margin (to data frame).

  float XWIN = 2 * xfac;     //0.1   //X space between WINdows
  float YWIN = 2 * yfac;     //0.1   //Y space between WINdows

  // Data area :
  //float wData = XSIZ-XMGL-XMGR;
  //float hData = YSIZ-YMGL-YMGU;
  //float dData = ZSIZ-ZMGD-ZMGU;

  SO_NODE_ADD_FIELD(pageWidth,(XSIZ));
  SO_NODE_ADD_FIELD(pageHeight,(YSIZ));
  SO_NODE_ADD_FIELD(leftMargin,(XMGL));
  SO_NODE_ADD_FIELD(rightMargin,(XMGR));
  SO_NODE_ADD_FIELD(bottomMargin,(YMGL));
  SO_NODE_ADD_FIELD(topMargin,(YMGU));
  SO_NODE_ADD_FIELD(horizontalSpacing,(XWIN));
  SO_NODE_ADD_FIELD(verticalSpacing,(YWIN));

  SO_NODE_DEFINE_ENUM_VALUE(ResizePolicy,LEAVE_ALONE);
  SO_NODE_DEFINE_ENUM_VALUE(ResizePolicy,PAW_GRID);
  SO_NODE_SET_SF_ENUM_TYPE(regionsResizePolicy,ResizePolicy);

  SoTextRegion* soTitleRegion = (SoTextRegion*)titleRegion.getValue();
  soTitleRegion->horizontalJustification.setValue(SoTextRegion::CENTER);
  SoViewportRegion* viewportRegion = soTitleRegion->getViewportRegion();
  viewportRegion->setPositionPercent(0,1-titleHeight.getValue());
  viewportRegion->setSizePercent(1,titleHeight.getValue());
  viewportRegion->horizontalBorder.setValue(0);
  viewportRegion->verticalBorder.setValue(0);

  soTitleRegion->getStyle(0)->color.setValue(SbColor_black);  
  soTitleRegion->getStyle(0)->smoothing.setValue(TRUE);  

  //display area :
  fDisplayHeight = 1 - titleHeight.getValue();  //titleVisible == TRUE

  SoRegion* soRegion = 
    createRegion("SoDisplayRegion",0,0,1,fDisplayHeight);
  soRegion->setPAW_Placement(1,1,0);
  highlightRegion(soRegion);

  fOld_title = new SoMFString;
}

SoPage::~SoPage(){delete fOld_title;}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoPage::doAction(SoAction* aAction){
  // In case of SoAlternateRepAction the below returns.
  // Then it is assumed that we do not need to call updateChildren()
  SO_ALTERNATEREP_DO_ACTION(aAction) 

  const SbViewportRegion& vp = SoViewportRegionElement::get(aAction->getState());
  //short vpx = vp.getViewportOriginPixels()[0];
  //short vpy = vp.getViewportOriginPixels()[1];
  short wvp = vp.getViewportSizePixels()[0];
  short hvp = vp.getViewportSizePixels()[1];
 
  if(wvp!=fOldWidth) {
    fOldWidth = wvp;
    fUpdate = TRUE;
  }

  if(hvp!=fOldHeight) {
    fOldHeight = hvp;
    fUpdate = TRUE;
  }

  updateChildren();

  //FIXME ? SoCanvas::doAction
}
#define ACTION(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::doAction(action);\
  aSuper::aMethod(action);\
}
ACTION(SoPage,SoCanvas,SoCallbackAction,callback)
ACTION(SoPage,SoCanvas,SoGLRenderAction,GLRender)
ACTION(SoPage,SoCanvas,SoGetBoundingBoxAction,getBoundingBox)
//ACTION(SoPage,SoCanvas,SoGetMatrixAction,getMatrix)
ACTION(SoPage,SoCanvas,SoHandleEventAction,handleEvent)
ACTION(SoPage,SoCanvas,SoRayPickAction,rayPick)
ACTION(SoPage,SoCanvas,SoPickAction,pick)

#define ACTION1(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  aClass::updateChildren();\
  aSuper::aMethod(action);\
}
ACTION1(SoPage,SoCanvas,SoGetMatrixAction,getMatrix)

#define ACTION2(aClass,aSuper,aAction,aMethod)\
void aClass::aMethod(aAction* action){\
  /*printf("debug : %s::%s\n",#aClass,#aMethod);*/\
  aClass::updateChildren();\
  aSuper::aMethod(action);\
}
ACTION2(SoPage,SoCanvas,SoSearchAction,search)
//ACTION2(SoPage,SoCanvas,SoWriteAction,write)
void SoPage::write(SoWriteAction* aAction) {
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    //NOTE : to avoid writing HEPVis SoSFEnum.
    //       But the below logic loose the fact
    //       that we had a SoPage.
    altRep->write(aAction);
  } else {
    SoPage::updateChildren();
    // Do not write callbackList.
    // Else at read time, we shall loose the callbacks
    // installed in the constructor.
    SbBool old = callbackList.isDefault();
    callbackList.setDefault(TRUE); //enforce not writing.
    SoCanvas::write(aAction);
    callbackList.setDefault(old);

  }
}

//NOTE : a refresh does not do a doAction that
//       in general handle changement in the size 
//       of the viewport of regions.
void SoPage::refresh(){
 {SoTextRegion* soTitleRegion = (SoTextRegion*)titleRegion.getValue();
  soTitleRegion->refresh();}
  raiseUpdateFlag();
  updateChildren();
  refreshRegions();
}

void SoPage::raiseUpdateFlag(){fUpdate = TRUE;}

#define CHECK_IF(a__field,a__flag) \
  if(fOld_##a__field!=a__field.getValue()) {\
    fOld_##a__field = a__field.getValue();\
    a__flag = TRUE;\
  }

void SoPage::updateChildren(){

  if((*fOld_title)!=title) {
    *fOld_title = title;
    fUpdate = TRUE;
  }

  CHECK_IF(titleVisible,fUpdate)
  CHECK_IF(titleHeight,fUpdate)
  CHECK_IF(titleGeometryAutomated,fUpdate)

  CHECK_IF(pageWidth,fUpdate)
  CHECK_IF(pageHeight,fUpdate)
  CHECK_IF(leftMargin,fUpdate)
  CHECK_IF(rightMargin,fUpdate)
  CHECK_IF(bottomMargin,fUpdate)
  CHECK_IF(topMargin,fUpdate)
  CHECK_IF(horizontalSpacing,fUpdate)
  CHECK_IF(verticalSpacing,fUpdate)

  SbBool update_regionsResizePolicy = FALSE;
  if(regionsResizePolicy.getValue()!=fOld_regionsResizePolicy) {
    fOld_regionsResizePolicy = regionsResizePolicy.getValue();
    fUpdate = TRUE;
    update_regionsResizePolicy = TRUE;
  }

 {int n = getTopSeparator()->getNumChildren();
  if(n!=fOld_numberOfRegions) {
    fOld_numberOfRegions = n;
    fUpdate = TRUE;
  }}

  if(fUpdate==FALSE) return;

  if(verbose.getValue()==TRUE) {
    SoDebugError::postInfo("SoPage::updateChildren","%lu : update",(unsigned long)this);
  }
  
  SoTextRegion* soTitleRegion = (SoTextRegion*)titleRegion.getValue();
  soTitleRegion->text = title;
  soTitleRegion->visible.setValue(titleVisible.getValue());

  float new_DisplayHeight = 0;
  if(titleGeometryAutomated.getValue()==TRUE) {
    float th = titleHeight.getValue();
    //FIXME : problems if some region has zero size.
    if(th<=0) th = 0.01F; 
    if(th>=1) th = 0.99F;

    SoViewportRegion* viewportRegion = soTitleRegion->getViewportRegion();
    viewportRegion->setSizePercent(1,th);
    viewportRegion->setPositionPercent(0,1-th);

    new_DisplayHeight = (titleVisible.getValue()==TRUE) ? 1-th:1;
  } else {
    new_DisplayHeight = 1;
  }

  if(regionsResizePolicy.getValue()==LEAVE_ALONE) {
    if(update_regionsResizePolicy==TRUE) resetGrid();

    SoSearchAction action;
    SoPathList& pathList = findRegions(action);

    int pathn = pathList.getLength();
    for(int index=0;index<pathn;index++) {
      SoFullPath* path = (SoFullPath*)pathList[index];
      if(!getPathRegion(*path)) { //root regions.
        SoRegion* soRegion = (SoRegion*)path->getTail();
        SoViewportRegion* viewportRegion = soRegion->getViewportRegion();
        float x,y;
        viewportRegion->getPositionPercent(x,y);
        viewportRegion->setPositionPercent(x,
                                           new_DisplayHeight/fDisplayHeight*y);
        float w,h;
        viewportRegion->getSizePercent(w,h);
        if((w<=0)&&(h<=0)) { //Never rendered.
        } else {
          viewportRegion->setSizePercent(w,
                                         new_DisplayHeight/fDisplayHeight*h);
        }
      }
    }

    fDisplayHeight = new_DisplayHeight;

  } else if(regionsResizePolicy.getValue()==PAW_GRID) {
    fDisplayHeight = new_DisplayHeight;

    float wvp = float(fOldWidth);
    float hvp = float(fOldHeight);
    if((wvp>0)&&(hvp>0)&&
       (pageWidth.getValue()>0)&&(pageHeight.getValue()>0)
      ) {
      //    wpix = w * wvp pixels
      //    hpix = h * hvp pixels
      //  wpix/hpix = cst = (w * wvp) / (h * hvp) = pageWidth / pageHeight
      float h,w,xo,yo;
      h = w = xo = yo = 0;
      float waspect = wvp/hvp;
      float paspect = pageWidth.getValue()/pageHeight.getValue();
      if(waspect>=paspect) {
        h = fDisplayHeight;
        w = paspect * h * (hvp/wvp);
        xo = (1-w)/2;
        yo = 0;
      } else {
        w = 1;
        h = w * (wvp/hvp)/paspect;
        xo = 0;
        yo = (1-h)/2;
      }
      SoSearchAction action;
      SoPathList& pathList = findRegions(action);
      int pathn = pathList.getLength(); 
      for(int index=0;index<pathn;index++) {
        SoFullPath* path = (SoFullPath*)pathList[index];
        if(!getPathRegion(*path)) { //root regions.
          SoRegion* soRegion = (SoRegion*)path->getTail();
          int rows,cols,iregion;
          soRegion->getPAW_Placement(rows,cols,iregion);
          int n = rows * cols;
          if((rows>0)&&(cols>0)&&(iregion>=0)&&(iregion<n)) {
            //iregion = col + row * cols
            int row = iregion/cols;
            int col = iregion - row * cols;
            SoViewportRegion* viewportRegion = soRegion->getViewportRegion();
            float wData = (pageWidth.getValue() - 
                           leftMargin.getValue() - rightMargin.getValue() -
                           (cols-1)*horizontalSpacing.getValue())/cols;
            float hData = (pageHeight.getValue() - 
                           bottomMargin.getValue() - topMargin.getValue() -
                           (rows-1)*verticalSpacing.getValue())/rows;
            if((wData>0)&&(hData>0)) {
              float wr,hr,x,y,lm,rm,bm,tm;
              wr = hr = x = y = 0;
              lm = rm = bm = tm = 0;
              if(cols==1) {
                wr = pageWidth.getValue();
                x = 0;
                lm = leftMargin.getValue();
                rm = rightMargin.getValue();
              } else {
                float wrl = 
                  leftMargin.getValue()+wData+horizontalSpacing.getValue()/2;
                float wrr = 
                  rightMargin.getValue()+wData+horizontalSpacing.getValue()/2;
                float wri = wData+horizontalSpacing.getValue();
                if(col==0) {
                  wr = wrl;
                  x = 0;
                  lm = leftMargin.getValue();
                  rm = horizontalSpacing.getValue()/2;
                } else if(col==(cols-1)) {
                  wr = wrr;
                  x = pageWidth.getValue() - wrr;
                  lm = horizontalSpacing.getValue()/2;
                  rm = rightMargin.getValue();
                } else {
                  wr = wri;
                  x = wrl + (col-1) * wri;
                  lm = horizontalSpacing.getValue()/2;
                  rm = horizontalSpacing.getValue()/2;
                }
              }
              float xfac = w / pageWidth.getValue();
 
              if(rows==1) {
                hr = pageHeight.getValue();
                y = 0;
                tm = topMargin.getValue();
                bm = bottomMargin.getValue();
              } else {
                float hrt =  //top
                  topMargin.getValue()+hData+verticalSpacing.getValue()/2;
                float hrb =  //bottom
                   bottomMargin.getValue()+hData+verticalSpacing.getValue()/2;
                float hri = hData+verticalSpacing.getValue();
                if(row==0) { //top row.
                  hr = hrt;
                  y = pageHeight.getValue()-hrt;
                  tm = topMargin.getValue();
                  bm = verticalSpacing.getValue()/2;
                } else if(row==(rows-1)) {
                  hr = hrb;
                  y = 0;
                  tm = verticalSpacing.getValue()/2;
                  bm = bottomMargin.getValue();
                } else {
                  hr = hri;
                  y = pageHeight.getValue()- (hrt + row * hri);
                  tm = verticalSpacing.getValue()/2;
                  bm = verticalSpacing.getValue()/2;
                }
              }
              float yfac = h / pageHeight.getValue();

              viewportRegion->setPositionPercent(xo+x*xfac,yo+y*yfac);
              viewportRegion->setSizePercent(wr*xfac,hr*yfac);

              soRegion->placeInPage(pageWidth.getValue(),pageHeight.getValue(),
                                    wr,hr,
                                    lm,rm,tm,bm);
            }

         /* ignore margins and spacings.
            float wr = w/cols;
            float hr = h/rows; 
            float x = xo + col * wr;
            float y = yo + (rows-1-row) * hr;
            viewportRegion->setPositionPercent(x,y);
            viewportRegion->setSizePercent(wr,hr); */

          }
        }
      }
    }
  } 

  fUpdate = FALSE;
}

SoImageWriter* SoPage::getImageWriter() const{
  return (SoImageWriter*)imageWriter.getValue();
}

SoTextRegion* SoPage::getTitleRegion() const{
  return (SoTextRegion*)titleRegion.getValue();
}

void SoPage::createRegions(const SbString& aClass,int aCols,int aRows,int aIndex){
  // First region is top left :
  //     c=0 c=1 c=2
  // r=0 0   1   2
  // r=1 3   4   5

  deleteRegions();
  fUpdate = TRUE;

  if((aRows<=0)||(aCols<=0)) return;

  // Create new ones :
  float wr = 1.0F/aCols;
  float hr = fDisplayHeight/aRows;
  float x = 0;
  float y = hr * (aRows - 1);

  int iregion = 0;
  for(int row=0;row<aRows;row++) {
    for(int col=0;col<aCols;col++) {
      SoRegion* soRegion = createRegion(aClass,x,y,wr,hr);
      soRegion->setPAW_Placement(aRows,aCols,iregion);
      iregion++;
      x += wr;
    }
    y -= hr;
    x = 0;
  }

  SoRegion* soRegion = getRootRegion(aIndex);
  if(soRegion) highlightRegion(soRegion);
}

void SoPage::setTitleVisible(SbBool aValue) {
  titleVisible.setValue(aValue);
  refresh();
}

void SoPage::createRegions(int aCols,int aRows,int aIndex){
  createRegions("SoDisplayRegion",aCols,aRows,aIndex);
}

void SoPage::addRegions(const SbString& aClass,int aCols,int aRows){
  // Do not delete existing regions.
  float aX = 0;
  float aY = 0;
  float aWidth = 1;
  float aHeight = fDisplayHeight;                           

  if(aRows<=0) aRows = 1;
  if(aCols<=0) aCols = 1;

  // Remove unecessary ones :
 {SoNodeList nodeList;
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength();
  int wanted = aRows * aCols;
  int iroot = 0;
  int index = 0;
  for(index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    if(!getPathRegion(*path)) { //Root region.
      SoRegion* soRegion = (SoRegion*)path->getTail();
      if(iroot>=wanted) {
        nodeList.append(soRegion); // To be removed :
      } else {
        // Check class :
        if(soRegion->getTypeId().getName()!=SbName(aClass.getString())) {
          nodeList.append(soRegion); // To be removed :
        }
      }
      iroot++;
    }
  } 
  disableRegionCamera();
  int number = nodeList.getLength();
  for(index=0;index<number;index++) {
    deleteRegion((SoRegion*)nodeList[index]);
  }}

  int number = getNumberOfRootRegions();

  // Create new ones :
  float wr = aWidth/aCols;
  float hr = aHeight/aRows;
  float x = aX;
  float y = aY + hr * (aRows - 1);
  int iregion = 0;
  SoRegion* soRegion = 0;
  for(int row=0;row<aRows;row++) {
    for(int col=0;col<aCols;col++) {
      if(iregion<number) {
        // Do not destroy existing region (and its content !) :
        soRegion = getRootRegion(iregion);
        SoViewportRegion* viewportRegion  = 
          (SoViewportRegion*)soRegion->getViewportRegion();
        viewportRegion->setPositionPercent(x,y);
        viewportRegion->setSizePercent(wr,hr);
        //last = askMore ? iregion + 1 : iregion;
      } else {
        soRegion = createRegion(aClass,x,y,wr,hr);
        soRegion->setPAW_Placement(aRows,aCols,iregion);
      }
      iregion++;
      x += wr;
    }
    y -= hr;
    x = aX;
  }

  highlightRegion(soRegion);

  fUpdate = TRUE;
}

SoRegion* SoPage::createGridRegion(const SbString& aClass,int aCols,int aRows,int aIndex){
  // Create one region. Size and position is given by specifiying
  // the region index in a grid.
  // First region is top left :
  //  0 1 2
  //  3 4 5
  if(aRows<=0) return 0;
  if(aCols<=0) return 0;
  int n = aRows * aCols;
  if((aIndex<0)||(aIndex>=n)) return 0;
  SoRegion* soRegion = createRegion(aClass,0,0,1,1);
  soRegion->setPAW_Placement(aRows,aCols,aIndex);
  return soRegion;
}

void SoPage::mapRegionToPage(SoRegion* aRegion){
  // It is assumed that aRegion is a valid one !
  aRegion->ref();
  deleteRegions();
  regionsResizePolicy.setValue(LEAVE_ALONE);
  
  aRegion->setPAW_Placement(-1,-1,-1);

  SoSeparator* sep = getTopSeparator();
  sep->addChild(aRegion);
  aRegion->unref();

  SoViewportRegion* viewportRegion  = 
    (SoViewportRegion*)aRegion->getViewportRegion();
  viewportRegion->setPositionPercent(0,0);
  viewportRegion->setSizePercent(1,fDisplayHeight);

  highlightRegion(aRegion);

  fUpdate = TRUE;
}

void SoPage::mapHighlightedRegionToPage(){
  SoRegion* soRegion = getHighlightedRegion();
  if(!soRegion) return;
  mapRegionToPage(soRegion);
}

void SoPage::resetGrid(){
  SoSearchAction action;
  SoPathList& pathList = findRegions(action);
  int pathn = pathList.getLength(); 
  for(int index=0;index<pathn;index++) {
    SoFullPath* path = (SoFullPath*)pathList[index];
    if(!getPathRegion(*path)) { //root regions.
      SoRegion* soRegion = (SoRegion*)path->getTail();
      int rows,cols,iregion;
      soRegion->getPAW_Placement(rows,cols,iregion);
      int n = rows * cols;
      if((rows>0)&&(cols>0)&&(iregion>=0)&&(iregion<n)) {
        //iregion = col + row * cols
        int row = iregion/cols;
        int col = iregion - row * cols;
        SoViewportRegion* viewportRegion = soRegion->getViewportRegion();
        float wr = 1.0F/cols;
        float hr = fDisplayHeight/rows; 
        float x = col * wr;
        float y = row * hr;
        viewportRegion->setPositionPercent(x,y);
        viewportRegion->setSizePercent(wr,hr);
      }
    }
  }
}

/*
void SoPage::enforceWinSize(const SbVec2s& aSize){
  //used with SoOffscreenRenderer.
  SoCanvas::enforceWinSize(aSize);
  SoTextRegion* soTitleRegion = (SoTextRegion*)titleRegion.getValue();
  SoViewportRegion* viewportRegion = soTitleRegion->getViewportRegion();
  viewportRegion->enforceWinSize(aSize);
}
*/

void SoPage::generateAlternateRep(SoAction* aAction){
  static bool message = false;
  if(!message) {
    message = true;
    SoDebugError::postInfo("SoPage::generateAlternateRep",
      "for the moment, we produce an alternate rep of the highlighted region only.");
  }

  // Since we do not have an alternate rep
  // for the SoViewportRegion, we save
  // today only the highlighted region.

  SoSeparator* soSep = new SoSeparator;

  SoRegion* soRegion = SoCanvas::getHighlightedRegion();
  if(soRegion!=NULL) soSep->addChild(soRegion);

  alternateRep.setValue(soSep);
}

void SoPage::clearAlternateRep(){alternateRep.setValue(NULL);}
