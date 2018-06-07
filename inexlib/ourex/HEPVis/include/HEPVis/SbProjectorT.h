#ifndef HEPVis_SbProjectorT_h
#define HEPVis_SbProjectorT_h 

/*!
 * class SbProjector
 *
 * To handle special (non-linear) projections (rz, phiz, etc...)
*/

#include <Inventor/SbBasic.h>

#include <HEPVis/SbStyles.h>
#include <HEPVis/SbMath.h> //arctan

namespace HEPVis {

template <class T,class Vec3T>
class SbProjectorT {
public:
  SbProjectorT(SbProjectionType aType):fType(aType){}
  SbProjectorT(const SbProjectorT& aFrom):fType(aFrom.fType){}
  virtual ~SbProjectorT(){}
  SbProjectionType type() const { return fType;}
  SbBool project(int aNumber,Vec3T* aPoints) const {
         if(fType==SbProjectionRZ)   return projectRZ(aNumber,aPoints);
    else if(fType==SbProjectionPHIZ) return projectPHIZ(aNumber,aPoints);
    else if(fType==SbProjectionZR)   return projectZR(aNumber,aPoints);
    else if(fType==SbProjectionZPHI) return projectZPHI(aNumber,aPoints);
    else return TRUE; // Do nothing. It is ok.
  }
protected:
  SbBool projectRZ(int aNumber,Vec3T* aPoints) const {
    for(int index=0;index<aNumber;index++) {
      Vec3T& point = aPoints[index];
      T x = point[0];
      T y = point[1];
      T z = point[2];
      T r = (T)::sqrt((T)(x*x+y*y));
      //printf("debug : SbProjector::projectRZ : %d (%d) : %g %g\n",
      // index,aNumber,r,z);
      point[0] = r;
      point[1] = z;
      point[2] = 0;
    }
    return TRUE;
  }
  SbBool projectPHIZ(int aNumber,Vec3T* aPoints) const {
    for(int index=0;index<aNumber;index++) {
      Vec3T& point = aPoints[index];
      T x = point[0];
      T y = point[1];
      T z = point[2];
      T v;
      if(!arc_tan(y,x,v)) return FALSE;
      point[0] = v;
      point[1] = z;
      point[2] = 0;
    }
    return TRUE;
  }
  SbBool projectZR(int aNumber,Vec3T* aPoints) const {
    for(int index=0;index<aNumber;index++) {
      Vec3T& point = aPoints[index];
      T x = point[0];
      T y = point[1];
      T z = point[2];
      T r = (T)::sqrt((T)(x*x+y*y));
      //printf("debug : SbProjector::projectRZ : %d (%d) : %g %g\n",
      // index,aNumber,r,z);
      point[0] = z;
      point[1] = r;
      point[2] = 0;
    }
    return TRUE;
  }
  SbBool projectZPHI(int aNumber,Vec3T* aPoints) const {
    for(int index=0;index<aNumber;index++) {
      Vec3T& point = aPoints[index];
      T x = point[0];
      T y = point[1];
      T z = point[2];
      T v;
      if(!arc_tan(y,x,v)) return FALSE;
      point[0] = z;
      point[1] = v;
      point[2] = 0;
    }
    return TRUE;
  }
private:
  bool arc_tan(T aX,T aY,T& aValue) const {
    if((aX==0)&&(aY==0)) { 
      aValue = 0;
      return false;
    } else {
      aValue = (T)::atan2((T)aX,(T)aY); // return value in [-PI,PI]
      return true;
    }
  }
private:
  SbProjectionType fType;
};

}

#endif
