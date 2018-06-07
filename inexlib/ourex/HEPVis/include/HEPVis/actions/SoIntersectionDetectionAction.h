#ifndef HEPVIS_SOINTERSECTIONDETECTIONACTION_H
#define HEPVIS_SOINTERSECTIONDETECTIONACTION_H

// A "double" version of the CoinGL/SoIntersectionDetectionAction.

#include <Inventor/actions/SoSubAction.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoCallbackAction.h>

#include <Inventor/SbVec3d.h>

struct HEPVis_SoIntersectingPrimitive {
  SoPath * path;
  enum PrimitiveType {
    SEGMENT = 2,
    LINE_SEGMENT = 2,
    TRIANGLE = 3
  } type;
  SbVec3d vertex[3];
  SbVec3d xf_vertex[3];
};

class HEPVis_SoIntersectionDetectionActionP;

class /*COIN_DLL_API*/ HEPVis_SoIntersectionDetectionAction : public SoAction {
  typedef SoAction hinherited;
  SO_ACTION_HEADER(HEPVis_SoIntersectionDetectionAction);
public:
  static void initClass(void);
  HEPVis_SoIntersectionDetectionAction(void);
  virtual ~HEPVis_SoIntersectionDetectionAction(void);

  enum Resp {
    NEXT_PRIMITIVE,
    NEXT_SHAPE,
    ABORT
  };

  typedef SoCallbackAction::Response HEPVis_SoIntersectionVisitationCB(void * closure, const SoPath * where);
  typedef SbBool HEPVis_SoIntersectionFilterCB(void * closure, const SoPath * p1, const SoPath * p2);
  typedef Resp HEPVis_SoIntersectionCB(void * closure, const HEPVis_SoIntersectingPrimitive * p1, const HEPVis_SoIntersectingPrimitive * p2);

  void setIntersectionDetectionEpsilon(double epsilon);
  double getIntersectionDetectionEpsilon(void) const;

  static void setIntersectionEpsilon(double epsilon);
  static double getIntersectionEpsilon(void);

  void setTypeEnabled(SoType type, SbBool enable);
  SbBool isTypeEnabled(SoType type, SbBool checkgroups = FALSE) const;

  void setManipsEnabled(SbBool enable);
  SbBool isManipsEnabled(void) const;

  void setDraggersEnabled(SbBool enable);
  SbBool isDraggersEnabled(void) const;

  void setShapeInternalsEnabled(SbBool enable);
  SbBool isShapeInternalsEnabled(void) const;

  void addVisitationCallback(SoType type, HEPVis_SoIntersectionVisitationCB * cb, void * closure);
  void removeVisitationCallback(SoType type, HEPVis_SoIntersectionVisitationCB * cb, void * closure);

  virtual void apply(SoNode * node);
  virtual void apply(SoPath * path);
  virtual void apply(const SoPathList & paths, SbBool obeysRules = FALSE);

  virtual void setFilterCallback(HEPVis_SoIntersectionFilterCB * cb, void * closure = NULL);
  virtual void addIntersectionCallback(HEPVis_SoIntersectionCB * cb, void * closure  = NULL);
  virtual void removeIntersectionCallback(HEPVis_SoIntersectionCB * cb, void * closure  = NULL);

private:
  HEPVis_SoIntersectionDetectionActionP * pimpl;
};

#endif // !COIN_SOINTERSECTIONDETECTIONACTION_H
