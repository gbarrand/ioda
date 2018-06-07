#ifndef SPHEREHEALPIX_SEEN
#define SPHEREHEALPIX_SEEN

#include "sphericalmap.h"
#include "tvector.h"
#include "ndatablock.h"

#include "anydataobj.h"
#include "ppersist.h"

#include "HEALPixUtils.h"

namespace SOPHYA {


// ***************** CLASSE SphereHEALPix *****************************

  /*! Class SphereHEALPix */


template<class T>
class FIO_SphereHEALPix;

template<class T>
class FITS_SphereHEALPix;

template<class T>
class SphereHEALPix : public SphericalMap<T>
{
public :
// Static Methods to ease the use of HEALPix index <> angle conversion methods

  static inline int_4  nest2ring(int_4 nside,int_4 ipnest) 
         { return HEALPix::nest2ring(nside, ipnest); }
  static inline int_4  ring2nest(int_4 nside,int_4 ipring) 
         { return HEALPix::ring2nest(nside, ipring); }
  static inline int_4  ang2pix_ring(int_4 nside,double theta,double phi) 
         { return HEALPix::ang2pix_ring(nside, theta, phi); }
  static inline int_4  ang2pix_nest(int_4 nside,double theta,double phi) 
         { return HEALPix::ang2pix_nest(nside, theta, phi); }
  static inline void   pix2ang_ring(int_4 nside,int_4 ipix,double& theta,double& phi) 
         { HEALPix::pix2ang_ring(nside, ipix, theta, phi); }
  static inline void   pix2ang_nest(int_4 nside,int_4 ipix,double& theta,double& phi) 
         { HEALPix::pix2ang_nest(nside, ipix, theta, phi); }

  //! return the size index (=nside) corresponding to resolution res (in radian)
  static inline int_4 ResolToSizeIndex(double res)
         { return  HEALPix::ResolToSizeIndex(res); }

