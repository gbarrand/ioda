// this :
#include <HEPVis/SbTri3d.h>

#include <HEPVis/SbPlaned.h>
#include <HEPVis/SbLined.h>

#include <Inventor/errors/SoDebugError.h>

#include <assert.h>
#include <float.h> //DBL_MAX

#define SBTRI_DEBUG 0

// *************************************************************************

SbTri3d::SbTri3d(void)
{
}

SbTri3d::SbTri3d(const SbTri3d & t)
{
  a = t.a;
  b = t.b;
  c = t.c;
}

SbTri3d::SbTri3d(const SbVec3d & na, const SbVec3d & nb, const SbVec3d & nc)
    : a(na), b(nb), c(nc)
{
  // FIXME: fix IDAction so this assert doesn't hit. 20030328 mortene.
  assert(a != b && a != c && b != c);
}

SbTri3d::~SbTri3d(void)
{
}

SbTri3d &
SbTri3d::setValue(const SbTri3d & t)
{
  this->a = t.a;
  this->b = t.b;
  this->c = t.c;
  assert(this->a != this->b && this->a != this->c && this->b != this->c);
  return *this;
}

SbTri3d &
SbTri3d::setValue(const SbVec3d & a, const SbVec3d & b, const SbVec3d & c)
{
  assert(a != b && a != c && b != c);
  this->a = a;
  this->b = b;
  this->c = c;
  return *this;
}

void
SbTri3d::getValue(SbTri3d & t) const
{
  t.a = this->a;
  t.b = this->b;
  t.c = this->c;
}

void
SbTri3d::getValue(SbVec3d & a, SbVec3d & b, SbVec3d & c) const
{
  a = this->a;
  b = this->b;
  c = this->c;
}

SbTri3d &
SbTri3d::operator = (const SbTri3d & t)
{
  this->a = t.a;
  this->b = t.b;
  this->c = t.c;
  return *this;
}

// *************************************************************************

