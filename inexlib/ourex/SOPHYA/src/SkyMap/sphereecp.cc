#include "sopnamsp.h"
#include <math.h>

#define SPHEREECP_CC_BFILE  // avoid extern template declarations 
#include "sphereecp.h"

/*!
  \class SOPHYA::SphereECP
  \ingroup SkyMap
  \brief Spherical maps in Equatorial Cylindrical Projection

  Class implementing spherical maps, with a pixelisation
  corresponding to the an Equatorial Cylindrical Projection (ECP).
  Pixel size varie from equator to poles. The sphere is divided 
  into a number of slices along the parallels. Each slice is subdivided
  into the same number of pixels.
  This class provides the possibility to have partial coverage.
  Can be used for the following types: <br>
  <tt> int_4 , r_4 , r_8 , complex<r_4> , complex<r_8> </tt>
*/

namespace SOPHYA {

//! Default constructor
template <class T>
SphereECP<T>::SphereECP()
  : _pixels()
{
  _partial = false;
  _theta1 = 0.;
  _theta2 = M_PI;
  _phi1 = 0.;
  _phi2 = 2*M_PI;
  _outofmapidx = -99;
  _outofmapnphi = 0;
  _outofmapntet = 0;
  _outofmappix = 0;
  _outofmapval = 0;
  _dtheta = 0.;
  _dphi = 0.;
}

/*!
  \brief  Constructor with the pixelisation parameter m
  
  Complete coverage.
  The sphere is divided into \b m slices in theta (0..pi). each slice is divided
  into \b 2m pixels (along phi) 
*/
template <class T>
SphereECP<T>::SphereECP(int m)
  : _pixels(2*m,m,0,0,0,true)
{
  _partial = false;
  _theta1 = 0.;
  _theta2 = M_PI;
  _phi1 = 0.;
  _phi2 = 2*M_PI;
  _outofmapidx = -99;
  _outofmapnphi = 0;
  _outofmapntet = 0;
  _outofmappix = 0;
  _outofmapval = 0;
  _dtheta = _dphi = M_PI/m;
}

/*! 
  Complete coverage.
  \b ntet slices in theta (0..pi) and \b nphi pixels (along phi) for 
  each slice in theta
*/
template <class T>
SphereECP<T>::SphereECP(int ntet, int nphi)
  : _pixels(nphi,ntet,0,0,0,true)
{
  _partial = false;
  _theta1 = 0.;
  _theta2 = M_PI;
  _phi1 = 0.;
  _phi2 = 2*M_PI; 
  _outofmapidx = -99;
  _outofmapnphi = 0;
  _outofmapntet = 0;
  _outofmappix = 0;
  _outofmapval = 0;
  _dtheta = M_PI/ntet;
  _dphi = 2*M_PI/nphi;
}

/*! 
  Partial coverage : 
  \b ntet slices in theta , in the range (tet1 .. tet2) and \b nphi pixels (along phi) for 
  each slice in theta in the range (phi1 .. phi2) 
*/
template <class T>
SphereECP<T>::SphereECP(r_8 tet1, r_8 tet2, int ntet, r_8 phi1, r_8 phi2, int nphi)
  : _pixels(nphi,ntet,0,0,0,true)
{
  if( (tet1 > M_PI) || (tet1 < 0.) || (tet2 > M_PI) || (tet2 < 0.) || 
      (phi1 < 0.) || (phi1 > 2*M_PI) || (phi1 < 0.) || (phi1 > 2*M_PI) ||
      (tet2 <= tet1) || (phi2 <= phi1) )
    throw  ParmError("SphereECP::SphereECP() Bad range for theta/phi limits");

   _partial = true;
  _theta1 = tet1;
  _theta2 = tet2;
  _phi1 = phi1;
  _phi2 = phi2; 
  _outofmapidx = _pixels.Size()+659;
  // Reza, 23 sep 2004 
  // pour des cartes partielles, et des pixels sur la sphere, mais 
  // en dehors de la couverture, on code le numero de tranche en theta, 
  // compte a partir de theta=0,phi=0 +  _outofmapidx
  // theta -> idx =  _outofmapidx + theta/_dtheta*_outofmapnphi +  _outofmapntet
  _outofmapntet = 0;
  _outofmappix = 0;
  _outofmapval = 0;
  _dtheta = (_theta2-_theta1)/ntet;
  _dphi = (_phi2-_phi1)/nphi;

  _outofmapntet = M_PI/_dtheta;
  _outofmapnphi = 2*M_PI/_dphi;
}

//! Copy constructor - shares the pixel data if \b share=true
template <class T>
SphereECP<T>::SphereECP(const SphereECP<T>& a, bool share)
  : _pixels(a._pixels, share)
{
  _partial = a._partial;
  _theta1 = a._theta1;
  _theta2 = a._theta2;
  _phi1 = a._phi1;
  _phi2 = a._phi2;   
  _outofmapidx = a._outofmapidx;
  _outofmapnphi = a._outofmapnphi;
  _outofmapntet = a._outofmapntet;
  _outofmappix = a._outofmappix;
  _outofmapval = a._outofmapval;
  _dtheta = a._dtheta;
  _dphi = a._dphi;
}

//! Copy constructor - shares the pixel data 
template <class T>
SphereECP<T>::SphereECP(const SphereECP<T>& a)
  : _pixels(a._pixels)
{
  _partial = a._partial;
  _theta1 = a._theta1;
  _theta2 = a._theta2;
  _phi1 = a._phi1;
  _phi2 = a._phi2;   
  _outofmapidx = a._outofmapidx;
  _outofmapnphi = a._outofmapnphi;
  _outofmapntet = a._outofmapntet;
  _outofmappix = a._outofmappix;
  _outofmapval = a._outofmapval;
  _dtheta = a._dtheta;
  _dphi = a._dphi;
}

template <class T>
SphereECP<T>::~SphereECP()
{
}

/*!
  \brief Extract a partial map from a full ECP skymap
  Theta and Phi limits are adjusted automatically to be correspond to the source map pixel boundaries
*/
template <class T>
SphereECP<T> SphereECP<T>::ExtractPartial(r_8 tet1, r_8 tet2, r_8 phi1, r_8 phi2)
{
  if (IsPartial())      
    throw  ParmError("SphereECP::ExtractPartial() source map NOT a full sky map");
  if( (tet1 > M_PI) || (tet1 < 0.) || (tet2 > M_PI) || (tet2 < 0.) || 
      (phi1 < 0.) || (phi1 > 2*M_PI) || (phi1 < 0.) || (phi1 > 2*M_PI) ||
      (tet2 <= tet1) || (phi2 <= phi1) )
    throw  ParmError("SphereECP::ExtractPartial() Bad range for theta/phi limits");

// correction/calcul des limites en theta et nombre de rangees en theta  
  sa_size_t ntet = (sa_size_t)((tet2-tet1)/_dtheta + 0.5);
  sa_size_t offt = (sa_size_t)(tet1/_dtheta + 1e-3);
  tet1 = _dtheta*offt;
  tet2 = tet1+ntet*_dtheta;
// correction/calcul des limites en phi et nombre de colonnes en phi
  sa_size_t nphi = (sa_size_t)((phi2-phi1)/_dphi + 0.5);
  sa_size_t offp = (sa_size_t)(phi1/_dphi + 1e-3);
  phi1 = _dphi*offp;
  phi2 = phi1+nphi*_dphi;

// On cree une carte partielle avec les bons parametres
  SphereECP<T> pmap(tet1, tet2, ntet, phi1, phi2, nphi);
// On recopie les valeurs de pixels de la zone correspondante
  pmap.GetPixelArray() = _pixels.SubArray(Range(offp, 0, nphi, 0), Range(offt, 0, ntet, 0),
	                                  Range::first(), Range::first(), Range::first());
  return pmap;
}

template <class T>
string SphereECP<T>::TypeOfMap() const
{
  return string("ECP");
}

template <class T>
void SphereECP<T>::SetTemp(bool temp) const
{
  _pixels.SetTemp(temp);
}

template <class T>
int_4 SphereECP<T>::NbPixels() const
{
  return _pixels.Size();
}

template <class T>
T& SphereECP<T>::PixVal(int_4 k)
{
  // On pourrait etre plus strict en demandant aussi k<_outofmapidx+_outofmapnphi*_outofmapntet (Reza 23/09/04)
  if ((_partial) && (k>=_outofmapidx))   return _outofmappix;
  if((k < 0) || (k >= _pixels.Size()) )  
     throw RangeCheckError("SphereECP::PIxVal() Pixel index out of range");
  return _pixels.DataBlock()(k);
}

template <class T>
T const& SphereECP<T>::PixVal(int_4 k)  const
{
  // On pourrait etre plus strict en demandant aussi k<_outofmapidx+_outofmapnphi*_outofmapntet (Reza 23/09/04)
  if ((_partial) && (k>=_outofmapidx))   return _outofmapval;
  if((k < 0) || (k >= _pixels.Size()) )  
     throw RangeCheckError("SphereECP::PIxVal() Pixel index out of range");
  //  return _pixels.DataBlock()(k);
  return _pixels.Data()[k];
}

template <class T>
bool  SphereECP<T>::ContainsSph(double theta, double phi) const
{
  if (_partial) {
    if ( (theta >= _theta1) && (theta <= _theta2) &&
	 (phi >= _phi1) && (phi <= _phi2) )  return true;
    else return false;
  }
  else return false;
}

template <class T>
int_4  SphereECP<T>::PixIndexSph(double theta, double phi) const
{
  if((theta > M_PI) || (theta < 0.)) return(-1);
  if((phi < 0.) || (phi > 2*M_PI)) return(-1);
  if (_partial) {
    if ( (theta < _theta1) || (theta > _theta2) ||
	 (phi < _phi1) || (phi > _phi2) )  return _outofmapidx+theta/_dtheta*_outofmapnphi+phi/_dphi;
  }
  int_4 it = (theta-_theta1)/_dtheta;
  int_4 jp = (phi-_phi1)/_dphi;
  return (it*_pixels.SizeX()+jp);
}

template <class T>
void  SphereECP<T>::PixThetaPhi(int_4 k, double& theta, double& phi) const
{
  if ((_partial) && (k>=_outofmapidx)) {
    theta = (((k-_outofmapidx) / _outofmapnphi)+0.5)*_dtheta;
    phi = (((k-_outofmapidx) % _outofmapnphi)+0.5)*_dphi;;
    return;
  }
  if((k < 0) || (k >= _pixels.Size()) )  
     throw RangeCheckError("SphereECP::PixThetaPhi() Pixel index out of range");

  int_4 it = k / _pixels.SizeX();
  int_4 jp = k % _pixels.SizeX();
  theta = _theta1+(it+0.5)*_dtheta;
  phi = _phi1+(jp+0.5)*_dphi;
  return;
}

template <class T>
T  SphereECP<T>::SetPixels(T v)
{
  _pixels = v;
  return v;
}

template <class T>
double  SphereECP<T>::PixSolAngle(int_4 k) const
{
  if ((_partial) && (k>=_outofmapidx)) {
    double theta = (((k-_outofmapidx) / _outofmapnphi)+0.5)*_dtheta;
    return (_dtheta*_dphi*sin(theta));
  }
  if((k < 0) || (k >= _pixels.Size()) )  
     throw RangeCheckError("SphereECP::PixSolAngle(int_4 k) Pixel index out of range");

  int_4 it = k / _pixels.SizeX();
  double theta = _theta1+it*_dtheta;
  return (_dtheta*_dphi*sin(theta));
}

template <class T>
int_4  SphereECP<T>::SizeIndex() const
{
  return _pixels.SizeY();
}

template <class T>
void  SphereECP<T>::Resize(int_4 m)
{
  if ( (m <= 0) || ( m == _pixels.SizeY()) ) {
    if (PixelMap_GetGlobalPrintLevel()>0)  
      cout << " SphereECP<T>::Resize(int_4 " << m << ") m<0 ou m=NTheta - Ne fait rien " << endl;
    return;
  }
  int mphi = m;
  if (_pixels.Size() > 0)  mphi = m*_pixels.SizeX()/_pixels.SizeY();
  if (PixelMap_GetGlobalPrintLevel()>0) 
    cout << " SphereECP<T>::Resize(" << m 
	 << ") -> _pixels.Resize(" << mphi << "," << m << ")" << endl; 
  sa_size_t sz[5] = {0,0,0,0,0};
  sz[0] = mphi;  sz[1] = m;
  _pixels.ReSize(2,sz);
  _outofmapidx = _pixels.Size()+659;
  _dtheta = (_theta2-_theta1)/m;
  _dphi = (_phi2-_phi1)/mphi;

  _outofmapntet = M_PI/_dtheta;
  _outofmapnphi = 2*M_PI/_dphi;
  return;
}

template <class T>
uint_4  SphereECP<T>::NbThetaSlices() const
{
  return _pixels.SizeY();
}

template <class T>
r_8  SphereECP<T>::ThetaOfSlice(int_4 index) const
{
  if( (index < 0) || (index >= _pixels.SizeY()) )  
     throw RangeCheckError("SphereECP::ThetaOfSlice() theta index out of range");
  return (_theta1 + (index+0.5)*_dtheta);
 
}

template <class T>
void  SphereECP<T>::GetThetaSlice(int_4 index,r_8& theta, 
				  TVector<r_8>& phi, TVector<T>& value) const 
{
  if( (index < 0) || (index >= _pixels.SizeY()) )  
     throw RangeCheckError("SphereECP::GetThetaSlice() index out of range");

  theta = _theta1 + (index+0.5)*_dtheta;
  int_4 nphit = 2.*M_PI/_dphi;
  phi.ReSize(nphit); 
  value.ReSize(nphit); 
  int_4 ioff = _phi1/_dphi;
  int_4 i;
  for(i=0; i<ioff; i++) {
    phi(i) = _phi1 + (i-ioff+0.5)*_dphi;
    value(i) = _outofmapval;
  }
  for(i=ioff; i<ioff+_pixels.SizeX(); i++) {
    phi(i) = _phi1 + (i-ioff+0.5)*_dphi;
    value(i) = _pixels(i-ioff,index,0);
  }
  for(i=ioff+_pixels.SizeX(); i<nphit; i++) {
    phi(i) = _phi1 + (i-ioff+0.5)*_dphi;
    value(i) = _outofmapval;
  }
  return;
}

template <class T>
void  SphereECP<T>::GetThetaSlice(int_4 index, r_8& theta, r_8& phi0, 
				  TVector<int_4>& pixidx, TVector<T>& value) const
{
  if( (index < 0) || (index >= _pixels.SizeY()) )  
    throw RangeCheckError("SphereECP::GetThetaSlice() index out of range");
  
  theta = _theta1 + (index+0.5)*_dtheta;
  int_4 ioff = _phi1/_dphi;
  phi0 = _phi1 - (ioff-0.5)*_dphi;

  int_4 nphit = 2.*M_PI/_dphi;
  pixidx.ReSize(nphit); 
  value.ReSize(nphit);
 
  int_4 i;
  for(i=0; i<ioff; i++) {
    pixidx(i) = _outofmapidx+theta/_dtheta*_outofmapnphi+phi0/_dphi+i;
    value(i) = _outofmapval;
  }
  for(i=ioff; i<ioff+_pixels.SizeX(); i++) {
    pixidx(i) = index*_pixels.SizeX()+(i-ioff);
    value(i) = _pixels(i-ioff,index,0);
  }
  for(i=ioff+_pixels.SizeX(); i<nphit; i++) {
    pixidx(i) = _outofmapidx+theta/_dtheta*_outofmapnphi+phi0/_dphi+i;
    value(i) = _outofmapval;
  }
  return;

}

template <class T>
T*  SphereECP<T>::GetThetaSliceDataPtr(int_4 index)
{
  if( (index < 0) || (index >= _pixels.SizeY()) )  
    throw RangeCheckError("SphereECP::GetThetaSliceDataPtr() index out of range");
  int_4 ioff = _phi1/_dphi;
  int_4 nphit = 2.*M_PI/_dphi;
// Correction bug signale par cmv le 27/08/2008, debordement memoire lors
// de synthese par SphericalTransformServer ...
  if ( (ioff != 0) || (nphit!=_pixels.SizeX()) ) return NULL;
  return _pixels.Data() + index*_pixels.SizeX();
}

template <class T>
void  SphereECP<T>::Show(ostream& os) const
{
  PixelMap<T>::Show(os);
  if (_partial)
    os << "SphereECP<T>::Print() - Partial ECP Map NPhi=" << _pixels.SizeX() 
       << " x NTheta= " <<  _pixels.SizeY() << " SolidAngle=" << PixSolAngle() << "\n"
       << "ThetaLimits= " << _theta1 << " .. " << _theta2 
       << "  PhiLimits= " << _phi1 << " .. " << _phi2 << endl;
  else 
    os << "SphereECP<T>::Print() - Full ECP Map NPhi=" << _pixels.SizeX() 
       << " x NTheta= " <<  _pixels.SizeY() << " SolidAngle=" << PixSolAngle() << endl;
}

template <class T>
void  SphereECP<T>::Print(ostream& os) const
{
  Show(os);
  os << _pixels;
}

template <class T>
SphereECP<T>&  SphereECP<T>::Set(const SphereECP<T>& a)
{
  _pixels.Set(a._pixels);
  _partial = a._partial;
  _theta1 = a._theta1;
  _theta2 = a._theta2;
  _phi1 = a._phi1;
  _phi2 = a._phi2;   
  _outofmapidx = a._outofmapidx;
  _outofmapnphi = a._outofmapnphi;
  _outofmapntet = a._outofmapntet;
  _outofmappix = a._outofmappix;
  _outofmapval = a._outofmapval;
  _dtheta = a._dtheta;
  _dphi = a._dphi; 
  return *this ;
}

template <class T>
SphereECP<T>&  SphereECP<T>::SetCst(T x)
{
  _pixels.SetCst(x);
  return *this ;
}

template <class T>
SphereECP<T>&  SphereECP<T>::AddCst(T x)
{
  _pixels += x;
  return *this ;
}

template <class T>
SphereECP<T>&  SphereECP<T>::MulCst(T x)
{
  _pixels *= x;
  return *this ;
}



}// Fin du namespace


#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template SphereECP<int_4>
#pragma define_template SphereECP<r_4>
#pragma define_template SphereECP<r_8>
#pragma define_template SphereECP< complex<r_4> >
#pragma define_template SphereECP< complex<r_8> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
template class SphereECP<int_4>;
template class SphereECP<r_4>;
template class SphereECP<r_8>;
template class SphereECP< complex<r_4> >;
template class SphereECP< complex<r_8> >;
}
#endif

