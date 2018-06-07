#ifndef HEPVis_SoHighlightMaterial_h
#define HEPVis_SoHighlightMaterial_h

/**
 * SoHighlightMaterial
 * 
 * To handle multiple highlighted objects within a scene.
 * 
 * @author Guy Barrand
 * 
 */

// Inheritance :
#include <Inventor/nodes/SoMaterial.h>

#include <Inventor/fields/SoMFColor.h>
#include <Inventor/fields/SoSFNode.h>

class SoHighlightMaterial : public SoMaterial {
  SO_NODE_HEADER(SoHighlightMaterial);
public :
  SoMFColor highlightColor;
  SoSFNode alternateRep;
public :
  SoHighlightMaterial();
  void setSavedMaterial(SoMaterial*);
  SoMaterial* getSavedMaterial() const;
  virtual void generateAlternateRep(SoAction*);
  virtual void clearAlternateRep();
public :
  static void initClass();
protected :
  virtual ~SoHighlightMaterial();
  virtual void doAction(SoAction*);
  virtual void write(SoWriteAction*);
private:
  SoMaterial* fSavedMaterial;
};

#endif
