// this :
#include <HEPVis/SbLined.h>

SbLined::SbLined(void){}

SbLined::SbLined(const SbVec3d& origin, const SbVec3d& point) {
  this->setValue(origin, point);
}

void SbLined::setValue(const SbVec3d& origin, const SbVec3d& point) {
  this->pos = origin;
  this->dir = point - origin;
  // we test for a null vector above, just normalize
  this->dir.normalize();
}

SbBool SbLined::getClosestPoints(const SbLined& line2,SbVec3d& ptOnThis, SbVec3d& ptOnLine2) const {
  // Check if the lines are parallel.
  // FIXME: should probably use equals() here.
  if(line2.dir == this->dir) return FALSE;
  else if(line2.dir == -this->dir) return FALSE;

  SbVec3d P0 = this->pos;
  SbVec3d P1 = line2.pos;
  SbVec3d D0 = this->dir;
  SbVec3d D1 = line2.dir;
  SbVec3d D0N = D0;

  // we warn about lines with no direction above, just normalize
  D0N.normalize();

  double c[3], d[3];

  for (int i=0; i < 3; i++) {
    d[i] =
      D1[i] - D0N[i]*(D0[0]*D1[0] + D0[1]*D1[1] + D0[2]*D1[2]);
    c[i] =
      P1[i] - P0[i] + D0N[i]*(D0[0]*P0[0] + D0[1]*P0[1] + D0[2]*P0[2]);
  }

  double t = 
    -(c[0]*d[0]+c[1]*d[1]+c[2]*d[2]) / (d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);

  ptOnLine2 = line2.pos + line2.dir * t;
  ptOnThis = this->getClosestPoint(ptOnLine2);

  return TRUE;
}

SbVec3d SbLined::getClosestPoint(const SbVec3d & point) const {
  double numerator = (point - this->pos).dot(this->dir);
  double denumerator = this->dir.length();
  return (this->pos + this->dir * (numerator/denumerator));
}

const SbVec3d& SbLined::getPosition(void) const { return this->pos;}

const SbVec3d& SbLined::getDirection(void) const { return this->dir;}
