#ifndef HEPVIS_SOINDEXEDMARKERSET_H
#define HEPVIS_SOINDEXEDMARKERSET_H

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoMarkerSet.h> //GB : not in SGI-Inventor-2.1.5.10
#include <Inventor/fields/SoMFInt32.h>

class HEPVis_SoIndexedMarkerSet : public SoMarkerSet {
  SO_NODE_HEADER(SoIndexedMarkerSet);

public:
  static void initClass(void);
  HEPVis_SoIndexedMarkerSet(void);

  SoMFInt32 index;

  virtual void GLRender(SoGLRenderAction * action);
  virtual void getPrimitiveCount(SoGetPrimitiveCountAction * action);

protected:

  virtual void computeBBox(SoAction * action, SbBox3f & box,
                           SbVec3f & center);

  virtual ~HEPVis_SoIndexedMarkerSet();

private:
  enum Binding {
    OVERALL = 0,
    PER_VERTEX
  };
  Binding findMaterialBinding(SoState * const state) const;
  
};

#endif // !COIN_SOINDEXEDMARKERSET_H
