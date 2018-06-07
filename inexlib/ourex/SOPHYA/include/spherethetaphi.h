#ifndef SPHERETHETAPHI_SEEN
#define SPHERETHETAPHI_SEEN

#include "sphericalmap.h"
#include "ndatablock.h"
#include "tvector.h"

#include "anydataobj.h"
#include "ppersist.h"

namespace SOPHYA {



template <class T>
class FIO_SphereThetaPhi;  

template<class T>
class FITS_SphereThetaPhi;


// ***************** Class SphereThetaPhi *****************************
template <class T>
class SphereThetaPhi : public SphericalMap<T>
{

public :

//! return the size index value corresponding to resolution \b res (in radian)
static inline int_4 ResolToSizeIndex(double res)
  { return (int_4)((M_PI/2./res)+0.5); }
//! return the pixel resolution (in radian) for the size index \b m 
static inline double  SizeIndexToResol(int_4 m) 
  { return (M_PI/(2.*(double)m)); }

SphereThetaPhi();
SphereThetaPhi(int_4 m);
SphereThetaPhi(const SphereThetaPhi<T>& s, bool share);
SphereThetaPhi(const SphereThetaPhi<T>& s);
virtual ~SphereThetaPhi();

  // Temporaire?
inline virtual bool IsTemp(void) const {

    if (  NPhi_.IsTemp()  != pixels_.IsTemp() || 
	  TNphi_.IsTemp() != pixels_.IsTemp()|| 
	  Theta_.IsTemp() != pixels_.IsTemp()    )
      throw PException(" l'etat 'temporaire' de la spherethetaphi est incoherent"); 
    return pixels_.IsTemp();
}
/*! Setting blockdata to temporary (see ndatablock documentation) */
inline virtual void SetTemp(bool temp=false) const 
  {
    NPhi_.SetTemp(temp);
    TNphi_.SetTemp(temp);
    Theta_.SetTemp(temp);
    pixels_.SetTemp(temp);
  };

// ------------ Definition of PixelMap abstract methods -

/* retourne le nombre de pixels */ 
virtual int_4 NbPixels() const;

/* retourne la valeur du pixel d'indice k */ 
virtual T&       PixVal(int_4 k);
virtual T const& PixVal(int_4 k) const;

/* Return true if teta,phi in map  */
virtual bool ContainsSph(double theta, double phi) const;
/* retourne l'indice du pixel a (theta,phi) */ 
/*    Return index of the pixel corresponding to direction (theta, phi). */
virtual int_4 PixIndexSph(double theta, double phi) const;

/* retourne les coordonnees Spheriques du centre du pixel d'indice k */ 
virtual void PixThetaPhi(int_4 k, double& theta, double& phi) const;

/* Setting pixel values to a constant */
virtual T SetPixels(T v);

/* retourne/fixe l'angle Solide de Pixel   (steradians) */ 
virtual double PixSolAngle(int_4 dummy=0) const;
 
/* retourne/fixe la valeur du parametre de decoupage m */ 
//! Return the pixelisation parameter (number of slices in a hemisphere)
inline virtual int_4 SizeIndex() const { return( NTheta_); }

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

// ------------- Specific methods  ----------------------

virtual void Resize(int_4 m);

inline virtual string TypeOfMap() const {return string("TETAFI");};

/* Valeurs de theta des paralleles et phi des meridiens limitant le pixel d'indice k */
/*   Return values of theta,phi which limit the pixel with  index k */
virtual void Limits(int_4 k,double& th1,double& th2,double& phi1,double& phi2);

/* Nombre de tranches en theta */ 
/*    Return number of theta-slices on the sphere */
uint_4 NbThetaSlices() const;

/* Nombre de pixels en phi de la tranche d'indice kt */ 
int_4 NPhi(int_4 kt) const;

/* Renvoie dans t1,t2 les valeurs respectives de theta min et theta max  */
/* de la tranche d'indice kt  */ 
void Theta(int_4 kt, double& t1, double& t2) const;

/* Renvoie dans p1,p2 les valeurs phimin et phimax du pixel d'indice jp  */
/* dans la tranche d'indice kt  */ 
void Phi(int_4 kt, int_4 jp, double& p1, double& p2) const;

/* Renvoie l'indice k du pixel d'indice jp dans la tranche d'indice kt   */
/*!   Return pixel index  with sequence index jp in the slice kt */
int_4 Index(int_4 kt, int_4 jp) const;

/* Indice kt de la tranche et indice jp du pixel d'indice k  */ 
void ThetaPhiIndex(int_4 k,int_4& kt,int_4& jp);

void Pixelize(int_4); 

virtual r_8  ThetaOfSlice(int_4 index) const;
virtual int_4 GetSymThetaSliceIndex(int_4 idx) const;
virtual bool  HasSymThetaSlice() const;

virtual void GetThetaSlice(int_4 index,r_8& theta,TVector<r_8>& phi,TVector<T>& value) const; 
virtual void GetThetaSlice(int_4 index, r_8& theta, r_8& phi0,TVector<int_4>& pixelIndices, TVector<T>& value) const ;
virtual T*   GetThetaSliceDataPtr(int_4 sliceIndex);


//! ASCII dump (print) of the pixel map on stream \b os
void print(ostream& os) const;
//! ASCII dump (print) of the pixel map 
inline void Print(ostream& os) const { print(os); }
//! ASCII dump (print) of the pixel map on cout
inline void Print() const { print(cout); }



// Operations diverses  = , +=, ...


SphereThetaPhi<T>& Set(const SphereThetaPhi<T>& a);
inline  SphereThetaPhi<T>& operator = (const SphereThetaPhi<T>& a) 
                                                        {return Set(a);}

// A += -= *= /= x (ajoute, soustrait, ... x a tous les elements)

