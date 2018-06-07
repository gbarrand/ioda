/*
 *  SoAxisGroup.cxx
 *  
 *
 *  Created by Laurent Garnier on 06/01/2004.
 *
 */

// this :  
#include <HEPVis/nodekits/SoAxes2D.h>

// Inventor :
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

// HEPVis :
#include <HEPVis/SbMath.h>
#include <HEPVis/nodekits/SoAxis.h>
#include <HEPVis/nodes/SoGrid.h>
#include <HEPVis/nodes/SoTextStyle.h>

// #define DEBUG

#define LIST_SET(list,number,x,y,z) list[number].setValue((x),(y),(z))

#define xAxisNode ((SoAxis*)xAxis.getValue())
#define yAxisNode ((SoAxis*)yAxis.getValue())

SO_KIT_SOURCE(SoAxes2D)

//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::initClass(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{ 
  SO_KIT_INIT_CLASS(SoAxes2D,SoBaseKit,"BaseKit");
}
//////////////////////////////////////////////////////////////////////////////
SoAxes2D::SoAxes2D(
) 
:fOld_gridColor(SbColor(0,0,0))
,fOld_gridTransparency(0)
,fOld_gridLinePattern(SbLinePattern_solid)
,fOld_gridVisible(FALSE)
,fOld_wallColor(SbColor(0,0,0))
,fOld_wallTransparency(0)
,fOld_wallOffset(0)
,fOld_wallVisible(FALSE)
,fOld_topAxisVisible(FALSE)
,fOld_topAxisY(0)
,fOld_rightAxisVisible(FALSE)
,fOld_rightAxisX(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoAxes2D);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(xAxis,SoAxis,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(yAxisSeparator,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(yAxisTransform,SoTransform,FALSE,yAxisSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(yAxis,SoAxis,FALSE,yAxisSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(grid,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(wall,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(topAxis,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(rightAxis,SoSeparator,FALSE,topSeparator, \0,TRUE);

  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(gridColor,(SbColor(0.5f,0.5f,0.5f)));
  SO_NODE_ADD_FIELD(gridTransparency,(0));
  SO_NODE_ADD_FIELD(gridLinePattern,(SbLinePattern_solid));
  SO_NODE_ADD_FIELD(gridVisible,(TRUE));

  SO_NODE_ADD_FIELD(wallColor,(SbColor(0.5f,0.5f,0.5f)));
  SO_NODE_ADD_FIELD(wallTransparency,(0));
  SO_NODE_ADD_FIELD(wallOffset,(0));
  SO_NODE_ADD_FIELD(wallVisible,(TRUE));

  SO_NODE_ADD_FIELD(topAxisVisible,(FALSE));
  SO_NODE_ADD_FIELD(topAxisY,(1));
  SO_NODE_ADD_FIELD(rightAxisVisible,(FALSE));
  SO_NODE_ADD_FIELD(rightAxisX,(1));

  xAxisNode->tickUp.setValue(TRUE);
 {SoTextStyle* style = xAxisNode->getLabelsStyle(); 
  style->orientationX.setValue(SbVec3f(1,0,0));
  style->orientationY.setValue(SbVec3f(0,1,0));
  style->horizontalJustification.setValue(SoTextStyle::CENTER);
  style->verticalJustification.setValue(SoTextStyle::TOP);}
 {SoTextStyle* style = xAxisNode->getTitleStyle(); 
  style->orientationX.setValue(SbVec3f(1,0,0));
  style->orientationY.setValue(SbVec3f(0,1,0));
  style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  style->verticalJustification.setValue(SoTextStyle::TOP);}
 {SoTextStyle* style = xAxisNode->getMagStyle(); 
  style->horizontalJustification.setValue(SoTextStyle::LEFT);
  style->verticalJustification.setValue(SoTextStyle::MIDDLE);}

  yAxisNode->tickUp.setValue(TRUE);
 {SoTextStyle* style = yAxisNode->getLabelsStyle(); 
  style->orientationX.setValue(SbVec3f(0,1,0));
  style->orientationY.setValue(SbVec3f(1,0,0));
  style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  style->verticalJustification.setValue(SoTextStyle::MIDDLE);}

 {SoTextStyle* style = yAxisNode->getTitleStyle(); 
  style->rotated.setValue(TRUE);
  style->orientationX.setValue(SbVec3f(1, 0,0));
  style->orientationY.setValue(SbVec3f(0,-1,0));
  style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  style->verticalJustification.setValue(SoTextStyle::BOTTOM);}
 {SoTextStyle* style = yAxisNode->getMagStyle(); 
  style->horizontalJustification.setValue(SoTextStyle::CENTER);
  style->verticalJustification.setValue(SoTextStyle::BOTTOM);}

 {SoTransform* transf = (SoTransform*)yAxisTransform.getValue();
  SbMatrix roty;
  roty.setRotate(SbRotation(SbVec3f(0,1,0),FM_PI));
  transf->multLeft(roty);
  SbMatrix rotz;
  rotz.setRotate(SbRotation(SbVec3f(0,0,1),FM_PI_2));
  transf->multLeft(rotz);} //Applied first.*/
}
//////////////////////////////////////////////////////////////////////////////
SoAxes2D::~SoAxes2D(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::doAction(
 SoAction* //aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  //printf("debug : SoAxes2D::doAction\n");

  SbBool update = FALSE;

  SbBool update_grid = FALSE;
  SbBool update_wall = FALSE;
  SbBool update_topAxis = FALSE;
  SbBool update_rightAxis = FALSE;

  if(fOld_gridColor!=gridColor.getValue()) {
    fOld_gridColor = gridColor.getValue();
    update = TRUE;
    update_grid = TRUE;
  }
  if(fOld_gridTransparency!=gridTransparency.getValue()) {
    fOld_gridTransparency = gridTransparency.getValue();
    update = TRUE;
    update_grid = TRUE;
  }
  if(fOld_gridLinePattern!=gridLinePattern.getValue()) {
    fOld_gridLinePattern = gridLinePattern.getValue();
    update = TRUE;
    update_grid = TRUE;
  }
  if(fOld_gridVisible!=gridVisible.getValue()) {
    fOld_gridVisible = gridVisible.getValue();
    update = TRUE;
    update_grid = TRUE;
  }
  //FIXME : update only if axes had changed.
  update_grid = TRUE;

  if(fOld_wallColor!=wallColor.getValue()) {
    fOld_wallColor = wallColor.getValue();
    update = TRUE;
    update_wall = TRUE;
  }
  if(fOld_wallTransparency!=wallTransparency.getValue()) {
    fOld_wallTransparency = wallTransparency.getValue();
    update = TRUE;
    update_wall = TRUE;
  }
  if(fOld_wallOffset!=wallOffset.getValue()) {
    fOld_wallOffset = wallOffset.getValue();
    update = TRUE;
    update_wall = TRUE;
  }
  if(fOld_wallVisible!=wallVisible.getValue()) {
    fOld_wallVisible = wallVisible.getValue();
    update = TRUE;
    update_wall = TRUE;
  }

  if(fOld_topAxisVisible!=topAxisVisible.getValue()) {
    fOld_topAxisVisible = topAxisVisible.getValue();
    update = TRUE;
    update_topAxis = TRUE;
  }
  if(fOld_topAxisY!=topAxisY.getValue()) {
    fOld_topAxisY = topAxisY.getValue();
    update = TRUE;
    update_topAxis = TRUE;
  }

  if(fOld_rightAxisVisible!=rightAxisVisible.getValue()) {
    fOld_rightAxisVisible = rightAxisVisible.getValue();
    update = TRUE;
    update_rightAxis = TRUE;
  }
  if(fOld_rightAxisX!=rightAxisX.getValue()) {
    fOld_rightAxisX = rightAxisX.getValue();
    update = TRUE;
    update_rightAxis = TRUE;
  }

  if(update==FALSE) return;

  if(update_grid == TRUE) {
    update_grid = FALSE;
    addGrid();
  }
  if (update_wall == TRUE) {
    update_wall = FALSE;
    addWall();
  }
  if (update_topAxis == TRUE) {
    update_topAxis = FALSE;
    addTopAxis();
  }
  if (update_rightAxis == TRUE) {
    update_rightAxis = FALSE;
    addRightAxis();
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::GLRender (
 SoGLRenderAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxes2D::doAction(aAction);
  SoBaseKit::GLRender(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::callback (
 SoCallbackAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxes2D::doAction(aAction);
  SoBaseKit::callback(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::rayPick (
 SoRayPickAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxes2D::doAction(aAction);
  SoBaseKit::rayPick(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::pick (
 SoPickAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxes2D::doAction(aAction);
  SoBaseKit::pick(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::getBoundingBox (
 SoGetBoundingBoxAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxes2D::doAction(aAction);
  SoBaseKit::getBoundingBox(aAction);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SoAxis* SoAxes2D::getXAxis(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return xAxisNode;
}
//////////////////////////////////////////////////////////////////////////////
SoAxis* SoAxes2D::getYAxis(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return yAxisNode;
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::addGrid(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef DEBUG
  printf("SoAxes2D ::  update grid Visible = %d\n",gridVisible.getValue());
#endif
  SoSeparator* sep = (SoSeparator*)grid.getValue();
  sep->removeAllChildren();
  if(gridVisible.getValue()==TRUE) {
    int pos = 0;
    int number =  xAxisNode->tickNumber.getValue() +
      yAxisNode->tickNumber.getValue();

    SoMaterial* material = new SoMaterial;
    material->diffuseColor.setValue(gridColor.getValue());
    material->transparency.setValue(gridTransparency.getValue());
    sep->addChild(material);

    SoDrawStyle* drawStyle = new SoDrawStyle;
    drawStyle->style.setValue(SoDrawStyle::LINES);
    drawStyle->linePattern.setValue(gridLinePattern.getValue());

    sep->addChild(drawStyle);

    if(number>0) {
      float gridOffset = 0.01f; //PLANE_OFFSET
      SbVec3f* points = new SbVec3f[2 * number];
      number = xAxisNode->tickNumber.getValue();
      for(int count=0;count<number;count++) { 
        float xx = xAxisNode->coords[count];
        LIST_SET(points,pos,xx, 0,gridOffset);pos++;
        LIST_SET(points,pos,xx, yAxisNode->width.getValue(),gridOffset);pos++;
      }
      number = yAxisNode->tickNumber.getValue();
      for(int count=0;count<number;count++) {
        float yy = yAxisNode->coords[count];
        LIST_SET(points,pos,0,yy,gridOffset);pos++;
        LIST_SET(points,pos,xAxisNode->width.getValue(),yy,gridOffset);pos++;
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
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::addWall(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef DEBUG
  printf("SoAxes2D ::   update wall visible = %d\n",wallVisible.getValue());  
#endif
  SoSeparator* sep = (SoSeparator*)wall.getValue();
  sep->removeAllChildren();
  if (wallVisible.getValue() == TRUE) {
    SoMaterial* material = new SoMaterial;
    material->diffuseColor.setValue(wallColor.getValue());
    material->transparency.setValue(wallTransparency.getValue());
    sep->addChild(material);
      
    SoDrawStyle* drawStyle = new SoDrawStyle;
    drawStyle->style.setValue(SoDrawStyle::FILLED);
    sep->addChild(drawStyle);
      
    buildWall(SbVec3f(0,0,-wallOffset.getValue()),
              SbVec3f(1,0,0),SbVec3f(0,1,0),
              xAxisNode->width.getValue(),
              yAxisNode->width.getValue(),
              sep);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::buildWall(
 const SbVec3f& aVertex
,const SbVec3f& aX
,const SbVec3f& aY
,float aSx  
,float aSy
,SoSeparator* aWall
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
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
//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::addTopAxis(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef DEBUG
  printf("SoAxes2D ::  update grid Visible = %d\n",gridVisible.getValue());
#endif
  SoSeparator* sep = (SoSeparator*)topAxis.getValue();
  sep->removeAllChildren();
  if(topAxisVisible.getValue()==TRUE) {
    SoTransform* transf = new SoTransform;
    transf->translation.setValue(SbVec3f(0,topAxisY.getValue(),0));
    sep->addChild(transf);

    SoAxis* axis = (SoAxis*)xAxisNode->copy();
    SoTextStyle* style = axis->getLabelsStyle(); 
    style->visible.setValue(FALSE);
    axis->tickUp.setValue(FALSE);
    axis->title.setValue("");
    sep->addChild(axis);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes2D::addRightAxis(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef DEBUG
  printf("SoAxes2D ::  update grid Visible = %d\n",gridVisible.getValue());
#endif
  SoSeparator* sep = (SoSeparator*)rightAxis.getValue();
  sep->removeAllChildren();
  if(rightAxisVisible.getValue()==TRUE) {
    SoTransform* transf = new SoTransform;
    SbMatrix trans;
    trans.setTranslate(SbVec3f(rightAxisX.getValue(),0,0));
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
  }
}