  //! return the size index (=nside) corresponding to resolution res (in radian)
  static inline int_4 ResolToNSide(double res)
         { return  HEALPix::ResolToSizeIndex(res); }
  //! return the pixel resolution (in radian) for the size index (=nside) m 
  static inline double  SizeIndexToResol(int_4 m) 
  { return  HEALPix::SizeIndexToResol(m); }
  //! return the pixel resolution (in radian) for the size index (=nside) m 
  static inline double  NSideToResol(int_4 m) 
  { return  HEALPix::SizeIndexToResol(m); }

SphereHEALPix(bool fgring=true);
SphereHEALPix(int_4 m, bool fgring=true);
SphereHEALPix(const SphereHEALPix<T>& s, bool share);
SphereHEALPix(const SphereHEALPix<T>& s);
virtual ~SphereHEALPix();

inline virtual bool   IsTemp(void) const {

    if (sliceBeginIndex_.IsTemp() != pixels_.IsTemp() || sliceLenght_.IsTemp() != pixels_.IsTemp() )
      throw PException(" l'etat 'temporaire' de la spherehealpix est incoherent"); 
    return pixels_.IsTemp();
}

inline virtual void SetTemp(bool temp=false) const 
  {
    pixels_.SetTemp(temp);
    sliceBeginIndex_.SetTemp(temp);
    sliceLenght_.SetTemp(temp);
  };
// ------------------ Definition of PixelMap abstract methods

virtual int_4 NbPixels() const;

virtual T& PixVal(int_4 k);
virtual T const& PixVal(int_4 k) const;

virtual uint_4 NbThetaSlices() const;
virtual r_8  ThetaOfSlice(int_4 index) const;
virtual bool  HasSymThetaSlice() const;
virtual int_4 GetSymThetaSliceIndex(int_4 idx) const;

virtual void GetThetaSlice(int_4 index,r_8& theta,TVector<r_8>& phi,TVector<T>& value) const;
virtual void GetThetaSlice(int_4 sliceIndex,r_8& theta, r_8& phi0, TVector<int_4>& pixelIndices,TVector<T>& value) const ;
virtual T*   GetThetaSliceDataPtr(int_4 sliceIndex);

virtual bool ContainsSph(double theta, double phi) const;
virtual int_4 PixIndexSph(double theta,double phi) const;

virtual void PixThetaPhi(int_4 k,double& theta,double& phi) const;

virtual T SetPixels(T v);

/*! \brief Pixel Solid angle  (steradians)

    All the pixels have the same solid angle. The dummy argument is
    for compatibility with eventual pixelizations which would not 
   fulfil this requirement.
*/
inline virtual double PixSolAngle(int_4 dummy=0) const {return omeg_;}

/*  Acces to the DataBlock  */
//! Acces to the pixel data NDataBlock<T> object
inline       NDataBlock<T>& DataBlock()       {return pixels_;}
//! Acces to the pixel data NDataBlock<T> object (const version)
inline const NDataBlock<T>& DataBlock() const {return pixels_;}

//--- Changement et acces a l'unite physique associee
virtual void SetUnits(const Units& un)  
  { pixels_.SetUnits(un); } 
virtual Units GetUnits(bool& sdone) const
  { return pixels_.GetUnits(sdone); } 

// --------------- Specific methods 

virtual void Resize(int_4 m);
virtual string TypeOfMap() const;

inline bool IfRING() const { return fgring_; } 
inline bool IfNESTED() const { return ( (fgring_) ? false : true ); } 


void Pixelize(int_4); 

int_4 NestToRing(int_4) const;

int_4 RingToNest(int_4) const;


/*! \return value of healpix nside */
inline virtual int_4 SizeIndex() const {return(nSide_);}

void print(ostream& os) const;
inline void Print(ostream& os) const { print(os); }

//--------------------- Operations diverses  = , +=, ...

SphereHEALPix<T>& Set(const SphereHEALPix<T>& a);
inline  SphereHEALPix<T>& operator = (const SphereHEALPix<T>& a) 
                                                        {return Set(a);}

// A += -= *= /= x (ajoute, soustrait, ... x a tous les elements)

//! Fill SphereHEALPix with all elements equal to \b x
virtual SphereHEALPix<T>& SetT(T a); 
inline  SphereHEALPix<T>& operator = (T a) {return SetT(a);}

//! Add \b x to all elements
virtual SphereHEALPix<T>& Add(T a); 
inline  SphereHEALPix<T>&  operator += (T x)  { pixels_ += x; return *this; }
//! Substract \b x to all elements
virtual SphereHEALPix<T>& Sub(T a,bool fginv=false); 
inline   SphereHEALPix<T>&  operator -= (T x)  { pixels_ -= x; return *this; }
//! Multiply all elements by \b x
virtual SphereHEALPix<T>& Mul(T a); 
inline  SphereHEALPix<T>&  operator *= (T x)  {pixels_ *= x; return *this; }
//! Divide all elements by \b x
virtual SphereHEALPix<T>& Div(T a); 
inline  SphereHEALPix<T>&  operator /= (T x)  {pixels_ /= x; return *this; }

// A += -=  (ajoute, soustrait element par element les deux spheres )
  //! Operator SphereHEALPix += SphereHEALPix
  virtual SphereHEALPix<T>&  AddElt(const SphereHEALPix<T>& a);
  inline  SphereHEALPix<T>&  operator += (const SphereHEALPix<T>& a)  { return AddElt(a); }



  virtual SphereHEALPix<T>&  SubElt(const SphereHEALPix<T>& a);
  //! Operator SphereHEALPix -= SphereHEALPix
  inline  SphereHEALPix<T>&  operator -= (const SphereHEALPix<T>& a)  { return SubElt(a); }
// Multiplication, division element par element les deux SphereHEALPix 
  virtual SphereHEALPix<T>&  MulElt(const SphereHEALPix<T>& a);
  inline  SphereHEALPix<T>&  operator *= (const SphereHEALPix<T>& a)  { return MulElt(a); }
  virtual SphereHEALPix<T>&  DivElt(const SphereHEALPix<T>& a);
  inline  SphereHEALPix<T>&  operator /= (const SphereHEALPix<T>& a)  { return DivElt(a); }


 void CloneOrShare(const SphereHEALPix<T>& a);
 void Share(const SphereHEALPix<T>& a);
 SphereHEALPix<T>& CopyElt(const SphereHEALPix<T>& a);

//! assign a new object Id (or DataRef Id) - useful for PPF write operations
// Reza 02/2007 : Est-il suffisant de faire l'operation sur pixels_ ?
  inline void RenewObjId() { pixels_.RenewObjId(); }

