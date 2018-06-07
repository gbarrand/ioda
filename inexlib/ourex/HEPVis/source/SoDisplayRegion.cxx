// this :
#include <HEPVis/nodekits/SoDisplayRegion.h>

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoGroup.h>

// HEPVis :
#include <HEPVis/SbString.h>
#include <HEPVis/SbFile.h>
#include <HEPVis/SbMath.h>
#include <HEPVis/nodes/SoViewportRegion.h>
#include <HEPVis/actions/SoAlternateRepAction.h>

SO_KIT_SOURCE(SoDisplayRegion) 

void SoDisplayRegion::initClass(){
  SO_KIT_INIT_CLASS(SoDisplayRegion,SoRegion,"SoRegion");
}

SoDisplayRegion::SoDisplayRegion(){
  SO_KIT_CONSTRUCTOR(SoDisplayRegion);
  SO_KIT_ADD_CATALOG_ENTRY(topSeparator,SoSeparator,FALSE,this,\0, FALSE);
  SO_KIT_ADD_CATALOG_ENTRY(light,SoDirectionalLight,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(lightModel,SoLightModel,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(cameraGroup,SoGroup,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(sceneSeparator,SoSeparator,FALSE,topSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(transform,SoTransform,FALSE,sceneSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(dynamicScene,SoSeparator,FALSE,sceneSeparator,\0,TRUE);
  SO_KIT_ADD_CATALOG_ENTRY(staticScene,SoSeparator,FALSE,sceneSeparator,\0,TRUE);
  SO_KIT_INIT_INSTANCE();

  SO_NODE_ADD_FIELD(projection,(""));

  SO_NODE_ADD_FIELD(alternateRep,(NULL));

  SoViewportRegion* soViewportRegion = 
    (SoViewportRegion*)viewportRegion.getValue();
  soViewportRegion->backgroundColor.setValue(SbColor(0,0,0));
  soViewportRegion->horizontalBorder.setValue(1);
  soViewportRegion->verticalBorder.setValue(1);

  // Default camera and light model setting for 3D :
  SoDirectionalLight* soLight = (SoDirectionalLight*)light.getValue();
  soLight->color.setValue(1,1,1);
  // Same as Coin<Gui>/So<Gui>Viewer :
  soLight->direction.setValue(SbVec3f(1,-1,-10));

  SoLightModel* soLightModel = (SoLightModel*)lightModel.getValue();
  soLightModel->model.setValue(SoLightModel::PHONG);

  // cameraGroup avoids to have the type of the camera
  // saved in the catalog list (and then let freedom
  // to change the type of camera).
  SoGroup* soCameraGroup = (SoGroup*)cameraGroup.getValue();
  soCameraGroup->addChild(new SoOrthographicCamera);

  SoDisplayRegion::resetCamera();
}

SoDisplayRegion::~SoDisplayRegion(){}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SoDisplayRegion::doAction(SoAction* aAction){
  SO_ALTERNATEREP_DO_ACTION(aAction)
  SoRegion::doAction(aAction);
}


void SoDisplayRegion::updateChildren() {}

void SoDisplayRegion::write(SoWriteAction* aAction) {
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    altRep->write(aAction);
  } else {
    SoBaseKit::write(aAction);
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SoSeparator* SoDisplayRegion::getSceneSeparator() const {return (SoSeparator*)sceneSeparator.getValue();}
SoSeparator* SoDisplayRegion::getStaticScene() const {return (SoSeparator*)staticScene.getValue();}
SoSeparator* SoDisplayRegion::getDynamicScene() const {return (SoSeparator*)dynamicScene.getValue();}
SoDirectionalLight* SoDisplayRegion::getLight() const {return (SoDirectionalLight*)light.getValue();}
SoLightModel* SoDisplayRegion::getLightModel() const {return (SoLightModel*)lightModel.getValue();}
SoTransform* SoDisplayRegion::getTransform() const {return (SoTransform*)transform.getValue();}

SbBool SoDisplayRegion::doIt(const SbDoIt& aDoIt) {
  SbAddNode* doIt = (SbAddNode*)aDoIt.cast("SbAddNode");
  if(!doIt) {
    SoDebugError::postInfo("SoDisplayRegion::doIt","unknown SbDoIt.");
    return FALSE;
  }
  SoNode* node = doIt->getNode();
  if(!node) return FALSE;
  SoNode* soPlace = getPart(doIt->getPlacement(),FALSE);
  if(!soPlace) {
    //backward compatibilty :
    SbString placement = doIt->getPlacement();
    if(placement=="static") {
      placement = "staticScene";
      SoDebugError::postInfo("SoDisplayRegion::doIt",
        "\"static\" deprecated. Use staticScene.");
    } else if(placement=="dynamic") {
      placement = "dynamicScene";
      SoDebugError::postInfo("SoDisplayRegion::doIt",
        "\"dynamic\" deprecated. Use dynamicScene.");
    }
    soPlace = getPart(placement,FALSE);
  }
  if(!soPlace) {
    SoDebugError::postInfo("SoDisplayRegion::doIt",
        "placement \"%s\" not found.",doIt->getPlacement().getString());
    return FALSE;
  }
  if(soPlace->isOfType(SoGroup::getClassTypeId())==FALSE) {
    SoDebugError::postInfo("SoDisplayRegion::doIt",
        "placement \"%s\" not an SoGroup.",doIt->getPlacement().getString());
    return FALSE;
  }
  ((SoGroup*)soPlace)->addChild(node);
  fDoList.push_back(std::pair<SoGroup*,SoNode*>((SoGroup*)soPlace,node));    
  doIt->reset(); // So that the aDoIt delete does not unref node.
  return TRUE;
}

void SoDisplayRegion::undo(){
  unsigned int number = fDoList.size();
  for(unsigned int index=0;index<number;index++) {
    fDoList[index].first->removeChild(fDoList[index].second);
  }
  fDoList.clear();
}

void SoDisplayRegion::resetUndo(){fDoList.clear();}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SoSeparator* SoDisplayRegion::getTopSeparator() const {return (SoSeparator*)topSeparator.getValue();}

SoCamera* SoDisplayRegion::getCamera() const {
  SoGroup* soCameraGroup = (SoGroup*)cameraGroup.getValue();
  if(!soCameraGroup->getNumChildren()) return 0;
  return (SoCamera*)soCameraGroup->getChild(0);
}

void SoDisplayRegion::clear(const SbString& aWhat){
  if(aWhat=="staticScene") {
    SoSeparator* soStatic = (SoSeparator*)staticScene.getValue();
    soStatic->removeAllChildren();
  } else if(aWhat=="dynamicScene") {
    SoSeparator* soDynamic = (SoSeparator*)dynamicScene.getValue();
    soDynamic->removeAllChildren();
  } else { 
    SoSeparator* soStatic = (SoSeparator*)staticScene.getValue();
    soStatic->removeAllChildren();
    SoSeparator* soDynamic = (SoSeparator*)dynamicScene.getValue();
    soDynamic->removeAllChildren();
  }
}

void SoDisplayRegion::resetCamera(){
  SoCamera* soCamera = getCamera();
  if(!soCamera) return;
  soCamera->viewportMapping.setValue(SoCamera::ADJUST_CAMERA);
  //camera->aspectRatio.setValue(10);
  soCamera->position.setValue(0,0,10);
  soCamera->orientation.setValue(SbRotation(SbVec3f(0,1,0),0));
  soCamera->nearDistance.setValue(1);
  soCamera->farDistance.setValue(100);
  soCamera->focalDistance.setValue(10);
  if(soCamera->isOfType(SoOrthographicCamera::getClassTypeId())==TRUE) {
    SoOrthographicCamera* soOrthoCamera = (SoOrthographicCamera*)soCamera;
    soOrthoCamera->height.setValue(10);
  } else if(soCamera->isOfType(SoPerspectiveCamera::getClassTypeId())==TRUE) {
    SoPerspectiveCamera* soPersCamera = (SoPerspectiveCamera*)soCamera;
    soPersCamera->heightAngle.setValue(FM_PI/4.0F);
  }
}

SbBool SoDisplayRegion::connectFrom(SoRegion* aFrom){
  // "this" region will display the aFrom scene.
  if(aFrom->isOfType(SoDisplayRegion::getClassTypeId())==FALSE) return FALSE;
  if(aFrom==this) return FALSE;
  SoSeparator* fromStaticNode = ((SoDisplayRegion*)aFrom)->getStaticScene();
  if(!fromStaticNode) return FALSE;
  SoSeparator* fromDynamicNode = ((SoDisplayRegion*)aFrom)->getDynamicScene();
  if(!fromDynamicNode) return FALSE;

  SoSeparator* staticNode = getStaticScene();
  if(!staticNode) return FALSE;
  SoSeparator* dynamicNode = getDynamicScene();
  if(!dynamicNode) return FALSE;

  // Put "aFrom" nodes under "this" ones.
  // "this" will then display also the "aFrom" nodes.
  staticNode->addChild(fromStaticNode);
  dynamicNode->addChild(fromDynamicNode);

  return TRUE;
}

SoNode* SoDisplayRegion::getNodeOfInterest(const SbString& aWhat) const{
  if(aWhat=="scene") {
    return sceneSeparator.getValue();
  } else if(aWhat=="staticScene") {
    return staticScene.getValue();
  } else if(aWhat=="dynamicScene") {
    return dynamicScene.getValue();
  } else { //region
    return topSeparator.getValue();
  }
}

SbBool SoDisplayRegion::readScene(
 const SbString& aFile
,const SbString& //aFormat
,const SbString& aPlacement //"staticScene"
){
  // Solve environment variables :
  SbString name = SbFile::getFullName(aFile);

  SoInput soInput;
  if(!soInput.openFile(name.getString())) return FALSE;

  SoSeparator* separator = SoDB::readAll(&soInput);
  if(!separator) return FALSE;
  //separator ref count is 0.
  separator->ref();

  SbAddNode sbAdd(separator,aPlacement==""?"staticScene":aPlacement);
  doIt(sbAdd);
  sbAdd.reset(); //to disconnect dstor unref.

  separator->unref();

  return TRUE;
}

void SoDisplayRegion::setCameraType(SoType aType){
  SoCamera* soCamera = getCamera();
  if(soCamera && soCamera->isOfType(aType)==TRUE) return;
  SoNode* node = (SoNode*)aType.createInstance();
  if(node->isOfType(SoCamera::getClassTypeId())==FALSE) {
    node->unref();
    return;
  }
  SoGroup* soCameraGroup = (SoGroup*)cameraGroup.getValue();
  soCameraGroup->removeAllChildren();
  soCameraGroup->addChild(node);
  resetCamera();
}

void SoDisplayRegion::setCamera(SoCamera* aCamera){
  SoGroup* soCameraGroup = (SoGroup*)cameraGroup.getValue();
  soCameraGroup->removeAllChildren();
  soCameraGroup->addChild(aCamera);
  //Don't do : resetCamera();
}

void SoDisplayRegion::generateAlternateRep(SoAction*) {alternateRep.setValue(getAlternateRep());}

void SoDisplayRegion::clearAlternateRep() {alternateRep.setValue(NULL);}