SbBool
SbTri3d::intersect(const SbTri3d & t) const
{
  // FIXME: remove all "programming logic" error messages and asserts from
  // this function when it's verified that those paths can't be taken.

  SbVec3d a1(this->a);
  SbVec3d b1(this->b);
  SbVec3d c1(this->c);
  SbPlaned plane1(a1, b1, c1);

  SbVec3d a2(t.a);
  SbVec3d b2(t.b);
  SbVec3d c2(t.c);
  SbPlaned plane2(a2, b2, c2);

  // FIXME: can ((n1 == -n2) && (d1 == -d2)) really happen?
  if (SBTRI_DEBUG &&
       (plane1.getNormal() == -plane2.getNormal()) &&
       (plane1.getDistanceFromOrigin() == -plane2.getDistanceFromOrigin())) {
    SoDebugError::post("SbTri3d::intersect", "The (n1 == -n2 && d1 == -d2) case happened");
  }
  if (plane1.getNormal() == plane2.getNormal()) {
    // fprintf(stderr, "normals are equal\n");
    if (plane1.getDistanceFromOrigin() != plane2.getDistanceFromOrigin())
      return FALSE; // parallel planes
    // we work around coplanar intersection testing by making it a case of
    // biplanar intersection testing.

    int vertex = 1;
    double distance = a1.sqrLength();
    double d;
    if ((d = b1.sqrLength()) > distance) {
      distance = d;
      vertex = 2;
    } else if ((d = c1.sqrLength()) > distance) {
      distance = d;
      vertex = 3;
    } else if ((d = a2.sqrLength()) > distance) {
      distance = d;
      vertex = 4;
    } else if ((d = b2.sqrLength()) > distance) {
      distance = d;
      vertex = 5;
    } else if ((d = c2.sqrLength()) > distance) {
      distance = d;
      vertex = 6;
    }
    switch (vertex) {
    case 1:
      break;
    case 2:
      do { SbVec3d temp(a1); a1 = b1; b1 = c1; c1 = temp; } while (FALSE);
      break;
    case 3:
      do { SbVec3d temp(a1); a1 = c1; c1 = b1; b1 = temp; } while (FALSE);
      break;
    case 4:
      do { SbVec3d temp(a1); a1 = a2; a2 = temp; } while (FALSE);
      do { SbVec3d temp(b1); b1 = b2; b2 = temp; } while (FALSE);
      do { SbVec3d temp(c1); c1 = c2; c2 = temp; } while (FALSE);
      break;
    case 5:
      do { SbVec3d temp(a1); a1 = b2; b2 = temp; } while (FALSE);
      do { SbVec3d temp(b1); b1 = c2; c2 = temp; } while (FALSE);
      do { SbVec3d temp(c1); c1 = a2; a2 = temp; } while (FALSE);
      break;
    case 6:
      do { SbVec3d temp(a1); a1 = c2; c2 = temp; } while (FALSE);
      do { SbVec3d temp(b1); b1 = a2; a2 = temp; } while (FALSE);
      do { SbVec3d temp(c1); c1 = b2; b2 = temp; } while (FALSE);
      break;
    }
    vertex = 1;
    distance = (a2-a1).sqrLength();
    if ((d = (b2-a1).sqrLength()) > distance) {
      distance = d;
      vertex = 2;
    } else if ((d = (c2-a1).sqrLength()) > distance) {
      distance = d;
      vertex = 3;
    }
    switch (vertex) {
    case 1:
      break;
    case 2:
      do { SbVec3d temp(a2); a2 = b2; b2 = b2; c2 = temp; } while (FALSE);
      break;
    case 3:
      do { SbVec3d temp(a2); a2 = c2; c2 = b2; b2 = temp; } while (FALSE);
      break;
    }

    // FIXME: I'm not confident we've actually found the two vertices that should
    // be lifted up at this point.  I can think of cases that will give false
    // negatives.  20021024 larsa
    a1 = a1 + plane1.getNormal();
    a2 = a2 + plane1.getNormal();
    // regenerate planes
    plane1 = SbPlaned(a1, b1, c1);
    plane2 = SbPlaned(a2, b2, c2);
  // } else {
    // fprintf(stderr, "normals are different\n");
  }

  // set up point a on one side, and b and c on the other

  const SbBool a1hs = plane2.isInHalfSpace(a1);
  const SbBool b1hs = plane2.isInHalfSpace(b1);
  const SbBool c1hs = plane2.isInHalfSpace(c1);
  if ((a1hs == b1hs) && (a1hs == c1hs)) {
    // no intersection
    return FALSE;
  } else if (a1hs == c1hs) { // b is in other halfspace
    SbVec3d temp(a1); a1 = b1; b1 = c1; c1 = temp;
  } else if (a1hs == b1hs) { // c is in other halfspace
    SbVec3d temp(a1); a1 = c1; c1 = b1; b1 = temp;
  //} else if (b1hs == c1hs) { // a is in other halfspace
  //  SbVec3d temp(b1); b1 = a1; a1 = c1; c1 = temp;
  }

  const SbBool a2hs = plane1.isInHalfSpace(a2);
  const SbBool b2hs = plane1.isInHalfSpace(b2);
  const SbBool c2hs = plane1.isInHalfSpace(c2);
  if ((a2hs == b2hs) && (a2hs == c2hs)) {
    // no intersection
    return FALSE;
  } else if (a2hs == c2hs) { // b is in other halfspace
    SbVec3d temp(a2); a2 = b2; b2 = c2; c2 = temp;
  } else if (a2hs == b2hs) { // c is in other halfspace
    SbVec3d temp(a2); a2 = c2; c2 = b2; b2 = temp;
  //} else if (b2hs == c2hs) { // a is in other halfspace
  //  SbVec3d temp(b2); b2 = a2; a2 = c2; c2 = temp;
  }

  // find intersection points on line for triangles
  SbVec3d p11, p12;
  if (!plane2.intersect(SbLined(a1, b1), p11)) {
    // should really never happen
    if (SBTRI_DEBUG) {
      SoDebugError::post("SbTri3d::intersect", "programming logic error 1");
      SoDebugError::post("-", "SbVec3d a1(%g, %g, %g);", a1[0], a1[1], a1[2]);
      SoDebugError::post("-", "SbVec3d b1(%g, %g, %g);", b1[0], b1[1], b1[2]);
      SoDebugError::post("-", "SbVec3d c1(%g, %g, %g);", c1[0], c1[1], c1[2]);
      SoDebugError::post("-", "SbVec3d a2(%g, %g, %g);", a2[0], a2[1], a2[2]);
      SoDebugError::post("-", "SbVec3d b2(%g, %g, %g);", b2[0], b2[1], b2[2]);
      SoDebugError::post("-", "SbVec3d c2(%g, %g, %g);", c2[0], c2[1], c2[2]);
      assert(0);
    }
    return FALSE;
  }
  if (!plane2.intersect(SbLined(a1, c1), p12)) {
    // should never happen
    if (SBTRI_DEBUG) {
      SoDebugError::post("SbTri3d::intersect", "programming logic error 2");
      SoDebugError::post("-", "SbVec3d a1(%g, %g, %g);", a1[0], a1[1], a1[2]);
      SoDebugError::post("-", "SbVec3d b1(%g, %g, %g);", b1[0], b1[1], b1[2]);
      SoDebugError::post("-", "SbVec3d c1(%g, %g, %g);", c1[0], c1[1], c1[2]);
      SoDebugError::post("-", "SbVec3d a2(%g, %g, %g);", a2[0], a2[1], a2[2]);
      SoDebugError::post("-", "SbVec3d b2(%g, %g, %g);", b2[0], b2[1], b2[2]);
      SoDebugError::post("-", "SbVec3d c2(%g, %g, %g);", c2[0], c2[1], c2[2]);
      assert(0);
    }
    return FALSE;
  }

  SbVec3d p21, p22;
  if (!plane1.intersect(SbLined(a2, b2), p21)) {
    // should never happen
    // but since it does, it means something
    // possibly that a2 and b2 are in plane1, and halfspace values were wrong in
    // some way.  we should either return FALSE or set p21 to something
    if (SBTRI_DEBUG) {
      SoDebugError::post("SbTri3d::intersect", "programming logic error 3");
      SoDebugError::post("-", "SbVec3d a1(%g, %g, %g);", a1[0], a1[1], a1[2]);
      SoDebugError::post("-", "SbVec3d b1(%g, %g, %g);", b1[0], b1[1], b1[2]);
      SoDebugError::post("-", "SbVec3d c1(%g, %g, %g);", c1[0], c1[1], c1[2]);
      SoDebugError::post("-", "SbVec3d a2(%g, %g, %g);", a2[0], a2[1], a2[2]);
      SoDebugError::post("-", "SbVec3d b2(%g, %g, %g);", b2[0], b2[1], b2[2]);
      SoDebugError::post("-", "SbVec3d c2(%g, %g, %g);", c2[0], c2[1], c2[2]);
      assert(0);
    }
    return FALSE;
  }
  if (!plane1.intersect(SbLined(a2, c2), p22)) {
    // should never happen
    if (SBTRI_DEBUG) {
      SoDebugError::post("SbTri3d::intersect", "programming logic error 4\n");
      SoDebugError::post("-", "SbVec3d a1(%g, %g, %g);", a1[0], a1[1], a1[2]);
      SoDebugError::post("-", "SbVec3d b1(%g, %g, %g);", b1[0], b1[1], b1[2]);
      SoDebugError::post("-", "SbVec3d c1(%g, %g, %g);", c1[0], c1[1], c1[2]);
      SoDebugError::post("-", "SbVec3d a2(%g, %g, %g);", a2[0], a2[1], a2[2]);
      SoDebugError::post("-", "SbVec3d b2(%g, %g, %g);", b2[0], b2[1], b2[2]);
      SoDebugError::post("-", "SbVec3d c2(%g, %g, %g);", c2[0], c2[1], c2[2]);
      assert(0);
    }
    return FALSE;
  }

  // find end point of the four (the one furtest from origo would be 
  // an end point)
  // and the length of that line segment
  double distance, maxdistance;
  int vertex = 1;
  maxdistance = p11.sqrLength();
  distance = p12.sqrLength();
  if (distance > maxdistance) {
    vertex = 2;
    maxdistance = distance;
  }
  distance = p21.sqrLength();
  if (distance > maxdistance) {
    vertex = 3;
    maxdistance = distance;
  }
  distance = p22.sqrLength();
  if (distance > maxdistance) {
    vertex = 4;
    maxdistance = distance;
  }

  // check if a vertec from the other line segment is within 
  // the perimeter of the line
  SbVec3d p, e, p1, p2;
  switch (vertex) {
  case 1:
    p = p11; e = p12; p1 = p21; p2 = p22;
    break;
  case 2:
    p = p12; e = p11; p1 = p21; p2 = p22;
    break;
  case 3:
    p = p21; e = p22; p1 = p11; p2 = p12;
    break;
  case 4:
    p = p22; e = p21; p1 = p11; p2 = p12;
    break;
  default:
    if (SBTRI_DEBUG)
      SoDebugError::post("SbTri3d::intersect", "programming logic error 5\n");
    assert(0);
  }
  double pedistance = (e - p).sqrLength();
  if (pedistance > (p1-p).sqrLength()) {
    return TRUE;
  }
  if (pedistance > (p2-p).sqrLength()) {
    return TRUE;
  }
  return FALSE;
}

