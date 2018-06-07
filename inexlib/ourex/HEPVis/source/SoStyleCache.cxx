// this :
#include <HEPVis/misc/SoStyleCache.h>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoMaterialBinding.h>

#include <HEPVis/nodes/SoHighlightMaterial.h>

const SbName& SoStyleCache::cachedName() {
  static const SbName name("cached"); //read only static, then ok.
  return name;
}

SoStyleCache::SoStyleCache() 
:fMaterials(0)
,fHighlightMaterials(0)
,fLineStyles(0)
,fFilled(0)
,fPoints(0)
,fPhong(0)
,fBaseColor(0)
,fPerFace(0)
,fPerFaceIndexed(0)
,fOverall(0)
,fNormalZ(0)
,fMB_PerFace(0)
,fMB_PerFaceIndexed(0)
,fMB_Overall(0)
,fResetTransform(0)
{}

SoStyleCache::~SoStyleCache(){}

void SoStyleCache::clearCache(){
  if(fMaterials) fMaterials->removeAllChildren();
  if(fHighlightMaterials) fHighlightMaterials->removeAllChildren();
  if(fLineStyles) fLineStyles->removeAllChildren();

  removeAllChildren();

  fMaterials = 0;
  fHighlightMaterials = 0;
  fLineStyles = 0;
  fFilled = 0;
  fPoints = 0;
  fPhong = 0;
  fBaseColor = 0;
  fPerFace = 0;
  fPerFaceIndexed = 0;
  fOverall = 0;
  fNormalZ = 0;
  fMB_PerFace = 0;
  fMB_PerFaceIndexed = 0;
  fMB_Overall = 0;

  fResetTransform = 0;
}

SoMaterial* SoStyleCache::getMaterial(
 const SbColor& aRGB
,float aTransparency
,SbBool aSetEmissiveColor
){
  if(!fMaterials) {
    fMaterials = new SoGroup;
    addChild(fMaterials);
  }
  int number = fMaterials->getNumChildren();
  for(int index=0;index<number;index++) { 
    SoMaterial* material = (SoMaterial*)fMaterials->getChild(index);
    if( (material->diffuseColor[0]==aRGB) &&
        (material->transparency[0]==aTransparency) &&
        ( ((aSetEmissiveColor==TRUE) && (material->emissiveColor[0]==aRGB)) ||
          ((aSetEmissiveColor==FALSE) && 
             (material->emissiveColor[0]==SbColor(0.0F,0.0F,0.0F))) )
      ) {
      return material;
    }
  }
  SoMaterial* material = new SoMaterial;
  material->setName(cachedName());
  material->diffuseColor.setValue(aRGB);
  if(aSetEmissiveColor==TRUE) material->emissiveColor.setValue(aRGB);
  material->transparency.setValue(aTransparency);
  fMaterials->addChild(material);
  return material;
}

SoMaterial* SoStyleCache::getMaterial(
 float aRed
,float aGreen
,float aBlue
,float aTransparency
,SbBool aSetEmissiveColor
){
  return getMaterial
           (SbColor(aRed,aGreen,aBlue),aTransparency,aSetEmissiveColor);
}

SoHighlightMaterial* SoStyleCache::getHighlightMaterial(
 const SbColor& aRGB
,const SbColor& aHighlightRGB
,float aTransparency
,SbBool aSetEmissiveColor
){
  if(!fHighlightMaterials) {
    fHighlightMaterials = new SoGroup;
    addChild(fHighlightMaterials);
  }
  int number = fHighlightMaterials->getNumChildren();
  for(int index=0;index<number;index++) { 
    SoHighlightMaterial* material = 
      (SoHighlightMaterial*)fHighlightMaterials->getChild(index);
    if( (material->diffuseColor[0]==aRGB) &&
        (material->highlightColor[0]==aHighlightRGB) &&
        (material->transparency[0]==aTransparency) &&
        ( ((aSetEmissiveColor==TRUE) && (material->emissiveColor[0]==aRGB)) ||
          ((aSetEmissiveColor==FALSE) && 
             (material->emissiveColor[0]==SbColor(0.0F,0.0F,0.0F))) )
      ) {
      return material;
    }
  }
  SoHighlightMaterial* material = new SoHighlightMaterial;
  material->setName(cachedName());
  material->diffuseColor.setValue(aRGB);
  if(aSetEmissiveColor==TRUE) material->emissiveColor.setValue(aRGB);
  material->highlightColor.setValue(aHighlightRGB);
  material->transparency.setValue(aTransparency);
  fHighlightMaterials->addChild(material);
  return material;
}

SoHighlightMaterial* SoStyleCache::getHighlightMaterial(
 float aRed
,float aGreen
,float aBlue
,float aHRed
,float aHGreen
,float aHBlue
,float aTransparency
,SbBool aSetEmissiveColor
){
  return getHighlightMaterial(SbColor(aRed,aGreen,aBlue),
                              SbColor(aHRed,aHGreen,aHBlue),
                              aTransparency,aSetEmissiveColor);
}

