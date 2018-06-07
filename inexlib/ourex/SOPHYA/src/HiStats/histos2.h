//
// histogrammes 2D     cmv 2/8/96
//

#ifndef HISTOS2_SEEN
#define HISTOS2_SEEN

#include "machdefs.h"
#include <string>

#include <list>
#if defined(__KCC__)
#include <list.h>
#endif

#include "peida.h"
#include "utils.h"
#include "histos.h"

namespace SOPHYA {

//  Forward class declaration for Fits handler
template <class T>  class FitsHandler;

//! 2 dimensions histograms
class Histo2D : public AnyDataObj {
  friend class ObjFileIO<Histo2D>;
  friend class FitsHandler<Histo2D>;
public:

  // CREATOR / DESTRUCTOR
  Histo2D(r_8 xMin,r_8 xMax,int_4 nxBin,r_8 yMin,r_8 yMax,int_4 nyBin);
  Histo2D(r_4 xMin,r_4 xMax,int_4 nxBin,r_4 yMin,r_4 yMax,int_4 nyBin);
  Histo2D(const Histo2D& h);
  Histo2D();
  virtual ~Histo2D();

  // OPTIONS
  void Errors();
  void ReCenterBinX(void);
  void ReCenterBinY(void);
  void ReCenterBin(void);

  // UPDATING
  void Zero();
  void Add(r_8 x, r_8 y, r_8 w = 1.);

  // Operators
  Histo2D& operator = (const Histo2D& h);
  Histo2D& operator *= (r_8 b);
  Histo2D& operator /= (r_8 b);
  Histo2D& operator += (r_8 b);
  Histo2D& operator -= (r_8 b);
  Histo2D& operator += (const Histo2D& a);
  Histo2D& operator -= (const Histo2D& a);
  Histo2D& operator *= (const Histo2D& a);
  Histo2D& operator /= (const Histo2D& a);

  // get/put dans/depuis une matrice / vector
  void GetXCoor(TVector<r_8>& v) const;
  void GetValue(TMatrix<r_8> &v) const;
  void GetYCoor(TVector<r_8>& v) const;
  void GetError2(TMatrix<r_8>& v) const;
  void GetError(TMatrix<r_8>& v) const;
  void PutValue(TMatrix<r_8>& v, int_4 ierr=0);
  void PutValueAdd(TMatrix<r_8>& v, int_4 ierr=0);
  void PutError2(TMatrix<r_8>& v);
  void PutError2Add(TMatrix<r_8>& v);
  void PutError(TMatrix<r_8>& v);