  //! Fill SphereThetaPhi with all elements equal to \b x
virtual SphereThetaPhi<T>& SetT(T a); 
inline  SphereThetaPhi<T>& operator = (T a) {return SetT(a);}

//! Add \b x to all elements
virtual SphereThetaPhi<T>& Add(T a); 
inline  SphereThetaPhi<T>&  operator += (T x)  { return Add(x); }
//! Substract \b x to all elements
virtual SphereThetaPhi<T>& Sub(T a,bool fginv=false); 
inline   SphereThetaPhi<T>&  operator -= (T x)  { return Sub(x); }
//! Multiply all elements by \b x
virtual SphereThetaPhi<T>& Mul(T a); 
inline  SphereThetaPhi<T>&  operator *= (T x)  { return Mul(x); }
//! Divide all elements by \b x
virtual SphereThetaPhi<T>& Div(T a); 
inline  SphereThetaPhi<T>&  operator /= (T x)  { return Div(x); }

// A += -=  (ajoute, soustrait element par element les deux spheres )
  //! Operator SphereThetaPhi += SphereThetaPhi
  virtual SphereThetaPhi<T>&  AddElt(const SphereThetaPhi<T>& a);
  inline  SphereThetaPhi<T>&  operator += (const SphereThetaPhi<T>& a)  { return AddElt(a); }



  virtual SphereThetaPhi<T>&  SubElt(const SphereThetaPhi<T>& a);
  //! Operator SphereThetaPhi -= SphereThetaPhi
  inline  SphereThetaPhi<T>&  operator -= (const SphereThetaPhi<T>& a)  { return SubElt(a); }
// Multiplication, division element par element les deux SphereThetaPhi 
  virtual SphereThetaPhi<T>&  MulElt(const SphereThetaPhi<T>& a);
  inline  SphereThetaPhi<T>&  operator *= (const SphereThetaPhi<T>& a)  { return MulElt(a); }
  virtual SphereThetaPhi<T>&  DivElt(const SphereThetaPhi<T>& a);
  inline  SphereThetaPhi<T>&  operator /= (const SphereThetaPhi<T>& a)  { return DivElt(a); }


  void CloneOrShare(const SphereThetaPhi<T>& a);
  void Share(const SphereThetaPhi<T>& a);

  SphereThetaPhi<T>& CopyElt(const SphereThetaPhi<T>& a);

//! assign a new object Id (or DataRef Id) - useful for PPF write operations
// Reza 02/2007 : Est-il suffisant de faire l'operation sur pixels_ ?
  inline void RenewObjId() { pixels_.RenewObjId(); }


 // friend declaration for classes which handle persistence and FITS IO
  friend class FIO_SphereThetaPhi<T>;
  friend class FITS_SphereThetaPhi<T>;

protected :

// ------------- méthodes internes ----------------------          
void InitNul();
inline void setParameters( int nbThetaIndex, int nbpix, double omega) 
  {
    NPix_= nbpix;
    Omega_= omega;
    NTheta_= nbThetaIndex;
  }

// ------------- variables internes ---------------------
 int_4 NTheta_;   // nombre de tranches en theta, pour une demi-sphere
 int_4 NPix_;     // nombre total de pixels
 double Omega_; // angle solide constant pour chaque pixel
 NDataBlock<int_4>  NPhi_;  // tableau donnant, pour chaque bande en theta, 
                         //le nombre de pixels selon phi
 NDataBlock<int_4> TNphi_; // tableau donnant ke nombre de pixels cumule,
                           // jusqu'au debut de chaque tranche
 NDataBlock<r_8> Theta_;
 NDataBlock<T> pixels_;
};

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs A (+,-,*,/) (T) x
/*! \ingroup SkyMap \fn operator+(const SphereThetaPhi<T>&,T)
  \brief Operator SphereThetaPhi = SphereThetaPhi + constant */
template <class T> inline SphereThetaPhi<T> operator + (const SphereThetaPhi<T>& a, T b)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Add(b); return result;}
/*! \ingroup SkyMap \fn operator+(T,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = constant + SphereThetaPhi */
template <class T> inline SphereThetaPhi<T> operator + (T b,const SphereThetaPhi<T>& a)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Add(b); return result;}


