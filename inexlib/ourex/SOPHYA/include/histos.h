// This may look like C code, but it is really -*- C++ -*-
//
// $Id: histos.h,v 1.27 2011/02/18 11:15:25 cmv Exp $
//

#ifndef HISTOS_SEEN
#define HISTOS_SEEN

#include "objfio.h"
#include <iostream>
#include <stdio.h>
#include "peida.h"
#include "tvector.h"
#include "ppersist.h"
#include "anydataobj.h"

namespace SOPHYA {

//  Forward class declaration for Fits handler
template <class T>  class FitsHandler;

//! 1 dimension histograms
class Histo : public AnyDataObj {
  friend class ObjFileIO<Histo>;
  friend class FitsHandler<Histo>;
public:

  // CREATOR / DESTRUCTOR
  Histo();
  Histo(r_8 xMin, r_8 xMax, int_4 nBin=100);
  Histo(r_4 xMin, r_4 xMax, int_4 nBin=100);
  Histo(const Histo& H);
  virtual ~Histo();

  // OPTIONS
  void Errors();
  void ReCenterBin();

  // UPDATING or SETTING
  void Zero(void);
  void ReSize(r_8 xMin, r_8 xMax, int_4 nBin);
  void Add(r_8 x, r_8 w);
  inline void Add(r_8 x) {Add(x,1.);}
  void AddBin(int_4 numBin, r_8 w);
  inline void AddBin(int_4 numBin) {AddBin(numBin,1.);}
  void SetBin(r_8 x, r_8 w);
  inline void SetBin(r_8 x) {SetBin(x,1.);}
  void SetBin(int_4 numBin, r_8 w);
  inline void SetBin(int_4 numBin) {SetBin(numBin,1.);}
  void SetErr2(r_8 x, r_8 e2);
  void SetErr2(int_4 numBin, r_8 e2);
  void SetErr(r_8 x, r_8 e);
  void SetErr(int_4 numBin, r_8 e);
  virtual void UpdateHisto(bool force=false) const;

  // Operators
  Histo& operator = (const Histo& h);
  Histo& operator *= (r_8 b);
  Histo& operator /= (r_8 b);
  Histo& operator += (r_8 b);
  Histo& operator -= (r_8 b);
  Histo& operator += (const Histo& a);
  Histo& operator -= (const Histo& a);
  Histo& operator *= (const Histo& a);
  Histo& operator /= (const Histo& a);

  // get/put dans/depuis un vector
  void GetAbsc(TVector<r_8>& v) const;
  void GetValue(TVector<r_8>& v) const;
  void GetError2(TVector<r_8>& v) const;
  void GetError(TVector<r_8>& v) const;
  void PutValue(TVector<r_8>& v, int_4 ierr=0);
  void PutValueAdd(TVector<r_8> &v, int_4 ierr=0);
  void PutError2(TVector<r_8>& v);
  void PutError2Add(TVector<r_8>& v);
  void PutError(TVector<r_8>& v);

  // INLINES
  //! Retourne l'abscisse minimum
  inline r_8 XMin() const {return mMin;}
  //! Retourne l'abscisse maximum
  inline r_8 XMax() const {return mMax;}
  //! Retourne le nombre de bins
  inline int_4 NBins() const {return mBins;}
  //! Retourne la largeur du bin
  inline r_8 BinWidth() const {return binWidth;}
  //! Retourne le pointeur sur le tableaux des contenus
  inline r_8* Bins() const {return mData;}
  //! Retourne le contenu du bin i
  inline r_8 operator()(int_4 i) const  {return mData[i];}
  //! Remplit le contenu du bin i
  inline r_8& operator()(int_4 i) {return mData[i];}
  //! retourne "true" si il y a des erreurs stoquees
  inline bool HasErrors() const
         {if(mErr2) return true; else return false;}
  //! Retourne l'erreur du bin i
  inline r_8 Error(int_4 i) const
         {if(mErr2) {if(mErr2[i]>0.) return sqrt(mErr2[i]); else return 0.;}
                    else return 0.;}
  //! Retourne l'erreur au carre du bin i
  inline r_8 Error2(int_4 i) const
         {if(mErr2) return mErr2[i]; else return 0.;}
  //! Remplit l'erreur au carre du bin i
  inline r_8& Error2(int_4 i) {return mErr2[i];}
  //! Retourne la somme ponderee
  inline r_8 NData() const            {return nHist;}
  //! Retourne le nombre d'entrees
  inline uint_8 NEntries() const         {return nEntries;}
  //! Retourne le nombre d'overflow
  inline r_8 NOver() const            {return mOver;}
  //! Retourne le nombre d'underflow
  inline r_8 NUnder() const           {return mUnder;}

