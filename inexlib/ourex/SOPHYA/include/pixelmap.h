// This may look like C code, but it is really -*- C++ -*-
#ifndef PIXELMAP_SEEN
#define PIXELMAP_SEEN

#include "ppersist.h" 
#include "dvlist.h"
#include "spherepos.h"
#include "datatype.h"
#include "sunitpcst.h"
#include <iostream>
#include <typeinfo>

namespace SOPHYA {

/*!
 \class PixelMap
 \ingroup SkyMap
 General map of pixels on part of sphere or whole sphere.

 Class hierarchy :
\verbatim
   PixelMap
     SphericalMap
         SphereThetaPhi
         SphereHEALPix
         SphereIco
      LocalMap
\endverbatim
*/

//! Set global print level for all pixel maps
void PixelMap_SetGlobalPrintLevel(int_4 lev=0); 
//! Get global print level for all pixel maps 
int_4 PixelMap_GetGlobalPrintLevel(); 

template<class T>
class PixelMap  : public AnyDataObj 
{
public:
  
              PixelMap(SphereCoordSys* cs = NULL) : mInfo_(NULL) 
                    { if (cs) cs_ = cs; else cs_ = new SphereCoordSys; }
virtual       ~PixelMap() 
                    { if (mInfo_) delete mInfo_; if (cs_) delete cs_; }

// Set/Change/Get the coordinate system
virtual void  SetCoordSys(SphereCoordSys* cs)
                    { if (cs) { delete cs_;  cs_ = cs; } }
inline SphereCoordSys* GetCoordSys() const { return(cs_); }

/*! Number of pixels */
virtual int_4 NbPixels() const=0;
   
/*! Value of pixel number k */
virtual T& PixVal(int_4 k)=0;
virtual T const& PixVal(int_4 k) const=0;

// Map s coverage
virtual bool ContainsSph(double theta, double phi) const=0;
virtual bool Contains(const SpherePosition& spos) const;

/*! Index of pixel at (theta,phi) */
virtual int_4 PixIndexSph(double theta, double phi) const=0;
// Index of pixel at a sky-position 
virtual int_4 PixIndex(const SpherePosition& spos);

/*! Value of pixel number at (theta,phi) */
virtual T& PixValSph(double theta, double phi)
                           {return PixVal(PixIndexSph(theta,phi));}
virtual T const& PixValSph(double theta, double phi) const
                           {return PixVal(PixIndexSph(theta,phi));}

/*! Spherical coordinates of center of pixel number k */
virtual void PixThetaPhi(int_4 k, double& theta, double& phi) const=0;

/*! provides a integer characterizing the pixelization refinement 
 (depending of the type of the map) 
*/             
virtual int_4 SizeIndex() const=0;
virtual string TypeOfMap() const =0;

/*! Pixel  (steradians) */
virtual double PixSolAngle(int_4 k) const =0; 

/*! Setting blockdata to temporary (see ndatablock documentation) */
virtual void SetTemp(bool temp=false) const =0;


/*! Overloading of () to access pixel number k. */
inline T& operator()(int_4 k) {return(PixVal(k));}
inline T const& operator()(int_4 k) const {return(PixVal(k));}

// Overloading of () to access pixel at a sky position .
inline T& operator()(const SpherePosition& spos) 
                     { return(PixVal(PixIndex(spos))); }
inline T const& operator()(const SpherePosition& spos) const 
                     { return(PixVal(PixIndex(spos))); }


// Note : no overloading of (double,double) to access pixel (theta,phi).
// overloading of (double,double) in SphericalMap
// overloading of (int,int)     in CartesianMap

/*! Setting pixel values to a constant */
virtual T SetPixels(T v);
PixelMap<T>& SetT(T v);
  //inline T operator = (T v) { return(SetPixels(v)); }
inline PixelMap<T>& operator = (T v) { return(SetT(v)); }

//! Ascii print of synthetic information about the pixelmap on stream os
virtual void  Show(ostream& os) const;
//! Show information on \b cout
inline  void  Show() const { Show(cout); }

                     
//! Return a reference to the associated DVList object
DVList& Info()
  {
    if (mInfo_ == NULL)  mInfo_ = new DVList;
    return(*mInfo_);
  }

const DVList*  ptrInfo() const  { return mInfo_; }

  //--- Changement et acces a l'unite physique associee
  //! Define or change the physical unit of the data.
  virtual void SetUnits(const Units& un) = 0;
  /*!
    \brief Return the physical unit of the data. 
    Flag sdone is set to true if SetUnits() has been called. 
  */
  virtual Units GetUnits(bool& sdone) const = 0;
  //! Return the physical unit of the data 
  inline Units GetUnits() const  
    {bool fg; return GetUnits(fg); }

protected :
    SphereCoordSys *cs_;   // Coordinate system used in the map
    DVList* mInfo_;        // Infos (variables) attachees 
};

template <class T>
inline ostream& operator << (ostream& s,  const PixelMap<T> & a)
{ a.Show(s);   return(s); }

template <class T>
int_4 PixelMap<T>::PixIndex(const SpherePosition& spos)
{
UnitVector v = spos.Transform(*cs_);
return(PixIndexSph(v.Theta(), v.Phi()));
}

template <class T>
bool PixelMap<T>::Contains(const SpherePosition& spos) const
{
UnitVector v = spos.Transform(*cs_);
return(ContainsSph(v.Theta(), v.Phi()));
}
template <class T>
T PixelMap<T>::SetPixels(T v)
{
int k;
for(k=0; k<NbPixels(); k++) PixVal(k) = v;
return(v);
}
template <class T>
PixelMap<T>& PixelMap<T>::SetT(T v)
{
int k;
for(k=0; k<NbPixels(); k++) PixVal(k) = v;
return(*this);
}

template <class T>
void PixelMap<T>::Show(ostream& os) const
{
os << "PixelMap<T>::Show() class: " << typeid(*this).name() 
   << " T= " << DataTypeInfo<T>::getTypeName() 
   << " TypeOfMap= " << TypeOfMap() << endl;
double solang = 0.;
if (NbPixels() > 0) solang = PixSolAngle(NbPixels()/2);
 double frac = NbPixels()*solang*100/(4.*M_PI);
if (frac > 100.) frac = 100.;
os << " NbPixels()= " << NbPixels() << " ResolArcMin ~=" 
   << Angle(sqrt(solang)).ToArcMin() << " SphereFrac ~= " 
   << frac << " % of 4Pi" << endl;
}


} // Fin du namespace

#endif
