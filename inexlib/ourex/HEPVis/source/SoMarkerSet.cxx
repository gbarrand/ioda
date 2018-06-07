// this :
#include <HEPVis/nodes/SoMarkerSet.h>

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoPointSet.h>

#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoLazyElement.h>

#include <HEPVis/SbGL.h>
#include <HEPVis/SbPixmap.h>
#include <HEPVis/actions/SoGL2PSAction.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

/* 
  "  x  "
  "  x  "
  "xxxxx"
  "  x  "
  "  x  "

  Should produce bitmap :
  0x20,0x20,0xf8,0x20,0x20

  The rows will be rendered down to top ; first row at bottom, last at top.
  In the below, '-' means that glBitmap will move the pointer to next byte.

  32103210 32103210 32103210 32103210 32103210
  ..1..--- ..1..--- 11111--- ..1..--- ..1..---

  0x20     0x20     0xf8     0x20     0x20
*/

///////////////////////////////////////////////////////////////
/// 5 5 ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
static unsigned char plus_5_5[] = {
  "  x  "
  "  x  "
  "xxxxx"
  "  x  "
  "  x  "
};
static unsigned char asterisk_5_5[] = {
  "x x x"
  " xxx "
  "  x  "
  " xxx "
  "x x x"
};
static unsigned char cross_5_5[] = {
  "x   x"
  " x x "
  "  x  "
  " x x "
  "x   x"
};
static unsigned char star_5_5[] = {
  "x x x"
  " xxx "
  "xxxxx"
  " xxx "
  "x x x"
};
static unsigned char circle_line_5_5[] = {
  " xxx "
  "x   x"
  "x   x"
  "x   x"
  " xxx "
};       
static unsigned char circle_filled_5_5[] = {
  " xxx "
  "xxxxx"
  "xxxxx"
  "xxxxx"
  " xxx "
};       
static unsigned char triangle_up_line_5_5[] = { 
  //OpenGL will draw with y reversed.
  "xxxxx"
  " x x "
  " x x "
  "  x  "
  "  x  "
};
static unsigned char triangle_up_filled_5_5[] = {
  "xxxxx"
  " xxx "
  " xxx "
  "  x  "
  "  x  "
};
static unsigned char triangle_down_line_5_5[] = {
  "  x  "
  "  x  "
  " x x "
  " x x "
  "xxxxx"
};
static unsigned char triangle_down_filled_5_5[] = {
  "  x  "
  "  x  "
  " xxx "
  " xxx "
  "xxxxx"
};
static unsigned char david_star_line_5_5[] = {
  "  x  "
  "xxxxx"
  " x x "
  "xxxxx"
  "  x  "
};       
static unsigned char david_star_filled_5_5[] = {
  "  x  "
  "xxxxx"
  " xxx "
  "xxxxx"
  "  x  "
};       
static unsigned char swiss_cross_line_5_5[] = {
  " xxx "
  "xx xx"
  "x   x"
  "xx xx"
  " xxx "
};       
static unsigned char swiss_cross_filled_5_5[] = {
  " xxx "
  "xxxxx"
  "xxxxx"
  "xxxxx"
  " xxx "
};       
static unsigned char diamond_line_5_5[] = {
  "  x  "
  " x x "
  "x   x"
  " x x "
  "  x  "
};
static unsigned char diamond_filled_5_5[] = {
  "  x  "
  " xxx "
  "xxxxx"
  " xxx "
  "  x  "
};
static unsigned char square_line_5_5[] = {
  "xxxxx"
  "x   x"
  "x   x"
  "x   x"
  "xxxxx"
};
static unsigned char square_filled_5_5[] = {
  "xxxxx"
  "xxxxx"
  "xxxxx"
  "xxxxx"
  "xxxxx"
};
//GB
static unsigned char penta_star_line_5_5[] = {
  //OpenGL will draw with y reversed.
  "x   x"
  " xxx "
  " x x "
  "xxxxx"
  "  x  "
};       
static unsigned char penta_star_filled_5_5[] = {
  //OpenGL will draw with y reversed.
  "x   x"
  " xxx "
  " xxx "
  "xxxxx"
  "  x  "
};       
///////////////////////////////////////////////////////////////
/// 7 7 ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
static unsigned char plus_7_7[] = {
  "   x   "
  "   x   "
  "   x   "
  "xxxxxxx"
  "   x   "
  "   x   "
  "   x   "
};
static unsigned char asterisk_7_7[] = {
  "x  x  x"
  " x x x "
  "  xxx  "
  "   x   "
  "  xxx  "
  " x x x "
  "x  x  x"
};
static unsigned char cross_7_7[] = {
  "x     x"
  " x   x "
  "  xxx  "
  "   x   "
  "  xxx  "
  " x   x "
  "x     x"
};
static unsigned char star_7_7[] = {
  "x  x  x"
  " x x x "
  "  xxx  "
  "xxxxxxx"
  "  xxx  "
  " x x x "
  "x  x  x"
};
static unsigned char circle_line_7_7[] = {
  " xxxxx "
  "x     x"
  "x     x"
  "x     x"
  "x     x"
  "x     x"
  " xxxxx "
};       
static unsigned char circle_filled_7_7[] = {
  " xxxxx "
  "xxxxxxx"
  "xxxxxxx"
  "xxxxxxx"
  "xxxxxxx"
  "xxxxxxx"
  " xxxxx "
};       
static unsigned char triangle_up_line_7_7[] = { 
  //OpenGL will draw with y reversed.
  "xxxxxxx"
  " x   x "
  " x   x "
  "  x x  "
  "  x x  "
  "   x   "
  "   x   "
};
static unsigned char triangle_up_filled_7_7[] = {
  "xxxxxxx"
  " xxxxx "
  " xxxxx "
  "  xxx  "
  "  xxx  "
  "   x   "
  "   x   "
};
static unsigned char triangle_down_line_7_7[] = {
  "   x   "
  "   x   "
  "  x x  "
  "  x x  "
  " x   x "
  " x   x "
  "xxxxxxx"
};
static unsigned char triangle_down_filled_7_7[] = {
  "   x   "
  "   x   "
  "  xxx  "
  "  xxx  "
  " xxxxx "
  " xxxxx "
  "xxxxxxx"
};
static unsigned char david_star_line_7_7[] = {
  "   x   "
  "xxxxxxx"
  " x   x "
  "  x x  "
  " x   x "
  "xxxxxxx"
  "   x   "
};       
static unsigned char david_star_filled_7_7[] = {
  "   x   "
  "xxxxxxx"
  " xxxxx "
  "  xxx  "
  " xxxxx "
  "xxxxxxx"
  "   x   "
};       
static unsigned char swiss_cross_line_7_7[] = {
  "  xxx  "
  "  x x  "
  "xxx xxx"
  "x     x"
  "xxx xxx"
  "  x x  "
  "  xxx  "
};       
static unsigned char swiss_cross_filled_7_7[] = {
  "  xxx  "
  "  xxx  "
  "xxxxxxx"
  "xxxxxxx"
  "xxxxxxx"
  "  xxx  "
  "  xxx  "
};       
static unsigned char diamond_line_7_7[] = {
  "   x   "
  "  x x  "
  " x   x "
  "x     x"
  " x   x "
  "  x x  "
  "   x   "
};
static unsigned char diamond_filled_7_7[] = {
  "   x   "
  "  xxx  "
  " xxxxx "
  "xxxxxxx"
  " xxxxx "
  "  xxx  "
  "   x   "
};
static unsigned char square_line_7_7[] = {
  "xxxxxxx"
  "x     x"
  "x     x"
  "x     x"
  "x     x"
  "x     x"
  "xxxxxxx"
};
static unsigned char square_filled_7_7[] = {
  "xxxxxxx"
  "xxxxxxx"
  "xxxxxxx"
  "xxxxxxx"
  "xxxxxxx"
  "xxxxxxx"
  "xxxxxxx"
};
//GB
static unsigned char penta_star_line_7_7[] = {
  //OpenGL will draw with y reversed.
  " x   x "
  " xx xx "
  "  xxx  "
  " x   x "
  "xxx xxx"
  "  x x  "
  "   x   "
};       
static unsigned char penta_star_filled_7_7[] = {
  //OpenGL will draw with y reversed.
  " x   x "
  " xx xx "
  "  xxx  "
  " xxxxx "
  "xxxxxxx"
  "  xxx  "
  "   x   "
};       