  //! Retourne l'abscisse du bord inferieur du bin i
  inline r_8 BinLowEdge(int_4 i)  const {return mMin + i*binWidth;}
  //! Retourne l'abscisse du centre du bin i
  inline r_8 BinCenter(int_4 i)   const {return mMin + (i+0.5)*binWidth;}
  //! Retourne l'abscisse du bord superieur du bin i
  inline r_8 BinHighEdge(int_4 i) const {return mMin + (i+1)*binWidth;} 
  //! Retourne le numero du bin contenant l'abscisse x
  inline int_4 FindBin(r_8 x) const   
         {return (int_4) floor((x - mMin) / binWidth);}

  // Info, statistique et calculs sur les histogrammes
  int_4     BinNonNul() const;
  int_4     ErrNonNul() const;
  int_4     IMax() const;
  int_4     IMin() const;
  r_8       VMax() const;
  r_8       VMin() const;
  r_8       Sum() const;
  r_8       Sum2() const;
  r_8       Mean() const;
  r_8       Sigma() const;
  r_8       MeanLH(int_4 il,int_4 ih) const;
  r_8       SigmaLH(int_4 il,int_4 ih) const;
  r_8       Mean(r_8 x0, r_8 dx) const;
  r_8       Sigma(r_8 x0, r_8 dx) const;
  r_8       CleanedMean() const;
  r_8       CleanedMean(r_8& sigma) const;
  int_4     BinPercent(r_8 per) const;
  int_4     BinPercent(r_8 x,r_8 per,int_4& imin,int_4& imax) const;
  int_4     BinPercent(r_8 x,r_8 per,r_8& xmin,r_8& xmax) const;
  void      HInteg(r_8 norm = 0.);
  void      HDeriv();
  virtual void HRebin(int_4 nbinew);

  int_4     MaxiLocal(r_8& maxi,int_4& imax,r_8& maxn,int_4& imaxn) const;
  r_8       FitMax(int_4 degree=2, r_8 frac=0.5f, int_4 debug=0) const;
  r_8       FindWidth(r_8 xmax,r_8 frac=0.5f, int_4 debug=0) const;
  r_8       FindWidth(r_8 frac=0.5f, int_4 debug=0) const;
  int_4     EstimeMax(r_8& xm,int_4 SzPav = 3) const;
  int_4     EstimeMax(int_4& im,r_8& xm,int_4 SzPav = 3) const;
  void      EstimeWidthS(r_8 frac,r_8& widthG,r_8& widthD) const;

  // Print et Display ASCII
  virtual void Show(ostream& os) const;
  inline  void Show() const { Show(cout); }
  void Print(int_4 dyn = 100, r_8 hmin = 1., r_8 hmax = -1.,
	     int_4 pflag = 0, int_4 il = 1, int_4 ih = -1) const;
  
protected:
  void CreateOrResize(r_8 xMin, r_8 xMax, int_4 nBin);
  void Delete(); // Ne pas passer en "virtual"

