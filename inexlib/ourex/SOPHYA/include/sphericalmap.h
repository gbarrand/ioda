#ifndef SPHERICALMAP_SEEN
#define SPHERICALMAP_SEEN

// valeurs de Pi, 2*Pi, etc                                              
#include "smathconst.h"
#include <math.h>
#include "pixelmap.h"
#include "tvector.h" 

// Map of pixels on a whole sphere.
// Class hierarchy :
//   PixelMap
//      SphericalMap
//         SphereThetaPhi
//         SphereGorski
//         SphereIco
//      LocalMap




namespace SOPHYA {

/*!
  \class SphericalMap
  \ingroup SkyMap
  Base class (pure virtual) for 4Pi spherical maps
*/

template<class T>
class SphericalMap : public PixelMap<T> 
{
  
public :
SphericalMap() {};
virtual	 ~SphericalMap() {};

// Overloading of () to access pixel number k.
inline T& operator()(int k) {return(this->PixVal(k));}
inline T  const& operator()(int k) const {return(this->PixVal(k));}
inline T& operator()(double theta,double phi) {return(this->PixValSph(theta,phi));};
inline T  const& operator()(double theta,double phi) const {return(this->PixValSph(theta,phi));};

// index characterizing the size pixelization : m for SphereThetaPhi
// nside for Gorski sphere...
virtual void Resize(int_4 m)=0;
virtual uint_4 NbThetaSlices() const=0;
virtual r_8  ThetaOfSlice(int_4 index) const=0;
/*!
  Return true if some theta slices have a symmetric counter-part at (Pi-Theta)
  (the default implementation return true) 
*/
virtual bool  HasSymThetaSlice() const { return false; }
/*! 
  Return the slice index for the symmetric slice theta=Pi-ThetaOfSlice(idx) 
  an invalid index is returned if the symmetric slice does not exist
*/
virtual int_4 GetSymThetaSliceIndex(int_4 idx) const { return -1; }
virtual void GetThetaSlice(int_4 index,r_8& theta, TVector<r_8>& phi, TVector<T>& value) const=0; 
virtual void GetThetaSlice(int_4 sliceIndex, r_8& theta, r_8& phi0, TVector<int_4>& pixelIndices,TVector<T>& value) const=0 ;
/*!
  If possible return a pointer to the slice pixel data. return NULL otherwise 
*/
virtual T*   GetThetaSliceDataPtr(int_4 sliceIndex) { return NULL; }

protected:
  static int_4 prt_lev_;  //!< Print level
};


} // Fin du namespace




#endif