///////////////////////////////////////////////////////////////
/// 9 9 ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
static unsigned char plus_9_9[] = {
  "    x    "
  "    x    "
  "    x    "
  "    x    "
  "xxxxxxxxx"
  "    x    "
  "    x    "
  "    x    "
  "    x    "
};
static unsigned char asterisk_9_9[] = {
  "x   x   x"
  " x  x  x "
  "  x x x  "
  "   xxx   "
  "    x    "
  "   xxx   "
  "  x x x  "
  " x  x  x "
  "x   x   x"
};
static unsigned char cross_9_9[] = {
  "x       x"
  " x     x "
  "  x   x  "
  "   x x   "
  "    x    "
  "   x x   "
  "  x   x  "
  " x     x "
  "x       x"
};
static unsigned char star_9_9[] = {
  "x   x   x"
  " x  x  x "
  "  x x x  "
  "   xxx   "
  "xxxxxxxxx"
  "   xxx   "
  "  x x x  "
  " x  x  x "
  "x   x   x"
};
static unsigned char circle_line_9_9[] = {
  "   xxx   "
  " xx   xx "
  " x     x "
  "x       x"
  "x       x"
  "x       x"
  " x     x "
  " xx   xx "
  "   xxx   "
};       
static unsigned char circle_filled_9_9[] = {
  "   xxx   "
  " xxxxxxx "
  " xxxxxxx "
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  " xxxxxxx "
  " xxxxxxx "
  "   xxx   "
};       
static unsigned char triangle_up_line_9_9[] = { 
  //OpenGL will draw with y reversed.
  "xxxxxxxxx"
  " x     x "
  " x     x "
  "  x   x  "
  "  x   x  "
  "   x x   "
  "   x x   "
  "    x    "
  "    x    "
};
static unsigned char triangle_up_filled_9_9[] = {
  "xxxxxxxxx"
  " xxxxxxx "
  " xxxxxxx "
  "  xxxxx  "
  "  xxxxx  "
  "   xxx   "
  "   xxx   "
  "    x    "
  "    x    "
};
static unsigned char triangle_down_line_9_9[] = {
  "    x    "
  "    x    "
  "   x x   "
  "   x x   "
  "  x   x  "
  "  x   x  "
  " x     x "
  " x     x "
  "xxxxxxxxx"
};
static unsigned char triangle_down_filled_9_9[] = {
  "    x    "
  "    x    "
  "   xxx   "
  "   xxx   "
  "  xxxxx  "
  "  xxxxx  "
  " xxxxxxx "
  " xxxxxxx "
  "xxxxxxxxx"
};
static unsigned char david_star_line_9_9[] = {
  "    x    "
  "   x x   "
  "xxxxxxxxx"
  " x     x "
  "  x   x  "
  " x     x "
  "xxxxxxxxx"
  "   x x   "
  "    x    "
};       
static unsigned char david_star_filled_9_9[] = {
  "    x    "
  "   xxx   "
  "xxxxxxxxx"
  " xxxxxxx "
  "  xxxxx  "
  " xxxxxxx "
  "xxxxxxxxx"
  "   xxx   "
  "    x    "
};       
static unsigned char swiss_cross_line_9_9[] = {
  "   xxx   "
  "   x x   "
  "   x x   "
  "xxxx xxxx"
  "x       x"
  "xxxx xxxx"
  "   x x   "
  "   x x   "
  "   xxx   "
};       
static unsigned char swiss_cross_filled_9_9[] = {
  "   xxx   "
  "   xxx   "
  "   xxx   "
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "   xxx   "
  "   xxx   "
  "   xxx   "
};       
static unsigned char diamond_line_9_9[] = {
  "    x    "
  "   x x   "
  "  x   x  "
  " x     x "
  "x       x"
  " x     x "
  "  x   x  "
  "   x x   "
  "    x    "
};
static unsigned char diamond_filled_9_9[] = {
  "    x    "
  "   xxx   "
  "  xxxxx  "
  " xxxxxxx "
  "xxxxxxxxx"
  " xxxxxxx "
  "  xxxxx  "
  "   xxx   "
  "    x    "
};
static unsigned char square_line_9_9[] = {
  "xxxxxxxxx"
  "x       x"
  "x       x"
  "x       x"
  "x       x"
  "x       x"
  "x       x"
  "x       x"
  "xxxxxxxxx"
};
static unsigned char square_filled_9_9[] = {
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
  "xxxxxxxxx"
};
//GB
static unsigned char penta_star_line_9_9[] = {
  //OpenGL will draw with y reversed.
  " x     x "
  " xxx xxx "
  " x  x  x "
  "  x   x  "
  "  x   x  "
  " x     x "
  "xxxx xxxx"
  "   x x   "
  "    x    "
};       
static unsigned char penta_star_filled_9_9[] = {
  //OpenGL will draw with y reversed.
  " x     x "
  " xxx xxx "
  " xxxxxxx "
  "  xxxxx  "
  "  xxxxx  "
  " xxxxxxx "
  "xxxxxxxxx"
  "   xxx   "
  "    x    "
};       