  // INLINES
  //! Retourne l'abscisse minimum.
  inline r_8 XMin() const {return mXmin;}
  //! Retourne l'abscisse maximum.
  inline r_8 XMax() const {return mXmax;}
  //! Retourne l'ordonnee minimum.
  inline r_8 YMin() const {return mYmin;}
  //! Retourne l'ordonnee maximum.
  inline r_8 YMax() const {return mYmax;}
  //! Retourne le nombre de bins selon X.
  inline int_4 NBinX() const {return mNx;}
  //! Retourne le nombre de bins selon Y.
  inline int_4 NBinY() const {return mNy;}
  //! Retourne la largeur du bin selon X.
  inline r_8 WBinX() const {return mWBinx;}
  //! Retourne la largeur du bin selon Y.
  inline r_8 WBinY() const {return mWBiny;}
  //! Retourne le pointeur sur le tableaux des contenus.
  inline r_8* Bins() const  {return mData;}
  //! Retourne le contenu du bin i,j.
  inline r_8 operator()(int_4 i,int_4 j) const {return mData[j*mNx+i];}
  //! Remplit le contenu du bin i,j.
  inline r_8& operator()(int_4 i,int_4 j)      {return mData[j*mNx+i];}
  //! retourne "true" si il y a des erreurs stoquees
  inline bool HasErrors() const { if(mErr2) return true; else return false;}
  //! Retourne l'erreur du bin i,j.
  inline r_8 Error(int_4 i,int_4 j)  const
    {if(mErr2)
       {if(mErr2[j*mNx+i]>0.) return sqrt(mErr2[j*mNx+i]); else return 0.;}
     else return 0.;}
  //! Remplit l'erreur au carre du bin i,j.
  inline r_8 Error2(int_4 i,int_4 j) const
             {if(mErr2) return mErr2[j*mNx+i]; else return 0.;}
  //! Remplit l'erreur au carre du bin i,j.
  inline r_8& Error2(int_4 i,int_4 j) {return mErr2[j*mNx+i];}
  //! Retourne la somme ponderee.
  inline r_8 NData() const    {return nHist;}
  //! Retourne le nombre d'entrees.
  inline int_4 NEntries() const {return nEntries;}
  //! Retourne l'abscisse et l'ordonnee du coin inferieur du bin i,j.
  inline void BinLowEdge(int_4 i,int_4 j,r_8& x,r_8& y) const
              {x = mXmin + i*mWBinx; y = mYmin + j*mWBiny;}
  //! Retourne l'abscisse et l'ordonnee du coin inferieur du bin i,j.
  inline void BinLowEdge(int_4 i,int_4 j,r_4& xf,r_4& yf) const
              {r_8 x,y; BinLowEdge(i,j,x,y); xf=x; yf=y;}
  //! Retourne l'abscisse et l'ordonnee du centre du bin i,j.
  inline void BinCenter(int_4 i,int_4 j,r_8& x,r_8& y) const
              {x = mXmin + (i+0.5)*mWBinx; y = mYmin + (j+0.5)*mWBiny;}
  //! Retourne l'abscisse et l'ordonnee du centre du bin i,j.
  inline void BinCenter(int_4 i,int_4 j,r_4& xf,r_4& yf) const
              {r_8 x,y; BinCenter(i,j,x,y); xf=x; yf=y;}
  //! Retourne l'abscisse et l'ordonnee du coin superieur du bin i,j.
  inline void BinHighEdge(int_4 i,int_4 j,r_8& x,r_8& y) const
              {x = mXmin + (i+1)*mWBinx; y = mYmin + (j+1)*mWBiny;}
  //! Retourne l'abscisse et l'ordonnee du coin superieur du bin i,j.
  inline void BinHighEdge(int_4 i,int_4 j,r_4& xf,r_4& yf) const
              {r_8 x,y; BinHighEdge(i,j,x,y); xf=x; yf=y;}
  //! Retourne les numeros du bin contenant l'abscisse et l'ordonnee x,y.
  inline void FindBin(r_8 x,r_8 y,int_4& i,int_4& j) const
    {i=(int_4) floor((x-mXmin)/mWBinx); j=(int_4) floor((y-mYmin)/mWBiny);}

  // Info, statistique et calculs sur les histogrammes
  r_8   NOver(int_4 i=-1,int_4 j=-1) const;
  int_4 BinNonNul() const;
  int_4 ErrNonNul() const;
  void IJMax(int_4& imax,int_4& jmax,int_4 il=1,int_4 ih= -1,int_4 jl=1,int_4 jh= -1) const;
  void IJMin(int_4& imax,int_4& jmax,int_4 il=1,int_4 ih= -1,int_4 jl=1,int_4 jh= -1) const;
  r_8 VMax(int_4 il=1,int_4 ih= -1,int_4 jl=1,int_4 jh= -1) const;
  r_8 VMin(int_4 il=1,int_4 ih= -1,int_4 jl=1,int_4 jh= -1) const;
  int_4 EstimeMax(r_8& xm,r_8& ym,int_4 SzPav = 3
                 ,int_4 il=1,int_4 ih= -1,int_4 jl=1,int_4 jh= -1) const;
  int_4 EstimeMax(int_4 im,int_4 jm,r_8& xm,r_8& ym,int_4 SzPav = 3) const;
  int_4 FindMax(int_4& im,int_4& jm,int_4 SzPav = 3,r_8 Dz = 0.
               ,int_4 il=1,int_4 ih= -1,int_4 jl=1,int_4 jh= -1) const;

  // Print et Display ASCII
  virtual void Show(ostream& os) const;
  inline  void Show() const { Show(cout); }
  inline void  PrintStatus() const { Show(cout); }
  
  void Print(r_8 min=1.,r_8 max=-1.
            ,int_4 il=1,int_4 ih= -1,int_4 jl=1,int_4 jh= -1) const;

