// this :
#include <HEPVis/nodes/SoHighlightMaterial.h>

// HEPVis :
#include <HEPVis/actions/SoAlternateRepAction.h>

SO_NODE_SOURCE(SoHighlightMaterial)

void SoHighlightMaterial::initClass() {
  SO_NODE_INIT_CLASS(SoHighlightMaterial,SoMaterial,"Material");
}

SoHighlightMaterial::SoHighlightMaterial() 
:fSavedMaterial(0)
{
  SO_NODE_CONSTRUCTOR(SoHighlightMaterial);

  SO_NODE_ADD_FIELD(highlightColor,(1,0,0));
  SO_NODE_ADD_FIELD(alternateRep,(NULL));
}

SoHighlightMaterial::~SoHighlightMaterial() {}

void SoHighlightMaterial::setSavedMaterial(SoMaterial* aSavedMaterial) { 
  fSavedMaterial = aSavedMaterial;
}

SoMaterial* SoHighlightMaterial::getSavedMaterial() const {
  return fSavedMaterial;
}

void SoHighlightMaterial::generateAlternateRep(SoAction*) {
  SoMaterial* material = new SoMaterial;
  material->ambientColor = ambientColor;;
  material->diffuseColor = diffuseColor;
  material->specularColor = specularColor;
  material->emissiveColor = emissiveColor;
  material->shininess = shininess;
  material->transparency = transparency;
  alternateRep.setValue(material);
}

void SoHighlightMaterial::clearAlternateRep() {
  alternateRep.setValue(NULL);
}

#include <Inventor/nodes/SoGroup.h>
#include <HEPVis/nodes/SoSubNode.h>

void SoHighlightMaterial::doAction(SoAction* aAction) {
  SO_ALTERNATEREP_DO_ACTION(aAction)
  SoMaterial::doAction(aAction);
}

void SoHighlightMaterial::write(SoWriteAction* aAction){
  SoNode* altRep = alternateRep.getValue();
  if(altRep) {
    HEPVIS_SET_FIELDS_DEFAULT(FALSE)
    SoMaterial::write(aAction);
    HEPVIS_RESTORE_FIELDS_DEFAULT
    // other solution :
    //altRep->write(aAction);
  } else {
    SoMaterial::write(aAction);
  }
}