static unsigned char* sFigures_5_5[] = {
 plus_5_5,
 asterisk_5_5,
 cross_5_5,
 star_5_5,
 circle_line_5_5,
 circle_filled_5_5,
 triangle_up_line_5_5,
 triangle_up_filled_5_5,
 triangle_down_line_5_5,
 triangle_down_filled_5_5,
 david_star_line_5_5,
 david_star_filled_5_5,
 swiss_cross_line_5_5,
 swiss_cross_filled_5_5,
 diamond_line_5_5,
 diamond_filled_5_5,
 square_line_5_5,
 square_filled_5_5,
 penta_star_line_5_5,
 penta_star_filled_5_5
};

static unsigned char* sFigures_7_7[] = {
 plus_7_7,
 asterisk_7_7,
 cross_7_7,
 star_7_7,
 circle_line_7_7,
 circle_filled_7_7,
 triangle_up_line_7_7,
 triangle_up_filled_7_7,
 triangle_down_line_7_7,
 triangle_down_filled_7_7,
 david_star_line_7_7,
 david_star_filled_7_7,
 swiss_cross_line_7_7,
 swiss_cross_filled_7_7,
 diamond_line_7_7,
 diamond_filled_7_7,
 square_line_7_7,
 square_filled_7_7,
 penta_star_line_7_7,
 penta_star_filled_7_7
};