SoDrawStyle* SoStyleCache::getLineStyle(
 SbLinePattern aPattern
,float aWidth
){
  if(!fLineStyles) {
    fLineStyles = new SoGroup;
    addChild(fLineStyles);
  }
  int number = fLineStyles->getNumChildren();
  for(int index=0;index<number;index++) { 
    SoDrawStyle* drawStyle = (SoDrawStyle*)fLineStyles->getChild(index);
    if( (drawStyle->style.getValue()==SoDrawStyle::LINES) &&
        (drawStyle->lineWidth.getValue()==aWidth) &&
        (drawStyle->linePattern.getValue()==aPattern) ) {
      return drawStyle;
    }
  }
  SoDrawStyle* drawStyle = new SoDrawStyle;
  drawStyle->setName(cachedName());
  drawStyle->style.setValue(SoDrawStyle::LINES);
  drawStyle->lineWidth.setValue(aWidth);
  drawStyle->linePattern.setValue(aPattern);
  fLineStyles->addChild(drawStyle);
  return drawStyle;
}

SoDrawStyle* SoStyleCache::getFilled(){
  if(!fFilled) {
    fFilled = new SoDrawStyle;
    fFilled->setName(cachedName());
    fFilled->style.setValue(SoDrawStyle::FILLED);
    addChild(fFilled);
  } 
  return fFilled;
}

SoDrawStyle* SoStyleCache::getPoints(){
  if(!fPoints) {
    fPoints = new SoDrawStyle;
    fPoints->setName(cachedName());
    fPoints->style.setValue(SoDrawStyle::POINTS);
    addChild(fPoints);
  } 
  return fPoints;
}

SoLightModel* SoStyleCache::getLightModelPhong(){
  if(!fPhong) {
    fPhong = new SoLightModel;
    fPhong->setName(cachedName());
    fPhong->model.setValue(SoLightModel::PHONG);
    addChild(fPhong);
  } 
  return fPhong;
}

SoLightModel* SoStyleCache::getLightModelBaseColor(){
  if(!fBaseColor) {
    fBaseColor = new SoLightModel;
    fBaseColor->setName(cachedName());
    fBaseColor->model.setValue(SoLightModel::BASE_COLOR);
    addChild(fBaseColor);
  } 
  return fBaseColor;
}

SoResetTransform* SoStyleCache::getResetTransform(){
  if(!fResetTransform) {
    fResetTransform = new SoResetTransform;
    fResetTransform->setName(cachedName());
    addChild(fResetTransform);
  } 
  return fResetTransform;
}

SoNormalBinding* SoStyleCache::getNormalBindingOverall(){
  if(!fOverall) {
    fOverall = new SoNormalBinding;
    fOverall->setName(cachedName());
    fOverall->value = SoNormalBinding::OVERALL;
    addChild(fOverall);
  } 
  return fOverall;
}

SoNormalBinding* SoStyleCache::getNormalBindingPerFace(){
  if(!fPerFace) {
    fPerFace = new SoNormalBinding;
    fPerFace->setName(cachedName());
    fPerFace->value = SoNormalBinding::PER_FACE;
    addChild(fPerFace);
  } 
  return fPerFace;
}

SoNormalBinding* SoStyleCache::getNormalBindingPerFaceIndexed(){
  if(!fPerFaceIndexed) {
    fPerFaceIndexed = new SoNormalBinding;
    fPerFaceIndexed->setName(cachedName());
    fPerFaceIndexed->value = SoNormalBinding::PER_FACE_INDEXED;
    addChild(fPerFaceIndexed);
  } 
  return fPerFaceIndexed;
}

SoNormal* SoStyleCache::getNormalZ(){
  if(!fNormalZ) {
    fNormalZ = new SoNormal;
    fNormalZ->setName(cachedName());
    SbVec3f nz(0,0,1);
    fNormalZ->vector.setValues(0,1,&nz);
    addChild(fNormalZ);
  } 
  return fNormalZ;
}

SoMaterialBinding* SoStyleCache::getMaterialBindingOverall(){
  if(!fMB_Overall) {
    fMB_Overall = new SoMaterialBinding;
    fMB_Overall->setName(cachedName());
    fMB_Overall->value = SoMaterialBinding::OVERALL;
    addChild(fMB_Overall);
  } 
  return fMB_Overall;
}

SoMaterialBinding* SoStyleCache::getMaterialBindingPerFace(){
  if(!fMB_PerFace) {
    fMB_PerFace = new SoMaterialBinding;
    fMB_PerFace->setName(cachedName());
    fMB_PerFace->value = SoMaterialBinding::PER_FACE;
    addChild(fMB_PerFace);
  } 
  return fMB_PerFace;
}

SoMaterialBinding* SoStyleCache::getMaterialBindingPerFaceIndexed(){
  if(!fMB_PerFaceIndexed) {
    fMB_PerFaceIndexed = new SoMaterialBinding;
    fMB_PerFaceIndexed->setName(cachedName());
    fMB_PerFaceIndexed->value = SoMaterialBinding::PER_FACE_INDEXED;
    addChild(fMB_PerFaceIndexed);
  } 
  return fMB_PerFaceIndexed;
}
