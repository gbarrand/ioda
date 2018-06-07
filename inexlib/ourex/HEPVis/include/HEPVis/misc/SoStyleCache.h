//  27/03/2004 : G.Barrand : creation.

#ifndef HEPVis_SoStyleCache_h
#define HEPVis_SoStyleCache_h

#include <Inventor/nodes/SoGroup.h>

#include <HEPVis/SbStyles.h>

class SoMaterial;
class SoHighlightMaterial;
class SoDrawStyle;
class SoLightModel;
class SoResetTransform;
class SoNormalBinding;
class SoNormal;
class SoMaterialBinding;
class SbColor;
 
class SoStyleCache : public SoGroup {
public:
  SoStyleCache();
protected:
  virtual ~SoStyleCache();
public:
  static const SbName& cachedName(); 
public:
  SoMaterial* getMaterial(const SbColor&,float = 0,SbBool = FALSE); 
  SoMaterial* getMaterial(float,float,float,float = 0,SbBool = FALSE); 
  SoHighlightMaterial* getHighlightMaterial(const SbColor&,const SbColor&,float transparency = 0,SbBool set_emissiveColor = FALSE); 
  SoHighlightMaterial* getHighlightMaterial(float,float,float,float,float,float,float transparency = 0,SbBool set_emissiveColor = FALSE); 
  SoDrawStyle* getLineStyle(SbLinePattern = SbLinePattern_solid,float = 0); 
  SoDrawStyle* getFilled(); 
  SoDrawStyle* getPoints(); 
  SoLightModel* getLightModelPhong(); 
  SoLightModel* getLightModelBaseColor(); 
  SoResetTransform* getResetTransform();
  SoNormalBinding* getNormalBindingOverall();
  SoNormalBinding* getNormalBindingPerFace();
  SoNormalBinding* getNormalBindingPerFaceIndexed();
  SoNormal* getNormalZ();
  SoMaterialBinding* getMaterialBindingOverall();
  SoMaterialBinding* getMaterialBindingPerFace();
  SoMaterialBinding* getMaterialBindingPerFaceIndexed();
  void clearCache();
private:
  SoGroup* fMaterials;
  SoGroup* fHighlightMaterials;
  SoGroup* fLineStyles;
  SoDrawStyle* fFilled;
  SoDrawStyle* fPoints;
  SoLightModel* fPhong;
  SoLightModel* fBaseColor;
  SoNormalBinding* fPerFace;
  SoNormalBinding* fPerFaceIndexed;
  SoNormalBinding* fOverall;
  SoNormal* fNormalZ;
  SoMaterialBinding* fMB_PerFace;
  SoMaterialBinding* fMB_PerFaceIndexed;
  SoMaterialBinding* fMB_Overall;
  SoResetTransform* fResetTransform;
};

#endif