static unsigned char* sFigures_9_9[] = {
 plus_9_9,
 asterisk_9_9,
 cross_9_9,
 star_9_9,
 circle_line_9_9,
 circle_filled_9_9,
 triangle_up_line_9_9,
 triangle_up_filled_9_9,
 triangle_down_line_9_9,
 triangle_down_filled_9_9,
 david_star_line_9_9,
 david_star_filled_9_9,
 swiss_cross_line_9_9,
 swiss_cross_filled_9_9,
 diamond_line_9_9,
 diamond_filled_9_9,
 square_line_9_9,
 square_filled_9_9,
 penta_star_line_9_9,
 penta_star_filled_9_9
};

SO_NODE_SOURCE(HEPVis_SoMarkerSet)
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoMarkerSet::initClass (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_INIT_CLASS(HEPVis_SoMarkerSet,SoPointSet,"PointSet");
}
//////////////////////////////////////////////////////////////////////////////
HEPVis_SoMarkerSet::HEPVis_SoMarkerSet (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(HEPVis_SoMarkerSet);
  
  SO_NODE_ADD_FIELD(markerIndex,(CROSS_5_5));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));
}
//////////////////////////////////////////////////////////////////////////////
HEPVis_SoMarkerSet::~HEPVis_SoMarkerSet (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
static void marker_to_glBitmap(SoAction*,int);
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoMarkerSet::GLRender (
 SoGLRenderAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!shouldGLRender(aAction)) return;
  SoState* state = aAction->getState();

  const SoCoordinateElement* coordinateElement = 
    SoCoordinateElement::getInstance(state);
  if(coordinateElement==NULL) return;

  if(aAction->isOfType(SoGL2PSAction::getClassTypeId())) {
    SoCacheElement::invalidate(state);
  }

  const SbColor& color = SoLazyElement::getDiffuse(aAction->getState(),0);
  float red,green,blue;
  color.getValue(red,green,blue);

  int mark = markerIndex[0];

  int32_t num = coordinateElement->getNum();

  int starti = startIndex.getValue();
  int pointn = numPoints.getValue();

  glPushAttrib( (GLbitfield)(GL_CURRENT_BIT | GL_ENABLE_BIT));
  glDisable(GL_LIGHTING);
  glColor3f(red,green,blue);

#ifdef _WIN32  
  //WIN32 : depth test is out over bitmap !
  glDisable(GL_DEPTH_TEST);
#endif

  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  for(int pointi=starti;pointi<pointn;pointi++){
    if(pointi>=num) break;
    const SbVec3f& vec = coordinateElement->get3(pointi);
    glRasterPos3f(vec[0],vec[1],vec[2]);
    // Do a push, pop to correct a deffect of Mesa-3.1. 
    // If not, further line drawing will have bad colors.
    // The glPopAttrib will compell a reinitialisation of
    // some internal Mesa state.
    //glPushAttrib(GL_ALL_ATTRIB_BITS);
    //glPopAttrib();
    //
    marker_to_glBitmap(aAction,mark);
  }

  glPopAttrib();
}
//////////////////////////////////////////////////////////////////////////////
void marker_to_glBitmap(
 SoAction* aAction
,int aStyle
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  GLsizei w = 0,h = 0;
  GLfloat xorig = 0,yorig = 0;

  int group = int(aStyle/100);
  int rest = aStyle-group*100;
  int n_5_5 = sizeof(sFigures_5_5)/sizeof(unsigned char*);
  int n_7_7 = sizeof(sFigures_7_7)/sizeof(unsigned char*);
  int n_9_9 = sizeof(sFigures_9_9)/sizeof(unsigned char*);

  if((group==5)&&(rest<n_5_5)) {
    w = h = 5;
    xorig = yorig = 2;
    GLubyte* bitmap = 
      (GLubyte*)SbPixmap::figure2bitmap(w,h,sFigures_5_5[rest]);
    glBitmap(w,h,xorig,yorig,0.,0.,bitmap);
    delete [] bitmap;

  } else if((group==7)&&(rest<n_7_7)) {
    w = h = 7;
    xorig = yorig = 3;
    GLubyte* bitmap = 
      (GLubyte*)SbPixmap::figure2bitmap(w,h,sFigures_7_7[rest]);
    glBitmap(w,h,xorig,yorig,0.,0.,bitmap);
    delete [] bitmap;

  } else if((group==9)&&(rest<n_9_9)) {
    w = h = 9;
    xorig = yorig = 4;
    GLubyte* bitmap = 
      (GLubyte*)SbPixmap::figure2bitmap(w,h,sFigures_9_9[rest]);
    glBitmap(w,h,xorig,yorig,0.,0.,bitmap);
    delete [] bitmap;

  } else {
    SoDebugError::postInfo("SoMarkerSet::marker_to_glBitmap",
      "bad index : %d : group %d rest %d (%d %d %d)",
      aStyle,group,rest,n_5_5,n_7_7,n_9_9);
    return;
  }

  if(aAction->isOfType(SoGL2PSAction::getClassTypeId())) {
    ((SoGL2PSAction*)aAction)->addBitmap(w,h,xorig,yorig,0,0);
  }

}
//////////////////////////////////////////////////////////////////////////////
/// Alternate rep logic //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//  For SoMarkerSet, producing an alternate rep is not straightforward
// since the SoMarkerSet node does not bring with him the coordinates.
// It means that a usual generateAlternateRep() method can't be used 
// to produce directly the alternate rep.
//  The coordinate node is known only when traversing a scene graph
// and if the action has the SoCoordinateElement enabled.
//  In the below, the alternate rep is generated in the doAction
// for the SoAlternateRepAction.

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>

