#ifndef SoEdgedFaceSet_h
#define SoEdgedFaceSet_h 

#include <Inventor/nodes/SoFaceSet.h>

class SoEdgedFaceSet : public SoFaceSet {
  SO_NODE_HEADER(SoEdgedFaceSet);
public:
  SoEdgedFaceSet();
public:
  virtual void GLRender(SoGLRenderAction*);
public:
  static void initClass();
protected:
  ~SoEdgedFaceSet();
};

#endif
