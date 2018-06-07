#ifndef COIN_SBTRI3D_H
#define COIN_SBTRI3D_H

#include <Inventor/SbVec3d.h>
#include <Inventor/SbBox3d.h>

//class SbVec3f;

class SbTri3d {
  SbTri3d(void);
public:
  SbTri3d(const SbTri3d & t);
  SbTri3d(const SbVec3d & a, const SbVec3d & b, const SbVec3d & c);
  ~SbTri3d(void);

  SbTri3d & setValue(const SbTri3d & t);
  SbTri3d & setValue(const SbVec3d & a, const SbVec3d & b, const SbVec3d & c);
  void getValue(SbTri3d & t) const;
  void getValue(SbVec3d & a, SbVec3d & b, SbVec3d & c) const;
  //void getValue(SbVec3f & a, SbVec3f & b, SbVec3f & c) const;
  SbVec3d getNormal() const;
  double getDistance(const SbTri3d & t) const;
  static double getDistance(const SbVec3d & p, 
                           const SbVec3d & p1, const SbVec3d & p2);
  double getDistance(const SbVec3d & p) const;
  double getDistance(const SbVec3d & p1, const SbVec3d & p2) const;
  static double sqrDistance(const SbVec3d & a1, const SbVec3d & a2,
                           const SbVec3d & b1, const SbVec3d & b2,
                           double * linP0, double * linP1);
  double sqrDistance(const SbVec3d & p1, 
                    double * pfSParam = NULL, double * pfTParam = NULL) const;

  SbTri3d & operator = (const SbTri3d & t);

  SbBool intersect(const SbTri3d & triangle) const;
  SbBool intersect(const SbTri3d & triangle, double epsilon) const;

  const SbBox3d getBoundingBox(void) const;

private:
  SbVec3d a;
  SbVec3d b;
  SbVec3d c;
};

#endif // !COIN_SBTRI3F_H