  r_8*   mData;    //!< donnees
  r_8*   mErr2;	   //!< erreurs carrees
  r_8    mUnder;   //!< underflow
  r_8    mOver;    //!< overflow
  r_8    nHist;    //!< somme ponderee des entrees
  uint_8 nEntries; //!< nombre d'entrees
  int_4  mBins;    //!< nombre de bins
  r_8    mMin;     //!< abscisse minimum
  r_8    mMax;     //!< abscisse maximum
  r_8    binWidth; //!< largeur du bin
};

/*! Prints histogram information on stream \b s (h.Show(s)) */
inline ostream& operator << (ostream& s, Histo const & h)
  {  h.Show(s);  return(s);  }

/*! \ingroup HiStats \fn operator<<(POuttPersist&,Histo)
  \brief Persistance management */
inline POutPersist& operator << (POutPersist& os, Histo & obj)
{ ObjFileIO<Histo> fio(&obj);  fio.Write(os);  return(os); }
/*! \ingroup HiStats \fn operator<<(POuttPersist&,Histo)
  \brief Persistance management */
inline PInPersist& operator >> (PInPersist& is, Histo & obj)
{ ObjFileIO<Histo> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

// Classe pour la gestion de persistance
// ObjFileIO<Histo>

/*! \ingroup HiStats \fn operator*(const Histo&,r_8)
  \brief Operateur H2 = H1 * b */
inline Histo operator * (const Histo& a, r_8 b)
{
  Histo result(a);
  return (result *= b);
}

/*! \ingroup HiStats \fn operator*(r_8,const Histo&)
  \brief Operateur H2 = b * H1 */
inline Histo operator * (r_8 b, const Histo& a)
{
  Histo result(a);
  return (result *= b);
}

/*! \ingroup HiStats \fn operator/(const Histo&,r_8)
  \brief Operateur H2 = H1 / b */
inline Histo operator / (const Histo& a, r_8 b)
{
  Histo result(a);
  return (result /= b);
}

/*! \ingroup HiStats \fn operator+(const Histo&,r_8)
  \brief Operateur H2 = H1 + b */
inline Histo operator + (const Histo& a, r_8 b)
{
  Histo result(a);
  return (result += b);
}

/*! \ingroup HiStats \fn operator+(r_8,const Histo&)
  \brief Operateur H2 = b + H1 */
inline Histo operator + (r_8 b, const Histo& a)
{
  Histo result(a);
  return (result += b);
}

/*! \ingroup HiStats \fn operator-(const Histo&,r_8)
  \brief Operateur H2 = H1 - b */
inline Histo operator - (const Histo& a, r_8 b)
{
  Histo result(a);
  return (result -= b);
}

/*! \ingroup HiStats \fn operator-(r_8,const Histo&)
  \brief Operateur H2 = b - H1 */
inline Histo operator - (r_8 b, const Histo& a)
{
  Histo result(a);
  result *= -1.;
  return (result += b);
}

/*! \ingroup HiStats \fn operator+(const Histo&,const Histo&)
  \brief Operateur H = H1 + H2 */
inline Histo operator + (const Histo& a, const Histo& b)
{
if (b.NBins()!=a.NBins()) throw SzMismatchError(PExcLongMessage(""));
Histo c(a);
return (c += b);
}

/*! \ingroup HiStats \fn operator-(const Histo&,const Histo&)
  \brief Operateur H = H1 - H2 */
inline Histo operator - (const Histo& a, const Histo& b)
{
if (b.NBins()!=a.NBins()) throw SzMismatchError(PExcLongMessage(""));
Histo c(a);
return (c -= b);
}

/*! \ingroup HiStats \fn operator*(const Histo&,const Histo&)
  \brief Operateur H = H1 * H2 */
inline Histo operator * (const Histo& a, const Histo& b)
{
if (b.NBins()!=a.NBins()) throw SzMismatchError(PExcLongMessage(""));
Histo c(a);
return (c *= b);
}

/*! \ingroup HiStats \fn operator/(const Histo&,const Histo&)
  \brief Operateur H = H1 / H2 */
inline Histo operator / (const Histo& a, const Histo& b)
{
if (b.NBins()!=a.NBins()) throw SzMismatchError(PExcLongMessage(""));
Histo c(a);
return (c /= b);
}

} // Fin du namespace

#endif // HISTOS_SEEN
