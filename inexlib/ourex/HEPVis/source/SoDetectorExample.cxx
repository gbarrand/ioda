// this :
#include <HEPVis/nodekits/SoDetectorExample.h>

#include <Inventor/SbBasic.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/draggers/SoDragger.h>

#include <HEPVis/SbMath.h>
#include <HEPVis/SbString.h>
#include <HEPVis/nodekits/SoDetectorTreeKit.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

//#define WITH_CUBE

#ifdef WITH_CUBE
#include <Inventor/nodes/SoCube.h>
#else
#include <HEPVis/nodes/SoTubs.h>
#include <HEPVis/SbPolyhedron.h>
#include <HEPVis/nodes/SoTrd.h>
#include <HEPVis/nodes/SoPolyhedron.h>
#include <HEPVis/nodes/SoBox.h>
#endif

class SoDetKit : public SoDetectorTreeKit {
public:
  SoDetKit(SoDetectorExample& aExa):fExa(aExa){}
public:
  SoDetectorExample& fExa;
};

SO_KIT_SOURCE(SoDetectorExample) 
//////////////////////////////////////////////////////////////////////////////
void SoDetectorExample::initClass (
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_INIT_CLASS(SoDetectorExample,SoBaseKit,"BaseKit");
}
//////////////////////////////////////////////////////////////////////////////
SoDetectorExample::SoDetectorExample(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_KIT_CONSTRUCTOR(SoDetectorExample);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0,FALSE);
  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SoSeparator* sep = (SoSeparator*)topSeparator.getValue();

  SoDetectorTreeKit* SVX = new SoDetKit(*this);
  SVX->setName("SVX");
  SVX->setPreview(FALSE);
  sep->addChild(SVX);

  SoSeparator* previewSeparator = 
    (SoSeparator*)SVX->getPart("previewSeparator",TRUE);
  SoSeparator* fullSeparator = 
    (SoSeparator*)SVX->getPart("fullSeparator",TRUE);

  // Global Properties
  SoMaterial* RedPlastic = 
    (SoMaterial*)SVX->getPart("appearance.material",TRUE);
  RedPlastic->diffuseColor.setValue(1,0,0);

  // Preview Representation
#ifdef WITH_CUBE
  SoCube* SVXTubs = new SoCube();
  SVXTubs->width = 2*12;
  SVXTubs->height = 2*12;
  SVXTubs->depth = 2*48;
#else
  SoTubs* SVXTubs = new SoTubs();
  SVXTubs->pRMin = 2.5F;
  SVXTubs->pRMax = 12;
  SVXTubs->pDz = 48;
#endif
  SVXTubs->setName("svx");
  previewSeparator->addChild(SVXTubs);

  // Full Representation
  for (int j=0;j<3;j++) {
    SoDetectorTreeKit* Barrel = new SoDetKit(*this);
    Barrel->setName(SbStringPrintF(32,"Barrel_%d",j));
    float z = (float)(32 * (j-1));
    addBarrel(Barrel,z);
    fullSeparator->addChild(Barrel);
    if (j==2) Barrel->setPreview(FALSE);
  }

#ifdef WITH_CUBE
  SoCube* beamPipe = new SoCube();
  beamPipe->width = 2*2.4F;
  beamPipe->height = 2*2.4F;
  beamPipe->depth = 2*70;
#else
  SoTubs* beamPipe = new SoTubs();
  beamPipe->pRMin = 2.3F;
  beamPipe->pRMax = 2.4F;
  beamPipe->pDz = 70;
