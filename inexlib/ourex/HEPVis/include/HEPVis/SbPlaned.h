#ifndef COIN_SBPLANED_H
#define COIN_SBPLANED_H


#include <stdio.h>

#include <Inventor/SbVec3d.h>

class SbLined;

class SbPlaned {
  SbPlaned(void);
public:
  SbPlaned(const SbVec3d& normal, const double D);
  SbPlaned(const SbVec3d& p0, const SbVec3d& p1, const SbVec3d& p2);
  SbPlaned(const SbVec3d& normal, const SbVec3d& point);

  void offset(const double d);
  SbBool intersect(const SbLined& l, SbVec3d& intersection) const;
  SbBool isInHalfSpace(const SbVec3d& point) const;
  double getDistance(const SbVec3d &point) const;
  const SbVec3d& getNormal(void) const;
  double getDistanceFromOrigin(void) const;

  //friend int operator ==(const SbPlaned& p1, const SbPlaned& p2);
  //friend int operator !=(const SbPlaned& p1, const SbPlaned& p2);

  SbBool intersect(const SbPlaned & pl, SbLined & line);

private:
  SbVec3d normal;
  double distance;
};

//int operator ==(const SbPlaned& p1, const SbPlaned& p2);
//int operator !=(const SbPlaned& p1, const SbPlaned& p2);

#endif // !COIN_SBPLANE_H
