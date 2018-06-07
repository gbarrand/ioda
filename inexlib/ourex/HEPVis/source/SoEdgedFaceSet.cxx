// this :
#include <HEPVis/nodes/SoEdgedFaceSet.h>

// Inventor :
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoDrawStyleElement.h>

// HEPVis :
#include <HEPVis/SbGL.h>

// Else :
#include <math.h>

SO_NODE_SOURCE(SoEdgedFaceSet)
//////////////////////////////////////////////////////////////////////////////
void SoEdgedFaceSet::initClass (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_INIT_CLASS(SoEdgedFaceSet,SoFaceSet,"FaceSet");
}
//////////////////////////////////////////////////////////////////////////////
SoEdgedFaceSet::SoEdgedFaceSet (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_NODE_CONSTRUCTOR(SoEdgedFaceSet);
}
//////////////////////////////////////////////////////////////////////////////
SoEdgedFaceSet::~SoEdgedFaceSet (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
void SoEdgedFaceSet::GLRender (
 SoGLRenderAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  // Draw first filled polygons :
  SoFaceSet::GLRender(aAction);

  // Draw edge of polygons with OpenGL algorithm :
  SoState* state = aAction->getState();

  SoDrawStyleElement::Style drawStyle = SoDrawStyleElement::get(state);
  if(drawStyle!=SoDrawStyleElement::FILLED) return;

  const SoCoordinateElement* coordinateElement = 
    SoCoordinateElement::getInstance(state);
  if(coordinateElement==NULL) return;

  /*
  glDisable(GL_LINE_STIPPLE);
  glLineWidth(4.);
  //glDisable(GL_DEPTH_TEST);
  glEnable(GL_DEPTH_TEST);

  int polyn = numVertices.getNum();
  int index = 0;
  for(int polyi=0;polyi<polyn;polyi++){
    int vertn = numVertices[polyi];
    int verti;
    glColor3d(0.,1.,0.);
    glPolygonMode(GL_FRONT,GL_FILL);
    glBegin(GL_POLYGON);
    for(verti=0;verti<vertn;verti++){
      const SbVec3f& point = coordinateElement->get3(index+verti);
      glVertex3f(point[0],point[1],point[2]);
    }
    glEnd();
    glColor3d(1.,0.,0.);
    glPolygonMode(GL_FRONT,GL_LINE);
    glBegin(GL_POLYGON);
    for(verti=0;verti<vertn;verti++){
      const SbVec3f& point = coordinateElement->get3(index+verti);
      glVertex3f(point[0],point[1],point[2]);
    }
    glEnd();
    index += vertn;
  }
  glPolygonMode(GL_FRONT,GL_FILL);
  */


  glPushAttrib(GL_POLYGON_BIT);

  glColor3d(1.,0.,0.);

  glPolygonMode(GL_FRONT,GL_LINE);

  int polyn = numVertices.getNum();
  int index = 0;
  for(int polyi=0;polyi<polyn;polyi++){
    int vertn = numVertices[polyi];
    glBegin(GL_POLYGON);
    for(int verti=0;verti<vertn;verti++){
      const SbVec3f& point = coordinateElement->get3(index);
      glVertex3f(point[0],point[1],point[2]);
      index++;
    }
    glEnd();
  }

  glPopAttrib();
}


