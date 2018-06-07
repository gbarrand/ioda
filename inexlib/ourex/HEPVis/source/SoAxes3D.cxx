
/*
 *  SoAxes3D.cxx
 *  
 *
 *  Created by Laurent Garnier on 06/01/2004.
 *
 */

// this :  
#include <HEPVis/nodekits/SoAxes3D.h>

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
#define zAxisNode ((SoAxis*)zAxis.getValue())

SO_KIT_SOURCE(SoAxes3D)

//////////////////////////////////////////////////////////////////////////////
void SoAxes3D::initClass(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{ 
  SO_KIT_INIT_CLASS(SoAxes3D,SoBaseKit,"BaseKit");
}
//////////////////////////////////////////////////////////////////////////////
SoAxes3D::SoAxes3D(
) 
:fOld_gridColor(SbColor(0,0,0))
,fOld_gridTransparency(0)
,fOld_gridLinePattern(SbLinePattern_solid)
,fOld_gridVisible(FALSE)
,fOld_wallColor(SbColor(0,0,0))
,fOld_wallTransparency(0)
,fOld_wallOffset(0)
,fOld_wallVisible(FALSE)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoAxes3D);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0, FALSE);


  SO_KIT_ADD_CATALOG_ENTRY(xAxisSeparator,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(xAxisTransform,SoTransform,FALSE,xAxisSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(xAxis,SoAxis,FALSE,xAxisSeparator, \0,TRUE);


  SO_KIT_ADD_CATALOG_ENTRY(yAxisSeparator,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(yAxisTransform,SoTransform,FALSE,yAxisSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(yAxis,SoAxis,FALSE,yAxisSeparator, \0,TRUE);


  SO_KIT_ADD_CATALOG_ENTRY(zAxisSeparator,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(zAxisTransform,SoTransform,FALSE,zAxisSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(zAxis,SoAxis,FALSE,zAxisSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(grid,SoSeparator,FALSE,topSeparator, \0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(wall,SoSeparator,FALSE,topSeparator, \0,TRUE);

  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(gridColor,(SbColor(0.5f,0.5f,0.5f)));
  SO_NODE_ADD_FIELD(gridTransparency,(0));
  SO_NODE_ADD_FIELD(gridLinePattern,(SbLinePattern_solid));
  SO_NODE_ADD_FIELD(gridVisible,(TRUE));

  SO_NODE_ADD_FIELD(wallColor,(SbColor(0.5f,0.5f,0.5f)));
  SO_NODE_ADD_FIELD(wallTransparency,(0));
  SO_NODE_ADD_FIELD(wallOffset,(0.01f));
  SO_NODE_ADD_FIELD(wallVisible,(FALSE));

  xAxisNode->tickUp.setValue(FALSE);
 {SoTextStyle* style = xAxisNode->getLabelsStyle(); 
  style->fontName.setValue(SbFont_Hershey); //Enforce Hershey.
  style->orientationX.setValue(SbVec3f(1,0,0));
  style->orientationY.setValue(SbVec3f(0,1,0));
  style->horizontalJustification.setValue(SoTextStyle::CENTER);
  style->verticalJustification.setValue(SoTextStyle::TOP);}
 {SoTextStyle* style = xAxisNode->getTitleStyle(); 
  style->orientationX.setValue(SbVec3f(1,0,0));
  style->orientationY.setValue(SbVec3f(0,1,0));
  style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  style->verticalJustification.setValue(SoTextStyle::TOP);}
 //{SoTextStyle* style = xAxisNode->getMagStyle(); 
 // style->horizontalJustification.setValue(SoTextStyle::LEFT);
 // style->verticalJustification.setValue(SoTextStyle::MIDDLE);}


  yAxisNode->tickUp.setValue(FALSE);
 {SoTextStyle* style = yAxisNode->getLabelsStyle(); 
  style->fontName.setValue(SbFont_Hershey); //Enforce Hershey.
  style->orientationX.setValue(SbVec3f(-1,0,0));
  style->orientationY.setValue(SbVec3f( 0,1,0));
  style->horizontalJustification.setValue(SoTextStyle::CENTER);
  style->verticalJustification.setValue(SoTextStyle::TOP);}
 {SoTextStyle* style = yAxisNode->getTitleStyle(); 
  style->orientationX.setValue(SbVec3f(-1,0,0));
  style->orientationY.setValue(SbVec3f( 0,1,0));
  style->horizontalJustification.setValue(SoTextStyle::LEFT);
  style->verticalJustification.setValue(SoTextStyle::TOP);}
 //{SoTextStyle* style = yAxisNode->getMagStyle(); 
 // style->horizontalJustification.setValue(SoTextStyle::CENTER);
 // style->verticalJustification.setValue(SoTextStyle::BOTTOM);}

  zAxisNode->tickUp.setValue(FALSE);
 {SoTextStyle* style = zAxisNode->getLabelsStyle(); 
  style->fontName.setValue(SbFont_Hershey); //Enforce Hershey.
  style->orientationX.setValue(SbVec3f(0,1,0));
  style->orientationY.setValue(SbVec3f(1,0,0));
  style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  style->verticalJustification.setValue(SoTextStyle::MIDDLE);}
 {SoTextStyle* style = zAxisNode->getTitleStyle(); 
  style->orientationX.setValue(SbVec3f(0,1,0));
  style->orientationY.setValue(SbVec3f(1,0,0));
  style->horizontalJustification.setValue(SoTextStyle::RIGHT);
  style->verticalJustification.setValue(SoTextStyle::BOTTOM);}
 //{SoTextStyle* style = zAxisNode->getMagStyle(); 
 // style->horizontalJustification.setValue(SoTextStyle::CENTER);
 // style->verticalJustification.setValue(SoTextStyle::BOTTOM);}

 {SoTransform* transf = (SoTransform*)xAxisTransform.getValue();
  SbMatrix rotx;
  rotx.setRotate(SbRotation(SbVec3f(1,0,0),FM_PI_2));
  transf->multLeft(rotx);}

 {SoTransform* transf = (SoTransform*)yAxisTransform.getValue();
  SbMatrix roty;
  roty.setRotate(SbRotation(SbVec3f(0,1,0),FM_PI_2));
  transf->multLeft(roty);
  SbMatrix rotz;
  rotz.setRotate(SbRotation(SbVec3f(0,0,1),FM_PI_2));
  transf->multLeft(rotz);}

 {SoTransform* transf = (SoTransform*)zAxisTransform.getValue();
  SbMatrix trans;
  trans.setTranslate(SbVec3f(0,yAxisNode->width.getValue(),0));
  transf->multLeft(trans);
  SbMatrix rotz;
  rotz.setRotate(SbRotation(SbVec3f(0,0,1),-FM_PI_2));
  transf->multLeft(rotz);
  SbMatrix roty;
  roty.setRotate(SbRotation(SbVec3f(0,1,0),-FM_PI_2));
  transf->multLeft(roty);} //Applied first.
}
//////////////////////////////////////////////////////////////////////////////
SoAxes3D::~SoAxes3D(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SoAxes3D::doAction(
 SoAction* //aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  //printf("debug : SoAxes3D::doAction\n");

  SbBool update = FALSE;

  SbBool update_grid = FALSE;
  SbBool update_wall = FALSE;

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

  if(update==FALSE) return;

  if(update_grid==TRUE) {
    update_grid = FALSE;
    addGridToAxis();
  }
  if(update_wall==TRUE) {
    update_wall = FALSE;
    addWallToAxis();
  }

  // Update transformations (here the translation) :
 {SoTransform* transf = (SoTransform*)zAxisTransform.getValue();
  SbMatrix identity;
  identity.makeIdentity();
  transf->setMatrix(identity);
  SbMatrix trans;
  trans.setTranslate(SbVec3f(0,yAxisNode->width.getValue(),0));
  transf->multLeft(trans);
  SbMatrix rotz;
  rotz.setRotate(SbRotation(SbVec3f(0,0,1),-FM_PI_2));
  transf->multLeft(rotz);
  SbMatrix roty;
  roty.setRotate(SbRotation(SbVec3f(0,1,0),-FM_PI_2));
  transf->multLeft(roty);} //Applied first.
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes3D::GLRender (
 SoGLRenderAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxes3D::doAction(aAction);
  SoBaseKit::GLRender(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes3D::callback (
 SoCallbackAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxes3D::doAction(aAction);
  SoBaseKit::callback(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes3D::rayPick (
 SoRayPickAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxes3D::doAction(aAction);
  SoBaseKit::rayPick(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes3D::pick (
 SoPickAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxes3D::doAction(aAction);
  SoBaseKit::pick(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes3D::getBoundingBox (
 SoGetBoundingBoxAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoAxes3D::doAction(aAction);
  SoBaseKit::getBoundingBox(aAction);
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SoAxis* SoAxes3D::getXAxis(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return xAxisNode;
}
//////////////////////////////////////////////////////////////////////////////
SoAxis* SoAxes3D::getYAxis(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return yAxisNode;
}
//////////////////////////////////////////////////////////////////////////////
SoAxis* SoAxes3D::getZAxis(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return zAxisNode;
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes3D::addGridToAxis(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef DEBUG
  printf("SoAxes3D ::  update grid Visible = %d\n",gridVisible.getValue());
#endif

  ((SoSeparator*)grid.getValue())->removeAllChildren();
  if(gridVisible.getValue()==TRUE) {
    int ntick = zAxisNode->tickNumber.getValue();
    SoMaterial* material = new SoMaterial;
    material->diffuseColor.setValue(gridColor.getValue());
    material->transparency.setValue(gridTransparency.getValue());
    ((SoSeparator*)grid.getValue())->addChild(material);

    SoDrawStyle* drawStyle = new SoDrawStyle;
    drawStyle->style.setValue(SoDrawStyle::LINES);
    drawStyle->linePattern.setValue(gridLinePattern.getValue());
    ((SoSeparator*)grid.getValue())->addChild(drawStyle);

    if(ntick>0) {
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
        ((SoSeparator*)grid.getValue())->addChild(coordinate3);

        SoLineSet* lineSet = new SoLineSet;
        int segmentn = pointn/2;
        int32_t* vertices = new int32_t[segmentn];
        for (int count=0;count<segmentn;count++) vertices[count] = 2;
        lineSet->numVertices.setValues(0,segmentn,vertices);
        delete [] vertices;
        ((SoSeparator*)grid.getValue())->addChild(lineSet);
      }
      delete [] points;
    }
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes3D::addWallToAxis(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef DEBUG
  printf("SoAxes3D ::   update wall visible = %d\n",wallVisible.getValue());  
#endif

  ((SoSeparator*)wall.getValue())->removeAllChildren();
  if (wallVisible.getValue() == TRUE) {
    SoMaterial* material = new SoMaterial;
    material->diffuseColor.setValue(wallColor.getValue());
    material->transparency.setValue(wallTransparency.getValue());
    ((SoSeparator*)wall.getValue())->addChild(material);
      
    SoDrawStyle* drawStyle = new SoDrawStyle;
    drawStyle->style.setValue(SoDrawStyle::FILLED);
    ((SoSeparator*)wall.getValue())->addChild(drawStyle);
      
    float woffset = wallOffset.getValue();
    //float woffset = 0;
    buildWall(SbVec3f(0,0,-woffset),
              SbVec3f(1+woffset,0,0),
              SbVec3f(0,1+woffset,0),
              xAxisNode->width.getValue()+woffset,
              yAxisNode->width.getValue()+woffset,
              ((SoSeparator*)wall.getValue()));
    // plan z-y at x = 1.
    buildWall(SbVec3f(1+woffset,0,-woffset),
              SbVec3f(0,0,1),
              SbVec3f(0,1,0),
              zAxisNode->width.getValue()+woffset,
              yAxisNode->width.getValue()+woffset,
              ((SoSeparator*)wall.getValue()) );
    // plan x-z at y = 1.
    buildWall(SbVec3f(0,1+woffset,-woffset),
              SbVec3f(1,0,0),
              SbVec3f(0,0,1),
              xAxisNode->width.getValue()+woffset,
              zAxisNode->width.getValue()+woffset,
              ((SoSeparator*)wall.getValue()));
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoAxes3D::buildWall(
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