#endif
  beamPipe->setName("beamPipe");
  sep->addChild(beamPipe);

}
//////////////////////////////////////////////////////////////////////////////
SoDetectorExample::~SoDetectorExample(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
void SoDetectorExample::addWafer(
 SoDetectorTreeKit* Wafer
,float radius
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSeparator* previewSeparator = 
    (SoSeparator*) Wafer->getPart("previewSeparator",TRUE);
  SoSeparator* fullSeparator = 
    (SoSeparator*) Wafer->getPart("fullSeparator",   TRUE);

  SoTransform* Transform = 
    (SoTransform*) Wafer->getPart("transform",TRUE);
  Transform->translation.setValue(SbVec3f(0,0,radius));

  SoMaterial* Silicon  = 
    (SoMaterial*)Wafer->getPart("appearance.material",TRUE);
  Silicon->diffuseColor.setValue (1,0,0);

  //  Full Representation
#ifdef WITH_CUBE
  SoCube* BoxShape = new SoCube();
  BoxShape->width = 2 * 2 * radius * FSIN(15*FM_PI/180);
  BoxShape->height = 2*15;
  BoxShape->depth = 2*0.03F;
#else
  SoBox* BoxShape = new SoBox();
  BoxShape->fDx= 2 * radius * FSIN(15*FM_PI/180);
  BoxShape->fDy= 15;
  BoxShape->fDz= 0.03F;
#endif
  BoxShape->setName("wafer");

  // To test VRML production.
 {SoTransform* Transform = new SoTransform;
  Transform->rotation.setValue(SbRotation(SbVec3f(0,1,0),20*FM_PI/180));
  previewSeparator->addChild(Transform);}

  previewSeparator->addChild(BoxShape);
  
  //  Contracted Representation 
  SoSphere* Sphere = new SoSphere();
  fullSeparator->addChild(Sphere);
 
}
//////////////////////////////////////////////////////////////////////////////
void SoDetectorExample::addWedge(
 SoDetectorTreeKit* Wedge
,float angle
,SbBool aSolid
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSeparator* previewSeparator = 
    (SoSeparator*) Wedge->getPart("previewSeparator",TRUE);
  SoSeparator* fullSeparator    = 
    (SoSeparator*) Wedge->getPart("fullSeparator",   TRUE);

  // Global Properties
  SbMatrix mtx;
  mtx.makeIdentity();

 {SbMatrix tmp;
  tmp.setRotate(SbRotation(SbVec3f(0,0,1),angle));
  mtx.multLeft(tmp);}

 {SbMatrix tmp;
  tmp.setRotate(SbRotation(SbVec3f(1,0,0),FM_PI_2));
  mtx.multLeft(tmp);}

 {SbMatrix tmp;
  tmp.setTranslate(SbVec3f(0,0,7.25F));
  mtx.multLeft(tmp);} //Applied first.
  
  SoTransform* TotalTransform = 
    (SoTransform*)Wedge->getPart("transform",TRUE);
  TotalTransform->setMatrix(mtx); //will set the SoFields.

  // Preview Representation
#ifdef WITH_CUBE
  SoCube* WedgeTrd = new SoCube();
  WedgeTrd->width  = 2*0.65F;
  WedgeTrd->height  = 2*15;
  WedgeTrd->depth   = 2*4.75F;
  WedgeTrd->setName("wedge");
  previewSeparator->addChild(WedgeTrd); 
#else
/*
  SoTrd *WedgeTrd = new SoTrd();
  WedgeTrd->fDx1  = 0.65F;
  WedgeTrd->fDx2  = 3.1F;
  WedgeTrd->fDy1  = 15;
  WedgeTrd->fDy2  = 15;
  WedgeTrd->fDz   = 4.75F;
  WedgeTrd->setName("wedge");
  previewSeparator->addChild(WedgeTrd); 
*/
  // to test write/read of files :
 {SbPolyhedronTrd2 trd2(0.65F,3.1F,15,15,4.75F);
  SoPolyhedron* polyhedron = new SoPolyhedron(trd2);
  polyhedron->solid.setValue(aSolid);
  polyhedron->reducedWireFrame.setValue(TRUE);
  polyhedron->setName("wedge");
  previewSeparator->addChild(polyhedron);}
#endif

  for (int i=0;i<5;i++) {
    SoDetectorTreeKit* Wafer= new SoDetKit(*this);
    Wafer->setName(SbStringPrintF(32,"%s_Wafer_%d",
				  Wedge->getName().getString(),i));
    addWafer(Wafer,(float)(i+1));
    fullSeparator->addChild(Wafer);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoDetectorExample::addBarrel(
 SoDetectorTreeKit* Barrel
,float z
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoSeparator* previewSeparator = 
    (SoSeparator*)Barrel->getPart("previewSeparator",TRUE);
  SoSeparator* fullSeparator    = 
    (SoSeparator*)Barrel->getPart("fullSeparator",   TRUE);

  // Global Properties
  SoTransform* TheTransform  = 
    (SoTransform*)Barrel->getPart("transform",TRUE);
  TheTransform->translation.setValue(0,0,z);

  SoMaterial* BluePlastic  = 
    (SoMaterial*)Barrel->getPart("appearance.material",TRUE);
  BluePlastic->diffuseColor.setValue(0,0,1);

  // Preview Representation
#ifdef WITH_CUBE
  SoCube *BarrelTubs = new SoCube();
  BarrelTubs->width = 2*12;
  BarrelTubs->height = 2*12;
  BarrelTubs->depth   = 2*15;
#else
  SoTubs *BarrelTubs = new SoTubs();
  BarrelTubs->pRMin = 2.5F;
  BarrelTubs->pRMax = 12;
  BarrelTubs->pDz   = 15;
#endif
  BarrelTubs->setName("barrel");
  previewSeparator->addChild(BarrelTubs);

  // Full Representation
  for (int i=0;i<12;i++) {
    SoDetectorTreeKit* Wedge = new SoDetKit(*this);
    Wedge->setName(SbStringPrintF(32,"%s_Wedge_%d",
				  Barrel->getName().getString(),i));
    addWedge(Wedge,i*FM_PI/6,(i==5?FALSE:TRUE));
    fullSeparator->addChild(Wedge);
    SoMaterial* GreenPlastic = 
      (SoMaterial*)Wedge->getPart("appearance.material",TRUE); 
    GreenPlastic->diffuseColor.setValue(0,1,0);
    if (i==0) Wedge->setPreview(FALSE);
  }
}
//////////////////////////////////////////////////////////////////////////////
void SoDetectorExample::generateAlternateRep(SoAction*) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(topSeparator.getValue());
}
//////////////////////////////////////////////////////////////////////////////
void SoDetectorExample::clearAlternateRep(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  alternateRep.setValue(NULL);
}
//////////////////////////////////////////////////////////////////////////////
void SoDetectorExample::doAction(
 SoAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SO_ALTERNATEREP_DO_ACTION(aAction)
  SoBaseKit::doAction(aAction);
}
//////////////////////////////////////////////////////////////////////////////
void SoDetectorExample::write(
 SoWriteAction* aAction
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    altRep->write(aAction);
  } else {
    // Do not write callbackList.
    // Else at read time, we shall loose the callbacks
    // installed in the constructor.
    SbBool old = callbackList.isDefault();
    callbackList.setDefault(TRUE); //enforce not writing.
    SoBaseKit::write(aAction);
    callbackList.setDefault(old);
  }
}