  // PROJECTIONS
  void SetProjX();
  void SetProjY();
  void SetProj();
  void DelProjX();
  void DelProjY();
  void DelProj();
  void ZeroProjX();
  void ZeroProjY();
  void ZeroProj();
  //! Retourne le pointeur sur l'histo 1D de la projection selon X.
  inline Histo* HProjX() const {return mHprojx;}
  //! Retourne le pointeur sur l'histo 1D de la projection selon Y.
  inline Histo* HProjY() const {return mHprojy;}
  void ShowProj() const;

  // BANDES
  //! Retourne le nombre de bandes selon X
  inline int_4 NBandX() const {return mLBandx.size();}
  //! Retourne le nombre de bandes selon Y
  inline int_4 NBandY() const {return mLBandy.size();}
  int_4 SetBandX(r_8 ybmin,r_8 ybmax);
  int_4 SetBandY(r_8 xbmin,r_8 xbmax);
  void DelBandX();
  void DelBandY();
  void ZeroBandX();
  void ZeroBandY();
  Histo* HBandX(int_4 n) const;
  Histo* HBandY(int_4 n) const;
  void GetBandX(int_4 n,r_8& ybmin,r_8& ybmax) const;
  void GetBandY(int_4 n,r_8& xbmin,r_8& xbmax) const;
  void ShowBand(int_4 lp = 0) const;

  // SLICES
  //! Retourne le nombre de slices selon X
  inline int_4 NSliX() const {return mLSlix.size();}
  //! Retourne le nombre de slices selon Y
  inline int_4 NSliY() const {return mLSliy.size();}
  int_4 SetSliX(int_4 nsli);
  int_4 SetSliY(int_4 nsli);
  void DelSliX();
  void DelSliY();
  void ZeroSliX();
  void ZeroSliY();
  Histo* HSliX(int_4 n) const;
  Histo* HSliY(int_4 n) const;
  void GetSliX(int_4 n,r_8& ybmin,r_8& ybmax) const;
  void GetSliY(int_4 n,r_8& xbmin,r_8& xbmax) const;
  void ShowSli(int_4 lp = 0) const;

#ifndef __DECCXX
protected:
#endif
  //! structure de definition des bandes
  struct bande_slice {
    int_4 num;   //!< nombre de bandes
    r_8 min;     //!< limite minimum pour remplir la bande
    r_8 max;     //!< limite maximum pour remplir la bande
    Histo* H;    //!< pointer sur l Histo 1D de la bande
    STRUCTCOMP(bande_slice)
  };
#ifdef __DECCXX
protected:
#endif

  void CreateOrResize(r_8 xMin,r_8 xMax,int_4 nxBin,r_8 yMin,r_8 yMax,int_4 nyBin);
  void Delete(void);

  r_8*        mData; //!< donnees
  r_8*        mErr2; //!< erreurs carrees

  r_8         mOver[3][3]; //!< overflow table
  r_8         nHist;       //!< somme ponderee des entrees
  int_4       nEntries;    //!< nombre d'entrees

  int_4       mNx;    //!< nombre de bins en X
  int_4       mNy;    //!< nombre de bins en Y
  int_4       mNxy;   //!< nombre de bins total
  r_8         mXmin;  //!< abscisse minimum
  r_8         mXmax;  //!< abscisse maximum
  r_8         mYmin;  //!< ordonnee minimum
  r_8         mYmax;  //!< ordonnee maximum
  r_8         mWBinx; //!< largeur du bin en X
  r_8         mWBiny; //!< largeur du bin en Y

  bande_slice mB_s;

  Histo*      mHprojx; //!< pointer sur Histo des proj X
  Histo*      mHprojy; //!< pointer sur Histo des proj Y

  list<bande_slice>  mLBandx; //!< liste des bandes selon X
  list<bande_slice>  mLBandy; //!< liste des bandes selon Y
  
