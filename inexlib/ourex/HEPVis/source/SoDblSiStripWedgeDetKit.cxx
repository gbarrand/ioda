/*-----------------------------HEPVis----------------------------------------*/
/*                                                                           */
/* Node:             SoDblSiStripWedgeDetKit                                 */
/* Description:      Represents a double sided silicon strip wedge detector  */
/* Author:           Peter Hamel                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <math.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoAnnotation.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/draggers/SoScale1Dragger.h>
#include <Inventor/draggers/SoTranslate2Dragger.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodekits/SoAppearanceKit.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/nodes/SoFont.h>
#include <HEPVis/nodes/SoSiWedgeStrips.h>
#include <HEPVis/nodes/SoWedge.h>
#include <HEPVis/nodekits/SoDblSiStripWedgeDetKit.h>

// This statement is required
SO_KIT_SOURCE(SoDblSiStripWedgeDetKit) 

// initClass
void SoDblSiStripWedgeDetKit::initClass(){
  SO_KIT_INIT_CLASS(SoDblSiStripWedgeDetKit,SoBaseKit,"BaseKit");
}

// Constructor
SoDblSiStripWedgeDetKit::SoDblSiStripWedgeDetKit() {
  SO_KIT_CONSTRUCTOR(SoDblSiStripWedgeDetKit);
  SO_NODE_ADD_FIELD(alternateRep, (NULL));
  SO_NODE_ADD_FIELD(halfHeight,    (2));
  SO_NODE_ADD_FIELD(halfWidth1,    (2));
  SO_NODE_ADD_FIELD(halfWidth2,    (1));
  SO_NODE_ADD_FIELD(thickness,     (0.03F));
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,     SoSeparator,             FALSE,            this,           \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(transformSep,     SoSeparator,             FALSE,    topSeparator,           \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(transformFlip,    SoRotation,              FALSE,    transformSep,           \0,  FALSE); 
  SO_KIT_ADD_CATALOG_ENTRY(translateDragger, SoTranslate2Dragger,     TRUE,    transformSep,            \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(localTransform,   SoTransform,             FALSE,    topSeparator,           \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(wedgeSeparator,   SoSeparator,     FALSE,    topSeparator,            \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(wedgeAppearance,  SoAppearanceKit, TRUE,  wedgeSeparator,             \0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(wedge,            SoWedge,         FALSE, wedgeSeparator,             \0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(textSeparator,    SoSeparator,     FALSE,    topSeparator,            \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(textAppearance,   SoAppearanceKit, FALSE,   textSeparator,            \0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(textReset,        SoResetTransform,FALSE,   textSeparator,            \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(textTranslation,  SoTranslation,   FALSE,   textSeparator,            \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(text,             SoText2,         FALSE,   textSeparator,            \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(stripsSeparator,  SoSeparator,     FALSE,    topSeparator,            \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(stripsScale,      SoScale,         FALSE, stripsSeparator,            \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(stripsDragger,    SoScale1Dragger, TRUE,  stripsSeparator,            \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(phiSeparator,     SoSeparator,     FALSE, stripsSeparator,            \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(zSeparator,       SoSeparator,     FALSE, stripsSeparator,            \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(phiStripsAppearance, SoAppearanceKit, TRUE, phiSeparator,             \0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(phiTranslation,   SoTranslation,   FALSE, phiSeparator,               \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(phiRot,           SoRotationXYZ,   FALSE, phiSeparator,               \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(phiStrips,        SoSiWedgeStrips, FALSE, phiSeparator,               \0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(zStripsAppearance, SoAppearanceKit, TRUE, zSeparator,             \0,  TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(zTranslation,     SoTranslation,   FALSE, zSeparator,             \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(zRot,             SoRotationXYZ,   FALSE, zSeparator,             \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(zReflect,         SoRotationXYZ,   FALSE, zSeparator,             \0,  FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(zStrips,          SoSiWedgeStrips,      FALSE, zSeparator,        \0,  TRUE);
  SO_KIT_INIT_INSTANCE();
  createInitialStripDet();
}

// Destructor
SoDblSiStripWedgeDetKit::~SoDblSiStripWedgeDetKit() {
}

SbBool SoDblSiStripWedgeDetKit::affectsState() const {
  return FALSE;
}

void SoDblSiStripWedgeDetKit::createInitialStripDet() {

  myHalfHeight = halfHeight.getValue();
  myHalfWidth1 = halfWidth1.getValue();
  myHalfWidth2 = halfWidth2.getValue();
  myThickness  = thickness.getValue();

  SoWedge *theWedge = (SoWedge*) wedge.getValue();
  theWedge->halfWidth1.connectFrom(&halfWidth1);
  theWedge->halfWidth2.connectFrom(&halfWidth2);
  theWedge->halfHeight.connectFrom(&halfHeight);
  theWedge->thickness.connectFrom(&thickness);

  SoCalculator *StripCalc = new SoCalculator();
  SoCalculator *StripCalc2 = new SoCalculator();
  SoCalculator *StripCalc3 = new SoCalculator();
  StripCalc->a.connectFrom(&halfWidth1);
  StripCalc->b.connectFrom(&halfWidth2);
  StripCalc->c.connectFrom(&halfHeight);
  StripCalc->d.connectFrom(&(((SoSiWedgeStrips *)phiStrips.getValue())->pitch));
  StripCalc->e.connectFrom(&(((SoSiWedgeStrips *)phiStrips.getValue())->offset));
  StripCalc->expression.set1Value(0, "ta=(b < a) ? b : a");
  StripCalc->expression.set1Value(1, "oa=sqrt((b-a)*(b-a) + 4*c*c)"); // length
  StripCalc->expression.set1Value(2, "ob=4*(ta-e)*c/(d*oa)"); // cutoff (2*b*cos/d)
  StripCalc->expression.set1Value(3, "oc=d*oa*oa/(4*c*(b-a))"); // slope (d/(2*cos*sin))
  StripCalc->expression.set1Value(4, "od=-d*(b-a)/(2*c)"); // tilt (-d*tan)
  StripCalc2->a.connectFrom(&halfWidth1);
  StripCalc2->b.connectFrom(&halfWidth2);
  StripCalc2->c.connectFrom(&halfHeight);
  StripCalc2->d.connectFrom(&(((SoSiWedgeStrips *)zStrips.getValue())->pitch));
  StripCalc2->e.connectFrom(&(((SoSiWedgeStrips *)zStrips.getValue())->offset));
  StripCalc2->expression.set1Value(0, "ta=(b < a) ? b : a");
  StripCalc2->expression.set1Value(1, "oa=sqrt((b-a)*(b-a) + 4*c*c)"); // length
  StripCalc2->expression.set1Value(2, "ob=4*(ta-e)*c/(d*oa)"); // cutoff (2*b*cos/d)
  StripCalc2->expression.set1Value(3, "oc=d*oa*oa/(4*c*(b-a))"); // slope (d/(2*cos*sin))
  StripCalc2->expression.set1Value(4, "od=-d*(b-a)/(2*c)"); // tilt (-d*tan)
  StripCalc3->a.connectFrom(&halfWidth1);
  StripCalc3->b.connectFrom(&halfWidth2);
  StripCalc3->c.connectFrom(&halfHeight);
  StripCalc3->d.connectFrom(&StripCalc->oa);
  StripCalc3->e.connectFrom(&(((SoSiWedgeStrips *)phiStrips.getValue())->offset));
  StripCalc3->f.connectFrom(&(((SoSiWedgeStrips *)zStrips.getValue())->offset));
  StripCalc3->expression.set1Value(0, "oa=atan((b-a)/(2*c))"); // angle
  StripCalc3->expression.set1Value(1, "ob=-oa");
  StripCalc3->expression.set1Value(2, "oA=vec3f(0,-a + (c*(a+b))/d + (a-b)/2 + e,(b*b - a*a)/(2*d))"); // sin(oa)*(a+b)/2)");
  StripCalc3->expression.set1Value(3, "oB=vec3f(0,a - (c*(a+b))/d - (a-b)/2 - f,(b*b - a*a)/(2*d))"); // sin(oa)*(a+b)/2)");
  StripCalc3->expression.set1Value(4, "oc=(a+b)");

  SoSiWedgeStrips *theFamousPhiStrips = (SoSiWedgeStrips *) phiStrips.getValue();
  SoSiWedgeStrips *theFamousZStrips = (SoSiWedgeStrips *) zStrips.getValue();
  theFamousPhiStrips->length.connectFrom(&StripCalc->oa);
  theFamousPhiStrips->width.connectFrom(&StripCalc3->oc);
  theFamousPhiStrips->thickness.connectFrom(&thickness);
  theFamousPhiStrips->cutoff.connectFrom(&StripCalc->ob);
  theFamousPhiStrips->slope.connectFrom(&StripCalc->oc);
  theFamousPhiStrips->tilt.connectFrom(&StripCalc->od);
  theFamousZStrips->length.connectFrom(&StripCalc->oa);
  theFamousZStrips->width.connectFrom(&StripCalc3->oc);
  theFamousZStrips->thickness.connectFrom(&thickness);
  theFamousZStrips->cutoff.connectFrom(&StripCalc2->ob);
  theFamousZStrips->slope.connectFrom(&StripCalc2->oc);
  theFamousZStrips->tilt.connectFrom(&StripCalc2->od);

  SoAppearanceKit *TextAppearance = (SoAppearanceKit *) textAppearance.getValue();  
  TextAppearance->set("font","size 22");
  TextAppearance->set("material",  "diffuseColor .2 .2 1.0");
  TextAppearance->set("lightModel","model BASE_COLOR");

  // Mr. Rotation
  SoRotationXYZ *MisterPhiRXYZ = (SoRotationXYZ *) phiRot.getValue();
  MisterPhiRXYZ->axis=SoRotationXYZ::X;
  MisterPhiRXYZ->angle.connectFrom(&StripCalc3->oa);
  
  SoRotationXYZ *MisterZRXYZ = (SoRotationXYZ *) zRot.getValue();  
  MisterZRXYZ->axis=SoRotationXYZ::X;
  MisterZRXYZ->angle.connectFrom(&StripCalc3->ob);

  SoRotationXYZ *MisterZReflect = (SoRotationXYZ *) zReflect.getValue();
  MisterZReflect->axis=SoRotationXYZ::Z;
  MisterZReflect->angle= 3.1415926F;

  // Translation to align strips to wedge edges

  SoTranslation *ThePhiTranslation = (SoTranslation *) phiTranslation.getValue();
  ThePhiTranslation->translation.connectFrom(&StripCalc3->oA);

  SoTranslation *TheZTranslation = (SoTranslation *) zTranslation.getValue();
  TheZTranslation->translation.connectFrom(&StripCalc3->oB);

  // New, for detector aligment
  SoRotation   *MrTransformFlip = (SoRotation *) transformFlip.getValue();
  MrTransformFlip->rotation.setValue(SbVec3f(0,1,0),float(M_PI/2.0));
  // End new part


  // Set up the callbacks
  SoEventCallback *myCallback = new SoEventCallback();
  myCallback->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
                               SoDblSiStripWedgeDetKit::activateDetector,
                               this);
  myCallback->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
                               SoDblSiStripWedgeDetKit::deactivateDetector  ,
                               this);
  myCallback->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
                               SoDblSiStripWedgeDetKit::translateDetector,
                               this);
  myCallback->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
                               SoDblSiStripWedgeDetKit::detranslateDetector  ,
                               this);
  myCallback->addEventCallback(SoMouseButtonEvent::getClassTypeId(),
                               SoDblSiStripWedgeDetKit::activateStrip,
                               this);
  setPart("callbackList[0]",myCallback); 
  isActivated=0; 
  isAligning=0;
  isAlignable=0; 
}
  
void SoDblSiStripWedgeDetKit::translateDetector(void *userData, SoEventCallback *eventCB){

  // Was the event previously handled?
  if (eventCB->isHandled()) return;

  // Which Strip Detector is this being called for?
  SoDblSiStripWedgeDetKit *This = (SoDblSiStripWedgeDetKit *) userData;

  // Find out whether that's the one that has been picked
  SoHandleEventAction *handleEventAction = eventCB->getAction();
  const SoPickedPoint *pickedPoint = handleEventAction->getPickedPoint();
  if (pickedPoint) {
    SoFullPath          *path        = (SoFullPath *) pickedPoint->getPath();
    SoNode *ancestorNode=NULL;
    for (int i=0;i<path->getLength();i++) {
      ancestorNode  = path->getNodeFromTail(i);
      if (ancestorNode->isOfType(SoDblSiStripWedgeDetKit::getClassTypeId()))  break;
    }
    if (This!=ancestorNode) return;
  }
  else {
    return;
  }

  // Is the detector already activated?
  if (This->isAligning) return;
  if (!This->isAlignable) return;

  // Was it a SHIFT_MB2_DOWN  event?
  const SoMouseButtonEvent *event= (SoMouseButtonEvent *) eventCB->getEvent();
  if (!SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON2)) return;
  if (event->wasCtrlDown() || !event->wasShiftDown()) return;

  // Then put a dragger on the wafer, set a bunch of flags and say goodbye.
  if (!This->translateDragger.getValue()) {
    SoTranslate2Dragger *theDragger=new SoTranslate2Dragger();
    This->translateDragger.setValue(theDragger);
    ((SoSeparator *) (This->transformSep.getValue()))->addChild(This->translateDragger.getValue());
    
    // Turn off the horrid White an yellow crosses!
    SoCube *MyCube = new SoCube();
    SoCalculator *MyCalculator = new SoCalculator();
    MyCalculator->a.connectFrom(&This->halfHeight);
    MyCalculator->b.connectFrom(&This->halfWidth1);
    MyCalculator->c.connectFrom(&This->thickness);
    MyCalculator->expression.set1Value(0,"oa=1.1*a");
    MyCalculator->expression.set1Value(1,"ob=1.1*b");
    MyCalculator->expression.set1Value(2,"oc=1.1*c");
    MyCube->width.connectFrom(&MyCalculator->oa);
    MyCube->height.connectFrom(&MyCalculator->ob);
    MyCube->depth.connectFrom(&MyCalculator->oc); 
    SoSeparator *MySeparator = new SoSeparator();
    SoMaterial  *MyMaterial = new SoMaterial;
    MyMaterial->diffuseColor.setValue(0.6F,0.6F,0);
    MyMaterial->emissiveColor.setValue(0.6F,0.6F,0);
    MySeparator->addChild(MyMaterial);
    MySeparator->addChild(MyCube);
    
    theDragger->setPart("translator",MySeparator);
    theDragger->setPart("translatorActive",NULL);
  }
  
  SoTransform         *theFamousTransform = (SoTransform *) This->localTransform.getValue();
  SoTranslate2Dragger *theFamousDragger = (SoTranslate2Dragger *) This->translateDragger.getValue();
  SbVec3f translation =theFamousDragger->translation.getValue();
  theFamousDragger->translation.setValue(translation[2],translation[1],-translation[0]);

  SoCalculator *MyCalculator2 = new SoCalculator();
  MyCalculator2->A.connectFrom(&theFamousDragger->translation);
  MyCalculator2->expression.set1Value(0,"oA=vec3f(A[2],A[1],-A[0])");

  theFamousTransform->translation.connectFrom(&MyCalculator2->oA);
  This->isAligning=TRUE;
  eventCB->setHandled();
     
}

void SoDblSiStripWedgeDetKit::detranslateDetector(void *userData, SoEventCallback *eventCB){
  // Was the event previously handled?
  if (eventCB->isHandled()) return;

  // Which Strip Detector is this being called for?
  SoDblSiStripWedgeDetKit *This = (SoDblSiStripWedgeDetKit *) userData; // Just as if ...

  // Find out whether that's the one that has been picked
  SoHandleEventAction *handleEventAction = eventCB->getAction();
  const SoPickedPoint *pickedPoint = handleEventAction->getPickedPoint();
  if (pickedPoint) {
    SoFullPath          *path        = (SoFullPath *) pickedPoint->getPath();
    SoNode *ancestorNode=NULL;
    for (int i=0;i<path->getLength();i++) {
      ancestorNode  = path->getNodeFromTail(i);
      if (ancestorNode->isOfType(SoDblSiStripWedgeDetKit::getClassTypeId()))  break;
    }
    if (This!=ancestorNode) return;
  }
  else {
    return;
  }

  // Is the detector already deactivated?
  if (!This->isAligning) return;

  // Was it a CTRL_MB2_DOWN  event?
  const SoMouseButtonEvent *event= (SoMouseButtonEvent *) eventCB->getEvent();
  if (!SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON2)) return;
  if (event->wasCtrlDown() || !event->wasShiftDown()) return;

  // Take the transform off the wafer
  SoTransform *theFamousTransform = (SoTransform *) This->localTransform.getValue();
  theFamousTransform->translation.disconnect();

  SoSeparator *ParentSeparator = (SoSeparator *) This->transformSep.getValue();
  for (int child=0;child<ParentSeparator->getNumChildren();child++) {
    if (ParentSeparator->getChild(child)->isOfType(SoTranslate2Dragger::getClassTypeId())) {
      ParentSeparator->removeChild(child);
      break;
    }
  }
  This->translateDragger.setValue(NULL);
  This->isAligning=FALSE;
  eventCB->setHandled();
}

void SoDblSiStripWedgeDetKit::activateDetector(void *userData, SoEventCallback *eventCB){

  // Was the event previously handled?
  if (eventCB->isHandled()) return;

  // Which Strip Detector is this being called for?
  SoDblSiStripWedgeDetKit *This = (SoDblSiStripWedgeDetKit *) userData;

  // Find out whether that's the one that has been picked
  SoHandleEventAction *handleEventAction = eventCB->getAction();
  const SoPickedPoint *pickedPoint = handleEventAction->getPickedPoint();

  if (pickedPoint) {
    SoFullPath          *path        = (SoFullPath *) pickedPoint->getPath();
    SoNode *ancestorNode=NULL;
    for (int i=0;i<path->getLength();i++) {
      ancestorNode  = path->getNodeFromTail(i);
      if (ancestorNode->isOfType(SoDblSiStripWedgeDetKit::getClassTypeId()))  break;
    }
    if (This!=ancestorNode) return;
  }
  else {
    return;
  }

  // Is the detector already activated?
  if (This->isActivated) return;

  // Was it a CTRL_MB1_DOWN  event?
  const SoMouseButtonEvent *event= (SoMouseButtonEvent *) eventCB->getEvent();
  if (!SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON2)) return;
  if (!event->wasCtrlDown() || event->wasShiftDown()) return;

  // Then put a dragger on the wafer, set a bunch of flags and say goodbye.
  if (!This->stripsDragger.getValue()) {
    This->stripsDragger.setValue(new SoScale1Dragger());
    ((SoSeparator *) (This->stripsSeparator.getValue()))->addChild(This->stripsDragger.getValue());
  }
  
  SoScale *theFamousScale = (SoScale *) This->stripsScale.getValue();
  SoScale1Dragger *theFamousDragger = (SoScale1Dragger *) This->stripsDragger.getValue();

  theFamousDragger->setMinScale(0.5);
  theFamousDragger->scaleFactor.setValue(theFamousScale->scaleFactor.getValue());
  theFamousScale->scaleFactor.connectFrom(&theFamousDragger->scaleFactor);
  This->isActivated=TRUE;
  eventCB->setHandled();
     
}

void SoDblSiStripWedgeDetKit::deactivateDetector(void *userData, SoEventCallback *eventCB){
  // Was the event previously handled?
  if (eventCB->isHandled()) return;

  // Which Strip Detector is this being called for?
  SoDblSiStripWedgeDetKit *This = (SoDblSiStripWedgeDetKit *) userData; // Just as if ...

  // Find out whether that's the one that has been picked
  SoHandleEventAction *handleEventAction = eventCB->getAction();
  const SoPickedPoint *pickedPoint = handleEventAction->getPickedPoint();

  if (pickedPoint) {
    SoFullPath          *path        = (SoFullPath *) pickedPoint->getPath();
    SoNode *ancestorNode=NULL;
    for (int i=0;i<path->getLength();i++) {
      ancestorNode  = path->getNodeFromTail(i);
      if (ancestorNode->isOfType(SoDblSiStripWedgeDetKit::getClassTypeId()))  break;
    }
    if (This!=ancestorNode) return;
  }
  else {
    return;
  }

  // Is the detector already deactivated?
  if (!This->isActivated) return;

  // Was it a CTRL_MB1_DOWN  event?
  const SoMouseButtonEvent *event= (SoMouseButtonEvent *) eventCB->getEvent();
  if (!SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON2)) return;
  if (!event->wasCtrlDown() || event->wasShiftDown()) return;

  // Then put a dragger on the wafer, set a bunch of flags and say goodbye.
  SoScale *theFamousScale = (SoScale *) This->stripsScale.getValue();
  theFamousScale->scaleFactor.disconnect();

  SoSeparator *ParentSeparator = (SoSeparator *) This->stripsSeparator.getValue();
  for (int child=0;child<ParentSeparator->getNumChildren();child++) {
    if (ParentSeparator->getChild(child)->isOfType(SoScale1Dragger::getClassTypeId())) {
      ParentSeparator->removeChild(child);
      break;
    }
  }
  This->stripsDragger.setValue(NULL);
  This->isActivated=FALSE;
  eventCB->setHandled();
}


void SoDblSiStripWedgeDetKit::activateStrip(void *userData, SoEventCallback *eventCB){

  // Which Strip Detector is this being called for?
  SoDblSiStripWedgeDetKit *This = (SoDblSiStripWedgeDetKit *) userData;

  // Reset the text string whenever a button goes down, handled or not:
  ((SoText2 *) (This->text.getValue()))->string="";

  // Was the event previously handled?
  if (eventCB->isHandled()) return;

  // Was it a MB1_DOWN  event?
  const SoMouseButtonEvent *event= (SoMouseButtonEvent *) eventCB->getEvent();
  if (!SoMouseButtonEvent::isButtonPressEvent(event,SoMouseButtonEvent::BUTTON1)) return;
  if (event->wasCtrlDown() || event->wasShiftDown()) return;

  // Has anything been picked?
  SoHandleEventAction *handleEventAction = eventCB->getAction();
  const SoPickedPoint *pickedPoint = handleEventAction->getPickedPoint();
  if (!pickedPoint) return;
  SoFullPath          *path        = (SoFullPath *) pickedPoint->getPath();
  SoNode *ancestorNode=NULL;
  for (int i=0;i<path->getLength();i++) {
    ancestorNode  = path->getNodeFromTail(i);
    if (ancestorNode->isOfType(SoDblSiStripWedgeDetKit::getClassTypeId()))  break;
  }
  if (This!=ancestorNode) return;

  // FIRST DEAL WITH PHI
  // Get gain, thickness
  SoSiWedgeStrips *ThePhiStrips = (SoSiWedgeStrips *) This->phiStrips.getValue();
  SoSiWedgeStrips *TheZStrips   = (SoSiWedgeStrips *) This->zStrips.getValue();
  if (ThePhiStrips) { 
    float thickness=ThePhiStrips->thickness.getValue(), gain=ThePhiStrips->gain.getValue(); 

    // Find out if you got a strip
    SoFullPath *theFullPath = (SoFullPath *) path;
    if (theFullPath->containsNode(ThePhiStrips)) {
      SoNode *theEndNode= theFullPath->getTail();
      if (theEndNode->isOfType(SoCube::getClassTypeId())) {
        SoCube *theHit = (SoCube *) theEndNode;
        char labelString[256];
        sprintf(labelString,"    %d", (int) ((theHit->width.getValue()-thickness)/gain + 0.5));
        ((SoText2 *) (This->text.getValue()))->string=labelString;
        ((SoTranslation *) This->textTranslation.getValue())->translation= pickedPoint->getPoint()+ 0.01f*pickedPoint->getNormal();
        eventCB->setHandled();
        return;
      }
    }
  }
  if (TheZStrips) {
    float thickness=TheZStrips->thickness.getValue(), gain=TheZStrips->gain.getValue(); 

    // Find out if you got a strip
    SoFullPath *theFullPath = (SoFullPath *) path;
    if (theFullPath->containsNode(TheZStrips)) {
      SoNode *theEndNode= theFullPath->getTail();
      if (theEndNode->isOfType(SoCube::getClassTypeId())) {
        SoCube *theHit = (SoCube *) theEndNode;
        char labelString[256];
        sprintf(labelString,"    %d", (int) ((theHit->width.getValue()-thickness)/gain + 0.5));
        ((SoText2 *) (This->text.getValue()))->string=labelString;
        ((SoTranslation *) This->textTranslation.getValue())->translation= pickedPoint->getPoint()+ 0.01f*pickedPoint->getNormal();
        eventCB->setHandled();
        return;
      }
    }
  }
}

// generateAlternateRep
void SoDblSiStripWedgeDetKit::generateAlternateRep(SoAction* aAction) {

  clearAlternateRep();  
  SoSiWedgeStrips *ThePhiStrips       = (SoSiWedgeStrips *)      phiStrips.getValue();
  SoSiWedgeStrips *TheZStrips         = (SoSiWedgeStrips *)      zStrips.getValue();
  SoWedge *TheWedge = (SoWedge *) wedge.getValue();
  if (ThePhiStrips)        ThePhiStrips->generateAlternateRep(aAction);
  if (TheZStrips)          TheZStrips->generateAlternateRep(aAction);
  if (TheWedge)            TheWedge->generateAlternateRep(aAction);
  alternateRep.setValue(topSeparator.getValue());
}

// clearAlternateRep
void SoDblSiStripWedgeDetKit::clearAlternateRep() {
  SoSiWedgeStrips      *ThePhiStrips                   = (SoSiWedgeStrips *)      phiStrips.getValue();
  if (ThePhiStrips) ThePhiStrips->clearAlternateRep();
  SoSiWedgeStrips      *TheZStrips                     = (SoSiWedgeStrips *)      zStrips.getValue();
  if (TheZStrips) TheZStrips->clearAlternateRep();
  SoWedge *TheWedge = (SoWedge *) wedge.getValue();
  if (TheWedge) TheWedge->clearAlternateRep();
  alternateRep.setValue(NULL);
}


void SoDblSiStripWedgeDetKit::setAlignable(SbBool flag) {
 isAlignable=flag;
}

SoTransform *SoDblSiStripWedgeDetKit::getLocalTransform () const {
  SoTransform         *theFamousTransform = (SoTransform *) localTransform.getValue();
  return theFamousTransform;
}