SbBool
SbTri3d::intersect(const SbTri3d & t, double e) const
{
  if (e == 0.0) return this->intersect(t);
  if (this->getDistance(t) <= e) return TRUE;
  return FALSE;
}

SbVec3d
SbTri3d::getNormal() const
{
  SbVec3d p[3];
  this->getValue(p[0], p[1], p[2]);
  SbPlaned pl(p[0], p[1], p[2]);
  return pl.getNormal();
}

/*!
  Returns the distance from the given point to this triangle.
*/
double 
SbTri3d::getDistance(const SbVec3d & p) const
{
  double dist = DBL_MAX;
  SbVec3d thisp[3];
  this->getValue(thisp[0], thisp[1], thisp[2]);
  SbPlaned pl(thisp[0], thisp[1], thisp[2]);

  SbVec3d intersect;
  SbVec3d n = this->getNormal();
  SbLined line(p, p+n);
  if (pl.intersect(line, intersect)) {
    int i;
    for (i=0;i<3;i++) {
      SbPlaned edgepl(thisp[i], thisp[i]+n, thisp[(i+1)%3]);
      if (!edgepl.isInHalfSpace(intersect)) break;
    }
    if (i == 3) dist = (double) fabs(pl.getDistance(p));
    else { // We didn't project inside triangle
      for (int j=0;j<3;j++) {
        double d = SbTri3d::getDistance(p, thisp[j], thisp[(j+1)%3]);
        if (d < dist) dist = d;
      }
    }
  }
  else {
    assert(FALSE);
  }
  return dist;
}

/*!
  Returns the distance from p to the line segment p1-p2.
*/
double 
SbTri3d::getDistance(const SbVec3d & p, 
                   const SbVec3d & p1, const SbVec3d & p2)
{
  SbVec3d normal = p2 - p1;
  SbPlaned pl1(normal, p1);
  SbPlaned pl2(-normal, p2);

  if (pl1.isInHalfSpace(p) && pl2.isInHalfSpace(p)) {
    SbLined line(p1, p2);
    return (line.getClosestPoint(p)-p).length();
  }
  else {
    double d1 = (p - p1).length();
    double d2 = (p - p2).length();
    return (d1<d2)?d1:d2;
  }
}

static const double gs_dTolerance = 1e-06f;