  list<bande_slice>  mLSlix; //!< liste des slices selon X
  list<bande_slice>  mLSliy; //!< liste des slices selon Y

};

/*! Prints histogram information on stream \b s (h.Show(s)) */
inline ostream& operator << (ostream& s, Histo2D const & h)
  {  h.Show(s);  return(s);  }

/////////////////////////////////////////////////////////////////////////
// Classe pour la gestion de persistance

/*! \ingroup HiStats \fn operator<<(POuttPersist&,Histo2D)
  \brief Persistance management */
inline POutPersist& operator << (POutPersist& os, Histo2D & obj)
{ ObjFileIO<Histo2D> fio(&obj);  fio.Write(os);  return(os); }
/*! \ingroup HiStats \fn operator<<(POuttPersist&,Histo2D)
  \brief Persistance management */
inline PInPersist& operator >> (PInPersist& is, Histo2D & obj)
{ ObjFileIO<Histo2D> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

// Classe pour la gestion de persistance
// ObjFileIO<Histo2D>

/*! \ingroup HiStats \fn operator*(const Histo2D&,r_8)
  \brief Operateur H2 = H1 * b */
inline Histo2D operator * (const Histo2D& a, r_8 b)
{
  Histo2D result(a);
  return (result *= b);
}

/*! \ingroup HiStats \fn operator*(r_8,const Histo2D&)
  \brief Operateur H2 = b * H1 */
inline Histo2D operator * (r_8 b, const Histo2D& a)
{
  Histo2D result(a);
  return (result *= b);
}

/*! \ingroup HiStats \fn operator/(const Histo2D&,r_8)
  \brief Operateur H2 = H1 / b */
inline Histo2D operator / (const Histo2D& a, r_8 b)
{
  Histo2D result(a);
  return (result /= b);
}

/*! \ingroup HiStats \fn operator+(const Histo2D&,r_8)
  \brief Operateur H2 = H1 + b */
inline Histo2D operator + (const Histo2D& a, r_8 b)
{
  Histo2D result(a);
  return (result += b);
}

/*! \ingroup HiStats \fn operator+(r_8,const Histo2D&)
  \brief Operateur H2 = b + H1 */
inline Histo2D operator + (r_8 b, const Histo2D& a)
{
  Histo2D result(a);
  return (result += b);
}

/*! \ingroup HiStats \fn operator-(const Histo2D&,r_8)
  \brief Operateur H2 = H1 - b */
inline Histo2D operator - (const Histo2D& a, r_8 b)
{
  Histo2D result(a);
  return (result -= b);
}

/*! \ingroup HiStats \fn operator-(r_8,const Histo2D&)
  \brief Operateur H2 = b - H1 */
inline Histo2D operator - (r_8 b, const Histo2D& a)
{
  Histo2D result(a);
  result *= -1.;
  return (result += b);
}

/*! \ingroup HiStats \fn operator+(const Histo2D&,const Histo2D&)
  \brief Operateur H = H1 + H2 */

inline Histo2D operator + (const Histo2D& a, const Histo2D& b)
{
if (b.NBinX()!=a.NBinX() || b.NBinY()!=a.NBinY()) throw SzMismatchError(PExcLongMessage(""));
Histo2D c(a);
return (c += b);
}

/*! \ingroup HiStats \fn operator-(const Histo2D&,const Histo2D&)
  \brief Operateur H = H1 - H2 */
inline Histo2D operator - (const Histo2D& a, const Histo2D& b)
{
if (b.NBinX()!=a.NBinX() || b.NBinY()!=a.NBinY()) throw SzMismatchError(PExcLongMessage(""));
Histo2D c(a);
return (c -= b);
}

/*! \ingroup HiStats \fn operator*(const Histo2D&,const Histo2D&)
  \brief Operateur H = H1 * H2 */
inline Histo2D operator * (const Histo2D& a, const Histo2D& b)
{
if (b.NBinX()!=a.NBinX() || b.NBinY()!=a.NBinY()) throw SzMismatchError(PExcLongMessage(""));
Histo2D c(a);
return (c *= b);
}

/*! \ingroup HiStats \fn operator/(const Histo2D&,const Histo2D&)
  \brief Operateur H = H1 / H2 */
inline Histo2D operator / (const Histo2D& a, const Histo2D& b)
{
if (b.NBinX()!=a.NBinX() || b.NBinY()!=a.NBinY()) throw SzMismatchError(PExcLongMessage(""));
Histo2D c(a);
return (c /= b);
}

} // Fin du namespace

#endif // HISTOS2_SEEN
