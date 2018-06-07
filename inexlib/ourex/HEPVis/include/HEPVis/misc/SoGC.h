#ifndef HEPVis_SoGC_h
#define HEPVis_SoGC_h

// GC is for Graphical Context.

// Inheritance :
#include <HEPVis/SbStyle.h>

class SbPolyhedron;
class SoSeparator;
class SoStyleCache;
class SoHighlightMaterial;
class SoLightModel;
class SoDrawStyle;
class SoPolyhedron;
class SoShapeCache;

class SoGC : public SbStyle {
public:
  SoGC();
  virtual ~SoGC();
public:
  void clear();
  virtual void reset();
  SoHighlightMaterial* getHighlightMaterial();
  SoLightModel* getLightModel();
  SoDrawStyle* getDrawStyle();

  void setSeparator(SoSeparator* aValue){fSeparator = aValue;}
  SoSeparator* getSeparator() const {return fSeparator;}

  void setStyleCache(SoStyleCache*);
  SoStyleCache* getStyleCache() const;

  void setLocalSetup(SbBool aValue){fLocalSetup = aValue;}
  SbBool getLocalSetup() const {return fLocalSetup;}

  SbPolyhedron* getShapeCut();
  SoPolyhedron* getPolyhedron(const SbPolyhedron&);
  void setDefaultStyleCache();
private:
  SoSeparator* fSeparator;
  SoStyleCache* fStyleCache;
  SbBool fLocalSetup;
  SbBool fShapeCutDone;
  SbPolyhedron* fShapeCut;
  SoShapeCache* fShapeCache;
};

#endif
