// this :
#include <HEPVis/SbPlaned.h>

#include <HEPVis/SbLined.h>

#include <float.h> //DBL_EPSILON

SbPlaned::SbPlaned(void){}

SbPlaned::SbPlaned(const SbVec3d& normalref, const double D) {
  this->normal = normalref;
  // we test for a null vector above, just normalize
  this->normal.normalize();
  this->distance = D;
}

SbPlaned::SbPlaned(const SbVec3d& p0, const SbVec3d& p1, const SbVec3d& p2) {
  this->normal = (p1 - p0).cross(p2 - p0);
  this->distance = this->normal.dot(p0);
}

SbPlaned::SbPlaned(const SbVec3d& normalref, const SbVec3d& point) {
  this->normal = normalref;
  this->normal.normalize();
  this->distance = this->normal.dot(point);
}

void SbPlaned::offset(const double d) {
  this->distance += d;
}

SbBool SbPlaned::intersect(const SbLined& l, SbVec3d& intersection) const {
  // Check if the line is parallel to the plane.
  if((l.getDirection()).dot(this->normal) == 0) return FALSE;

  double t = (this->distance - this->normal.dot(l.getPosition()))
             / this->normal.dot(l.getDirection());

  intersection = l.getPosition() + t * l.getDirection();

  return TRUE;
}


SbBool SbPlaned::isInHalfSpace(const SbVec3d& point) const {
  return this->getDistance(point) >= 0;
}

double SbPlaned::getDistance(const SbVec3d &point) const {
  return point.dot(this->normal) - this->distance;
}

const SbVec3d& SbPlaned::getNormal(void) const { return this->normal;}

double SbPlaned::getDistanceFromOrigin(void) const { return this->distance;}

SbBool SbPlaned::intersect(const SbPlaned & pl, SbLined & line) {
  double invdet;  // inverse of 2x2 matrix determinant
  SbVec3d dir2;  // holds the squares of the coordinates of xdir

  SbVec3d xpt;
  SbVec3d xdir;
  xdir = this->normal.cross(pl.normal);

  dir2[0] = xdir[0] * xdir[0];
  dir2[1] = xdir[1] * xdir[1];
  dir2[2] = xdir[2] * xdir[2];

  const SbVec3d & pl1n = this->normal;
  const SbVec3d & pl2n = pl.normal;
  const double pl1w = -this->distance;
  const double pl2w = -pl.distance;

  if ((dir2[2] > dir2[1]) && (dir2[2] > dir2[0]) && (dir2[2] > DBL_EPSILON)) {
    // then get a point on the XY plane
    invdet = 1.0 / xdir[2];
    xpt = SbVec3d(pl1n[1] * pl2w - pl2n[1] * pl1w,
                  pl2n[0] * pl1w - pl1n[0] * pl2w, 0);
  }
  else if ((dir2[1] > dir2[0]) && (dir2[1] > DBL_EPSILON)) {
    // then get a point on the XZ plane
    invdet = - 1.0 / xdir[1];
    xpt = SbVec3d(pl1n[2] * pl2w - pl2n[2] * pl1w, 0,
                  pl2n[0] * pl1w - pl1n[0] * pl2w);
  }
  else if (dir2[0] > DBL_EPSILON) {
    // then get a point on the YZ plane
    invdet = 1.0 / xdir[0];
    xpt = SbVec3d(0, pl1n[2] * pl2w - pl2n[2] * pl1w,
                  pl2n[1] * pl1w - pl1n[1] * pl2w);
  }
  else // xdir is zero, then no point of intersection exists
    return FALSE;

  xpt *= invdet;
  invdet = 1.0 / ::sqrt(dir2[0] + dir2[1] + dir2[2]);

  xdir *= invdet;
  line = SbLined(xpt, xpt + xdir);
  return TRUE;
}

/*
int operator ==(const SbPlaned& p1, const SbPlaned& p2)
{
  if(p1.getDistanceFromOrigin() == p2.getDistanceFromOrigin() &&
     p1.getNormal() == p2.getNormal()) return TRUE;
  return FALSE;
}

int operator !=(const SbPlaned& p1, const SbPlaned& p2)
{
  return !(p1 == p2);
}
*/