/*! \ingroup SphereThetaPhi\fn operator-(const SphereThetaPhi<T>&,T)
  \brief Operator SphereThetaPhi = SphereThetaPhi - constant */
template <class T> inline SphereThetaPhi<T> operator - (const SphereThetaPhi<T>& a, T b)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Sub(b); return result;}

/*! \ingroup  \fn operator-(T,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = constant - SphereThetaPhi */
template <class T> inline SphereThetaPhi<T> operator - (T b,const SphereThetaPhi<T>& a)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Sub(b,true); return result;}

/*! \ingroup SkyMap \fn operator*(const SphereThetaPhi<T>&,T)
  \brief Operator SphereThetaPhi = SphereThetaPhi * constant */
template <class T> inline SphereThetaPhi<T> operator * (const SphereThetaPhi<T>& a, T b)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Mul(b); return result;}

/*! \ingroup SkyMap \fn operator*(T,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = constant * SphereThetaPhi */
template <class T> inline SphereThetaPhi<T> operator * (T b,const SphereThetaPhi<T>& a)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Mul(b); return result;}

/*! \ingroup SkyMap \fn operator/(const SphereThetaPhi<T>&,T)
  \brief Operator SphereThetaPhi = SphereThetaPhi / constant */
template <class T> inline SphereThetaPhi<T> operator / (const SphereThetaPhi<T>& a, T b)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Div(b); return result;}

/*! \ingroup SkyMap \fn operator/(T,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = constant / SphereThetaPhi  */
template <class T> inline SphereThetaPhi<T> operator / (T b, const SphereThetaPhi<T>& a)
    {SphereThetaPhi<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Div(b, true); return result;}

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (+,-) B

/*! \ingroup SkyMap \fn operator+(const SphereThetaPhi<T>&,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = SphereThetaPhi + SphereThetaPhi */
template <class T>
inline SphereThetaPhi<T> operator + (const SphereThetaPhi<T>& a,const SphereThetaPhi<T>& b)
    { SphereThetaPhi<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.AddElt(a); }
    else { result.CloneOrShare(a); result.AddElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator-(const SphereThetaPhi<T>&,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = SphereThetaPhi - SphereThetaPhi */
template <class T>
inline SphereThetaPhi<T> operator - (const SphereThetaPhi<T>& a,const SphereThetaPhi<T>& b)
    { SphereThetaPhi<T> result; result.SetTemp(true); 
    result.CloneOrShare(a); result.SubElt(b); 
    return result; }

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (*,/) B

/*! \ingroup SkyMap \fn operator*(const SphereThetaPhi<T>&,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = SphereThetaPhi * SphereThetaPhi (pixel by pixel multiply)*/
template <class T>
inline SphereThetaPhi<T> operator * (const SphereThetaPhi<T>& a,const SphereThetaPhi<T>& b)
    { SphereThetaPhi<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.MulElt(a); }
    else { result.CloneOrShare(a); result.MulElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator/(const SphereThetaPhi<T>&,const SphereThetaPhi<T>&)
  \brief Operator SphereThetaPhi = SphereThetaPhi / SphereThetaPhi (pixel by pixel divide) */
template <class T>
inline SphereThetaPhi<T> operator / (const SphereThetaPhi<T>& a,const SphereThetaPhi<T>& b)
    { SphereThetaPhi<T> result; result.SetTemp(true); 
    result.CloneOrShare(a); result.DivElt(b); 
    return result; }


//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( SPHERETHETAPHI_CC_BFILE )
extern template class SphereThetaPhi<int_4>;
extern template class SphereThetaPhi<r_8>;
extern template class SphereThetaPhi<r_4>;
extern template class SphereThetaPhi< complex<r_4> >;
extern template class SphereThetaPhi< complex<r_8> >;
#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )

} // Fin du namespace

#endif
