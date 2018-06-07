#ifndef HEPVis_SbMath_h
#define HEPVis_SbMath_h

#include <cmath>
#ifndef M_PI
#define M_PI       3.1415926535897931160E0
#define M_PI_2     1.5707963267948965580E0  
#endif

#define FM_PI      ((float)M_PI)
#define FM_PI_2    ((float)M_PI_2)

#define SbMinimum(a,b) ((a)<(b)?(a):(b))
#define SbMaximum(a,b) ((a)>(b)?(a):(b))

#define FCOS(x)     ((float)::cos(double(x)))
#define FSIN(x)     ((float)::sin(double(x)))
#define FACOS(x)    ((float)::acos(double(x)))
#define FASIN(x)    ((float)::asin(double(x)))
#define FTAN(x)     ((float)::tan(double(x)))
#define FATAN(x)    ((float)::atan(double(x)))
#define FATAN2(x,y) ((float)::atan2(double(x),double(y)))
#define FSQRT(x)    ((float)::sqrt(double(x)))
#define FPOW(x,y)   ((float)::pow(double(x),(double)(y)))
#define FLOG(x)     ((float)::log(double(x)))
#define FLOG10(x)   ((float)::log10(double(x)))
#define FFLOOR(x)   ((float)::floor(double(x)))
#define FFABS(x)    ((float)::fabs(double(x)))
#define FCEIL(x)    ((float)::ceil(double(x)))
  
inline int SbFROUND(float aFloat) {
  // From CoinGL/src/base/SbViewportRegion.cpp.
  if (aFloat == (float) (int(aFloat))) return int(aFloat);
  else return (aFloat>0.0f) ? int(aFloat+0.5f) : -int(0.5f-aFloat);
}

#endif
