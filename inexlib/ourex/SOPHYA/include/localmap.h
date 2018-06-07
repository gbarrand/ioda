// This may look like C code, but it is really -*- C++ -*-
#ifndef LOCALMAP_SEEN
#define LOCALMAP_SEEN

#include "pixelmap.h"
#include "sphericalmap.h"
#include "ndatablock.h"

#include "anydataobj.h"
#include "ppersist.h"



// ***************** Class LocalMap *****************************



namespace SOPHYA {


template <class T>
class FIO_LocalMap;

template<class T>
class FITS_LocalMap;

/* Class LocalMap */


template<class T>
class LocalMap : public PixelMap<T>
{

 // friend declaration for classes which handle persistence and FITS IO
  friend class FIO_LocalMap<T>; 
  friend class FITS_LocalMap<T>; 

public:

LocalMap();
LocalMap(int_4 nx, int_4 ny);
LocalMap(int_4 nx, int_4 ny, double angleX,double angleY, double theta0,double phi0,int_4 x0,int_4 y0,double angle=0.);
LocalMap(int_4 nx, int_4 ny, double angleX,double angleY, double theta0,double phi0, double angle=0.);
LocalMap(const LocalMap<T>& lm, bool share);
LocalMap(const LocalMap<T>& lm);
virtual ~LocalMap();

inline virtual bool IsTemp(void) const { return pixels_.IsTemp();}

/*! Setting blockdata to temporary (see ndatablock documentation) */
inline virtual void SetTemp(bool temp=false) const {pixels_.SetTemp(temp);};


// ---------- Overloading of () to access pixel number k ----

inline T& operator()(int_4 k) {return(this->PixVal(k));}
inline T const& operator()(int_4 k) const {return(this->PixVal(k));}
inline T& operator()(int_4 ix, int_4 iy) {return this->PixVal(iy*nSzX_+ix);};
inline T const& operator()(int_4 ix, int_4 iy) const {return this->PixVal(iy*nSzX_+ix);};

// Acces a un pixel 
inline T& operator()(double theta,double phi) {return(this->PixValSph(theta,phi));};
inline T  const& operator()(double theta,double phi) const {return(this->PixValSph(theta,phi));};
   
// ---------- Definition of PixelMap abstract methods -------

virtual int_4 NbPixels() const;   // D.Y. int change en int_4 rationalisation Mac
  
virtual T& PixVal(int_4 k);
virtual T const& PixVal(int_4 k) const;
  //! if fg = true, will generate  exceptions , when theta-phi out of range
inline  void SetThrowExceptionWhenOutofMapFlag(bool fg = false) { exc_outofmap_ = fg; }

//! true --> exceptions wil be active when theta-phi out of range 
inline  bool GetThrowExceptionWhenOutofMapFlag() { return exc_outofmap_; }

virtual bool ContainsSph(double theta, double phi) const;
//! return pixel number correspoinding to (theta,phi)
virtual int_4 PixIndexSph(double theta,double phi) const;
   
//! return (theta,phi) correspoinding to pixel number k
virtual void PixThetaPhi(int_4 k,double& theta,double& phi) const;

//! set all pixel values equal to v
virtual T SetPixels(T v);

// return the solid angle covered by pixel k (steradians)
virtual double PixSolAngle(int_4 k) const; 

// ---------- Specific methods ------------------------------

void ReSize(int_4 nx, int_4 ny);

void SetOrigin(double theta=90.,double phi=0.,double angle=0.);
void SetOrigin(double theta,double phi,int_4 x0,int_4 y0,double angle=0.);
void SetSize(double angleX,double angleY);

