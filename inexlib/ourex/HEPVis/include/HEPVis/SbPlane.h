#ifndef HEPVis_SbPlane_h
#define HEPVis_SbPlane_h

/*
 * An SbPlane class that uses doubles instead of floats.
 * Used by the BooleanProcessor.
 */

#include <Inventor/SbVec3d.h>

namespace HEPVis {

class SbPlane {
public:
  //NOTE : CoinGL/SbPlane() does not initialize the fields !
  SbPlane()
  :m_normal(0,0,1) //CLHEP initialize with x-y plane.
  ,m_distance(0)
  {}

  SbPlane(const SbVec3d& a_normal,double a_D) {
    m_normal = a_normal;
    m_normal.normalize();
    m_distance = a_D;
    //NOTE : equation of the plan is then : n[0]*x+n[1]*y+n[2]*z-distance = 0
  }

  SbPlane(const SbVec3d& a_normal,const SbVec3d& a_point) {
    //FIXME if(normalref.sqrLength()==0) //throw
    m_normal = a_normal;
    m_normal.normalize();
    m_distance = m_normal.dot(a_point);
  }

  //SbPlane(double a,double b,double c,double d) {
  //  // To follow CLHEP.
  //  // Constructor from four numbers - creates plane a*x+b*y+c*z+d=0.
  //  normal.setValue(a,b,c);
  //  normal.normalize();
  //  distance = -d; //NOTE : yes, it is a minus.
  //}

  virtual ~SbPlane(){}
  
public:
  SbPlane(const SbPlane& aFrom)
  :m_normal(aFrom.m_normal)
  ,m_distance(aFrom.m_distance)
  {}

  SbPlane& operator=(const SbPlane& aFrom) {
    m_normal = aFrom.m_normal;
    m_distance = aFrom.m_distance;
    return *this;
  }
public:
  const SbVec3d& getNormal() const { return m_normal;}
  double getDistanceFromOrigin(void) const { return m_distance;}
  double getDistance(const SbVec3d& a_point) const {
    return a_point.dot(m_normal) - m_distance;
  }
public: //CLHEP compatibility
  double distance(const SbVec3d& a_point) const {
    return getDistance(a_point);
  }
private:
  SbVec3d m_normal;
  double m_distance;
};

}

#endif