#include <HEPVis/nodes/SoSubNode.h>

static void SoTools_addSegmentsToNode(SoSeparator* aSeparator,int aNumber,SbVec3f* aLine){
  if(aNumber<=1) return;

  SoCoordinate3* coordinate3 = new SoCoordinate3;
  int32_t pointn = aNumber;
  coordinate3->point.setValues(0,pointn,aLine);
  aSeparator->addChild(coordinate3);

  int segmentn = pointn/2;
  int32_t* vertices = new int32_t [segmentn];
  for (int count=0;count<segmentn;count++) vertices[count] = 2;

  SoLineSet* lineSet = new SoLineSet;
  lineSet->numVertices.setValues(0,segmentn,vertices);
  aSeparator->addChild(lineSet);

  delete [] vertices;
}


static void marker_to_lineSet(const SbVec3f&,int,SoSeparator*);
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoMarkerSet::doAction(
 SoAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ALTERNATEREP_DO_ACTION(aAction)
  SoPointSet::doAction(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoMarkerSet::write(
 SoWriteAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    HEPVIS_SET_FIELDS_DEFAULT(FALSE)
    SoPointSet::write(aAction);
    HEPVIS_RESTORE_FIELDS_DEFAULT
    // other solution :
    //altRep->write(aAction);
  } else {
    SoShape::write(aAction);
  }
}
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoMarkerSet::generateAlternateRep(
 SoAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(!aAction) return;

  SoState* state = aAction->getState();

  const SoCoordinateElement* coordinateElement = 
    SoCoordinateElement::getInstance(state);
  if(coordinateElement==NULL) return;

  //SoCacheElement::invalidate(state);

  int32_t num = coordinateElement->getNum();

  SoSeparator* separator = new SoSeparator;

  SoDrawStyle* drawStyle = new SoDrawStyle;
  drawStyle->style.setValue(SoDrawStyle::LINES);
  separator->addChild(drawStyle);

  int mark = markerIndex[0];

  int starti = startIndex.getValue();
  int pointn = numPoints.getValue();
  //printf("debug : SoMarkerSet::generateAltRep : %ld : %d : %d %d\n",
    //this,num,starti,pointn);

  for(int pointi=starti;pointi<pointn;pointi++){
    if(pointi>=num) break;
    const SbVec3f& vec = coordinateElement->get3(pointi);
    //printf("debug :  %ld : %d (%d) : %g %g %g\n",
      //this,pointi,pointn,vec[0],vec[1],vec[2]);
    marker_to_lineSet(vec,mark,separator);
  }

  alternateRep.setValue(separator);
}
void HEPVis_SoMarkerSet::clearAlternateRep() {
  alternateRep.setValue(NULL);
}
//////////////////////////////////////////////////////////////////////////////
void marker_to_lineSet(
 const SbVec3f& aVec
,int //aStyle
,SoSeparator* aParent
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#define LIST_SET(list,number,x,y,z) \
  list[number].setValue((x),(y),(z));\
  list[number] += aVec;

  //aStyle==36 //plus_9_9

  float s = 0.01F;
  //float s = 1.0F;

  SbVec3f points[4];
  LIST_SET(points,0,-s, 0,0);
  LIST_SET(points,1, s, 0,0);
  LIST_SET(points,2, 0,-s,0);
  LIST_SET(points,3, 0, s,0);

  SoTools_addSegmentsToNode(aParent,4,points);


}
//////////////////////////////////////////////////////////////////////////////
int HEPVis_SoMarkerSet::getMarkerIndex(
 SbMarkerStyle aStyle
,int aSize
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if((aSize>=9)) return 900+(int)(aStyle-1);
  if((aSize>=7)) return 700+(int)(aStyle-1);
  else           return 500+(int)(aStyle-1);
}
//////////////////////////////////////////////////////////////////////////////
void HEPVis_SoMarkerSet::getMarkerStyleSize(
 int aIndex
,SbMarkerStyle& aStyle
,int& aSize
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  int group = aIndex/100;
  int rest = aIndex-group*100;
  aStyle = (SbMarkerStyle)rest;
  if(group==5) aSize = 5;
  else if(group==7) aSize = 7;
  else if(group==9) aSize = 9;
  else aSize = 5;
}