  /*! \fn TypeOfMap()
 return the string "LOCAL"
  */ 
inline virtual string TypeOfMap() const {return string("LOCAL");};
 

/*! Check to see if the local mapping is done */
inline bool LocalMap_isDone() const {return( originSet_ && angleSized_ && pixelSized_);};

void PixThetaPhi(int_4 ip,int_4 it, double& theta,double& phi) const;


void ProjectionToSphere(SphericalMap<T>&) const;
  
/* There should be a more complex algorithm somewhere to combine *several* local maps to a full sphere.
      -> static method, or separate class */
  
/*! provides a integer characterizing the pixelization refinement  (here : number of pixels) */
inline virtual int_4 SizeIndex() const {return(nPix_);}
inline int_4 Size_x() const {return nSzX_;}
inline int_4 Size_y() const {return nSzY_;}

/* Je rajoute ces 2 fonctions inlines pour compatibilite d'interface 
   avec TArray  -   Reza 30/8/2000                                   */
inline int_4 SizeX() const {return nSzX_;}
inline int_4 SizeY() const {return nSzY_;}

inline void Origin(double& theta,double& phi,int_4& x0,int_4& y0,double& angle) const {theta= thetaDegresC_; phi= phiDegresC_; x0= x0_; y0= y0_;angle= angleDegres_;}

/*! total aperture in theta and phi, in degrees ( from SetSize() ) */
inline void Aperture(double& anglex,double& angley) const {anglex= angleDegresX_; angley= angleDegresY_;}


/*  Acces to the DataBlock  */
inline       NDataBlock<T>& DataBlock()       {return pixels_.DataBlock();}
inline const NDataBlock<T>& DataBlock() const {return pixels_.DataBlock();}

virtual void SetUnits(const Units& un)  
  { DataBlock().SetUnits(un); } 
virtual Units GetUnits(bool& sdone) const
  { return DataBlock().GetUnits(sdone); } 

/*  Acces to the matrix  */
//! access to matrix 
/*!
  \warning : a pixel is defined by the pair of a phi-like index 
   (x-axis) and a theta-like index (y-axis). The phi-like index denotes
   the column number in the matrix ; the theta-like index denotes the
   row number.
*/
inline       TMatrix<T>& Matrix()       {return pixels_;}
inline const TMatrix<T>& Matrix() const {return pixels_;}

/* impression */ 
void print(ostream& os) const;

// Operations diverses  = , +=, ...


virtual LocalMap<T>& Set(const LocalMap<T>& a);
inline  LocalMap<T>& operator = (const LocalMap<T>& a) {return Set(a);}

// A += -= *= /= x (ajoute, soustrait, ... x a tous les elements)

  //! Fill LocalMap with all elements equal to \b x
virtual LocalMap<T>& SetT(T a); 
inline  LocalMap<T>& operator = (T a) {return SetT(a);}

// Add \b x to all elements
virtual LocalMap<T>& Add(T a); 
inline  LocalMap<T>&  operator += (T x)  { return Add(x); }
// Substract \b x to all elements
virtual LocalMap<T>& Sub(T a,bool fginv=false); 
inline   LocalMap<T>&  operator -= (T x)  { return Sub(x); }
// Multiply all elements by \b x
virtual LocalMap<T>& Mul(T a); 
inline  LocalMap<T>&  operator *= (T x)  { return Mul(x); }
// Divide all elements by \b x
virtual LocalMap<T>& Div(T a); 
inline  LocalMap<T>&  operator /= (T x)  { return Div(x); }

// A += -=  (ajoute, soustrait element par element les deux spheres )
  //! Operator LocalMap += LocalMap
  virtual LocalMap<T>&  AddElt(const LocalMap<T>& a);
  inline  LocalMap<T>&  operator += (const LocalMap<T>& a)  { return AddElt(a); }



  virtual LocalMap<T>&  SubElt(const LocalMap<T>& a);
  //! Operator LocalMap -= LocalMap
  inline  LocalMap<T>&  operator -= (const LocalMap<T>& a)  { return SubElt(a); }
// Multiplication, division element par element les deux LocalMap 
  virtual LocalMap<T>&  MulElt(const LocalMap<T>& a);
  inline  LocalMap<T>&  operator *= (const LocalMap<T>& a)  { return MulElt(a); }
  virtual LocalMap<T>&  DivElt(const LocalMap<T>& a);
  inline  LocalMap<T>&  operator /= (const LocalMap<T>& a)  { return DivElt(a); }


void CloneOrShare(const LocalMap<T>& a);
void Share(const LocalMap<T>& a);
LocalMap<T>& CopyElt(const LocalMap<T>& a);

//! assign a new object Id (or DataRef Id) - useful for PPF write operations
  inline void RenewObjId() { pixels_.RenewObjId(); }

// ---------- Méthodes internes -----------------------------
          
private :

void InitNul();
void SetCoorC(double theta0, double phi0);
TMatrix<double> CalculMatricePassage();
void Getij(int_4 k,int_4& i,int_4& j) const;
  void PixToSphereC(int_4 ip, int_4 it, double& XP, double& YP, double& ZP) const;

void recopierVariablesSimples(const LocalMap<T>& lm);

void initializationError() const;


// ---------- Variables internes ----------------------------
  // variables suffisantes pour reconstruire l'objet

  bool pixelSized_;
  bool angleSized_;
  bool originSet_;

  int_4 nSzX_;
  int_4 nSzY_;
  double angleDegresX_;
  double angleDegresY_;
  double thetaDegresC_;
  double phiDegresC_;
  int_4 x0_;
  int_4 y0_;
  double angleDegres_;


  //  NDataBlock<T> pixels_;
  TMatrix<T> pixels_;


  // variables derivees (redondantes, precalculees pour ameliorer 
  // les performances)

  int_4 nPix_;


  double thetaC_;
  double phiC_;
  double csthC_;
  double snthC_;
  double csphC_;
  double snphC_;
  double XC_;
  double YC_;
  double ZC_;