/*!
  Returns the distance from this triangle to the given line segment.
*/
double 
SbTri3d::getDistance(const SbVec3d & p1, const SbVec3d & p2) const
{
  SbVec3d kDiff = this->a - p1;
  SbVec3d edge0 = this->b - this->a;
  SbVec3d edge1 = this->c - this->a;
  double fA00 = (p2-p1).sqrLength();
  double fA01 = -(p2-p1).dot(edge0);
  double fA02 = -(p2-p1).dot(edge1);
  double fA11 = edge0.sqrLength();
  double fA12 = edge0.dot(edge1);
  double fA22 = edge1.dot(edge1);
  double fB0  = -kDiff.dot(p2-p1);
  double fB1  = kDiff.dot(edge0);
  double fB2  = kDiff.dot(edge1);

  double fSqrDist, fSqrDist0, fR, fS, fT, fR0, fS0, fT0;

  // Set up for a relative error test on the angle between ray direction
  // and triangle normal to determine parallel/nonparallel status.
  SbVec3d kN = edge0.cross(edge1);
  double fNSqrLen = kN.sqrLength();
  double fDot = (p2-p1).dot(kN);
  SbBool bNotParallel = (fDot*fDot >= gs_dTolerance*fA00*fNSqrLen);

  if (bNotParallel) {
    double fCof00 = fA11*fA22-fA12*fA12;
    double fCof01 = fA02*fA12-fA01*fA22;
    double fCof02 = fA01*fA12-fA02*fA11;
    double fCof11 = fA00*fA22-fA02*fA02;
    double fCof12 = fA02*fA01-fA00*fA12;
    double fCof22 = fA00*fA11-fA01*fA01;
    double fInvDet = 1.0/(fA00*fCof00+fA01*fCof01+fA02*fCof02);
    double fRhs0 = -fB0*fInvDet;
    double fRhs1 = -fB1*fInvDet;
    double fRhs2 = -fB2*fInvDet;
    
    fR = fCof00*fRhs0+fCof01*fRhs1+fCof02*fRhs2;
    fS = fCof01*fRhs0+fCof11*fRhs1+fCof12*fRhs2;
    fT = fCof02*fRhs0+fCof12*fRhs1+fCof22*fRhs2;

    if (fR < 0.0) {
      if (fS+fT <= 1.0) {
        if (fS < 0.0) {
          if (fT < 0.0) {  // region 4m
            // min on face s=0 or t=0 or r=0
            fSqrDist = SbTri3d::sqrDistance(p1, p2, 
                                          this->a, this->c,
                                          &fR,&fT);
            fS = 0.0;
            fSqrDist0 = SbTri3d::sqrDistance(p1, p2, 
                                           this->a, this->b,
                                           &fR0,&fS0);
            fT0 = 0.0;
            if (fSqrDist0 < fSqrDist) {
              fSqrDist = fSqrDist0;
              fR = fR0;
              fS = fS0;
              fT = fT0;
            }
            fSqrDist0 = this->sqrDistance(p1,&fS0,&fT0);
            fR0 = 0.0;
            if (fSqrDist0 < fSqrDist) {
              fSqrDist = fSqrDist0;
              fR = fR0;
              fS = fS0;
              fT = fT0;
            }
          }
          else {  // region 3m
            // min on face s=0 or r=0
            fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->c,&fR,&fT);
            fS = 0.0;
            fSqrDist0 = this->sqrDistance(p1,&fS0,&fT0);
            fR0 = 0.0;
            if (fSqrDist0 < fSqrDist) {
              fSqrDist = fSqrDist0;
              fR = fR0;
              fS = fS0;
              fT = fT0;
            }
          }
        }
        else if (fT < 0.0) {  // region 5m
          // min on face t=0 or r=0
          fSqrDist = SbTri3d::sqrDistance(p1, p2, 
                                        this->a, this->b,
                                        &fR,&fS);
          fT = 0.0;
          fSqrDist0 = this->sqrDistance(p1,&fS0,&fT0);
          fR0 = 0.0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else {  // region 0m
          // min on face r=0
          fSqrDist = this->sqrDistance(p1,&fS,&fT);
          fR = 0.0;
        }
      }
      else {
        if (fS < 0.0) {  // region 2m
          // min on face s=0 or s+t=1 or r=0
          fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->c,&fR,&fT);
          fS = 0.0;
          fSqrDist0 = SbTri3d::sqrDistance(p1, p2, this->b, this->c,&fR0,&fT0);
          fS0 = 1.0-fT0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = this->sqrDistance(p1,&fS0,&fT0);
          fR0 = 0.0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if (fT < 0.0) {  // region 6m
          // min on face t=0 or s+t=1 or r=0
          fSqrDist = SbTri3d::sqrDistance(p1, p2, 
                                        this->a, this->b,
                                        &fR,&fS);
          fT = 0.0;
          fSqrDist0 = SbTri3d::sqrDistance(p1, p2, this->b, this->c,&fR0,&fT0);
          fS0 = 1.0-fT0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = this->sqrDistance(p1,&fS0,&fT0);
          fR0 = 0.0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else {  // region 1m
          // min on face s+t=1 or r=0
          fSqrDist = SbTri3d::sqrDistance(p1, p2, this->b, this->c,&fR,&fT);
          fS = 1.0-fT;
          fSqrDist0 = this->sqrDistance(p1,&fS0,&fT0);
          fR0 = 0.0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
    }
    else if (fR <= 1.0) {
      if (fS+fT <= 1.0) {
        if (fS < 0.0) {
          if (fT < 0.0) {  // region 4
            // min on face s=0 or t=0
            fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->c,&fR,&fT);
            fS = 0.0;
            fSqrDist0 = SbTri3d::sqrDistance(p1, p2, 
                                           this->a, this->b,
                                           &fR0,&fS0);
            fT0 = 0.0;
            if (fSqrDist0 < fSqrDist) {
              fSqrDist = fSqrDist0;
              fR = fR0;
              fS = fS0;
              fT = fT0;
            }
          }
          else {  // region 3
            // min on face s=0
            fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->c,&fR,&fT);
            fS = 0.0;
          }
        }
        else if (fT < 0.0) {  // region 5
          // min on face t=0
          fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->b,&fR,&fS);
          fT = 0.0;
        }
        else {  // region 0
          // global minimum is interior, done
          fSqrDist = 0.0;
        }
      }
      else {
        if (fS < 0.0) {  // region 2
          // min on face s=0 or s+t=1
          fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->c,&fR,&fT);
          fS = 0.0;
          fSqrDist0 = SbTri3d::sqrDistance(p1, p2, this->b, this->c,&fR0,&fT0);
          fS0 = 1.0-fT0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if (fT < 0.0) {  // region 6
          // min on face t=0 or s+t=1
          fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->b,&fR,&fS);
          fT = 0.0;
          fSqrDist0 = SbTri3d::sqrDistance(p1, p2, this->b, this->c,&fR0,&fT0);
          fS0 = 1.0-fT0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else {  // region 1
          // min on face s+t=1
          fSqrDist = SbTri3d::sqrDistance(p1, p2, this->b, this->c,&fR,&fT);
          fS = 1.0-fT;
        }
      }
    }
    else {  // fR > 1
      if (fS+fT <= 1.0) {
        if (fS < 0.0) {
          if (fT < 0.0) {  // region 4p
            // min on face s=0 or t=0 or r=1
            fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->c,&fR,&fT);
            fS = 0.0;
            fSqrDist0 = SbTri3d::sqrDistance(p1, p2, this->a, this->b,&fR0,&fS0);
            fT0 = 0.0;
            if (fSqrDist0 < fSqrDist) {
              fSqrDist = fSqrDist0;
              fR = fR0;
              fS = fS0;
              fT = fT0;
            }
            fSqrDist0 = this->sqrDistance(p2,&fS0,&fT0);
            fR0 = 1.0;
            if (fSqrDist0 < fSqrDist) {
              fSqrDist = fSqrDist0;
              fR = fR0;
              fS = fS0;
              fT = fT0;
            }
          }
          else {  // region 3p
            // min on face s=0 or r=1
            fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->c,&fR,&fT);
            fS = 0.0;
            fSqrDist0 = this->sqrDistance(p2,&fS0,&fT0);
            fR0 = 1.0;
            if (fSqrDist0 < fSqrDist) {
              fSqrDist = fSqrDist0;
              fR = fR0;
              fS = fS0;
              fT = fT0;
            }
          }
        }
        else if (fT < 0.0) {  // region 5p
          // min on face t=0 or r=1
          fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->b,&fR,&fS);
          fT = 0.0;
          fSqrDist0 = this->sqrDistance(p2,&fS0,&fT0);
          fR0 = 1.0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else {  // region 0p
          // min face on r=1
          fSqrDist = this->sqrDistance(p2,&fS,&fT);
          fR = 1.0;
        }
      }
      else {
        if (fS < 0.0) {  // region 2p
          // min on face s=0 or s+t=1 or r=1
          fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->c,&fR,&fT);
          fS = 0.0;
          fSqrDist0 = SbTri3d::sqrDistance(p1, p2, this->b, this->c,&fR0,&fT0);
          fS0 = 1.0-fT0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = this->sqrDistance(p2,&fS0,&fT0);
          fR0 = 1.0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if (fT < 0.0) {  // region 6p
          // min on face t=0 or s+t=1 or r=1
          fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->b,&fR,&fS);
          fT = 0.0;
          fSqrDist0 = SbTri3d::sqrDistance(p1, p2, this->b, this->c,&fR0,&fT0);
          fS0 = 1.0-fT0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = this->sqrDistance(p2,&fS0,&fT0);
          fR0 = 1.0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else {  // region 1p
          // min on face s+t=1 or r=1
          fSqrDist = SbTri3d::sqrDistance(p1, p2, this->b, this->c,&fR,&fT);
          fS = 1.0-fT;
          fSqrDist0 = this->sqrDistance(p2,&fS0,&fT0);
          fR0 = 1.0;
          if (fSqrDist0 < fSqrDist) {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
    }
  }
  else {
    // segment and triangle are parallel
    fSqrDist = SbTri3d::sqrDistance(p1, p2, this->a, this->b,&fR,&fS);
    fT = 0.0;

    fSqrDist0 = SbTri3d::sqrDistance(p1, p2, this->a, this->c,&fR0,&fT0);
    fS0 = 0.0;
    if (fSqrDist0 < fSqrDist) {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }

    fSqrDist0 = SbTri3d::sqrDistance(p1, p2, this->b, this->c,&fR0,&fT0);
    fS0 = 1.0-fT0;
    if (fSqrDist0 < fSqrDist) {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }

    fSqrDist0 = this->sqrDistance(p1,&fS0,&fT0);
    fR0 = 0.0;
    if (fSqrDist0 < fSqrDist)
      {
        fSqrDist = fSqrDist0;
        fR = fR0;
        fS = fS0;
        fT = fT0;
      }

    fSqrDist0 = this->sqrDistance(p2,&fS0,&fT0);
    fR0 = 1.0;
    if (fSqrDist0 < fSqrDist)
      {
        fSqrDist = fSqrDist0;
        fR = fR0;
        fS = fS0;
        fT = fT0;
      }
  }

//    if (pfSegP) *pfSegP = fR;
//    if (pfTriP0) *pfTriP0 = fS;
//    if (pfTriP1) *pfTriP1 = fT;

  return (double)sqrt(fSqrDist);
}

/*!
  Returns the distance between the two line segments.
*/
double 
SbTri3d::sqrDistance(const SbVec3d & a1, const SbVec3d & a2,
                   const SbVec3d & b1, const SbVec3d & b2,
                   double * pfSegP0, double * pfSegP1)
{
  SbVec3d kDiff = a1 - b1;
  SbVec3d dir0 = a2 - a1;
  SbVec3d dir1 = b2 - b1;
  double fA00 = dir0.sqrLength();
  double fA01 = -dir0.dot(dir1);
  double fA11 = dir1.sqrLength();
  double fB0 = kDiff.dot(dir0);
  double fC = kDiff.sqrLength();
  double fDet = (double)fabs(fA00*fA11-fA01*fA01);
  double fB1, fS, fT, fSqrDist, fTmp;

  if (fDet >= gs_dTolerance) {
    // line segments are not parallel
    fB1 = -kDiff.dot(dir1);
    fS = fA01*fB1-fA11*fB0;
    fT = fA01*fB0-fA00*fB1;
        
    if (fS >= 0.0) {
      if (fS <= fDet) {
        if (fT >= 0.0) {
          if (fT <= fDet) {  // region 0 (interior)
            // minimum at two interior points of 3D lines
            double fInvDet = 1.0/fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDist = fS*(fA00*fS+fA01*fT+2.0*fB0) +
              fT*(fA01*fS+fA11*fT+2.0*fB1)+fC;
          }
          else {  // region 3 (side)
            fT = 1.0;
            fTmp = fA01+fB0;
            if (fTmp >= 0.0) {
              fS = 0.0;
              fSqrDist = fA11+2.0*fB1+fC;
            }
            else if (-fTmp >= fA00) {
              fS = 1.0;
              fSqrDist = fA00+fA11+fC+2.0*(fB1+fTmp);
            }
            else {
              fS = -fTmp/fA00;
              fSqrDist = fTmp*fS+fA11+2.0*fB1+fC;
            }
          }
        }
        else {  // region 7 (side)
          fT = 0.0;
          if (fB0 >= 0.0) {
            fS = 0.0;
            fSqrDist = fC;
          }
          else if (-fB0 >= fA00) {
            fS = 1.0;
            fSqrDist = fA00+2.0*fB0+fC;
          }
          else {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
      }
      else {
        if (fT >= 0.0) {
          if (fT <= fDet) {  // region 1 (side)
            fS = 1.0;
            fTmp = fA01+fB1;
            if (fTmp >= 0.0) {
              fT = 0.0;
              fSqrDist = fA00+2.0*fB0+fC;
            }
            else if (-fTmp >= fA11) {
              fT = 1.0;
              fSqrDist = fA00+fA11+fC+2.0*(fB0+fTmp);
            }
            else {
              fT = -fTmp/fA11;
              fSqrDist = fTmp*fT+fA00+2.0*fB0+fC;
            }
          }
          else {  // region 2 (corner)
            fTmp = fA01+fB0;
            if (-fTmp <= fA00) {
              fT = 1.0;
              if (fTmp >= 0.0) {
                fS = 0.0;
                fSqrDist = fA11+2.0*fB1+fC;
              }
              else {
                fS = -fTmp/fA00;
                fSqrDist = fTmp*fS+fA11+2.0*fB1+fC;
              }
            }
            else {
              fS = 1.0;
              fTmp = fA01+fB1;
              if (fTmp >= 0.0) {
                fT = 0.0;
                fSqrDist = fA00+2.0*fB0+fC;
              }
              else if (-fTmp >= fA11) {
                fT = 1.0;
                fSqrDist = fA00+fA11+fC+2.0*(fB0+fTmp);
              }
              else {
                fT = -fTmp/fA11;
                fSqrDist = fTmp*fT+fA00+2.0*fB0+fC;
              }
            }
          }
        }
        else {  // region 8 (corner)
          if (-fB0 < fA00) {
            fT = 0.0;
            if (fB0 >= 0.0) {
              fS = 0.0;
              fSqrDist = fC;
            }
            else {
              fS = -fB0/fA00;
              fSqrDist = fB0*fS+fC;
            }
          }
          else {
            fS = 1.0;
            fTmp = fA01+fB1;
            if (fTmp >= 0.0) {
              fT = 0.0;
              fSqrDist = fA00+2.0*fB0+fC;
            }
            else if (-fTmp >= fA11) {
              fT = 1.0;
              fSqrDist = fA00+fA11+fC+2.0*(fB0+fTmp);
            }
            else {
              fT = -fTmp/fA11;
              fSqrDist = fTmp*fT+fA00+2.0*fB0+fC;
            }
          }
        }
      }
    }
    else {
      if (fT >= 0.0) {
        if (fT <= fDet) {  // region 5 (side)
          fS = 0.0;
          if (fB1 >= 0.0) {
            fT = 0.0;
            fSqrDist = fC;
          }
          else if (-fB1 >= fA11) {
            fT = 1.0;
            fSqrDist = fA11+2.0*fB1+fC;
          }
          else {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
        else {  // region 4 (corner)
          fTmp = fA01+fB0;
          if (fTmp < 0.0) {
            fT = 1.0;
            if (-fTmp >= fA00) {
              fS = 1.0;
              fSqrDist = fA00+fA11+fC+2.0*(fB1+fTmp);
            }
            else {
              fS = -fTmp/fA00;
              fSqrDist = fTmp*fS+fA11+2.0*fB1+fC;
            }
          }
          else {
            fS = 0.0;
            if (fB1 >= 0.0) {
              fT = 0.0;
              fSqrDist = fC;
            }
            else if (-fB1 >= fA11) {
              fT = 1.0;
              fSqrDist = fA11+2.0*fB1+fC;
            }
            else {
              fT = -fB1/fA11;
              fSqrDist = fB1*fT+fC;
            }
          }
        }
      }
      else {   // region 6 (corner)
        if (fB0 < 0.0) {
          fT = 0.0;
          if (-fB0 >= fA00) {
            fS = 1.0;
            fSqrDist = fA00+2.0*fB0+fC;
          }
          else {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
        else {
          fS = 0.0;
          if (fB1 >= 0.0) {
            fT = 0.0;
            fSqrDist = fC;
          }
          else if (-fB1 >= fA11) {
            fT = 1.0;
            fSqrDist = fA11+2.0*fB1+fC;
          }
          else {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
      }
    }
  }
  else {
    // line segments are parallel
    if (fA01 > 0.0) {
      // direction vectors form an obtuse angle
      if (fB0 >= 0.0) {
        fS = 0.0;
        fT = 0.0;
        fSqrDist = fC;
      }
      else if (-fB0 <= fA00) {
        fS = -fB0/fA00;
        fT = 0.0;
        fSqrDist = fB0*fS+fC;
      }
      else {
        fB1 = -kDiff.dot(dir1);
        fS = 1.0;
        fTmp = fA00+fB0;
        if (-fTmp >= fA01) {
          fT = 1.0;
          fSqrDist = fA00+fA11+fC+2.0*(fA01+fB0+fB1);
        }
        else {
          fT = -fTmp/fA01;
          fSqrDist = fA00+2.0*fB0+fC+fT*(fA11*fT+2.0*(fA01+fB1));
        }
      }
    }
    else {
      // direction vectors form an acute angle
      if (-fB0 >= fA00) {
        fS = 1.0;
        fT = 0.0;
        fSqrDist = fA00+2.0*fB0+fC;
      }
      else if (fB0 <= 0.0) {
        fS = -fB0/fA00;
        fT = 0.0;
        fSqrDist = fB0*fS+fC;
      }
      else {
        fB1 = -kDiff.dot(dir1);
        fS = 0.0;
        if (fB0 >= -fA01) {
          fT = 1.0;
          fSqrDist = fA11+2.0*fB1+fC;
        }
        else {
          fT = -fB0/fA01;
          fSqrDist = fC+fT*(2.0*fB1+fA11*fT);
        }
      }
    }
  }

  if (pfSegP0) *pfSegP0 = fS;

  if (pfSegP1) *pfSegP1 = fT;

  return (double)fabs(fSqrDist);

  //FIXME: Old code. not for segments but lines.
#if 0
  SbVec3d kDiff = a1 - b1;
  SbVec3d dir0 = a2 - a1;
  SbVec3d dir1 = b2 - b1;
  double fA00 = dir0.sqrLength();
  double fA01 = -dir0.dot(dir1);
  double fA11 = dir1.sqrLength();
  double fB0 = kDiff.dot(dir0);
  double fC = kDiff.sqrLength();
  double fDet = fabs(fA00*fA11-fA01*fA01);
  double fB1, fS, fT, fSqrDist;
  
  if (fDet >= gs_dTolerance) {
    // lines are not parallel
    fB1 = -kDiff.dot(dir1);
    double fInvDet = 1.0/fDet;
    fS = (fA01*fB1-fA11*fB0)*fInvDet;
    fT = (fA01*fB0-fA00*fB1)*fInvDet;
    fSqrDist = fS*(fA00*fS+fA01*fT+2.0*fB0) +
      fT*(fA01*fS+fA11*fT+2.0*fB1)+fC;
  }
  else {
    // lines are parallel, select any closest pair of points
    fS = -fB0/fA00;
    fT = 0.0;
    fSqrDist = fB0*fS+fC;
  }
  
  if (linP0) *linP0 = fS;
  
  if (linP1) *linP1 = fT;

  return fabs(fSqrDist);
#endif
}

/*!
  Returns the minimum distance from this triangle to the given triangle.
*/
double 
SbTri3d::getDistance(const SbTri3d & t) const
{
  double dist = DBL_MAX;
  SbVec3d p[3];
  t.getValue(p[0], p[1], p[2]);
  int i;
  for (i=0;i<3;i++) {
    double d = this->getDistance(p[i], p[(i+1)%3]);
    if (d < dist) dist = d;
  }
  this->getValue(p[0], p[1], p[2]);
  for (i=0;i<3;i++) {
    double d = t.getDistance(p[i], p[(i+1)%3]);
    if (d < dist) dist = d;
  }

#if 0 // debug output
  if (dist <= 1) {
    this->getValue(p[0], p[1], p[2]);
    for (int i=0;i<3;i++) {
      printf("%f %f %f (%x %x %x)\n", 
             p[i][0], p[i][1], p[i][2], 
             *(void **)(&p[i][0]), *(void **)(&p[i][1]), *(void **)(&p[i][2]));
    }
    t.getValue(p[0], p[1], p[2]);
    for (int i=0;i<3;i++) {
      printf("%f %f %f (%x %x %x)\n", 
             p[i][0], p[i][1], p[i][2], 
             *(void **)(&p[i][0]), *(void **)(&p[i][1]), *(void **)(&p[i][2]));
    }
    printf("Dist: %f\n", dist);
  }
#endif

  return dist;

  //FIXME: Old code. Kept for reference (kintel 20021121)
#if 0
  double dist = DBL_MAX;
  SbVec3d p[3];
  t.getValue(p[0], p[1], p[2]);
  int i;
  for (i=0;i<3;i++) {
    double d = this->getDistance(p[i]);
    if (d < dist) dist = d;
  }
  this->getValue(p[0], p[1], p[2]);
  for (i=0;i<3;i++) {
    double d = t.getDistance(p[i]);
    if (d < dist) dist = d;
  }

  return dist;
#endif
}

double
SbTri3d::sqrDistance (const SbVec3d & p1, 
                    double * pfSParam, double * pfTParam) const
{
  SbVec3d kDiff = this->a - p1;
  SbVec3d edge0 = this->b - this->a;
  SbVec3d edge1 = this->c - this->a;
  double fA00 = edge0.sqrLength();
  double fA01 = edge0.dot(edge1);
  double fA11 = edge1.sqrLength();
  double fB0 = kDiff.dot(edge0);
  double fB1 = kDiff.dot(edge1);
  double fC = kDiff.sqrLength();
  double fDet = (double)fabs(fA00*fA11-fA01*fA01);
  double fS = fA01*fB1-fA11*fB0;
  double fT = fA01*fB0-fA00*fB1;
  double fSqrDist;

  if (fS + fT <= fDet)
    {
      if (fS < 0.0)
        {
          if (fT < 0.0)  // region 4
            {
              if (fB0 < 0.0)
                {
                  fT = 0.0;
                  if (-fB0 >= fA00)
                    {
                      fS = 1.0;
                      fSqrDist = fA00+2.0*fB0+fC;
                    }
                  else
                    {
                      fS = -fB0/fA00;
                      fSqrDist = fB0*fS+fC;
                    }
                }
              else
                {
                  fS = 0.0;
                  if (fB1 >= 0.0)
                    {
                      fT = 0.0;
                      fSqrDist = fC;
                    }
                  else if (-fB1 >= fA11)
                    {
                      fT = 1.0;
                      fSqrDist = fA11+2.0*fB1+fC;
                    }
                  else
                    {
                      fT = -fB1/fA11;
                      fSqrDist = fB1*fT+fC;
                    }
                }
            }
          else  // region 3
            {
              fS = 0.0;
              if (fB1 >= 0.0)
                {
                  fT = 0.0;
                  fSqrDist = fC;
                }
              else if (-fB1 >= fA11)
                {
                  fT = 1.0;
                  fSqrDist = fA11+2.0*fB1+fC;
                }
              else
                {
                  fT = -fB1/fA11;
                  fSqrDist = fB1*fT+fC;
                }
            }
        }
      else if (fT < 0.0)  // region 5
        {
          fT = 0.0;
          if (fB0 >= 0.0)
            {
              fS = 0.0;
              fSqrDist = fC;
            }
          else if (-fB0 >= fA00)
            {
              fS = 1.0;
              fSqrDist = fA00+2.0*fB0+fC;
            }
          else
            {
              fS = -fB0/fA00;
              fSqrDist = fB0*fS+fC;
            }
        }
      else  // region 0
        {
          // minimum at interior point
          double fInvDet = 1.0/fDet;
          fS *= fInvDet;
          fT *= fInvDet;
          fSqrDist = fS*(fA00*fS+fA01*fT+2.0*fB0) +
            fT*(fA01*fS+fA11*fT+2.0*fB1)+fC;
        }
    }
  else
    {
      double fTmp0, fTmp1, fNumer, fDenom;

      if (fS < 0.0)  // region 2
        {
          fTmp0 = fA01 + fB0;
          fTmp1 = fA11 + fB1;
          if (fTmp1 > fTmp0)
            {
              fNumer = fTmp1 - fTmp0;
              fDenom = fA00-2.0*fA01+fA11;
              if (fNumer >= fDenom)
                {
                  fS = 1.0;
                  fT = 0.0;
                  fSqrDist = fA00+2.0*fB0+fC;
                }
              else
                {
                  fS = fNumer/fDenom;
                  fT = 1.0 - fS;
                  fSqrDist = fS*(fA00*fS+fA01*fT+2.0*fB0) +
                    fT*(fA01*fS+fA11*fT+2.0*fB1)+fC;
                }
            }
          else
            {
              fS = 0.0;
              if (fTmp1 <= 0.0)
                {
                  fT = 1.0;
                  fSqrDist = fA11+2.0*fB1+fC;
                }
              else if (fB1 >= 0.0)
                {
                  fT = 0.0;
                  fSqrDist = fC;
                }
              else
                {
                  fT = -fB1/fA11;
                  fSqrDist = fB1*fT+fC;
                }
            }
        }
      else if (fT < 0.0)  // region 6
        {
          fTmp0 = fA01 + fB1;
          fTmp1 = fA00 + fB0;
          if (fTmp1 > fTmp0)
            {
              fNumer = fTmp1 - fTmp0;
              fDenom = fA00-2.0*fA01+fA11;
              if (fNumer >= fDenom)
                {
                  fT = 1.0;
                  fS = 0.0;
                  fSqrDist = fA11+2.0*fB1+fC;
                }
              else
                {
                  fT = fNumer/fDenom;
                  fS = 1.0 - fT;
                  fSqrDist = fS*(fA00*fS+fA01*fT+2.0*fB0) +
                    fT*(fA01*fS+fA11*fT+2.0*fB1)+fC;
                }
            }
          else
            {
              fT = 0.0;
              if (fTmp1 <= 0.0)
                {
                  fS = 1.0;
                  fSqrDist = fA00+2.0*fB0+fC;
                }
              else if (fB0 >= 0.0)
                {
                  fS = 0.0;
                  fSqrDist = fC;
                }
              else
                {
                  fS = -fB0/fA00;
                  fSqrDist = fB0*fS+fC;
                }
            }
        }
      else  // region 1
        {
          fNumer = fA11 + fB1 - fA01 - fB0;
          if (fNumer <= 0.0)
            {
              fS = 0.0;
              fT = 1.0;
              fSqrDist = fA11+2.0*fB1+fC;
            }
          else
            {
              fDenom = fA00-2.0*fA01+fA11;
              if (fNumer >= fDenom)
                {
                  fS = 1.0;
                  fT = 0.0;
                  fSqrDist = fA00+2.0*fB0+fC;
                }
              else
                {
                  fS = fNumer/fDenom;
                  fT = 1.0 - fS;
                  fSqrDist = fS*(fA00*fS+fA01*fT+2.0*fB0) +
                    fT*(fA01*fS+fA11*fT+2.0*fB1)+fC;
                }
            }
        }
    }

  if (pfSParam)
    *pfSParam = fS;

  if (pfTParam)
    *pfTParam = fT;

  return (double)fabs(fSqrDist);
}

// *************************************************************************

/*!
  Returns bounding box fully enclosing the triangle.
*/
const SbBox3d SbTri3d::getBoundingBox(void) const
{
  // FIXME: this involves quite a lot of function calls, and can
  // probably be optimized simply by expanding the code. 20030328 mortene.

  SbBox3d b;
  b.extendBy(this->a);
  b.extendBy(this->b);
  b.extendBy(this->c);
  return b;
}
// *************************************************************************

/*
//G.Barrand (used in SoIntersectionDetectionAction)
#include <Inventor/SbVec3f.h>
void SbTri3d::getValue(SbVec3f & a, SbVec3f & b, SbVec3f & c) const {
  a = this->a;
  b = this->b;
  c = this->c;
}
*/

