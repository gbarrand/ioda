// this :
#include <HEPVis/nodekits/SoShell.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/elements/SoViewVolumeElement.h>

#include <HEPVis/nodes/SoGrid.h>

#include <math.h>

SO_KIT_SOURCE(SoShell) 
//////////////////////////////////////////////////////////////////////////////
void SoShell::initClass(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoShell,SoBaseKit,"BaseKit");
}
//////////////////////////////////////////////////////////////////////////////
SoShell::SoShell(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoShell);
  SO_KIT_ADD_CATALOG_ENTRY(shellSeparator,SoSeparator,FALSE,this,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(shellMaterial,SoMaterial,
                           FALSE,shellSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(shellGrid,SoGrid,FALSE,shellSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(shellTransform,SoTransform,
                           FALSE,shellSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(shellGroup,SoGroup,FALSE,shellSeparator,\0,TRUE);
  SO_KIT_INIT_INSTANCE();
  //
  SO_NODE_ADD_FIELD(background,(SbColor(1.,1.,1.)));
  //
  SoMaterial* material = (SoMaterial*)getPart("shellMaterial",TRUE);
  if(material!=NULL) material->diffuseColor.connectFrom(&background);
}
//////////////////////////////////////////////////////////////////////////////
SoShell::~SoShell(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoMaterial* material = (SoMaterial*)shellMaterial.getValue();
  if(material!=NULL) material->diffuseColor.disconnect();
}
//////////////////////////////////////////////////////////////////////////////
void SoShell::addChild(
 SoNode* aNode
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoGroup* group = SO_GET_PART(this,"shellGroup",SoGroup);
  if(group==NULL) return;
  group->addChild(aNode);
}
//////////////////////////////////////////////////////////////////////////////
void SoShell::doAction(
 SoAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
// Bring left side of "things" to left side of viewing volume.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoTransform* transform = SO_GET_PART(this,"shellTransform",SoTransform);
  if(transform==NULL) return;
  SoGroup* group = SO_GET_PART(this,"shellGroup",SoGroup);
  if(group==NULL) return;
  SoGrid* grid = (SoGrid*)getPart("shellGrid",TRUE);
  if(grid==NULL) return;

  const SbViewVolume& viewVolume = 
    SoViewVolumeElement::get(aAction->getState());
  float vvw = viewVolume.getWidth();
  float vvh = viewVolume.getHeight();
  float l = viewVolume.llf[0];
  float b = viewVolume.llf[1];
  /* FIXME : below not reliable with Coin :
  float r = viewVolume.lrf[0];
  float t = viewVolume.ulf[1];
  */

  SbBool flag;
  flag = grid->position.enableNotify(FALSE);
  grid->position.setValue(SbVec3f(l,b,-1.));
  grid->position.enableNotify(flag);

  flag = grid->width.enableNotify(FALSE);
  grid->width.setValue(vvw);
  grid->width.enableNotify(flag);

  flag = grid->height.enableNotify(FALSE);
  grid->height.setValue(vvh);
  grid->height.enableNotify(flag);

  // Retreive child object size :
  // Get bounding box :
  SbViewportRegion vp; 
  SoGetBoundingBoxAction* getBB = new SoGetBoundingBoxAction(vp);
  getBB->apply(group);
  SbBox3f bb = getBB->getBoundingBox();
  delete getBB;
  if(bb.isEmpty()==TRUE) return;
  const SbVec3f& bmin = bb.getMin();
  const SbVec3f& bmax = bb.getMax();

  /*
  static int debug = 0;
  debug++;
  printf("debug : SoShell::doAction : box : + %d %s : %g %g %g : %g %g %g\n",
         debug,getTypeId().getName().getString(),
         bmin[0],bmin[1],bmin[2],
         bmax[0],bmax[1],bmax[2]);
  */

  // Transform things to map view volume :
  flag = transform->enableNotify(FALSE);

  transform->setMatrix(SbMatrix::identity());
  SoScale* scale = new SoScale();
  scale->ref();
  SoTranslation* translation = new SoTranslation();
  translation->ref();

  // Below should be read bottom to top :
  // Bring to lower left corner :
  translation->translation.setValue(SbVec3f(l,b,0.));
  transform->combineLeft(translation);  // Applied last.
  // Scale so that height of object map height of view volume :
  float factor = vvh/(bmax[1]-bmin[1]);
  scale->scaleFactor.setValue(factor,factor,1.);
  transform->combineLeft(scale); // Second.
  // Bring volume in 0., 0. :
  translation->translation.setValue(SbVec3f(-bmin[0],-bmin[1],0.));
  transform->combineLeft(translation);  // Applied first !

  scale->unref();
  translation->unref();

  transform->enableNotify(flag);
}
//////////////////////////////////////////////////////////////////////////////
void SoShell::GLRender (
 SoGLRenderAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoShell::doAction(aAction);
  SoBaseKit::GLRender(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoShell::callback (
 SoCallbackAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoShell::doAction(aAction);
  SoBaseKit::callback(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoShell::rayPick (
 SoRayPickAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  //printf("debug : SoShell:rayPick\n");
  SoShell::doAction(aAction);
  SoBaseKit::rayPick(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoShell::pick (
 SoPickAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  //printf("debug : SoShell:rayPick\n");
  SoShell::doAction(aAction);
  SoBaseKit::pick(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoShell::getBoundingBox (
 SoGetBoundingBoxAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoShell::doAction(aAction);
  SoBaseKit::getBoundingBox(aAction);
}
