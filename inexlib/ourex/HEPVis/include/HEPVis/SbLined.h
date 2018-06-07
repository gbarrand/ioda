#ifndef COIN_SBLINED_H
#define COIN_SBLINED_H

#include <Inventor/SbVec3d.h>

class SbLined {
  SbLined(void);
public:
  SbLined(const SbVec3d & origin, const SbVec3d & point);
  void setValue(const SbVec3d & origin, const SbVec3d & point);
  SbBool getClosestPoints(const SbLined & line2,
                          SbVec3d & ptOnThis, SbVec3d & ptOnLine2) const;
  SbVec3d getClosestPoint(const SbVec3d & point) const;
  const SbVec3d & getPosition(void) const;
  const SbVec3d & getDirection(void) const;
private:
  SbVec3d pos, dir;
};

#endif // !COIN_SBLINE_H