  double angle_;
  double cosAngle_;
  double sinAngle_;
  double deltaPhi_;
  double deltaTheta_;

  bool exc_outofmap_;


};

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs A (+,-,*,/) (T) x
/*! \ingroup SkyMap \fn operator+(const LocalMap<T>&,T)
  \brief Operator LocalMap = LocalMap + constant */
template <class T> inline LocalMap<T> operator + (const LocalMap<T>& a, T b)
    {LocalMap<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Add(b); return result;}
/*! \ingroup SkyMap \fn operator+(T,const LocalMap<T>&)
  \brief Operator LocalMap = constant + LocalMap */
template <class T> inline LocalMap<T> operator + (T b,const LocalMap<T>& a)
    {LocalMap<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Add(b); return result;}


/*! \ingroup SkyMap\fn operator-(const LocalMap<T>&,T)
  \brief Operator LocalMap = LocalMap - constant */
template <class T> inline LocalMap<T> operator - (const LocalMap<T>& a, T b)
    {LocalMap<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Sub(b); return result;}

/*! \ingroup  \fn operator-(T,const LocalMap<T>&)
  \brief Operator LocalMap = constant - LocalMap */
template <class T> inline LocalMap<T> operator - (T b,const LocalMap<T>& a)
    {LocalMap<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Sub(b,true); return result;}

/*! \ingroup SkyMap \fn operator*(const LocalMap<T>&,T)
  \brief Operator LocalMap = LocalMap * constant */
template <class T> inline LocalMap<T> operator * (const LocalMap<T>& a, T b)
    {LocalMap<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Mul(b); return result;}

/*! \ingroup SkyMap \fn operator*(T,const LocalMap<T>&)
  \brief Operator LocalMap = constant * LocalMap */
template <class T> inline LocalMap<T> operator * (T b,const LocalMap<T>& a)
    {LocalMap<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Mul(b); return result;}

/*! \ingroup SkyMap \fn operator/(const LocalMap<T>&,T)
  \brief Operator LocalMap = LocalMap / constant */
template <class T> inline LocalMap<T> operator / (const LocalMap<T>& a, T b)
    {LocalMap<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Div(b); return result;}

/*! \ingroup SkyMap \fn operator/(T,const LocalMap<T>&)
  \brief Operator LocalMap = constant / LocalMap  */
template <class T> inline LocalMap<T> operator / (T b, const LocalMap<T>& a)
    {LocalMap<T> result; result.CloneOrShare(a); result.SetTemp(true); 
    result.Div(b, true); return result;}

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (+,-) B

/*! \ingroup SkyMap \fn operator+(const LocalMap<T>&,const LocalMap<T>&)
  \brief Operator LocalMap = LocalMap + LocalMap */
template <class T>
inline LocalMap<T> operator + (const LocalMap<T>& a,const LocalMap<T>& b)
    { LocalMap<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.AddElt(a); }
    else { result.CloneOrShare(a); result.AddElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator-(const LocalMap<T>&,const LocalMap<T>&)
  \brief Operator LocalMap = LocalMap - LocalMap */
template <class T>
inline LocalMap<T> operator - (const LocalMap<T>& a,const LocalMap<T>& b)
    { LocalMap<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.SubElt(a); }
    else { result.CloneOrShare(a); result.SubElt(b); }
    return result; }

////////////////////////////////////////////////////////////////
// Surcharge d'operateurs C = A (*,/) B

/*! \ingroup SkyMap \fn operator*(const LocalMap<T>&,const LocalMap<T>&)
  \brief Operator LocalMap = LocalMap * LocalMap (pixel by pixel multiply)*/
template <class T>
inline LocalMap<T> operator * (const LocalMap<T>& a,const LocalMap<T>& b)
    { LocalMap<T> result; result.SetTemp(true); 
    if (b.IsTemp())  { result.Share(b); result.MulElt(a); }
    else { result.CloneOrShare(a); result.MulElt(b); }
    return result; }

/*! \ingroup SkyMap \fn operator/(const LocalMap<T>&,const LocalMap<T>&)
  \brief Operator LocalMap = LocalMap / LocalMap (pixel by pixel divide)*/
template <class T>
inline LocalMap<T> operator / (const LocalMap<T>& a,const LocalMap<T>& b)
    { LocalMap<T> result; result.SetTemp(true); 
    result.CloneOrShare(a); result.DivElt(b); 
    return result; }

//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( LOCALMAP_CC_BFILE )
extern template class LocalMap<int_4>;
extern template class LocalMap<r_8>;
extern template class LocalMap<r_4>;
extern template class LocalMap< complex<r_8> >;
extern template class LocalMap< complex<r_4> >;
#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )



} // Fin du namespace

#endif