 // friend declaration for classes which handle persistence and FITS IO
 friend class FIO_SphereHEALPix<T>;
 friend class FITS_SphereHEALPix<T>;
       
protected :

// ------------- méthodes internes ----------------------
void InitNul();
void SetThetaSlices(); 

inline void setParameters(int_4 nside, int_4 nbpixels, double solangle, 
			  bool fgring) 
{
  nSide_= nside;
  nPix_= nbpixels;
  omeg_= solangle;
  fgring_ = fgring;
}

// ------------- variables internes -----------------------

int_4 nSide_;
int_4 nPix_;
double omeg_;
bool fgring_;  // true -> RING pixelisation , false -> NESTED

NDataBlock<T> pixels_;
NDataBlock<int_4> sliceBeginIndex_;    // Indices in RING scheme
NDataBlock<int_4> sliceLenght_;

};

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs A (+,-,*,/) (T) x
/*! \ingroup SkyMap \fn operator+(const SphereHEALPix<T>&,T)
  \brief Operator SphereHEALPix = SphereHEALPix + constant */
template <class T> inline SphereHEALPix<T> operator + (const SphereHEALPix<T>& a, T b)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Add(b); return result;}
/*! \ingroup SkyMap \fn operator+(T,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = constant + SphereHEALPix */
template <class T> inline SphereHEALPix<T> operator + (T b,const SphereHEALPix<T>& a)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Add(b); return result;}


/*! \ingroup SphereHEALPix\fn operator-(const SphereHEALPix<T>&,T)
  \brief Operator SphereHEALPix = SphereHEALPix - constant */
template <class T> inline SphereHEALPix<T> operator - (const SphereHEALPix<T>& a, T b)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Sub(b); return result;}

/*! \ingroup  \fn operator-(T,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = constant - SphereHEALPix */
template <class T> inline SphereHEALPix<T> operator - (T b,const SphereHEALPix<T>& a)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Sub(b,true); return result;}

/*! \ingroup SkyMap \fn operator*(const SphereHEALPix<T>&,T)
  \brief Operator SphereHEALPix = SphereHEALPix * constant */
template <class T> inline SphereHEALPix<T> operator * (const SphereHEALPix<T>& a, T b)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Mul(b); return result;}

/*! \ingroup SkyMap \fn operator*(T,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = constant * SphereHEALPix */
template <class T> inline SphereHEALPix<T> operator * (T b,const SphereHEALPix<T>& a)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Mul(b); return result;}

/*! \ingroup SkyMap \fn operator/(const SphereHEALPix<T>&,T)
  \brief Operator SphereHEALPix = SphereHEALPix / constant */
template <class T> inline SphereHEALPix<T> operator / (const SphereHEALPix<T>& a, T b)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Div(b); return result;}

/*! \ingroup SkyMap \fn operator/(T,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = constant / SphereHEALPix  */
template <class T> inline SphereHEALPix<T> operator / (T b, const SphereHEALPix<T>& a)
    {SphereHEALPix<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Div(b, true); return result;}

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (+,-) B

/*! \ingroup SkyMap \fn operator+(const SphereHEALPix<T>&,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = SphereHEALPix + SphereHEALPix */
template <class T>
inline SphereHEALPix<T> operator + (const SphereHEALPix<T>& a,const SphereHEALPix<T>& b)
    { SphereHEALPix<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.AddElt(a); }
    else { result.CloneOrShare(a); result.AddElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator-(const SphereHEALPix<T>&,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = SphereHEALPix - SphereHEALPix */
template <class T>
inline SphereHEALPix<T> operator - (const SphereHEALPix<T>& a,const SphereHEALPix<T>& b)
    { SphereHEALPix<T> result; result.SetTemp(true); 
    result.CloneOrShare(a); result.SubElt(b); 
    return result; }

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (*,/) B

/*! \ingroup SkyMap \fn operator*(const SphereHEALPix<T>&,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = SphereHEALPix * SphereHEALPix (pixel by pixel multiply) */
template <class T>
inline SphereHEALPix<T> operator * (const SphereHEALPix<T>& a,const SphereHEALPix<T>& b)
    { SphereHEALPix<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.MulElt(a); }
    else { result.CloneOrShare(a); result.MulElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator/(const SphereHEALPix<T>&,const SphereHEALPix<T>&)
  \brief Operator SphereHEALPix = SphereHEALPix / SphereHEALPix (pixel by pixel divide) */
template <class T>
inline SphereHEALPix<T> operator / (const SphereHEALPix<T>& a,const SphereHEALPix<T>& b)
    { SphereHEALPix<T> result; result.SetTemp(true); 
    result.CloneOrShare(a); result.DivElt(b); 
    return result; }

//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( SPHEREHEALPIX_CC_BFILE )
extern template class SphereHEALPix<uint_2>;
extern template class SphereHEALPix<int_4>;
extern template class SphereHEALPix<r_8>;
extern template class SphereHEALPix<r_4>;
extern template class SphereHEALPix< complex<r_4> >;
extern template class SphereHEALPix< complex<r_8> >;
#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )

} // Fin du namespace

#endif
